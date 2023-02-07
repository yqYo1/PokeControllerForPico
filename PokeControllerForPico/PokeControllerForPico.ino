#include "PokeControllerForPico_Func.h"

int ProgState = STATE0;
static char pc_report_str[MAX_BUFFER];
static uint8_t idx = 0;

void setup() {
  Serial1_Init();    // RX←0でのシリアル通信
  Controller_Init(); // コントローラーの準備
  Keyboard_Init();   // キーボードの初期化
  Controller_Reset();
}

void loop() {
  tud_task(); // tinyusb device task
  //状態に応じて切り替える
  switch (ProgState)
  {
    case STATE1:
      SwitchFunction();
      break;
    default:
      /* バグ回避を兼ねて状態の初期化を行う */
      ProgState = STATE0;
      break;
  }
}

//データが利用可能な時に呼び出される関数(Serial1)
void serialEvent1()
{
  // one character comes at a time
  if (Serial1.available()) {
    char c = Serial1.read();

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
}