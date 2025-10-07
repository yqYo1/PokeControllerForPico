#include <stdio.h>
#include <cstring>
#include "pico/stdlib.h"
#include "tusb.h"

#ifdef USE_DUAL_USB_DEVICE
#include "pico/multicore.h"
#include "pio_usb.h"
#include "hardware/clocks.h"
#else
#include "hardware/uart.h"
#endif

#include "PokeControllerForPico_Func.h"

#ifdef USE_DUAL_USB_DEVICE
//--------------------------------------------------------------------+
// PIO USB CDC Descriptors (Manually Defined for Independence)
//--------------------------------------------------------------------+
#define PIO_USB_VID   0x2E8A // Raspberry Pi
#define PIO_USB_PID   0x000a // Raspberry Pi Pico SDK CDC
#define PIO_USB_BCD   0x0100

// These values are from the USB CDC specification and TinyUSB's cdc.h
#define PIO_TUSB_CLASS_CDC                                0x02
#define PIO_CDC_COMM_SUBCLASS_ABSTRACT_CONTROL_MODEL      0x02
#define PIO_CDC_COMM_PROTOCOL_NONE                        0x00
#define PIO_TUSB_CLASS_CDC_DATA                           0x0A
#define PIO_CDC_FUNC_DESC_HEADER                          0x00
#define PIO_CDC_FUNC_DESC_CALL_MANAGEMENT                 0x01
#define PIO_CDC_FUNC_DESC_ABSTRACT_CONTROL_MANAGEMENT     0x02
#define PIO_CDC_FUNC_DESC_UNION                           0x06

static tusb_desc_device_t const pio_desc_device = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = PIO_USB_BCD,
    .bDeviceClass = PIO_TUSB_CLASS_CDC,
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,
    .bMaxPacketSize0 = 64,
    .idVendor = PIO_USB_VID,
    .idProduct = PIO_USB_PID,
    .bcdDevice = 0x0100,
    .iManufacturer = 0x01,
    .iProduct = 0x02,
    .iSerialNumber = 0x03,
    .bNumConfigurations = 0x01
};

#define EPNUM_CDC_NOTIF 0x81
#define EPNUM_CDC_OUT   0x02
#define EPNUM_CDC_IN    0x82
#define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_CDC_DESC_LEN)

static uint8_t const pio_desc_cfg[] = {
  // Config number, interface count, string index, total length, attribute, power in mA
  TUD_CONFIG_DESCRIPTOR(1, 2, 0, CONFIG_TOTAL_LEN, 0, 100),
  // IAD
  8, TUSB_DESC_INTERFACE_ASSOCIATION, 0, 2, PIO_TUSB_CLASS_CDC, PIO_CDC_COMM_SUBCLASS_ABSTRACT_CONTROL_MODEL, PIO_CDC_COMM_PROTOCOL_NONE, 0,
  // Communication Interface
  9, TUSB_DESC_INTERFACE, 0, 0, 1, PIO_TUSB_CLASS_CDC, PIO_CDC_COMM_SUBCLASS_ABSTRACT_CONTROL_MODEL, PIO_CDC_COMM_PROTOCOL_NONE, 0,
  // Header Functional
  5, TUSB_DESC_CS_INTERFACE, PIO_CDC_FUNC_DESC_HEADER, U16_TO_U8S_LE(0x0120),
  // Call Management Functional
  5, TUSB_DESC_CS_INTERFACE, PIO_CDC_FUNC_DESC_CALL_MANAGEMENT, 0, 1,
  // Abstract Control Management Functional
  4, TUSB_DESC_CS_INTERFACE, PIO_CDC_FUNC_DESC_ABSTRACT_CONTROL_MANAGEMENT, 2,
  // Union Functional
  5, TUSB_DESC_CS_INTERFACE, PIO_CDC_FUNC_DESC_UNION, 0, 1,
  // Notification Endpoint
  7, TUSB_DESC_ENDPOINT, EPNUM_CDC_NOTIF, TUSB_XFER_INTERRUPT, U16_TO_U8S_LE(8), 16,
  // Data Interface
  9, TUSB_DESC_INTERFACE, 1, 0, 2, PIO_TUSB_CLASS_CDC_DATA, 0, 0, 0,
  // Data Endpoint Out
  7, TUSB_DESC_ENDPOINT, EPNUM_CDC_OUT, TUSB_XFER_BULK, U16_TO_U8S_LE(64), 0,
  // Data Endpoint In
  7, TUSB_DESC_ENDPOINT, EPNUM_CDC_IN, TUSB_XFER_BULK, U16_TO_U8S_LE(64), 0
};

static const char *pio_string_descriptors[] = {
    [0] = (const char[]){0x09, 0x04},
    [1] = "Raspberry Pi",
    [2] = "Pico PIO USB Serial",
    [3] = "PIO-SERIAL",
};

static usb_device_t *pio_usb_device = NULL;

static usb_descriptor_buffers_t pio_desc_buffers = {
    .device = (uint8_t*)&pio_desc_device,
    .config = pio_desc_cfg,
    .hid_report = NULL,
    .string = (string_descriptor_t*)pio_string_descriptors
};

// Core 1 runs the PIO USB stack for the USB-A port
void core1_main() {
  sleep_ms(10);
  static pio_usb_configuration_t config = PIO_USB_DEFAULT_CONFIG;
  pio_usb_device = pio_usb_device_init(&config, &pio_desc_buffers);

  while (true) {
    pio_usb_device_task();
  }
}

#else
// Use UART0 for serial communication for non-PIO boards
#define UART_ID uart0
#define BAUD_RATE 9600
#define UART_TX_PIN 0
#define UART_RX_PIN 1
#endif

int ProgState = STATE0;
static char pc_report_str[MAX_BUFFER];
static uint8_t idx = 0;

int main() {
#ifdef USE_DUAL_USB_DEVICE
  // For PIO USB, the sysclock needs to be a multiple of 12MHz
  set_sys_clock_khz(120000, true);
  stdio_init_all();

  multicore_reset_core1();
  multicore_launch_core1(core1_main);
#else
  // Initialize UART for other boards
  uart_init(UART_ID, BAUD_RATE);
  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
#endif

  // Initialize TinyUSB for the native HID controller (USB-C)
  Controller_Init();
  Keyboard_Init();
  Controller_Reset();

  while (1) {
    // Run TinyUSB task for the native HID controller
    tud_task();

#ifdef USE_DUAL_USB_DEVICE
    uint8_t rx_buf[64];
    int len = pio_usb_get_in_data(pio_usb_get_endpoint(pio_usb_device, EPNUM_CDC_OUT), rx_buf, sizeof(rx_buf));
    if (len > 0) {
      for (int i = 0; i < len; i++) {
        char c = rx_buf[i];
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
    }
#else
    // Original UART receiving logic
    if (uart_is_readable(UART_ID)) {
        char c = uart_getc(UART_ID);
        if (c == '\n')
        {
            pc_report_str[idx++] = c;
            pc_report_str[idx++] = '\0';
            ParseLine(pc_report_str);
            idx = 0;
            memset(pc_report_str, 0, sizeof(pc_report_str));
            ProgState = STATE1;
        }
        else if (idx < MAX_BUFFER)
        {
            pc_report_str[idx++] = c;
        }
    }
#endif

    // Main state machine
    switch (ProgState)
    {
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