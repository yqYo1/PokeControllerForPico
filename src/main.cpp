#include <stdio.h>
#include <cstring>
#include "pico/stdlib.h"
#include "tusb.h"

#ifdef USE_PIO_USB_TEST_ONLY
#include "pio_usb.h"
#include "hardware/clocks.h"
#else
#include "hardware/uart.h"
#include "PokeControllerForPico_Func.h"
#endif

#ifdef USE_PIO_USB_TEST_ONLY
// --- Standalone PIO USB CDC Echo Test ---

// Manually define descriptor constants to avoid conflicts
#define PIO_USB_VID   0x2E8A // Raspberry Pi
#define PIO_USB_PID   0x000a // Raspberry Pi Pico SDK CDC

// CDC Descriptor definitions
#define EPNUM_CDC_NOTIF 0x81
#define EPNUM_CDC_OUT   0x02
#define EPNUM_CDC_IN    0x82

// Device Descriptor
static tusb_desc_device_t const desc_device = {
    .bLength         = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB          = 0x0110,
    .bDeviceClass    = TUSB_CLASS_CDC,
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,
    .bMaxPacketSize0 = 64,
    .idVendor        = PIO_USB_VID,
    .idProduct       = PIO_USB_PID,
    .bcdDevice       = 0x0100,
    .iManufacturer   = 0x01,
    .iProduct        = 0x02,
    .iSerialNumber   = 0x03,
    .bNumConfigurations = 0x01
};

// Configuration Descriptor
#define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_CDC_DESC_LEN)
static uint8_t const desc_cfg[] = {
    TUD_CONFIG_DESCRIPTOR(1, 2, 0, CONFIG_TOTAL_LEN, 0, 100),
    TUD_CDC_DESCRIPTOR(0, 0, EPNUM_CDC_NOTIF, 8, EPNUM_CDC_OUT, EPNUM_CDC_IN, 64)
};

// String Descriptors
static const char* string_desc_arr[] = {
    [0] = (const char[]){0x09, 0x04}, // 0: is supported language is English (0x0409)
    [1] = "Pico PIO USB",
    [2] = "PIO CDC Serial Test",
    [3] = "1234-TEST"
};

// --- PIO USB Device Initialization ---
static usb_device_t *pio_usb_device = NULL;

static usb_descriptor_buffers_t pio_desc_buffers = {
    .device = (uint8_t*)&desc_device,
    .config = desc_cfg,
    .hid_report = NULL,
    .string = (string_descriptor_t*)string_desc_arr,
};

int main(void) {
  // Required for PIO USB
  set_sys_clock_khz(120000, true);
  stdio_init_all();
  printf("--- PIO USB CDC Echo Test ---\n");

  // Initialize the PIO USB stack in DEVICE mode
  static pio_usb_configuration_t pio_cfg = PIO_USB_DEFAULT_CONFIG;
  pio_usb_device = pio_usb_device_init(&pio_cfg, &pio_desc_buffers);

  // Main echo loop
  while (true) {
    pio_usb_device_task();

    endpoint_t* ep_out = pio_usb_get_endpoint(pio_usb_device, EPNUM_CDC_OUT);
    uint8_t rx_buf[64];
    int len = pio_usb_get_in_data(ep_out, rx_buf, sizeof(rx_buf));

    if (len > 0) {
      printf("Received %d bytes\n", len);
      // Echo back the received data
      endpoint_t* ep_in = pio_usb_get_endpoint(pio_usb_device, EPNUM_CDC_IN);
      pio_usb_set_out_data(ep_in, rx_buf, len);
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