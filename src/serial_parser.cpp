#include "serial_parser.h"
#include <cstring>

namespace pokecon {

void ResetGamepadState(GamepadState& state) {
    state.buttons = 0;
    state.hat = 8;   // HAT_CENTER
    state.lx = 128;  // STICK_CENTER
    state.ly = 128;
    state.rx = 128;
    state.ry = 128;
}

// Skip leading whitespace (spaces and tabs)
static void skipWhitespace(const char* line, size_t& pos) {
    while (line[pos] == ' ' || line[pos] == '\t') pos++;
}

// Parse a single hex byte token. pos is updated to the next token.
// Handles optional "0x"/"0X" prefix, multiple spaces, and invalid characters.
static uint8_t parseHexByte(const char* line, size_t& pos) {
    skipWhitespace(line, pos);

    // Skip optional 0x / 0X prefix
    if (line[pos] == '0' && (line[pos + 1] == 'x' || line[pos + 1] == 'X')) {
        pos += 2;
    }

    uint8_t value = 0;
    while (line[pos] != '\0' && line[pos] != ' ' && line[pos] != '\t' && line[pos] != '\r' && line[pos] != '\n') {
        uint8_t digit;
        if (line[pos] >= '0' && line[pos] <= '9') {
            digit = static_cast<uint8_t>(line[pos] - '0');
        } else if (line[pos] >= 'A' && line[pos] <= 'F') {
            digit = static_cast<uint8_t>(line[pos] - 'A' + 10);
        } else if (line[pos] >= 'a' && line[pos] <= 'f') {
            digit = static_cast<uint8_t>(line[pos] - 'a' + 10);
        } else {
            // Invalid character — stop parsing this token
            break;
        }
        value = static_cast<uint8_t>((value * 16) + digit);
        pos++;
    }

    skipWhitespace(line, pos);
    return value;
}

// Parse a multi-digit hex word token (for buttons).
static uint16_t parseHexWord(const char* line, size_t& pos) {
    skipWhitespace(line, pos);

    // Skip optional 0x / 0X prefix
    if (line[pos] == '0' && (line[pos + 1] == 'x' || line[pos + 1] == 'X')) {
        pos += 2;
    }

    uint16_t value = 0;
    while (line[pos] != '\0' && line[pos] != ' ' && line[pos] != '\t' && line[pos] != '\r' && line[pos] != '\n') {
        uint16_t digit;
        if (line[pos] >= '0' && line[pos] <= '9') {
            digit = static_cast<uint16_t>(line[pos] - '0');
        } else if (line[pos] >= 'A' && line[pos] <= 'F') {
            digit = static_cast<uint16_t>(line[pos] - 'A' + 10);
        } else if (line[pos] >= 'a' && line[pos] <= 'f') {
            digit = static_cast<uint16_t>(line[pos] - 'a' + 10);
        } else {
            // Invalid character — stop parsing this token
            break;
        }
        value = static_cast<uint16_t>((value * 16) + digit);
        pos++;
    }

    skipWhitespace(line, pos);
    return value;
}

bool ParseSerialLine(const char* line, GamepadState& state) {
    if (line == nullptr || line[0] == '\0') return false;

    size_t pos = 0;
    skipWhitespace(line, pos);

    // After skipping whitespace, the line must not be empty and must start
    // with a digit (handles "0x..." format which is the protocol standard).
    if (line[pos] == '\0' || !(line[pos] >= '0' && line[pos] <= '9')) return false;

    // Buttons + stick flags
    uint16_t p_btns = parseHexWord(line, pos);

    // Hat
    uint8_t p_hat = parseHexByte(line, pos);

    // Read stick values if present in the string
    uint8_t val1 = state.lx, val2 = state.ly, val3 = state.rx, val4 = state.ry;

    if (line[pos] != '\r' && line[pos] != '\n' && line[pos] != '\0') {
        val1 = parseHexByte(line, pos);
    }
    if (line[pos] != '\r' && line[pos] != '\n' && line[pos] != '\0') {
        val2 = parseHexByte(line, pos);
    }
    if (line[pos] != '\r' && line[pos] != '\n' && line[pos] != '\0') {
        val3 = parseHexByte(line, pos);
    }
    if (line[pos] != '\r' && line[pos] != '\n' && line[pos] != '\0') {
        val4 = parseHexByte(line, pos);
    }

    // Stick flags
    bool use_right = (p_btns & 0x0001) != 0;
    bool use_left  = (p_btns & 0x0002) != 0;

    // Hat update
    state.hat = p_hat;

    // Left stick update
    if (use_left) {
        state.lx = val1;
        state.ly = val2;
    }

    // Right stick update
    // Note: original code used bitwise AND (use_right & use_left), but
    // logical AND is the intended behavior.
    if (use_right && use_left) {
        state.rx = val3;
        state.ry = val4;
    } else if (use_right) {
        // Original code assigned pc_lx/pc_ly here. Protocol-wise this means
        // "right stick only" uses the 3rd/4th token values (val1, val2).
        state.rx = val1;
        state.ry = val2;
    }

    // Buttons are shifted by 2 bits
    state.buttons = p_btns >> 2;

    return true;
}

} // namespace pokecon
