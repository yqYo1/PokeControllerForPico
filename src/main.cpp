#include <stdio.h>
#include <cstring>
#include "pico/stdlib.h"
#include "tusb.h"

#ifdef USE_PIO_USB_TEST_ONLY
#include "pio_usb.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#else
#include "hardware/uart.h"
#include "PokeControllerForPico_Func.h"
#endif

#ifdef USE_PIO_USB_TEST_ONLY
// --- Standalone PIO USB HID Mouse Test (Correct Pins) ---

// Correct GPIO pins for the Waveshare RP2350-USB-A board
// D+ = GP12, D- = GP13
#define PIO_USB_DP_PIN 12

// --- USB Descriptors for a simple HID Mouse ---
// Device Descriptor
static tusb_desc_device_t const desc_device = {
    .bLength         = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB          = 0x0110,
    .bDeviceClass    = 0x00, // Device class specified in interface descriptor
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,
    .bMaxPacketSize0 = 64,
    .idVendor        = 0xCafe, // Test VID
    .idProduct       = 0x4002, // Test PID for Mouse
    .bcdDevice       = 0x0100,
    .iManufacturer   = 0x01,
    .iProduct        = 0x02,
    .iSerialNumber   = 0x03,
    .bNumConfigurations = 0x01
};

// HID Report Descriptor
static uint8_t const desc_hid_report[] = { TUD_HID_REPORT_DESC_MOUSE() };
static const uint8_t* pio_hid_reports[] = { desc_hid_report };

// Configuration Descriptor
#define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN)
static uint8_t const desc_cfg[] = {
    TUD_CONFIG_DESCRIPTOR(1, 1, 0, CONFIG_TOTAL_LEN, 0, 100),
    TUD_HID_DESCRIPTOR(0, 0, 0, sizeof(desc_hid_report), 0x81, 16, 10)
};

// String Descriptors
static const char* string_desc_arr[] = {
    [0] = (const char[]){0x09, 0x04},
    [1] = "Pico PIO USB",
    [2] = "PIO HID Mouse Test (Correct Pins)",
    [3] = "1234-HID-CORRECT"
};

// --- PIO USB Device Initialization ---
static usb_device_t *pio_usb_device = NULL;

static usb_descriptor_buffers_t pio_desc_buffers = {
    .device = (uint8_t*)&desc_device,
    .config = desc_cfg,
    .hid_report = pio_hid_reports,
    .string = (string_descriptor_t*)string_desc_arr,
};

int main(void) {
  set_sys_clock_khz(120000, true);
  stdio_init_all();
  printf("--- PIO USB HID Mouse Test (Correct Pins: D+=GP12) ---\n");

  // Manually enable the D+ pull-up resistor on the correct pin.
  gpio_pull_up(PIO_USB_DP_PIN);

  // Initialize the PIO USB stack with the correct pin configuration.
  // The library default is DPDM (DM = DP+1), so setting DP to 12 automatically sets DM to 13.
  static pio_usb_configuration_t pio_cfg = PIO_USB_DEFAULT_CONFIG;
  pio_cfg.pin_dp = PIO_USB_DP_PIN;
  pio_usb_device = pio_usb_device_init(&pio_cfg, &pio_desc_buffers);

  while (true) {
    pio_usb_device_task();
    sleep_ms(500);

    endpoint_t* ep = pio_usb_get_endpoint(pio_usb_device, 0x81);
    if (ep) {
        hid_mouse_report_t mouse_report = {0};
        mouse_report.x = 5;
        pio_usb_set_out_data(ep, (uint8_t*)&mouse_report, sizeof(mouse_report));
    }
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