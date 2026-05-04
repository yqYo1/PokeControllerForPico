#pragma once
#include <cstdint>

namespace pokecon {

struct GamepadState {
    uint16_t buttons;  // 2ビットシフト後のボタン値
    uint8_t  hat;
    uint8_t  lx, ly;   // 左スティック
    uint8_t  rx, ry;   // 右スティック
};

// 状態を初期化（デフォルト位置）
void ResetGamepadState(GamepadState& state);

// シリアル文字列をパースして GamepadState を更新
// line: 入力文字列（例: "0x0012 8 FF 80" または "0x0001 2 80 FF" 等）
// state: 前回の状態。スティックフラグなし時にスティック値が維持される
// 戻り値: パース成功時 true
bool ParseSerialLine(const char* line, GamepadState& state);

} // namespace pokecon
