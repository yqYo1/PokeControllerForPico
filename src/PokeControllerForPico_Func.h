#pragma once

#include <cstdint>
#include "tusb.h"
#include "usb_descriptors.h"
#include "CustomKeyboard.h"

#define	INDEX_ARRAY_YEAR  (30)
#define	INDEX_ARRAY_MONTH (32)
#define	INDEX_ARRAY_DAY   (34)

#define HAT_TOP          0x00
#define HAT_TOP_RIGHT    0x01
#define HAT_RIGHT        0x02
#define HAT_BOTTOM_RIGHT 0x03
#define HAT_BOTTOM       0x04
#define HAT_BOTTOM_LEFT  0x05
#define HAT_LEFT         0x06
#define HAT_TOP_LEFT     0x07
#define HAT_CENTER       0x08

#define STICK_MIN      0
#define STICK_CENTER 128
#define STICK_MAX    255

// Serial communication
#define MAX_BUFFER 32

typedef enum
{
  STATE0,  // シリアル通信受信→受信成功
  STATE1,  // シリアル通信成功時
  STATE2,  // 判定
} LOOPSTATE;

const typedef enum {
  COMMAND_NONE,
  COMMAND_UP,
  COMMAND_DOWN,
  COMMAND_LEFT,
  COMMAND_RIGHT,
  COMMAND_UPLEFT,
  COMMAND_UPRIGHT,
  COMMAND_DOWNLEFT,
  COMMAND_DOWNRIGHT,
  COMMAND_X,
  COMMAND_Y,
  COMMAND_A,
  COMMAND_B,
  COMMAND_L,
  COMMAND_R,
  COMMAND_PLUS,
  COMMAND_MINUS,
  COMMAND_NOP,
  COMMAND_TRIGGERS,
  COMMAND_HOME,
  //RStick
  COMMAND_RS_UP,
  COMMAND_RS_DOWN,
  COMMAND_RS_LEFT,
  COMMAND_RS_RIGHT,
  COMMAND_RS_UPLEFT,
  COMMAND_RS_UPRIGHT,
  COMMAND_RS_DOWNLEFT,
  COMMAND_RS_DOWNRIGHT,
  //HAT
  COMMAND_HAT_TOP,
  COMMAND_HAT_TOP_RIGHT,
  COMMAND_HAT_RIGHT,
  COMMAND_HAT_BOTTOM_RIGHT,
  COMMAND_HAT_BOTTOM,
  COMMAND_HAT_BOTTOM_LEFT,
  COMMAND_HAT_LEFT,
  COMMAND_HAT_TOP_LEFT
} BUTTON_DEFINE;

typedef struct
{
  uint8_t command;
  int duration;
  int waittime;
} SetCommand;

typedef struct TU_ATTR_PACKED
{
  uint16_t	Button;
  uint8_t		Hat;
  uint8_t		LX;
  uint8_t		LY;
  uint8_t		RX;
  uint8_t		RY;
  uint8_t		Vendor;
} USB_JoystickReport_Input_t;

const SetCommand mash_a_commands[] =
{
  {COMMAND_A, 20, 20}
};

const SetCommand aaabb_commands[] =
{
  {COMMAND_A, 40, 260},
  {COMMAND_A, 40, 260},
  {COMMAND_A, 40, 260},
  {COMMAND_B, 40, 310},
  {COMMAND_B, 40, 310}
};

const SetCommand auto_league_commands[] =
{
  {COMMAND_A, 20, 980},
  {COMMAND_A, 20, 980},
  {COMMAND_A, 20, 980},
  {COMMAND_A, 20, 980},
  {COMMAND_A, 20, 980},
  {COMMAND_A, 20, 980},
  {COMMAND_A, 20, 980},
  {COMMAND_A, 20, 980},
  {COMMAND_A, 20, 980},
  {COMMAND_A, 20, 980},
  {COMMAND_B, 20, 980}
};

const SetCommand inf_watt_commands[] =
{
  {COMMAND_A, 40, 960},
  {COMMAND_B, 40, 1460},
  {COMMAND_B, 40, 1460},
  {COMMAND_B, 40, 1460},
  {COMMAND_B, 40, 1460},
  {COMMAND_B, 40, 1460},
  {COMMAND_A, 40, 1460},
  {COMMAND_A, 40, 3460},
  {COMMAND_HOME, 100, 700}, // HOME画面に遷移
  {COMMAND_LEFT, 40, 160},  // 画面右端に移動
  {COMMAND_DOWN, 40, 0},    // スリープに移動
  {COMMAND_LEFT, 40, 0},    // 設定に移動
  {COMMAND_A, 40, 860},     // 決定
  {COMMAND_DOWN, 40, 0},    // 画面の明るさに移動
  {COMMAND_RS_DOWN, 40, 0}, // ロックに移動
  {COMMAND_DOWN, 40, 0},    // みまもり設定に移動
  {COMMAND_RS_DOWN, 40, 0}, // インターネットに移動
  {COMMAND_DOWN, 40, 0},    // データ管理に移動
  {COMMAND_RS_DOWN, 40, 0}, //ユーザーに移動
  {COMMAND_DOWN, 40, 0},    // Miiに移動
  {COMMAND_RS_DOWN, 40, 0}, // amiiboに移動
  {COMMAND_DOWN, 40, 0},    // テーマに移動
  {COMMAND_RS_DOWN, 40, 0}, // 通知に移動
  {COMMAND_DOWN, 40, 0},    // スリープに移動
  {COMMAND_RS_DOWN, 40, 0}, // コントローラーとセンサーに移動
  {COMMAND_DOWN, 40, 0},    // Bluetoothオーディオに移動
  {COMMAND_RS_DOWN, 40, 0}, // テレビ出力に移動
  {COMMAND_DOWN, 40, 40},   // 本体に移動
  {COMMAND_A, 40, 260},     // 本体の更新に移動
  {COMMAND_DOWN, 40, 0},    // ドックの更新に移動
  {COMMAND_RS_DOWN, 40, 0}, // 本体のニックネームに移動
  {COMMAND_DOWN, 600, 0},   // 言語に移動(画面スクロール)
  {COMMAND_RS_DOWN, 40, 0}, // 地域に移動
  {COMMAND_DOWN, 40, 0},    // 日付と時刻に移動
  {COMMAND_A, 40, 210},     // 決定
  {COMMAND_DOWN, 40, 0},    // タイムゾーンに移動
  {COMMAND_RS_DOWN, 40, 40},// 現在の日付と時刻に移動
  {COMMAND_A, 40, 210},     // 決定
  {COMMAND_RIGHT, 40, 0},   // 月に移動
  {COMMAND_RS_RIGHT, 40, 0},// 日に移動
  {COMMAND_UP, 40, 0},      // 1日進める
  {COMMAND_RIGHT, 40, 0},   // 時に移動
  {COMMAND_RS_RIGHT, 40, 0},// 分に移動
  {COMMAND_RIGHT, 40, 40},  // OKに移動
  {COMMAND_A, 40, 310},     // 決定
  {COMMAND_HOME, 100, 700}, // HOMEに戻る
  {COMMAND_HOME, 100, 700}, // ゲームに戻る
  {COMMAND_B, 40, 740},
  {COMMAND_A, 40, 3440}
};
const SetCommand pickupberry_commands[] =
{
  {COMMAND_L, 40, 40},
  {COMMAND_A, 40, 360},
  {COMMAND_A, 40, 360},
  {COMMAND_A, 40, 360},
  {COMMAND_B, 40, 460},
  {COMMAND_B, 40, 460},
  {COMMAND_B, 40, 460},
  {COMMAND_B, 40, 460},
  {COMMAND_B, 40, 460},
  {COMMAND_B, 40, 460},
  {COMMAND_B, 40, 460},
  {COMMAND_B, 40, 460},
  {COMMAND_B, 40, 460},
  {COMMAND_B, 40, 460},
  {COMMAND_B, 40, 460},
  {COMMAND_B, 40, 460},
  {COMMAND_B, 40, 460},
  {COMMAND_B, 40, 460},
  {COMMAND_B, 40, 460},
  {COMMAND_B, 40, 460},
  {COMMAND_B, 40, 460},
  {COMMAND_B, 40, 460},
  {COMMAND_HOME, 100, 700}, // HOME画面に遷移
  {COMMAND_LEFT, 40, 160},  // 画面右端に移動
  {COMMAND_DOWN, 40, 0},    // スリープに移動
  {COMMAND_LEFT, 40, 0},    // 設定に移動
  {COMMAND_A, 40, 860},     // 決定
  {COMMAND_DOWN, 40, 0},    // 画面の明るさに移動
  {COMMAND_RS_DOWN, 40, 0}, // ロックに移動
  {COMMAND_DOWN, 40, 0},    // みまもり設定に移動
  {COMMAND_RS_DOWN, 40, 0}, // インターネットに移動
  {COMMAND_DOWN, 40, 0},    // データ管理に移動
  {COMMAND_RS_DOWN, 40, 0}, // ユーザーに移動
  {COMMAND_DOWN, 40, 0},    // Miiに移動
  {COMMAND_RS_DOWN, 40, 0}, // amiiboに移動
  {COMMAND_DOWN, 40, 0},    // テーマに移動
  {COMMAND_RS_DOWN, 40, 0}, // 通知に移動
  {COMMAND_DOWN, 40, 0},    // スリープに移動
  {COMMAND_RS_DOWN, 40, 0}, // コントローラーとセンサーに移動
  {COMMAND_DOWN, 40, 0},    // Bluetoothオーディオに移動
  {COMMAND_RS_DOWN, 40, 0}, // テレビ出力に移動
  {COMMAND_DOWN, 40, 40},   // 本体に移動
  {COMMAND_A, 40, 260},     // 本体の更新に移動
  {COMMAND_DOWN, 40, 0},    // ドックの更新に移動
  {COMMAND_RS_DOWN, 40, 0}, // 本体のニックネームに移動
  {COMMAND_DOWN, 600, 0},   // 言語に移動(画面スクロール)
  {COMMAND_RS_DOWN, 40, 0}, // 地域に移動
  {COMMAND_DOWN, 40, 0},    // 日付と時刻に移動
  {COMMAND_A, 40, 210},     // 決定
  {COMMAND_DOWN, 40, 0},    // タイムゾーンに移動
  {COMMAND_RS_DOWN, 40, 40},// 現在の日付と時刻に移動
  {COMMAND_A, 40, 210},     // 決定
  {COMMAND_RIGHT, 40, 0},   // 月に移動
  {COMMAND_RS_RIGHT, 40, 0},// 日に移動
  {COMMAND_UP, 40, 0},      // 1日進める
  {COMMAND_RIGHT, 40, 0},   // 時に移動
  {COMMAND_RS_RIGHT, 40, 0},// 分に移動
  {COMMAND_RIGHT, 40, 40},  // OKに移動
  {COMMAND_A, 40, 310},     // 決定
  {COMMAND_HOME, 100, 700}, // HOMEに戻る
  {COMMAND_HOME, 100, 700}, // ゲームに戻る
};

const SetCommand changethedate_commands[] =
{
  {COMMAND_NONE, 40, 40},   //
  {COMMAND_LEFT, 40, 160},  // 画面右端に移動
  {COMMAND_DOWN, 40, 0},    // スリープに移動
  {COMMAND_LEFT, 40, 0},    // 設定に移動
  {COMMAND_A, 40, 860},     // 決定
  {COMMAND_DOWN, 40, 0},    // 画面の明るさに移動
  {COMMAND_RS_DOWN, 40, 0}, // ロックに移動
  {COMMAND_DOWN, 40, 0},    // みまもり設定に移動
  {COMMAND_RS_DOWN, 40, 0}, // インターネットに移動
  {COMMAND_DOWN, 40, 0},    // データ管理に移動
  {COMMAND_RS_DOWN, 40, 0}, // ユーザーに移動
  {COMMAND_DOWN, 40, 0},    // Miiに移動
  {COMMAND_RS_DOWN, 40, 0}, // amiiboに移動
  {COMMAND_DOWN, 40, 0},    // テーマに移動
  {COMMAND_RS_DOWN, 40, 0}, // 通知に移動
  {COMMAND_DOWN, 40, 0},    // スリープに移動
  {COMMAND_RS_DOWN, 40, 0}, // コントローラーとセンサーに移動
  {COMMAND_DOWN, 40, 0},    // Bluetoothオーディオに移動
  {COMMAND_RS_DOWN, 40, 0}, // テレビ出力に移動
  {COMMAND_DOWN, 40, 40},   // 本体に移動
  {COMMAND_A, 40, 260},     // 本体の更新に移動
  {COMMAND_DOWN, 40, 0},    // ドックの更新に移動
  {COMMAND_RS_DOWN, 40, 0}, // 本体のニックネームに移動
  {COMMAND_DOWN, 600, 0},   // 言語に移動(画面スクロール)
  {COMMAND_RS_DOWN, 40, 0}, // 地域に移動
  {COMMAND_DOWN, 40, 0},    // 日付と時刻に移動
  {COMMAND_A, 40, 210},     // 決定
  {COMMAND_DOWN, 40, 0},    // タイムゾーンに移動
  {COMMAND_RS_DOWN, 40, 40},// 現在の日付と時刻に移動
  {COMMAND_A, 40, 210},     // 決定
  {COMMAND_UP, 40, 40},     // 1年進める
  {COMMAND_RIGHT, 40, 0},   // 月に移動
  {COMMAND_UP, 40, 40},     // 1月進める
  {COMMAND_RS_RIGHT, 40, 0},// 日に移動
  {COMMAND_UP, 40, 40},     // 1日進める
  {COMMAND_RIGHT, 40, 0},   // 時に移動
  {COMMAND_RS_RIGHT, 40, 0},// 分に移動
  {COMMAND_RIGHT, 40, 40},  // OKに移動
  {COMMAND_A, 40, 310},     // 決定
  {COMMAND_HOME, 100, 700}, // HOMEに戻る
  {COMMAND_NONE, 100, 700}, //
};

const SetCommand changetheyear_commands[] =
{
  {COMMAND_NONE, 40, 40},
/* 1年進めるブロック */
  {COMMAND_LEFT, 40, 0},     // 分に移動
  {COMMAND_RS_LEFT, 40, 0},  // 時に移動
  {COMMAND_LEFT, 40, 0},     // 日に移動
  {COMMAND_RS_LEFT, 40, 0},  // 月に移動
  {COMMAND_LEFT, 40, 0},     // 年に移動
  {COMMAND_UP, 40, 0},       // 1日進める
  {COMMAND_RIGHT, 40, 0},    // 月に移動
  {COMMAND_RS_RIGHT, 40, 0}, // 日に移動
  {COMMAND_RIGHT, 40, 0},    // 時に移動
  {COMMAND_RS_RIGHT, 40, 0}, // 分に移動
  {COMMAND_RIGHT, 40, 40},   // OKに移動
  {COMMAND_A, 40, 160},      // 決定
  {COMMAND_A, 40, 160},      // 決定
/* 2000年まで戻るブロック */
  {COMMAND_LEFT, 40, 0},     // 分に移動
  {COMMAND_RS_LEFT, 40, 0},  // 時に移動
  {COMMAND_LEFT, 40, 0},     // 日に移動
  {COMMAND_RS_LEFT, 40, 0},  // 月に移動
  {COMMAND_LEFT, 40, 0},     // 年に移動
  {COMMAND_DOWN, 5400, 0},   // 1日進める
  {COMMAND_RIGHT, 40, 0},    // 月に移動
  {COMMAND_RS_RIGHT, 40, 0}, // 日に移動
  {COMMAND_RIGHT, 40, 0},    // 時に移動
  {COMMAND_RS_RIGHT, 40, 0}, // 分に移動
  {COMMAND_RIGHT, 40, 40},   // OKに移動
  {COMMAND_A, 40, 160},      // 決定
  {COMMAND_A, 40, 160},      // 決定
/* 消費終了後のブロック */
  {COMMAND_B, 40, 160},      // HOMEに戻る
  {COMMAND_UP, 40, 160},     // ゲームに戻る
  {COMMAND_A, 40, 160},      // ゲームに戻る
  {COMMAND_NONE, 100, 700},  //
};

const int mash_a_size = (int)(sizeof(mash_a_commands) / sizeof(SetCommand));
const int aaabb_size = (int)(sizeof(aaabb_commands) / sizeof(SetCommand));
const int auto_league_size = (int)(sizeof(auto_league_commands) / sizeof(SetCommand));
const int inf_watt_size = (int)(sizeof(inf_watt_commands) / sizeof(SetCommand));
const int pickupberry_size = (int)(sizeof(pickupberry_commands) / sizeof(SetCommand));
const int changethedate_size = (int)(sizeof(changethedate_commands) / sizeof(SetCommand));
const int changetheyear_size = (int)(sizeof(changetheyear_commands) / sizeof(SetCommand));

extern int ProgState;

void Controller_Init(void);
void Controller_Reset(void);
void Keyboard_Init(void);
void ResetDirections(void);
void ParseLine(char* line);
void SwitchFunction(void);
