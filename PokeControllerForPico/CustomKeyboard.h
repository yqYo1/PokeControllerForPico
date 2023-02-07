#pragma once

#define HID_CUSTOM_LAYOUT
#define LAYOUT_JAPANESE
#include "Arduino.h"
#include "stdint.h"
#include "ImprovedKeyLayouts.h"
#include "usb_descriptors.h"

size_t key_press(uint8_t c);
size_t key_release(uint8_t c);
size_t key_write(uint8_t c);
size_t specialkey_press(uint8_t c);
size_t specialkey_release(uint8_t c);
size_t specialkey_write(uint8_t c);
void key_releaseAll(void);
