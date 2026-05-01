#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#include "serial_parser.h"

using json = nlohmann::json;
using namespace pokecon;

TEST_CASE("ResetGamepadState initializes to default values", "[parser]") {
    GamepadState state;
    state.buttons = 0xFF;
    state.hat = 0;
    state.lx = 0;
    state.ly = 0;
    state.rx = 0;
    state.ry = 0;

    ResetGamepadState(state);

    REQUIRE(state.buttons == 0);
    REQUIRE(state.hat == 8);    // HAT_CENTER
    REQUIRE(state.lx == 128);   // STICK_CENTER
    REQUIRE(state.ly == 128);
    REQUIRE(state.rx == 128);
    REQUIRE(state.ry == 128);
}

TEST_CASE("Parse button-only command", "[parser]") {
    GamepadState state;
    ResetGamepadState(state);

    // Aボタン（0x0010）のみ、スティックなし
    // Extension側では 0x0010 >> 2 = 0x0004 となるが、
    // パーサーはシフト後の値を返す
    REQUIRE(ParseSerialLine("0x0010 8", state) == true);
    REQUIRE(state.buttons == 0x0004);  // 0x0010 >> 2
    REQUIRE(state.hat == 8);
    REQUIRE(state.lx == 128);  // スティックフラグなしなので維持
    REQUIRE(state.ly == 128);
    REQUIRE(state.rx == 128);
    REQUIRE(state.ry == 128);
}

TEST_CASE("Parse button with left stick", "[parser]") {
    GamepadState state;
    ResetGamepadState(state);

    // Aボタン（0x0010）+ 左スティックフラグ（0x0002）= 0x0012
    // 左スティック X=255, Y=128
    REQUIRE(ParseSerialLine("0x0012 8 FF 80", state) == true);
    REQUIRE(state.buttons == 0x0004);  // 0x0012 >> 2 = 0x0004
    REQUIRE(state.hat == 8);
    REQUIRE(state.lx == 255);
    REQUIRE(state.ly == 128);
    REQUIRE(state.rx == 128);  // 右スティックフラグなし
    REQUIRE(state.ry == 128);
}

TEST_CASE("Parse button with right stick", "[parser]") {
    GamepadState state;
    ResetGamepadState(state);

    // 右スティックフラグ（0x0001）+ 十字キー右（2）
    // 右スティック X=128, Y=255
    REQUIRE(ParseSerialLine("0x0001 2 80 FF", state) == true);
    REQUIRE(state.buttons == 0x0000);  // 0x0001 >> 2 = 0x0000
    REQUIRE(state.hat == 2);
    REQUIRE(state.lx == 128);  // 左スティックフラグなしなので維持
    REQUIRE(state.ly == 128);
    REQUIRE(state.rx == 128);
    REQUIRE(state.ry == 255);
}

TEST_CASE("Parse full command with both sticks", "[parser]") {
    GamepadState state;
    ResetGamepadState(state);

    // A+Bボタン（0x0018）+ 両スティックフラグ（0x0003）= 0x001B
    REQUIRE(ParseSerialLine("0x001B 8 00 00 FF FF", state) == true);
    REQUIRE(state.buttons == 0x0006);  // 0x001B >> 2 = 0x0006
    REQUIRE(state.hat == 8);
    REQUIRE(state.lx == 0x00);
    REQUIRE(state.ly == 0x00);
    REQUIRE(state.rx == 0xFF);
    REQUIRE(state.ry == 0xFF);
}

TEST_CASE("Stick values persist when flags are not set", "[parser][stateful]") {
    GamepadState state;
    ResetGamepadState(state);

    // 最初に左スティックを設定
    REQUIRE(ParseSerialLine("0x0002 8 FF 80", state) == true);
    REQUIRE(state.lx == 255);
    REQUIRE(state.ly == 128);

    // スティックフラグなしでボタンのみ更新
    REQUIRE(ParseSerialLine("0x0010 8", state) == true);
    REQUIRE(state.lx == 255);  // 左スティックは維持
    REQUIRE(state.ly == 128);
}

TEST_CASE("Right stick overwrites previous value", "[parser][stateful]") {
    GamepadState state;
    ResetGamepadState(state);

    // 右スティックを設定
    REQUIRE(ParseSerialLine("0x0001 8 80 FF", state) == true);
    REQUIRE(state.rx == 128);
    REQUIRE(state.ry == 255);

    // 右スティックを別の値で上書き
    REQUIRE(ParseSerialLine("0x0001 8 FF 00", state) == true);
    REQUIRE(state.rx == 255);
    REQUIRE(state.ry == 0);
}

TEST_CASE("Boundary values are parsed correctly", "[parser]") {
    GamepadState state;
    ResetGamepadState(state);

    // 境界値 0, 128, 255
    REQUIRE(ParseSerialLine("0x0003 8 00 80 FF 00", state) == true);
    REQUIRE(state.lx == 0);
    REQUIRE(state.ly == 128);
    REQUIRE(state.rx == 255);
    REQUIRE(state.ry == 0);
}

TEST_CASE("Multiple buttons are ORed correctly", "[parser]") {
    GamepadState state;
    ResetGamepadState(state);

    // A(0x0010) + B(0x0008) = 0x0018、スティックなし
    // パーサーはシフト後の値を返す: 0x0018 >> 2 = 0x0006
    REQUIRE(ParseSerialLine("0x0018 8", state) == true);
    REQUIRE(state.buttons == 0x0006);
}

TEST_CASE("Empty string returns false", "[parser][error]") {
    GamepadState state;
    ResetGamepadState(state);

    REQUIRE(ParseSerialLine("", state) == false);
}

TEST_CASE("Non-numeric start returns false", "[parser][error]") {
    GamepadState state;
    ResetGamepadState(state);

    REQUIRE(ParseSerialLine("hello world", state) == false);
}

TEST_CASE("Leading whitespace is skipped", "[parser][robustness]") {
    GamepadState state;
    ResetGamepadState(state);

    REQUIRE(ParseSerialLine("  0x0010 8", state) == true);
    REQUIRE(state.buttons == 0x0004);
    REQUIRE(state.hat == 8);
}

TEST_CASE("Leading tab is skipped", "[parser][robustness]") {
    GamepadState state;
    ResetGamepadState(state);

    REQUIRE(ParseSerialLine("\t0x0010 8", state) == true);
    REQUIRE(state.buttons == 0x0004);
    REQUIRE(state.hat == 8);
}

TEST_CASE("Multiple spaces between tokens", "[parser][robustness]") {
    GamepadState state;
    ResetGamepadState(state);

    REQUIRE(ParseSerialLine("0x0012   8   FF   80", state) == true);
    REQUIRE(state.buttons == 0x0004);
    REQUIRE(state.hat == 8);
    REQUIRE(state.lx == 255);
    REQUIRE(state.ly == 128);
}

TEST_CASE("Mixed tabs and spaces between tokens", "[parser][robustness]") {
    GamepadState state;
    ResetGamepadState(state);

    REQUIRE(ParseSerialLine("0x0003\t8\t00\t80\tFF\t00", state) == true);
    REQUIRE(state.lx == 0);
    REQUIRE(state.ly == 128);
    REQUIRE(state.rx == 255);
    REQUIRE(state.ry == 0);
}

TEST_CASE("Invalid hex characters stop parsing for that token", "[parser][robustness]") {
    GamepadState state;
    ResetGamepadState(state);

    // "0x1G" should parse as 0x1 (stops at 'G'), then hat = 0
    REQUIRE(ParseSerialLine("0x1G 0", state) == true);
    REQUIRE(state.buttons == 0);  // 0x1 >> 2 = 0
    REQUIRE(state.hat == 0);
}

TEST_CASE("Empty line with only whitespace returns false", "[parser][error]") {
    GamepadState state;
    ResetGamepadState(state);

    REQUIRE(ParseSerialLine("   ", state) == false);
    REQUIRE(ParseSerialLine("\t\t", state) == false);
}

// ============================================================================
// Exhaustive button combination tests (all 2^14 = 16384 patterns)
// ============================================================================

TEST_CASE("Exhaustive button combinations (all 2^14 patterns)", "[parser][buttons][exhaustive]") {
    for (uint16_t buttons = 0; buttons < 16384; ++buttons) {
        GamepadState state;
        ResetGamepadState(state);

        // スティックフラグなしで全ボタン組み合わせをテスト
        // input = buttons << 2 (最下位2ビットはスティックフラグ)
        uint16_t input_val = buttons << 2;
        char line[32];
        snprintf(line, sizeof(line), "0x%04X 8", input_val);

        CAPTURE(line);
        REQUIRE(ParseSerialLine(line, state) == true);
        REQUIRE(state.buttons == buttons);
        REQUIRE(state.hat == 8);
        REQUIRE(state.lx == 128);
        REQUIRE(state.ly == 128);
        REQUIRE(state.rx == 128);
        REQUIRE(state.ry == 128);
    }
}

TEST_CASE("Exhaustive button combinations with left stick flag", "[parser][buttons][exhaustive]") {
    for (uint16_t buttons = 0; buttons < 16384; ++buttons) {
        GamepadState state;
        ResetGamepadState(state);

        // 左スティックフラグ(0x0002)付き
        uint16_t input_val = (buttons << 2) | 0x0002;
        char line[64];
        snprintf(line, sizeof(line), "0x%04X 8 80 80", input_val);

        CAPTURE(line);
        REQUIRE(ParseSerialLine(line, state) == true);
        REQUIRE(state.buttons == buttons);
        REQUIRE(state.hat == 8);
        REQUIRE(state.lx == 128);
        REQUIRE(state.ly == 128);
        REQUIRE(state.rx == 128);
        REQUIRE(state.ry == 128);
    }
}

TEST_CASE("Exhaustive button combinations with right stick flag", "[parser][buttons][exhaustive]") {
    for (uint16_t buttons = 0; buttons < 16384; ++buttons) {
        GamepadState state;
        ResetGamepadState(state);

        // 右スティックフラグ(0x0001)付き
        uint16_t input_val = (buttons << 2) | 0x0001;
        char line[64];
        snprintf(line, sizeof(line), "0x%04X 8 80 80", input_val);

        CAPTURE(line);
        REQUIRE(ParseSerialLine(line, state) == true);
        REQUIRE(state.buttons == buttons);
        REQUIRE(state.hat == 8);
        REQUIRE(state.lx == 128);
        REQUIRE(state.ly == 128);
        REQUIRE(state.rx == 128);
        REQUIRE(state.ry == 128);
    }
}

TEST_CASE("Exhaustive button combinations with both stick flags", "[parser][buttons][exhaustive]") {
    for (uint16_t buttons = 0; buttons < 16384; ++buttons) {
        GamepadState state;
        ResetGamepadState(state);

        // 両スティックフラグ(0x0003)付き
        uint16_t input_val = (buttons << 2) | 0x0003;
        char line[64];
        snprintf(line, sizeof(line), "0x%04X 8 80 80 80 80", input_val);

        CAPTURE(line);
        REQUIRE(ParseSerialLine(line, state) == true);
        REQUIRE(state.buttons == buttons);
        REQUIRE(state.hat == 8);
        REQUIRE(state.lx == 128);
        REQUIRE(state.ly == 128);
        REQUIRE(state.rx == 128);
        REQUIRE(state.ry == 128);
    }
}

// ============================================================================
// Stick state transition tests (stateful behavior across sequential commands)
// ============================================================================

TEST_CASE("Left stick persists through button-only commands", "[parser][stateful][transition]") {
    GamepadState state;
    ResetGamepadState(state);

    // Step 1: Set left stick to (0, 0)
    REQUIRE(ParseSerialLine("0x0002 8 00 00", state) == true);
    REQUIRE(state.lx == 0);
    REQUIRE(state.ly == 0);
    REQUIRE(state.rx == 128);
    REQUIRE(state.ry == 128);

    // Step 2: Button-only command — left stick must persist
    REQUIRE(ParseSerialLine("0x0004 8", state) == true);
    REQUIRE(state.lx == 0);
    REQUIRE(state.ly == 0);
    REQUIRE(state.rx == 128);
    REQUIRE(state.ry == 128);

    // Step 3: Another button-only command — left stick still persists
    REQUIRE(ParseSerialLine("0x0010 8", state) == true);
    REQUIRE(state.lx == 0);
    REQUIRE(state.ly == 0);
}

TEST_CASE("Right stick persists through button-only commands", "[parser][stateful][transition]") {
    GamepadState state;
    ResetGamepadState(state);

    // Step 1: Set right stick to (255, 255)
    REQUIRE(ParseSerialLine("0x0001 8 FF FF", state) == true);
    REQUIRE(state.rx == 255);
    REQUIRE(state.ry == 255);

    // Step 2: Button-only command — right stick must persist
    REQUIRE(ParseSerialLine("0x0004 8", state) == true);
    REQUIRE(state.rx == 255);
    REQUIRE(state.ry == 255);

    // Step 3: Another button-only command — right stick still persists
    REQUIRE(ParseSerialLine("0x0010 8", state) == true);
    REQUIRE(state.rx == 255);
    REQUIRE(state.ry == 255);
}

TEST_CASE("Left and right sticks are independent", "[parser][stateful][transition]") {
    GamepadState state;
    ResetGamepadState(state);

    // Step 1: Set left stick only
    REQUIRE(ParseSerialLine("0x0002 8 00 FF", state) == true);
    REQUIRE(state.lx == 0);
    REQUIRE(state.ly == 255);
    REQUIRE(state.rx == 128);
    REQUIRE(state.ry == 128);

    // Step 2: Set right stick only — left stick must not change
    REQUIRE(ParseSerialLine("0x0001 8 FF 00", state) == true);
    REQUIRE(state.lx == 0);   // preserved
    REQUIRE(state.ly == 255); // preserved
    REQUIRE(state.rx == 255);
    REQUIRE(state.ry == 0);

    // Step 3: Update left stick again — right stick must not change
    REQUIRE(ParseSerialLine("0x0002 8 80 80", state) == true);
    REQUIRE(state.lx == 128);
    REQUIRE(state.ly == 128);
    REQUIRE(state.rx == 255); // preserved
    REQUIRE(state.ry == 0);   // preserved
}

TEST_CASE("Stick values transition through multiple changes", "[parser][stateful][transition]") {
    GamepadState state;
    ResetGamepadState(state);

    // Left stick: 128(center) → 0(min) → 255(max) → 128(center)
    REQUIRE(ParseSerialLine("0x0002 8 80 80", state) == true);
    REQUIRE(state.lx == 128);
    REQUIRE(state.ly == 128);

    REQUIRE(ParseSerialLine("0x0002 8 00 00", state) == true);
    REQUIRE(state.lx == 0);
    REQUIRE(state.ly == 0);

    REQUIRE(ParseSerialLine("0x0002 8 FF FF", state) == true);
    REQUIRE(state.lx == 255);
    REQUIRE(state.ly == 255);

    REQUIRE(ParseSerialLine("0x0002 8 80 80", state) == true);
    REQUIRE(state.lx == 128);
    REQUIRE(state.ly == 128);

    // Right stick: 128(center) → 255(max) → 0(min) → 128(center)
    REQUIRE(ParseSerialLine("0x0001 8 FF FF", state) == true);
    REQUIRE(state.rx == 255);
    REQUIRE(state.ry == 255);

    REQUIRE(ParseSerialLine("0x0001 8 00 00", state) == true);
    REQUIRE(state.rx == 0);
    REQUIRE(state.ry == 0);

    REQUIRE(ParseSerialLine("0x0001 8 80 80", state) == true);
    REQUIRE(state.rx == 128);
    REQUIRE(state.ry == 128);
}

TEST_CASE("Both sticks then one stick preserves the other", "[parser][stateful][transition]") {
    GamepadState state;
    ResetGamepadState(state);

    // Step 1: Set both sticks
    REQUIRE(ParseSerialLine("0x0003 8 00 FF FF 00", state) == true);
    REQUIRE(state.lx == 0);
    REQUIRE(state.ly == 255);
    REQUIRE(state.rx == 255);
    REQUIRE(state.ry == 0);

    // Step 2: Update left stick only — right stick must persist
    REQUIRE(ParseSerialLine("0x0002 8 80 80", state) == true);
    REQUIRE(state.lx == 128);
    REQUIRE(state.ly == 128);
    REQUIRE(state.rx == 255); // preserved
    REQUIRE(state.ry == 0);   // preserved

    // Step 3: Update right stick only — left stick must persist
    REQUIRE(ParseSerialLine("0x0001 8 00 00", state) == true);
    REQUIRE(state.lx == 128); // preserved
    REQUIRE(state.ly == 128); // preserved
    REQUIRE(state.rx == 0);
    REQUIRE(state.ry == 0);
}

TEST_CASE("Complex transition: buttons and sticks mixed", "[parser][stateful][transition]") {
    GamepadState state;
    ResetGamepadState(state);

    // Sequence: left stick → buttons → right stick → buttons → both sticks
    REQUIRE(ParseSerialLine("0x0002 8 00 80", state) == true);
    REQUIRE(state.buttons == 0);
    REQUIRE(state.lx == 0);
    REQUIRE(state.ly == 128);

    REQUIRE(ParseSerialLine("0x0004 8", state) == true);
    REQUIRE(state.buttons == 1); // Y button
    REQUIRE(state.lx == 0);      // preserved
    REQUIRE(state.ly == 128);    // preserved

    REQUIRE(ParseSerialLine("0x0001 8 FF 00", state) == true);
    REQUIRE(state.buttons == 0); // cleared
    REQUIRE(state.lx == 0);      // preserved
    REQUIRE(state.ly == 128);    // preserved
    REQUIRE(state.rx == 255);
    REQUIRE(state.ry == 0);

    REQUIRE(ParseSerialLine("0x0010 8", state) == true);
    REQUIRE(state.buttons == 4); // A button
    REQUIRE(state.rx == 255);    // preserved
    REQUIRE(state.ry == 0);      // preserved

    REQUIRE(ParseSerialLine("0x0003 8 80 80 80 80", state) == true);
    REQUIRE(state.buttons == 0);
    REQUIRE(state.lx == 128);
    REQUIRE(state.ly == 128);
    REQUIRE(state.rx == 128);
    REQUIRE(state.ry == 128);
}

TEST_CASE("Buttons change but sticks persist across mixed commands", "[parser][stateful][transition]") {
    GamepadState state;
    ResetGamepadState(state);

    // Initialize with both sticks set
    REQUIRE(ParseSerialLine("0x0003 8 00 00 FF FF", state) == true);
    REQUIRE(state.lx == 0);
    REQUIRE(state.ly == 0);
    REQUIRE(state.rx == 255);
    REQUIRE(state.ry == 255);

    // Rapidly change buttons without touching sticks
    REQUIRE(ParseSerialLine("0x0004 8", state) == true);
    REQUIRE(state.buttons == 1);
    REQUIRE(state.lx == 0);
    REQUIRE(state.ly == 0);
    REQUIRE(state.rx == 255);
    REQUIRE(state.ry == 255);

    REQUIRE(ParseSerialLine("0x0008 8", state) == true);
    REQUIRE(state.buttons == 2);
    REQUIRE(state.lx == 0);
    REQUIRE(state.ly == 0);
    REQUIRE(state.rx == 255);
    REQUIRE(state.ry == 255);

    REQUIRE(ParseSerialLine("0x0010 8", state) == true);
    REQUIRE(state.buttons == 4);
    REQUIRE(state.lx == 0);
    REQUIRE(state.ly == 0);
    REQUIRE(state.rx == 255);
    REQUIRE(state.ry == 255);

    REQUIRE(ParseSerialLine("0x0018 8", state) == true);
    REQUIRE(state.buttons == 6);
    REQUIRE(state.lx == 0);
    REQUIRE(state.ly == 0);
    REQUIRE(state.rx == 255);
    REQUIRE(state.ry == 255);
}

TEST_CASE("Boundary value transitions for sticks", "[parser][stateful][transition]") {
    GamepadState state;
    ResetGamepadState(state);

    // 0 → 255 → 128 on left stick
    REQUIRE(ParseSerialLine("0x0002 8 00 00", state) == true);
    REQUIRE(state.lx == 0);
    REQUIRE(state.ly == 0);

    REQUIRE(ParseSerialLine("0x0002 8 FF FF", state) == true);
    REQUIRE(state.lx == 255);
    REQUIRE(state.ly == 255);

    REQUIRE(ParseSerialLine("0x0002 8 80 80", state) == true);
    REQUIRE(state.lx == 128);
    REQUIRE(state.ly == 128);

    // 255 → 0 → 128 on right stick
    REQUIRE(ParseSerialLine("0x0001 8 FF FF", state) == true);
    REQUIRE(state.rx == 255);
    REQUIRE(state.ry == 255);

    REQUIRE(ParseSerialLine("0x0001 8 00 00", state) == true);
    REQUIRE(state.rx == 0);
    REQUIRE(state.ry == 0);

    REQUIRE(ParseSerialLine("0x0001 8 80 80", state) == true);
    REQUIRE(state.rx == 128);
    REQUIRE(state.ry == 128);
}

TEST_CASE("Left stick X transitions through all values 0..255", "[parser][stateful][transition][exhaustive]") {
    GamepadState state;
    ResetGamepadState(state);

    for (int x = 0; x <= 255; ++x) {
        char line[32];
        snprintf(line, sizeof(line), "0x0002 8 %02X 80", x);
        CAPTURE(line);
        REQUIRE(ParseSerialLine(line, state) == true);
        REQUIRE(state.lx == x);
        REQUIRE(state.ly == 128);  // unchanged
    }
}

TEST_CASE("Left stick Y transitions through all values 0..255", "[parser][stateful][transition][exhaustive]") {
    GamepadState state;
    ResetGamepadState(state);

    for (int y = 0; y <= 255; ++y) {
        char line[32];
        snprintf(line, sizeof(line), "0x0002 8 80 %02X", y);
        CAPTURE(line);
        REQUIRE(ParseSerialLine(line, state) == true);
        REQUIRE(state.lx == 128);  // unchanged
        REQUIRE(state.ly == y);
    }
}

TEST_CASE("Right stick X transitions through all values 0..255", "[parser][stateful][transition][exhaustive]") {
    GamepadState state;
    ResetGamepadState(state);

    for (int x = 0; x <= 255; ++x) {
        char line[32];
        snprintf(line, sizeof(line), "0x0001 8 %02X 80", x);
        CAPTURE(line);
        REQUIRE(ParseSerialLine(line, state) == true);
        REQUIRE(state.rx == x);
        REQUIRE(state.ry == 128);  // unchanged
    }
}

TEST_CASE("Right stick Y transitions through all values 0..255", "[parser][stateful][transition][exhaustive]") {
    GamepadState state;
    ResetGamepadState(state);

    for (int y = 0; y <= 255; ++y) {
        char line[32];
        snprintf(line, sizeof(line), "0x0001 8 80 %02X", y);
        CAPTURE(line);
        REQUIRE(ParseSerialLine(line, state) == true);
        REQUIRE(state.rx == 128);  // unchanged
        REQUIRE(state.ry == y);
    }
}

TEST_CASE("Both sticks transition through all values 0..255 simultaneously", "[parser][stateful][transition][exhaustive]") {
    GamepadState state;
    ResetGamepadState(state);

    for (int v = 0; v <= 255; ++v) {
        char line[64];
        snprintf(line, sizeof(line), "0x0003 8 %02X %02X %02X %02X", v, v, v, v);
        CAPTURE(line);
        REQUIRE(ParseSerialLine(line, state) == true);
        REQUIRE(state.lx == v);
        REQUIRE(state.ly == v);
        REQUIRE(state.rx == v);
        REQUIRE(state.ry == v);
    }
}

// ============================================================================
// HAT direction tests
// ============================================================================

TEST_CASE("All HAT directions", "[parser][hat]") {
    GamepadState state; ResetGamepadState(state);

    REQUIRE(ParseSerialLine("0x0000 0", state) == true);   // TOP
    REQUIRE(state.hat == 0);

    REQUIRE(ParseSerialLine("0x0000 1", state) == true);   // TOP_RIGHT
    REQUIRE(state.hat == 1);

    REQUIRE(ParseSerialLine("0x0000 2", state) == true);   // RIGHT
    REQUIRE(state.hat == 2);

    REQUIRE(ParseSerialLine("0x0000 3", state) == true);   // BOTTOM_RIGHT
    REQUIRE(state.hat == 3);

    REQUIRE(ParseSerialLine("0x0000 4", state) == true);   // BOTTOM
    REQUIRE(state.hat == 4);

    REQUIRE(ParseSerialLine("0x0000 5", state) == true);   // BOTTOM_LEFT
    REQUIRE(state.hat == 5);

    REQUIRE(ParseSerialLine("0x0000 6", state) == true);   // LEFT
    REQUIRE(state.hat == 6);

    REQUIRE(ParseSerialLine("0x0000 7", state) == true);   // TOP_LEFT
    REQUIRE(state.hat == 7);

    REQUIRE(ParseSerialLine("0x0000 8", state) == true);   // CENTER
    REQUIRE(state.hat == 8);
}

// ============================================================================
// JSON test vectors
// ============================================================================

TEST_CASE("JSON test vectors can be loaded and verified", "[parser][json]") {
    std::filesystem::path vectors_dir = std::filesystem::path(__FILE__).parent_path() / "vectors";
    
    if (!std::filesystem::exists(vectors_dir)) {
        SKIP("Test vectors directory not found: " << vectors_dir);
    }

    for (const auto& entry : std::filesystem::directory_iterator(vectors_dir)) {
        if (entry.path().extension() != ".json") continue;

        std::ifstream f(entry.path());
        REQUIRE(f.is_open());
        
        json j;
        REQUIRE_NOTHROW(f >> j);

        GamepadState state;
        ResetGamepadState(state);

        std::string input_line = j.value("input_line", "");
        REQUIRE(!input_line.empty());

        REQUIRE(ParseSerialLine(input_line.c_str(), state) == true);
        
        REQUIRE(state.buttons == j.value("expected_buttons", 0));
        REQUIRE(state.hat == j.value("expected_hat", 8));
        REQUIRE(state.lx == j.value("expected_lx", 128));
        REQUIRE(state.ly == j.value("expected_ly", 128));
        REQUIRE(state.rx == j.value("expected_rx", 128));
        REQUIRE(state.ry == j.value("expected_ry", 128));
    }
}
