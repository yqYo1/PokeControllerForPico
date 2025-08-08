#include "CustomKeyboard.h"

typedef struct
{
  uint8_t modifiers;
  uint8_t reserved;
  uint8_t keys[6];
} KeyReport;

static KeyReport _keyReport;
static void sendReport(KeyReport* keys);

size_t key_press(uint8_t c)
{
  uint8_t i;
  uint16_t k = _asciimap[c];
  uint16_t m = k >> 8;
  uint8_t M = 0;
  if (m & (MOD_LEFT_CTRL >> 8)) M |= (1 << 0);
  if (m & (MOD_LEFT_SHIFT >> 8)) M |= (1 << 1);
  if (m & (MOD_LEFT_ALT >> 8)) M |= (1 << 2);
  if (m & (MOD_LEFT_GUI >> 8)) M |= (1 << 3);
  if (m & (MOD_RIGHT_CTRL >> 8)) M |= (1 << 4);
  if (m & (MOD_RIGHT_SHIFT >> 8)) M |= (1 << 5);
  if (m & (MOD_RIGHT_ALT >> 8)) M |= (1 << 6);
  if (m & (MOD_RIGHT_GUI >> 8)) M |= (1 << 7);
  _keyReport.modifiers |= M;
  c = (uint8_t)(k & 0xFF);

	// Add k to the key report only if it's not already present
	// and if there is an empty slot.
	if ((_keyReport.keys[0] != c) && (_keyReport.keys[1] != c) &&
		  (_keyReport.keys[2] != c) && (_keyReport.keys[3] != c) &&
		  (_keyReport.keys[4] != c) && (_keyReport.keys[5] != c)) {

    for (i=0; i<6; i++) {
			if (_keyReport.keys[i] == 0x00) {
				_keyReport.keys[i] = c;
				break;
			}
		}
		if (i == 6) {
			//setWriteError();
			return 0;
		}
	}
	sendReport(&_keyReport);
	return 1;
}

size_t key_release(uint8_t c)
{
  uint8_t i;
  uint16_t k = _asciimap[c];
  uint16_t m = k >> 8;
  uint8_t M = 0;
  if (m & (MOD_LEFT_CTRL >> 8)) M |= (1 << 0);
  if (m & (MOD_LEFT_SHIFT >> 8)) M |= (1 << 1);
  if (m & (MOD_LEFT_ALT >> 8)) M |= (1 << 2);
  if (m & (MOD_LEFT_GUI >> 8)) M |= (1 << 3);
  if (m & (MOD_RIGHT_CTRL >> 8)) M |= (1 << 4);
  if (m & (MOD_RIGHT_SHIFT >> 8)) M |= (1 << 5);
  if (m & (MOD_RIGHT_ALT >> 8)) M |= (1 << 6);
  if (m & (MOD_RIGHT_GUI >> 8)) M |= (1 << 7);
  _keyReport.modifiers &= ~M;
  c = (uint8_t)(k & 0xFF);

	for (i=0; i<6; i++) {
		if (0 != c && _keyReport.keys[i] == c) {
			_keyReport.keys[i] = 0x00;
		}
	}

	sendReport(&_keyReport);
	return 1;
}

size_t specialkey_press(uint8_t c)
{
  if ((c != KEY_JP_HANZEN)   && (c != KEY_JP_BACKSLASH) &&
      (c != KEY_JP_HIRAGANA) && (c != KEY_JP_YEN)       &&
      (c != KEY_JP_HENKAN)   && (c != KEY_JP_MUHENKAN)  &&
      (c != KEY_ENTER)       && (c != KEY_BACKSPACE)    &&
      (c != KEY_DELETE)      && (c != KEY_UP_ARROW)     &&
      (c != KEY_DOWN_ARROW)  && (c != KEY_LEFT_ARROW)   &&
      (c != KEY_RIGHT_ARROW)) {
    uint16_t k = _asciimap[c];
    uint16_t m = k >> 8;
    uint8_t M = 0;
    if (m & (MOD_LEFT_CTRL >> 8)) M |= (1 << 0);
    if (m & (MOD_LEFT_SHIFT >> 8)) M |= (1 << 1);
    if (m & (MOD_LEFT_ALT >> 8)) M |= (1 << 2);
    if (m & (MOD_LEFT_GUI >> 8)) M |= (1 << 3);
    if (m & (MOD_RIGHT_CTRL >> 8)) M |= (1 << 4);
    if (m & (MOD_RIGHT_SHIFT >> 8)) M |= (1 << 5);
    if (m & (MOD_RIGHT_ALT >> 8)) M |= (1 << 6);
    if (m & (MOD_RIGHT_GUI >> 8)) M |= (1 << 7);
    _keyReport.modifiers |= M;
    c = (uint8_t)(k & 0xFF);
  }
  uint8_t i;
	if ((_keyReport.keys[0] != c) && (_keyReport.keys[1] != c) &&
		  (_keyReport.keys[2] != c) && (_keyReport.keys[3] != c) &&
		  (_keyReport.keys[4] != c) && (_keyReport.keys[5] != c)) {

    for (i=0; i<6; i++) {
			if (_keyReport.keys[i] == 0x00) {
				_keyReport.keys[i] = c;
				break;
			}
		}
		if (i == 6) {
			//setWriteError();
			return 0;
		}
	}
	sendReport(&_keyReport);
	return 1;
}

size_t specialkey_release(uint8_t c)
{
  if ((c != KEY_JP_HANZEN)   && (c != KEY_JP_BACKSLASH) &&
      (c != KEY_JP_HIRAGANA) && (c != KEY_JP_YEN)       &&
      (c != KEY_JP_HENKAN)   && (c != KEY_JP_MUHENKAN)  &&
      (c != KEY_ENTER)       && (c != KEY_BACKSPACE)    &&
      (c != KEY_DELETE)      && (c != KEY_UP_ARROW)     &&
      (c != KEY_DOWN_ARROW)  && (c != KEY_LEFT_ARROW)   &&
      (c != KEY_RIGHT_ARROW)) {
    uint16_t k = _asciimap[c];
    uint16_t m = k >> 8;
    uint8_t M = 0;
    if (m & (MOD_LEFT_CTRL >> 8)) M |= (1 << 0);
    if (m & (MOD_LEFT_SHIFT >> 8)) M |= (1 << 1);
    if (m & (MOD_LEFT_ALT >> 8)) M |= (1 << 2);
    if (m & (MOD_LEFT_GUI >> 8)) M |= (1 << 3);
    if (m & (MOD_RIGHT_CTRL >> 8)) M |= (1 << 4);
    if (m & (MOD_RIGHT_SHIFT >> 8)) M |= (1 << 5);
    if (m & (MOD_RIGHT_ALT >> 8)) M |= (1 << 6);
    if (m & (MOD_RIGHT_GUI >> 8)) M |= (1 << 7);
    _keyReport.modifiers &= ~M;
    c = (uint8_t)(k & 0xFF);
  }
	uint8_t i;
	for (i=0; i<6; i++) {
		if (0 != c && _keyReport.keys[i] == c) {
			_keyReport.keys[i] = 0x00;
		}
	}

	sendReport(&_keyReport);
	return 1;
}

void key_releaseAll(void)
{
	_keyReport.keys[0] = 0;
	_keyReport.keys[1] = 0;
	_keyReport.keys[2] = 0;
	_keyReport.keys[3] = 0;
	_keyReport.keys[4] = 0;
	_keyReport.keys[5] = 0;
	_keyReport.modifiers = 0;
	sendReport(&_keyReport);
}

size_t key_write(uint8_t c)
{
	uint8_t p = key_press(c);
	delay(30);
	key_release(c);
	delay(30);
	return p;
}

size_t specialkey_write(uint8_t c)
{
	uint8_t p = specialkey_press(c);
	delay(30);
	specialkey_release(c);
	delay(30);
	return p;
}

static void sendReport(KeyReport* keys)
{
    tud_task();
    if (tud_hid_ready()) {
        tud_hid_n_keyboard_report(ITF_NUM_HID2, 0, keys->modifiers, keys->keys);
    }
    tud_task();
}
