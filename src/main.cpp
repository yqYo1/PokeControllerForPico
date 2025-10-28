#include <stdio.h>
#include <cstring>
#include "pico/stdlib.h"
#include "tusb.h"

#ifdef USE_PIO_USB_TEST_ONLY
#include "pio_usb.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "pico/multicore.h"
#else
#include "hardware/uart.h"
#include "PokeControllerForPico_Func.h"
#endif

#ifdef USE_PIO_USB_TEST_ONLY
// --- Standalone PIO USB HID Test (Based on Official Waveshare Example) ---

// Correct GPIO pins for the Waveshare RP2350-USB-A board
#define PIO_USB_DP_PIN 12

// --- USB Descriptors for a simple HID Mouse/Keyboard Device ---
static tusb_desc_device_t const desc_device = {
    .bLength         = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB          = 0x0110,
    .bDeviceClass    = 0x00,
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,
    .bMaxPacketSize0 = 64,
    .idVendor        = 0xCafe,
    .idProduct       = 0x4003, // PID for HID Test
    .bcdDevice       = 0x0100,
    .iManufacturer   = 0x01,
    .iProduct        = 0x02,
    .iSerialNumber   = 0x03,
    .bNumConfigurations = 0x01
};

enum {
  ITF_NUM_KEYBOARD,
  ITF_NUM_MOUSE,
  ITF_NUM_TOTAL,
};

enum {
  EPNUM_KEYBOARD = 0x81,
  EPNUM_MOUSE = 0x82,
};

static uint8_t const desc_hid_keyboard_report[] = { TUD_HID_REPORT_DESC_KEYBOARD() };
static uint8_t const desc_hid_mouse_report[] = { TUD_HID_REPORT_DESC_MOUSE() };

static const uint8_t* pio_hid_reports[] = { desc_hid_keyboard_report, desc_hid_mouse_report };

#define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + 2*TUD_HID_DESC_LEN)
static uint8_t const desc_cfg[] = {
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),
    TUD_HID_DESCRIPTOR(ITF_NUM_KEYBOARD, 0, HID_ITF_PROTOCOL_KEYBOARD, sizeof(desc_hid_keyboard_report), EPNUM_KEYBOARD, CFG_TUD_HID_EP_BUFSIZE, 10),
    TUD_HID_DESCRIPTOR(ITF_NUM_MOUSE, 0, HID_ITF_PROTOCOL_MOUSE, sizeof(desc_hid_mouse_report), EPNUM_MOUSE, CFG_TUD_HID_EP_BUFSIZE, 10),
};

static const char* string_descriptors_base[] = {
    [0] = (const char[]){0x09, 0x04},
    [1] = "PIO USB HID Test",
    [2] = "Keyboard and Mouse",
    [3] = "1234-HID-WAVESHARE"
};

static string_descriptor_t str_desc[4];

// Correct string descriptor initialization from the example
static void init_string_desc(void) {
  for (int idx = 0; idx < 4; idx++) {
    uint8_t len = 0;
    uint16_t *wchar_str = (uint16_t *)&str_desc[idx];
    if (idx == 0) {
      wchar_str[1] = string_descriptors_base[0][0] | ((uint16_t)string_descriptors_base[0][1] << 8);
      len = 1;
    } else if (idx <= 3) {
      len = strnlen(string_descriptors_base[idx], 31);
      for (int i = 0; i < len; i++) {
        wchar_str[i + 1] = string_descriptors_base[idx][i];
      }
    } else {
      len = 0;
    }
    wchar_str[0] = (TUSB_DESC_STRING << 8) | (2 * len + 2);
  }
}

static usb_descriptor_buffers_t pio_desc_buffers = {
    .device = (uint8_t*)&desc_device,
    .config = desc_cfg,
    .hid_report = pio_hid_reports,
    .string = str_desc
};

static usb_device_t *pio_usb_device = NULL;

void core1_main() {
  sleep_ms(10);
  static pio_usb_configuration_t config = PIO_USB_DEFAULT_CONFIG;
  config.pin_dp = PIO_USB_DP_PIN;

  init_string_desc();
  pio_usb_device = pio_usb_device_init(&config, &pio_desc_buffers);

  while (true) {
    pio_usb_device_task();
  }
}

int main(void) {
  set_sys_clock_khz(120000, true);
  stdio_init_all();
  printf("--- PIO USB HID Test (Based on Official Example) ---\n");

  gpio_pull_up(PIO_USB_DP_PIN);

  multicore_reset_core1();
  multicore_launch_core1(core1_main);

  while (true) {
    if (pio_usb_device != NULL) {
      hid_mouse_report_t mouse_report = {0};
      mouse_report.x = 5;
      endpoint_t *ep = pio_usb_get_endpoint(pio_usb_device, EPNUM_MOUSE & 0x7f);
      if (ep) {
        pio_usb_set_out_data(ep, (uint8_t *)&mouse_report, sizeof(mouse_report));
      }
    }
    sleep_ms(1000);
  }
  return 0;
}

#else // --- Original Main Application ---

#define UART_ID uart0
#define BAUD_RATE 9600
#define UART_TX_PIN 0
#define UART_RX_PIN 1

int ProgState = STATE0;
static char pc_report_str[MAX_BUFFER];
static uint8_t idx = 0;

int main() {
  uart_init(UART_ID, BAUD_RATE);
  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

  Controller_Init();
  Keyboard_Init();
  Controller_Reset();

  while (1) {
    tud_task();
    if (uart_is_readable(UART_ID)) {
      char c = uart_getc(UART_ID);
      if (c == '\n') {
        pc_report_str[idx++] = c;
        pc_report_str[idx++] = '\0';
        ParseLine(pc_report_str);
        idx = 0;
        memset(pc_report_str, 0, sizeof(pc_report_str));
        ProgState = STATE1;
      } else if (idx < MAX_BUFFER) {
        pc_report_str[idx++] = c;
      }
    }

    switch (ProgState) {
      case STATE1:
        SwitchFunction();
        break;
      default:
        ProgState = STATE0;
        break;
    }
  }
  return 0;
}
#endif