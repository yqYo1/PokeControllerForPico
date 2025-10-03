#include <stdio.h>
#include <cstring>
#include "pico/stdlib.h"
#include "tusb.h"

#include "PokeControllerForPico_Func.h"

#ifndef USE_USB_CDC
  #include "hardware/uart.h"
  // Use UART0 for serial communication
  #define UART_ID uart0
  #define BAUD_RATE 9600
  #define UART_TX_PIN 0
  #define UART_RX_PIN 1
#endif

int ProgState = STATE0;
static char pc_report_str[MAX_BUFFER];
static uint8_t idx = 0;

int main() {
#ifdef USE_USB_CDC
  // Initialize USB CDC
  stdio_init_all();
#else
  // Initialize UART
  uart_init(UART_ID, BAUD_RATE);
  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
#endif

  Controller_Init(); // This calls tusb_init()
  Keyboard_Init();
  Controller_Reset();

  while (1) {
    tud_task(); // tinyusb device task

#ifdef USE_USB_CDC
    if (tud_cdc_available()) {
      char c = tud_cdc_read_char();
#else
    if (uart_is_readable(UART_ID)) {
      char c = uart_getc(UART_ID);
#endif

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

    // Main state machine (from loop)
    switch (ProgState)
    {
      case STATE1:
        SwitchFunction();
        break;
      default:
        /* Initialize state to avoid bugs */
        ProgState = STATE0;
        break;
    }
  }

  return 0;
}
