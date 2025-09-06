#include "PokeControllerForPico_Func.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include <stdio.h>
#include <string.h>

#define YEAR_MAX 60

typedef enum {
  SWITCH_Y       = 0x0001,
  SWITCH_B       = 0x0002,
  SWITCH_A       = 0x0004,
  SWITCH_X       = 0x0008,
  SWITCH_L       = 0x0010,
  SWITCH_R       = 0x0020,
  SWITCH_ZL      = 0x0040,
  SWITCH_ZR      = 0x0080,
  SWITCH_MINUS   = 0x0100,
  SWITCH_PLUS    = 0x0200,
  SWITCH_LCLICK  = 0x0400,
  SWITCH_RCLICK  = 0x0800,
  SWITCH_HOME    = 0x1000,
  SWITCH_CAPTURE = 0x2000,
} JoystickButtons_t;

typedef enum {
  INIT,
  SYNC,
  PROCESS,
  CLEANUP,
  DONE
} State_t;
State_t state = INIT;

typedef enum {
  NONE,   // do nothing
  // On MCU
  MASH_A,   // mash button A
  AAABB,   // AAABB
  AUTO_LEAGUE,// auto league
  INF_WATT,   // infinity watt
  PICKUPBERRY,
  CHANGETHEDATE,  // Change the Date
  CHANGETHEYEAR,  // Change the Year
  P_SYNC,
  P_UNSYNC,
  DEBUG,
  DEBUG2,

  // From PC
  PC_CALL,
  PC_CALL_STRING,
  PC_CALL_KEYBOARD,
  PC_CALL_KEYBOARD_PRESS,
  PC_CALL_KEYBOARD_RELEASE,
} Proc_State_t;

volatile Proc_State_t proc_state = NONE;
const char* cmd_name[MAX_BUFFER] = {
  "none\r\n",
  "mash_a\r\n",
  "aaabb\r\n",
  "auto_league\r\n",
  "inf_watt\r\n",
  "pickupberry\r\n",
  "changethedate\r\n",
  "changetheyear\r\n",
  "p_sync\r\n",
  "p_unsync\r\n",
  "debug\r\n",
  "debug2\r\n",

  "pc_call\r\n",
  "pc_call_string\r\n",
  "pc_call_keyboard\r\n",
};

static int step_size_buf;

uint8_t pc_lx, pc_ly, pc_rx, pc_ry;
uint32_t KeyValue;
uint32_t YearChangeCnt;//0~4294967295までの整数
uint32_t MonthChangeCnt;//0~4294967295までの整数
uint32_t DayChangeCnt;//0~4294967295までの整数
int NowYear = 0;

static unsigned long s_ultime;
static bool blduration = false;
static bool blwaittime = false;
static int cnt_command = 0;
static USB_JoystickReport_Input_t pc_report;
static USB_JoystickReport_Input_t last_pc_report;
static char chrread[MAX_BUFFER];

void GetNextReportFromCommands(const SetCommand* commands, const int step_size);
void GetNextReportFromCommandsforChangeTheDate(const SetCommand* commands, const int step_size);
void GetNextReportFromCommandsforChangeTheYear(const SetCommand* commands, const int step_size);
USB_JoystickReport_Input_t ApplyButtonCommand(const SetCommand* commands, USB_JoystickReport_Input_t ReportData);
void Type_string(char* str);
void Type_stringBySerialCommunication(void);
void sendReportOnly(USB_JoystickReport_Input_t report);

void Controller_Init(void)
{
  tusb_init();
}

void Controller_Reset(void)
{
  ResetDirections();//送信するデータをリセットする
  /* 認識させるため、複数回Switchにデータを送信する */
  for (int i = 0; i < 5; i++)
  {
    sendReportOnly(pc_report);//解析したデータをSwitchに送信
    sleep_ms(40);
  }
}

void Keyboard_Init(void)
{
  key_releaseAll();
}

/* 送信するデータをリセットする */
void ResetDirections(void)
{
  pc_report.LX = 128;
  pc_report.LY = 128;
  pc_report.RX = 128;
  pc_report.RY = 128;
  pc_report.Hat = HAT_CENTER;
}

void ParseLine(char* line)
{
  char cmd[16];
  uint16_t p_btns;
  uint8_t p_hat;
  // get command
  int ret = sscanf(line, "%s", cmd);
  if (ret == EOF) {
    proc_state = DEBUG;
  } else if (strncmp(cmd, "end", 16) == 0) {
    proc_state = NONE;
    ResetDirections();
  } else if (cmd[0] >= '0' && cmd[0] <= '9') {
    uint8_t char_pos = 0;

    p_btns = 0;
    while (line[char_pos] != ' ' && line[char_pos] != '\r') {
      p_btns *= 16;
      if (line[char_pos] >= '0' && line[char_pos] <= '9') {
        p_btns += (line[char_pos] - '0');
      } else if (line[char_pos] >= 'A' && line[char_pos] <= 'F') {
        p_btns += (line[char_pos] - 'A' + 10);
      } else {
        p_btns += (line[char_pos] - 'a' + 10);
      }
      char_pos++;
    }
    if (line[char_pos] != '\r') char_pos++;

    p_hat = 0;
    while (line[char_pos] != ' ' && line[char_pos] != '\r') {
      p_hat *= 16;
      if (line[char_pos] >= '0' && line[char_pos] <= '9') {
        p_hat += (line[char_pos] - '0');
      } else if (line[char_pos] >= 'A' && line[char_pos] <= 'F') {
        p_hat += (line[char_pos] - 'A' + 10);
      } else {
        p_hat += (line[char_pos] - 'a' + 10);
      }
      char_pos++;
    }
    if (line[char_pos] != '\r') char_pos++;

    while (line[char_pos] != ' ' && line[char_pos] != '\r') {
      pc_lx *= 16;
      if (line[char_pos] >= '0' && line[char_pos] <= '9') {
        pc_lx += (line[char_pos] - '0');
      } else if (line[char_pos] >= 'A' && line[char_pos] <= 'F') {
        pc_lx += (line[char_pos] - 'A' + 10);
      } else {
        pc_lx += (line[char_pos] - 'a' + 10);
      }
      char_pos++;
    }
    if (line[char_pos] != '\r') char_pos++;

    while (line[char_pos] != ' ' && line[char_pos] != '\r') {
      pc_ly *= 16;
      if (line[char_pos] >= '0' && line[char_pos] <= '9') {
        pc_ly += (line[char_pos] - '0');
      } else if (line[char_pos] >= 'A' && line[char_pos] <= 'F') {
        pc_ly += (line[char_pos] - 'A' + 10);
      } else {
        pc_ly += (line[char_pos] - 'a' + 10);
      }
      char_pos++;
    }
    if (line[char_pos] != '\r') char_pos++;

    while (line[char_pos] != ' ' && line[char_pos] != '\r') {
      pc_rx *= 16;
      if (line[char_pos] >= '0' && line[char_pos] <= '9') {
        pc_rx += (line[char_pos] - '0');
      } else if (line[char_pos] >= 'A' && line[char_pos] <= 'F') {
        pc_rx += (line[char_pos] - 'A' + 10);
      } else {
        pc_rx += (line[char_pos] - 'a' + 10);
      }
      char_pos++;
    }
    if (line[char_pos] != '\r') char_pos++;

    while (line[char_pos] != ' ' && line[char_pos] != '\r') {
      pc_ry *= 16;
      if (line[char_pos] >= '0' && line[char_pos] <= '9') {
        pc_ry += (line[char_pos] - '0');
      } else if (line[char_pos] >= 'A' && line[char_pos] <= 'F') {
        pc_ry += (line[char_pos] - 'A' + 10);
      } else {
        pc_ry += (line[char_pos] - 'a' + 10);
      }
      char_pos++;
    }

    // HAT : 0(TOP) to 7(TOP_LEFT) in clockwise | 8(CENTER)
    pc_report.Hat = p_hat;

    // we use bit array for buttons(2 Bytes), which last 2 bits are flags of directions
    bool use_right = p_btns & 0x0001;
    bool use_left  = p_btns & 0x0002;

    // Left stick
    if (use_left) {
      pc_report.LX = pc_lx;
      pc_report.LY = pc_ly;
    }

    // Right stick
    if (use_right & use_left) {
      pc_report.RX = pc_rx;
      pc_report.RY = pc_ry;
    } else if (use_right) {
      pc_report.RX = pc_lx;
      pc_report.RY = pc_ly;
    }

    p_btns >>= 2;
    pc_report.Button = p_btns;

    proc_state = PC_CALL;
  } else if (strncmp(line, "\"", 1) == 0) {
    for (int i = 0; i < MAX_BUFFER; i++)
    {
      chrread[i] = (char)line[i];
    }
    proc_state = PC_CALL_STRING;
  } else if (strncmp(cmd, "Key", 3) == 0) {
    sscanf(line, "Key %lu", &KeyValue);
    proc_state = PC_CALL_KEYBOARD;
    ProgState = STATE1;
  } else if (strncmp(cmd, "Press", 5) == 0) {
    sscanf(line, "Press %lu", &KeyValue);
    proc_state = PC_CALL_KEYBOARD_PRESS;
    ProgState = STATE1;
  } else if (strncmp(cmd, "Release", 7) == 0) {
    sscanf(line, "Release %lu", &KeyValue);
    proc_state = PC_CALL_KEYBOARD_RELEASE;
    ProgState = STATE1;
  } else if (strncmp(cmd, cmd_name[MASH_A], 6) == 0) {
    proc_state = MASH_A;
    ProgState = STATE1;
  } else if (strncmp(cmd, cmd_name[AAABB], 5) == 0) {
    proc_state = AAABB;
    ProgState = STATE1;
  } else if (strncmp(cmd, cmd_name[AUTO_LEAGUE], 6) == 0) {
    proc_state = AUTO_LEAGUE;
    ProgState = STATE1;
  } else if (strncmp(cmd, cmd_name[INF_WATT], 6) == 0) {
    proc_state = INF_WATT;
    ProgState = STATE1;
  } else if (strncmp(cmd, cmd_name[PICKUPBERRY], 6) == 0) {
    proc_state = PICKUPBERRY;
    ProgState = STATE1;
  } else if (strncmp(cmd, "Date", 4) == 0) {
    sscanf(line, "Date %lu/%lu/%lu", &YearChangeCnt, &MonthChangeCnt, &DayChangeCnt);
    proc_state = CHANGETHEDATE;
    ProgState = STATE1;
  } else if (strncmp(cmd, "Year", 4) == 0) {
    proc_state = CHANGETHEYEAR;
    sscanf(line, "Year %lu", &YearChangeCnt);
    NowYear = 0;
  } else if (strncmp(cmd, cmd_name[P_SYNC], 6) == 0) {
    proc_state = P_SYNC;
  } else if (strncmp(cmd, cmd_name[P_UNSYNC], 6) == 0) {
    proc_state = P_UNSYNC;
  } else {
    proc_state = DEBUG2;
  }
  // Serial1.println(proc_state, DEC);
  cnt_command = 0;
  step_size_buf = INT8_MAX;
}

void GetNextReportFromCommands(const SetCommand* commands, const int step_size)
{
  if ((blduration == false) && (blwaittime == false))
  {
    memcpy(&last_pc_report, &pc_report,  sizeof(USB_JoystickReport_Input_t));
    ApplyButtonCommand(commands, pc_report);
    sendReportOnly(pc_report);//解析したデータをSwitchに送信
    s_ultime = to_ms_since_boot(get_absolute_time());
    blduration = true;
    blwaittime = true;
    return;
  }
  else if ((blduration == true) && (blwaittime == true))
  {
    if (to_ms_since_boot(get_absolute_time()) - s_ultime > commands[cnt_command].duration)
    {
      sendReportOnly(last_pc_report);//解析したデータをSwitchに送信
      s_ultime = to_ms_since_boot(get_absolute_time());
      blduration = false;
    }
    return;
  }
  else
  {
    if (to_ms_since_boot(get_absolute_time()) - s_ultime > commands[cnt_command].waittime)
    {

      memcpy(&pc_report, &last_pc_report, sizeof(USB_JoystickReport_Input_t));
      cnt_command++;
      if (cnt_command >= step_size)
      {
        cnt_command = 0;
      }
      blduration = false;
      blwaittime = false;
    }
  }
}

void GetNextReportFromCommandsforChangeTheDate(const SetCommand* commands, const int step_size)
{
  if ((blduration == false) && (blwaittime == false))
  {
    memcpy(&last_pc_report, &pc_report,  sizeof(USB_JoystickReport_Input_t));
    ApplyButtonCommand(commands, pc_report);
    sendReportOnly(pc_report);//解析したデータをSwitchに送信
    s_ultime = to_ms_since_boot(get_absolute_time());
    blduration = true;
    blwaittime = true;
    return;
  }
  else if ((blduration == true) && (blwaittime == true))
  {
    if (to_ms_since_boot(get_absolute_time()) - s_ultime > commands[cnt_command].duration)
    {
      sendReportOnly(last_pc_report);//解析したデータをSwitchに送信
      s_ultime = to_ms_since_boot(get_absolute_time());
      blduration = false;
    }
    return;
  }
  else
  {
    if (to_ms_since_boot(get_absolute_time()) - s_ultime > commands[cnt_command].waittime)
    {
      memcpy(&pc_report, &last_pc_report, sizeof(USB_JoystickReport_Input_t));
      cnt_command++;
      if(cnt_command >= step_size)
      {
        cnt_command = step_size - 1;
      }

      if(YearChangeCnt > 0)
      {
        if((cnt_command == INDEX_ARRAY_YEAR + 1) && (YearChangeCnt > 1))
        {
          cnt_command = INDEX_ARRAY_YEAR;
          YearChangeCnt--;
        }
      }
      else
      {
        if(cnt_command == INDEX_ARRAY_YEAR)
        {
          cnt_command++;
        }
      }

      if(MonthChangeCnt > 0)
      {
        if((cnt_command == INDEX_ARRAY_MONTH + 1) && (MonthChangeCnt > 1))
        {
          cnt_command = INDEX_ARRAY_MONTH;
          MonthChangeCnt--;
        }
      }
      else
      {
        if(cnt_command == INDEX_ARRAY_MONTH)
        {
          cnt_command++;
        }
      }

      if(DayChangeCnt > 0)
      {
        if((cnt_command == INDEX_ARRAY_DAY + 1) && (DayChangeCnt > 1))
        {
          cnt_command = INDEX_ARRAY_DAY;
          DayChangeCnt--;
        }
      }
      else
      {
        if(cnt_command == INDEX_ARRAY_DAY)
        {
          cnt_command++;
        }
      }
      blduration = false;
      blwaittime = false;
    }
  }
}

void GetNextReportFromCommandsforChangeTheYear(const SetCommand* commands, const int step_size)
{
  if ((blduration == false) && (blwaittime == false))
  {
    memcpy(&last_pc_report, &pc_report,  sizeof(USB_JoystickReport_Input_t));
    ApplyButtonCommand(commands, pc_report);
    sendReportOnly(pc_report);//解析したデータをSwitchに送信
    s_ultime = to_ms_since_boot(get_absolute_time());
    blduration = true;
    blwaittime = true;
    return;
  }
  else if ((blduration == true) && (blwaittime == true))
  {
    if (to_ms_since_boot(get_absolute_time()) - s_ultime > commands[cnt_command].duration)
    {
      sendReportOnly(last_pc_report);//解析したデータをSwitchに送信
      s_ultime = to_ms_since_boot(get_absolute_time());
      blduration = false;
    }
    return;
  }
  else
  {
    if (to_ms_since_boot(get_absolute_time()) - s_ultime > commands[cnt_command].waittime)
    {

      memcpy(&pc_report, &last_pc_report, sizeof(USB_JoystickReport_Input_t));
      cnt_command++;

      if ((YearChangeCnt > 0) && (cnt_command == 14))
      {
        YearChangeCnt--;
        if (YearChangeCnt == 0)
        {
          cnt_command = 27;
        }
        else if (NowYear < YEAR_MAX - 1)
        {
          cnt_command = 1;
          NowYear++;
        }
        else
        {
          //何もしない
        }
      }
      else if ((YearChangeCnt > 0) && (cnt_command == 27))
      {
        cnt_command = 1;
        NowYear = 0;
      }
      else if ((YearChangeCnt == 0) && (cnt_command > step_size_buf - 1))
      {
        cnt_command = step_size_buf - 1;
      }
      else
      {
        //何もしない
      }

      blduration = false;
      blwaittime = false;
    }
  }
}

USB_JoystickReport_Input_t ApplyButtonCommand(const SetCommand* commands, USB_JoystickReport_Input_t ReportData)
{
  uint8_t button = commands[cnt_command].command;
  switch (button)
  {
    case COMMAND_UP:
      pc_report.LY = STICK_MIN;
      break;

    case COMMAND_LEFT:
      pc_report.LX = STICK_MIN;
      break;

    case COMMAND_DOWN:
      pc_report.LY = STICK_MAX;
      break;

    case COMMAND_RIGHT:
      pc_report.LX = STICK_MAX;
      break;

    case COMMAND_A:
      pc_report.Button |= SWITCH_A;
      break;

    case COMMAND_B:
      pc_report.Button |= SWITCH_B;
      break;

    case COMMAND_X:
      pc_report.Button |= SWITCH_X;
      break;

    case COMMAND_Y:
      pc_report.Button |= SWITCH_Y;
      break;

    case COMMAND_L:
      pc_report.Button |= SWITCH_L;
      break;

    case COMMAND_R:
      pc_report.Button |= SWITCH_R;
      break;

    case COMMAND_TRIGGERS:
      pc_report.Button |= SWITCH_L | SWITCH_R;
      break;

    case COMMAND_UPLEFT:
      pc_report.LX = STICK_MIN;
      pc_report.LY = STICK_MIN;
      break;

    case COMMAND_UPRIGHT:
      pc_report.LX = STICK_MAX;
      pc_report.LY = STICK_MIN;
      break;

    case COMMAND_DOWNRIGHT:
      pc_report.LX = STICK_MAX;
      pc_report.LY = STICK_MAX;
      break;

    case COMMAND_DOWNLEFT:
      pc_report.LX = STICK_MIN;
      pc_report.LY = STICK_MAX;
      break;

    case COMMAND_PLUS:
      pc_report.Button |= SWITCH_PLUS;
      break;

    case COMMAND_MINUS:
      pc_report.Button |= SWITCH_MINUS;
      break;

    case COMMAND_HOME:
      pc_report.Button |= SWITCH_HOME;
      break;

    case COMMAND_RS_UP:
      pc_report.RY = STICK_MIN;
      break;

    case COMMAND_RS_LEFT:
      pc_report.RX = STICK_MIN;
      break;

    case COMMAND_RS_DOWN:
      pc_report.RY = STICK_MAX;
      break;

    case COMMAND_RS_RIGHT:
      pc_report.RX = STICK_MAX;
      break;

    case COMMAND_RS_UPLEFT:
      pc_report.RX = STICK_MIN;
      pc_report.RY = STICK_MIN;
      break;

    case COMMAND_RS_UPRIGHT:
      pc_report.RX = STICK_MAX;
      pc_report.RY = STICK_MIN;
      break;

    case COMMAND_RS_DOWNRIGHT:
      pc_report.RX = STICK_MAX;
      pc_report.RY = STICK_MAX;
      break;

    case COMMAND_RS_DOWNLEFT:
      pc_report.RX = STICK_MIN;
      pc_report.RY = STICK_MAX;
      break;

    case COMMAND_HAT_TOP:
      pc_report.Hat |= HAT_TOP;
      break;

    case COMMAND_HAT_TOP_RIGHT:
      pc_report.Hat |= HAT_TOP_RIGHT;
      break;

    case COMMAND_HAT_BOTTOM_RIGHT:
      pc_report.Hat |= HAT_BOTTOM_RIGHT;
      break;

    case COMMAND_HAT_BOTTOM:
      pc_report.Hat |= HAT_BOTTOM;
      break;

    case COMMAND_HAT_BOTTOM_LEFT:
      pc_report.Hat |= HAT_BOTTOM_LEFT;
      break;

    case COMMAND_HAT_LEFT:
      pc_report.Hat |= HAT_LEFT;
      break;

    default:
      pc_report.LX = STICK_CENTER;
      pc_report.LY = STICK_CENTER;
      pc_report.RX = STICK_CENTER;
      pc_report.RY = STICK_CENTER;
      pc_report.Hat = HAT_CENTER;
      break;
  }
  return ReportData;
}

void SwitchFunction(void)
{
  switch (proc_state)
  {
    case PC_CALL:
      sendReportOnly(pc_report);//解析したデータをSwitchに送信
      ProgState = STATE0;
      break;
    case PC_CALL_STRING:
      Type_stringBySerialCommunication();//シリアル通信を用いてキーボードを使用する
      memset(chrread, 0, sizeof(chrread));
      ProgState = STATE0;
      break;
    case PC_CALL_KEYBOARD:
      specialkey_write(KeyValue);
      ProgState = STATE0;
      break;
    case PC_CALL_KEYBOARD_PRESS:
      specialkey_press(KeyValue);
      ProgState = STATE0;
      break;
    case PC_CALL_KEYBOARD_RELEASE:
      specialkey_release(KeyValue);
      ProgState = STATE0;
      break;
    case MASH_A:
      GetNextReportFromCommands(&mash_a_commands[0], mash_a_size);
      break;
    case AAABB:
      GetNextReportFromCommands(&aaabb_commands[0], aaabb_size);
      break;
    case AUTO_LEAGUE:
      pc_report.LX = 172;
      pc_report.LY = 7;
      GetNextReportFromCommands(&auto_league_commands[0], auto_league_size);
      break;
    case INF_WATT:
      GetNextReportFromCommands(&inf_watt_commands[0], inf_watt_size);
      break;
    case PICKUPBERRY:
      GetNextReportFromCommands(&pickupberry_commands[0], pickupberry_size);
      break;
    case CHANGETHEDATE:
      GetNextReportFromCommandsforChangeTheDate(&changethedate_commands[0], changethedate_size);
      break;
    case CHANGETHEYEAR:
      GetNextReportFromCommandsforChangeTheYear(&changetheyear_commands[0], changetheyear_size);
      break;
    default:
      /* 何もしない */
      break;
  }
}

void Type_string(char* str)
{
  //先頭の「"」を無視する
  int len = 1;
  char* t_str = str;
  while (true)
  {
    if (t_str[len] == '\0')
    {
      break;
    }
    else if((t_str[len] == '\"')&&(t_str[len+1] == '\r')&&(t_str[len+2] == '\n'))
    {
      break;
    }
    else
    {
      key_write(t_str[len]);
    }
    len++;
  }
}

void Type_stringBySerialCommunication(void)
{
  /* シリアル通信で受け取ったデータの先頭末尾にある「"」を削除 */
  Type_string(chrread);
  ResetDirections();
  sendReportOnly(pc_report);//解析したデータをSwitchに送信
  sleep_ms(40);
}

void sendReportOnly(USB_JoystickReport_Input_t report)
{
    USB_JoystickReport_Input_t sendBuffer;
	// Wait for previous transmission to complete.
	while (tud_ready() && !tud_hid_ready())
	{
		tud_task();
	}

	memcpy(&sendBuffer, &report, sizeof(USB_JoystickReport_Input_t));
	if (tud_hid_ready())
	{
		tud_hid_report(0, &sendBuffer, sizeof(USB_JoystickReport_Input_t));
	}
}
