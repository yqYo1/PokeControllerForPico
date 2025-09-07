#include <stdio.h>
#include <cstring>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "tusb.h"

#include "PokeControllerForPico_Func.h"

// Use UART0 for serial communication
#define UART_ID uart0
#define BAUD_RATE 9600
#define UART_TX_PIN 0
#define UART_RX_PIN 1

int ProgState = STATE0;
static char pc_report_str[MAX_BUFFER];
static uint8_t idx = 0;

int main() {
  // Initialize UART
  uart_init(UART_ID, BAUD_RATE);
  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

  Controller_Init(); // This calls tusb_init()
  Keyboard_Init();
  Controller_Reset();

  while (1) {
    tud_task(); // tinyusb device task

    // Serial receiving logic (from serialEvent1)
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
