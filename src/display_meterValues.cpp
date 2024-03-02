#include "display_meterValues.h"
#include "Variants.h"
#include "ATM90E36.h"
#include <Preferences.h>
#include "LCD_I2C.h"
#include "MeteringService.h"
#include "OcppMessage.h"
#include "CustomGsm.h"

extern TinyGsmClient client;

extern LCD_I2C lcd;
extern uint8_t gu8_online_flag;
extern ATM90E36 eic;
extern int globalmeterstartA;
extern int globalmeterstartB;
extern int globalmeterstartC;
float online_charging_Enargy_A = 0;
float online_charging_Enargy_B = 0;
float online_charging_Enargy_C = 0;
extern Preferences energymeter;
extern Preferences evse_display;
extern bool EMGCY_FaultOccured_A;
extern bool EMGCY_FaultOccured_B;
extern bool EMGCY_FaultOccured_C;
extern uint8_t offline_charging_A;
extern uint8_t offline_charging_B;
extern uint8_t offline_charging_C;

extern float offline_charging_Enargy_A;
extern float offline_charging_Enargy_B;
extern float offline_charging_Enargy_C;

unsigned long offline_t_A = 0;
unsigned long offline_t_B = 0;
unsigned long offline_t_C = 0;

time_t lastsampledTimeA_off = 0;
time_t lastsampledTimeB_off = 0;
time_t lastsampledTimeC_off = 0;

extern bool isInternetConnected;
extern bool wifi_connect;
extern bool gsm_connect;
extern bool ethernet_connect;

extern volatile bool session_ongoing_flag;

extern MeteringService *meteringService;

extern bool webSocketConncted;

extern float current_energy_A;
extern float current_energy_B;
extern float current_energy_C;

extern unsigned long st_timeA;
extern unsigned long st_timeB;
extern unsigned long st_timeC;

static uint8_t gu8_EVSE_Disp_state = 0;
uint8_t Cuurent_Status_Disp = 0;

Connector_Charging_Status Connector_Charging_Status_Disp;

extern uint8_t gu8_Remote_start_A;
extern uint8_t gu8_Remote_start_B;
extern uint8_t gu8_Remote_start_C;

extern float discurrEnergy_A;
extern float discurrEnergy_B;
extern float discurrEnergy_C;

#if DWIN_ENABLED
#include "dwin.h"
unsigned long onTime = 0;
uint8_t state_timer = 0;
uint8_t disp_evse = 0;
extern bool disp_evse_A;
extern bool disp_evse_B;
extern bool disp_evse_C;
extern bool notFaulty_A;
extern bool notFaulty_B;
extern bool notFaulty_C;
extern int8_t fault_code_A;
extern int8_t fault_code_B;
extern int8_t fault_code_C;

extern unsigned char ct[22];        // connected
extern unsigned char nct[22];       // not connected
extern unsigned char et[22];        // ethernet
extern unsigned char wi[22];        // wifi
extern unsigned char tr[22];        // tap rfid
extern unsigned char utr[22];       // rfid unavailable
extern unsigned char g[22];         // 4g
extern unsigned char clu[22];       // connected
extern unsigned char clun[22];      // not connected
extern unsigned char avail[22];     // available
extern unsigned char not_avail[22]; // not available
extern unsigned char change_page[10];
extern unsigned char tap_rfid[30];
extern unsigned char clear_tap_rfid[30];
extern unsigned char CONN_UNAVAIL[28];
extern bool flag_faultOccured_A;
extern bool flag_faultOccured_B;
extern bool flag_faultOccured_C;
extern unsigned char v1[8];
extern unsigned char v2[8];
extern unsigned char v3[8];
extern unsigned char i1[8];
extern unsigned char i2[8];
extern unsigned char i3[8];
extern unsigned char e1[8];
extern unsigned char e2[8];
extern unsigned char e3[8];
extern unsigned char charging[28];
extern unsigned char cid1[8];
extern unsigned char cid2[8];
extern unsigned char cid3[8];
extern unsigned char unavail[30];

void stateTimer()
{
  switch (state_timer)
  {
  case 0:
    onTime = millis();
    state_timer = 1;
    disp_evse = 1;
    break;
  case 1:
    if ((millis() - onTime) > 3000)
    {
      state_timer = 2;
    }
    break;
  case 2:
    onTime = millis();
    state_timer = 3;
    disp_evse = 2;
    break;
  case 3:
    if ((millis() - onTime) > 3000)
    {
      state_timer = 4;
    }
    break;
  case 4:
    onTime = millis();
    state_timer = 5;
    disp_evse = 3;
    break;
  case 5:
    if ((millis() - onTime) > 3000)
    {
      state_timer = 6;
    }
    break;
  case 6:
    state_timer = 0;
  }
}

uint8_t avail_counter = 0;

void disp_dwin_meter()
{
  /*
   * @brief : Energy is being stored in the variable instantPower_A - do not get confused!
   */
  uint8_t err = 0;
  float instantCurrrent_A = eic.GetLineCurrentA();
  int instantVoltage_A = eic.GetLineVoltageA();
  float instantPower_A = 0.0f;
  if ((online_charging_Enargy_A - globalmeterstartA) > 0)
    instantPower_A = (online_charging_Enargy_A - globalmeterstartA) / 1000;
  else
    instantPower_A = 0; // Since it will initially be -ve due to float - int conversion
  /*if (instantCurrrent_A < 0.15) {
    instantPower_A = 0;
  } else {
    instantPower_A = (instantCurrrent_A * instantVoltage_A) / 1000.0;
  }*/

  float instantCurrrent_B = eic.GetLineCurrentB();
  int instantVoltage_B = eic.GetLineVoltageB();
  float instantPower_B = 0.0f;
  if ((online_charging_Enargy_B - globalmeterstartB) > 0)
    instantPower_B = (online_charging_Enargy_B - globalmeterstartB) / 1000;
  else
    instantPower_B = 0; // Since it will initially be -ve due to float - int conversion

  /*if (instantCurrrent_B < 0.15) {
    instantPower_B = 0;
  } else {
    instantPower_B = (instantCurrrent_B * instantVoltage_B) / 1000.0;
  }*/

  float instantCurrrent_C = eic.GetLineCurrentC();
  int instantVoltage_C = eic.GetLineVoltageC();
  float instantPower_C = 0.0f;
  if ((online_charging_Enargy_C - globalmeterstartC) > 0)
    instantPower_C = (online_charging_Enargy_C - globalmeterstartC) / 1000;
  else
    instantPower_C = 0; // Since it will initially be -ve due to float - int conversion
  // Serial.printf("[Display] The Energy C is : %f",instantPower_C);
  /*if (instantCurrrent_C < 0.15) {
    instantPower_C = 0;
  } else {
    instantPower_C = (instantCurrrent_C * instantVoltage_C) / 1000.0;
  }*/
  switch (disp_evse)
  {
  case 1:
    if (disp_evse_A)
    {
      if (notFaulty_A && !EMGCY_FaultOccured_A)
      {
        change_page[9] = 4;

        v1[4] = 0X6A;
        instantVoltage_A = instantVoltage_A * 10;
        v1[6] = instantVoltage_A >> 8;
        v1[7] = instantVoltage_A & 0xff;
        i1[4] = 0X6C;
        i1[7] = instantCurrrent_A * 10;
        e1[4] = 0X6E;
        e1[7] = instantPower_A * 1000;
        err = DWIN_SET(cid1, sizeof(cid1) / sizeof(cid1[0]));
        err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0])); // page 0
        charging[4] = 0X66;
        charging[5] = 0X00;
        err = DWIN_SET(charging, sizeof(charging) / sizeof(charging[0]));
        err = DWIN_SET(v1, sizeof(v1) / sizeof(v1[0]));
        err = DWIN_SET(i1, sizeof(i1) / sizeof(i1[0]));
        err = DWIN_SET(e1, sizeof(e1) / sizeof(e1[0]));
      }
    }
    break;
  case 2:
    if (disp_evse_B)
    {
      if (notFaulty_B && !EMGCY_FaultOccured_B)
      {

        change_page[9] = 5;

        v2[4] = 0X75;
        instantVoltage_B = instantVoltage_B * 10;
        v2[6] = instantVoltage_B >> 8;
        v2[7] = instantVoltage_B & 0xff;

        i2[4] = 0X77;
        i2[7] = instantCurrrent_B * 10;

        e2[4] = 0X79;
        e2[7] = instantPower_B * 1000;

        err = DWIN_SET(cid2, sizeof(cid2) / sizeof(cid2[0]));

        err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0])); // page 0

        charging[4] = 0X71;
        charging[5] = 0X00;
        err = DWIN_SET(charging, sizeof(charging) / sizeof(charging[0]));

        err = DWIN_SET(v2, sizeof(v2) / sizeof(v2[0]));

        err = DWIN_SET(i2, sizeof(i2) / sizeof(i2[0]));

        err = DWIN_SET(e2, sizeof(e2) / sizeof(e2[0]));
      }
    }

    break;
  case 3:
    if (disp_evse_C)
    {
      if (notFaulty_C && !EMGCY_FaultOccured_C)
      {
        change_page[9] = 6;

        v3[4] = 0X7F;
        instantVoltage_C = instantVoltage_C * 10;
        v3[6] = instantVoltage_C >> 8;
        v3[7] = instantVoltage_C & 0xff;

        i3[4] = 0X82;
        i3[7] = instantCurrrent_C * 10;

        e3[4] = 0X84;
        e3[7] = instantPower_C * 1000;
        err = DWIN_SET(cid3, sizeof(cid3) / sizeof(cid3[0]));

        err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0])); // page 0

        charging[4] = 0X7B;
        charging[5] = 0X00;
        err = DWIN_SET(charging, sizeof(charging) / sizeof(charging[0]));

        err = DWIN_SET(v3, sizeof(v3) / sizeof(v3[0]));

        err = DWIN_SET(i3, sizeof(i3) / sizeof(i3[0]));

        err = DWIN_SET(e3, sizeof(e3) / sizeof(e3[0]));
      }
    }

    break;
  default:
    Serial.println(F("**Display default**"));
    break;
  }
}

#endif

#if DISPLAY_ENABLED
unsigned long onTime = 0;
uint8_t state_timer = 0;
uint8_t disp_evse = 0;
extern bool disp_evse_A;
extern bool disp_evse_B;
extern bool disp_evse_C;
extern bool notFaulty_A;
extern bool notFaulty_B;
extern bool notFaulty_C;
extern int8_t fault_code_A;
extern int8_t fault_code_B;
extern int8_t fault_code_C;
void stateTimer()
{
  switch (state_timer)
  {
  case 0:
    onTime = millis();
    state_timer = 1;
    disp_evse = 1;
    break;
  case 1:
    if ((millis() - onTime) > 3000)
    {
      state_timer = 2;
    }
    break;
  case 2:
    onTime = millis();
    state_timer = 3;
    disp_evse = 2;
    break;
  case 3:
    if ((millis() - onTime) > 3000)
    {
      state_timer = 4;
    }
    break;
  case 4:
    onTime = millis();
    state_timer = 5;
    disp_evse = 3;
    break;
  case 5:
    if ((millis() - onTime) > 3000)
    {
      state_timer = 6;
    }
    break;
  case 6:
    state_timer = 0;
  }
}

uint8_t avail_counter = 0;

void disp_lcd_meter()
{
  float instantCurrrent_A = eic.GetLineCurrentA();
  float instantVoltage_A = eic.GetLineVoltageA();
  float instantPower_A = 0.0f;

  if (instantCurrrent_A < 0.15)
  {
    instantPower_A = 0;
  }
  else
  {
    instantPower_A = (instantCurrrent_A * instantVoltage_A) / 1000.0;
  }

  float instantCurrrent_B = eic.GetLineCurrentB();
  int instantVoltage_B = eic.GetLineVoltageB();
  float instantPower_B = 0.0f;

  if (instantCurrrent_B < 0.15)
  {
    instantPower_B = 0;
  }
  else
  {
    instantPower_B = (instantCurrrent_B * instantVoltage_B) / 1000.0;
  }

  float instantCurrrent_C = eic.GetLineCurrentC();
  int instantVoltage_C = eic.GetLineVoltageC();
  float instantPower_C = 0.0f;

  if (instantCurrrent_C < 0.15)
  {
    instantPower_C = 0;
  }
  else
  {
    instantPower_C = (instantCurrrent_C * instantVoltage_C) / 1000.0;
  }
  switch (disp_evse)
  {
  case 1:
    if (disp_evse_A)
    {
      if (notFaulty_A && !EMGCY_FaultOccured_A)
      {
        // connector, voltage, current, power
        displayEnergyValues_Disp_AC("1", String(instantVoltage_A), String(instantCurrrent_A), String(instantPower_A));
      }
      else
      {
        avail_counter++;
        switch (fault_code_A)
        {
        case -1:
          break; // It means default.
        case 0:
          displayEnergyValues_Disp_AC("1-Over Voltage", String(instantVoltage_A), String(instantCurrrent_A), String(instantPower_A));
          break;
        case 1:
          displayEnergyValues_Disp_AC("1-Under Voltage", String(instantVoltage_A), String(instantCurrrent_A), String(instantPower_A));
          break;
        case 2:
          displayEnergyValues_Disp_AC("1-Over Current", String(instantVoltage_A), String(instantCurrrent_A), String(instantPower_A));
          break;
        case 3:
          displayEnergyValues_Disp_AC("1-Under Current", String(instantVoltage_A), String(instantCurrrent_A), String(instantPower_A));
          break;
        case 4:
          displayEnergyValues_Disp_AC("1-Over Temp", String(instantVoltage_A), String(instantCurrrent_A), String(instantPower_A));

          break;
        case 5:
          displayEnergyValues_Disp_AC("1-Under Temp", String(instantVoltage_A), String(instantCurrrent_A), String(instantPower_A));
          break;
        case 6:
          displayEnergyValues_Disp_AC("1-GFCI", String(instantVoltage_A), String(instantCurrrent_A), String(instantPower_A));
          break;
        case 7:
          displayEnergyValues_Disp_AC("1-Earth Disc", String(instantVoltage_A), String(instantCurrrent_A), String(instantPower_A));
          break;
        default:
          SerialMon.println(F("Default in display"));
        }
      }
      checkForResponse_Disp();
    }
    else // we shall use this case to refresh home page.
    {
      if (!notFaulty_A || EMGCY_FaultOccured_A)
      {
        avail_counter++;
      }
    }

    break;
  case 2:
    if (disp_evse_B)
    {
      if (notFaulty_B && !EMGCY_FaultOccured_B)
      {
        displayEnergyValues_Disp_AC("2", String(instantVoltage_B), String(instantCurrrent_B), String(instantPower_B));
      }
      else
      {
        avail_counter++;
        switch (fault_code_B)
        {
        case -1:
          break; // It means default.
        case 0:
          displayEnergyValues_Disp_AC("2-Over Voltage", String(instantVoltage_A), String(instantCurrrent_A), String(instantPower_A));
          break;
        case 1:
          displayEnergyValues_Disp_AC("2-Under Voltage", String(instantVoltage_A), String(instantCurrrent_A), String(instantPower_A));
          break;
        case 2:
          displayEnergyValues_Disp_AC("2-Over Current", String(instantVoltage_A), String(instantCurrrent_A), String(instantPower_A));
          break;
        case 3:
          displayEnergyValues_Disp_AC("2-Under Current", String(instantVoltage_A), String(instantCurrrent_A), String(instantPower_A));
          break;
        case 4:
          displayEnergyValues_Disp_AC("2-Over Temp", String(instantVoltage_A), String(instantCurrrent_A), String(instantPower_A));

          break;
        case 5:
          displayEnergyValues_Disp_AC("2-Under Temp", String(instantVoltage_A), String(instantCurrrent_A), String(instantPower_A));
          break;
        case 6:
          displayEnergyValues_Disp_AC("2-GFCI", String(instantVoltage_A), String(instantCurrrent_A), String(instantPower_A));
          break;
        case 7:
          displayEnergyValues_Disp_AC("2-Earth Disc", String(instantVoltage_A), String(instantCurrrent_A), String(instantPower_A));
          break;
        default:
          SerialMon.println(F("Default in display"));
        }
      }
      checkForResponse_Disp();
    }
    else
    {
      if (!notFaulty_B || EMGCY_FaultOccured_B)
      {
        avail_counter++;
      }
    }

    break;
  case 3:
    if (disp_evse_C)
    {
      if (notFaulty_C && !EMGCY_FaultOccured_C)
      {
        displayEnergyValues_Disp_AC("3", String(instantVoltage_C), String(instantCurrrent_C), String(instantPower_C));
      }
      else
      {
        avail_counter++;
        switch (fault_code_C)
        {
        case -1:
          break; // It means default.
        case 0:
          displayEnergyValues_Disp_AC("3-Over Voltage", String(instantVoltage_A), String(instantCurrrent_A), String(instantPower_A));
          break;
        case 1:
          displayEnergyValues_Disp_AC("3-Under Voltage", String(instantVoltage_A), String(instantCurrrent_A), String(instantPower_A));
          break;
        case 2:
          displayEnergyValues_Disp_AC("3-Over Current", String(instantVoltage_A), String(instantCurrrent_A), String(instantPower_A));
          break;
        case 3:
          displayEnergyValues_Disp_AC("3-Under Current", String(instantVoltage_A), String(instantCurrrent_A), String(instantPower_A));
          break;
        case 4:
          displayEnergyValues_Disp_AC("3-Over Temp", String(instantVoltage_A), String(instantCurrrent_A), String(instantPower_A));

          break;
        case 5:
          displayEnergyValues_Disp_AC("3-Under Temp", String(instantVoltage_A), String(instantCurrrent_A), String(instantPower_A));
          break;
        case 6:
          displayEnergyValues_Disp_AC("3-GFCI", String(instantVoltage_A), String(instantCurrrent_A), String(instantPower_A));
          break;
        case 7:
          displayEnergyValues_Disp_AC("3-Earth Disc", String(instantVoltage_A), String(instantCurrrent_A), String(instantPower_A));
          break;
        default:
          SerialMon.println(F("Default in display"));
        }
      }

      checkForResponse_Disp();
    }
    else
    {
      if (!notFaulty_C || EMGCY_FaultOccured_C)
      {
        avail_counter++;
      }
    }

    break;
  default:
    Serial.println(F("**Display default**"));
    break;
  }
  /*
    @brief: If all 3 are faulted, then set the response to rfid unavailable.
  */
  if (avail_counter == 3)
  {
    setHeader("RFID UNAVAILABLE");
    checkForResponse_Disp();
    avail_counter = 0;
  }

  if (flag_tapped)
  {
    setHeader("TAP RFID TO START/STOP");
    checkForResponse_Disp();
    flag_tapped = false;
  }

  if (flag_unfreeze)
  {
    if (disp_evse_A == false)
    {
      if (disp_evse_B == false)
      {
        if (disp_evse_C == false)
        {
          flag_freeze = false;
          flag_unfreeze = false;
        }
      }
    }
  }

  // if(disp_evse_A || disp_evse_B || disp_evse_C)
  if (flag_freeze)
  {
    Serial.println(F("**skip**"));
  }
  else
  {
    if (isInternetConnected)
    {
      if (notFaulty_A && !EMGCY_FaultOccured_A && !disp_evse_A)
      {
        connAvail(1, "AVAILABLE");
        checkForResponse_Disp();
        setHeader("TAP RFID TO START/STOP");
        checkForResponse_Disp();
      }
      if (notFaulty_B && !EMGCY_FaultOccured_B && !disp_evse_B)
      {
        connAvail(2, "AVAILABLE");
        checkForResponse_Disp();
        setHeader("TAP RFID TO START/STOP");
        checkForResponse_Disp();
      }
      if (notFaulty_C && !EMGCY_FaultOccured_C && !disp_evse_C)
      {
        connAvail(3, "AVAILABLE");
        checkForResponse_Disp();
        setHeader("TAP RFID TO START/STOP");
        checkForResponse_Disp();
      }
    }
  }
}

#endif

#if LCD_DISPLAY_ENABLED
unsigned long onTime = 0;
uint8_t state_timer = 0;
uint8_t disp_evse = 0;
extern bool disp_evse_A;
extern bool disp_evse_B;
extern bool disp_evse_C;
extern bool notFaulty_A;
extern bool notFaulty_B;
extern bool notFaulty_C;
extern int8_t fault_code_A;
extern int8_t fault_code_B;
extern int8_t fault_code_C;
extern bool disp_evse_A_finished;
extern LCD_I2C lcd;
ChargePointStatus inferencedStatus;

uint8_t gu8_state_count_flag = 0;
uint8_t gu8_state_count = 1;

uint8_t evse_display_state = 0;
void stateTimer()
{
  if (gu8_state_count_flag == 0)
  {
    gu8_state_count_flag = 1;
    onTime = millis();
    Cuurent_Status_Disp = EVSE_Display_Read_status();
    Serial.println("EVSE_Display_Read_status():" + String(Cuurent_Status_Disp));
    switch (EVSE_Display_Read_status())
    {
    case NotCharging:
      Serial.println(F("**Display Screen 1**"));
      evse_display_state = 1;
      break;
    case Connector_A:
    case Connector_B:
    case Connector_C:
      Serial.println(F("**Display Screen 2**"));
      evse_display_state = 2;
      break;

    case Connector_A_B:
    case Connector_A_C:
    case Connector_B_C:
      Serial.println(F("**Display Screen 3**"));
      evse_display_state = 3;
      break;

    case Connector_A_B_C:
      Serial.println(F("**Display Screen 4**"));
      evse_display_state = 4;
      break;

    default:
      Serial.println(F("**Display Screen defalut**"));
      evse_display_state = 0;
      break;
    }
  }
  else if (gu8_state_count_flag == 1)
  {
    if ((millis() - onTime) > 3000)
    {
      gu8_state_count_flag = 0;
      gu8_state_count++;
      if (gu8_state_count > evse_display_state)
      {
        gu8_state_count = 1;
      }
    }
  }
  switch (gu8_state_count)
  {
  case 0:
    Serial.println(F("**Display Screen 0**........"));
    break;
  case 1:
    Serial.println(F("**Display Screen 1**..........."));
    disp_lcd_meter();
    break;
  case 2:
    Serial.println(F("**Display Screen 2**............."));
    switch (Cuurent_Status_Disp)
    {
    case Connector_A:     //
    case Connector_A_B:   //
    case Connector_A_C:   //
    case Connector_A_B_C: //
      Serial.println("charging_disp_........A************************");
      charging_disp_A();
      break;
    case Connector_B:   //
    case Connector_B_C: //
      Serial.println("charging_disp_........B************************");
      charging_disp_B();

      break;
    case Connector_C: //
      Serial.println("charging_disp_........C************************");
      charging_disp_C();
      break;
    }
    break;
  case 3:
    switch (Cuurent_Status_Disp)
    {
    case Connector_A_B:   //
    case Connector_A_B_C: //
      Serial.println("charging_disp_........A************************");
      charging_disp_B();
      break;
    case Connector_A_C: //
    case Connector_B_C: //
      Serial.println("charging_disp_........B************************");
      charging_disp_C();
      break;
    }
    Serial.println(F("**Display Screen 3**............"));
    break;
  case 4:
    switch (Cuurent_Status_Disp)
    {
    case Connector_A_B_C: //
      Serial.println("charging_disp_........C************************");
      charging_disp_C();
      break;
    }
    Serial.println(F("**Display Screen 4**.............."));
    break;

  default:

    break;
  }
}

uint8_t Display_Set(uint8_t lu8_Status)
{
  gu8_EVSE_Disp_state = gu8_EVSE_Disp_state | (1 << lu8_Status);
  Serial.println("SET:");
  Serial.println(gu8_EVSE_Disp_state);
  return gu8_EVSE_Disp_state;
}

uint8_t Display_Clear(uint8_t lu8_Status)
{
  gu8_EVSE_Disp_state = gu8_EVSE_Disp_state & ~(1 << lu8_Status);
  Serial.println("CLEAR:");
  Serial.println(gu8_EVSE_Disp_state);
  return gu8_EVSE_Disp_state;
}

uint8_t EVSE_Display_Read_status(void)
{
  return gu8_EVSE_Disp_state;
}

void disp_lcd_meter()
{
  lcd.clear();
  if (webSocketConncted)
  {
    if (isInternetConnected)
    {
      if (wifi_connect)
      {
        // lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
        // lcd.print("                    "); // Clear the line
        lcd.setCursor(15, 0);
        lcd.print("WI-FI");
      }
      if (gsm_connect && client.connected())
      {
        // lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
        // lcd.print("                    "); // Clear the line
        lcd.setCursor(18, 0);
        lcd.print("4G");
      }
      if (ethernet_connect)
      {
        // lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
        // lcd.print("                    "); // Clear the line
        lcd.setCursor(12, 0);
        lcd.print("ETHERNET");
      }
    }
  }
  else
  {
    lcd.setCursor(13, 0);
    lcd.print("OFFLINE");
  }
    lcd.setCursor(0, 1);
    inferencedStatus = getChargePointStatusService_A()->inferenceStatus();
    if (notFaulty_A)
    {
      switch (inferencedStatus)
      {
      case Available:
        lcd.print("OUTPUT 1:AVAILABLE");
        break;
      case Preparing:
        lcd.print("OUTPUT 1:PREPARING");
        break;
      case Charging:
        lcd.print("OUTPUT 1:CHARGING");
        break;
      case Finishing: // not supported by this client
        Serial.print("Finishing");
        lcd.print("OUTPUT 1:FINISHING");
        break;
      case Reserved: // Implemented reserve now
        Serial.print("Reserved");
        lcd.print("OUTPUT 1:RESERVED");
        break;
      case Unavailable: // Implemented Change Availability
        lcd.print("OUTPUT 1:UNAVAILABLE");
        break;
      default:
        lcd.print("OUTPUT 1:AVAILABLE");
        break;
      }
    }
    else
    {
      switch (inferencedStatus)
      {
      case Faulted: // Implemented Faulted.
        disp_fault_code_A();
        break;
      default:
        break;
      }
    }
    lcd.setCursor(0, 2);
    inferencedStatus = getChargePointStatusService_B()->inferenceStatus();
    if (notFaulty_B)
    {
      switch (inferencedStatus)
      {
      case Available:
        lcd.print("OUTPUT 2:AVAILABLE");
        break;
      case Preparing:
        lcd.print("OUTPUT 2:PREPARING");
        break;
      case Charging:
        lcd.print("OUTPUT 2:CHARGING");
        break;
      case Finishing: // not supported by this client
        Serial.print("Finishing");
        lcd.print("OUTPUT 2:FINISHING");
        break;
      case Reserved: // Implemented reserve now
        Serial.print("Reserved");
        lcd.print("OUTPUT 2:RESERVED");
        break;
      case Unavailable: // Implemented Change Availability
        lcd.print("OUTPUT 2:UNAVAILABLE");
        break;
      default:
        lcd.print("OUTPUT 2:AVAILABLE");
        break;
      }
    }
    else
    {
      switch (inferencedStatus)
      {
      case Faulted: // Implemented Faulted.
        disp_fault_code_B();
        break;
      default:
        break;
      }
    }
    lcd.setCursor(0, 3);
    inferencedStatus = getChargePointStatusService_C()->inferenceStatus();
    if (notFaulty_C)
    {
      switch (inferencedStatus)
      {
      case Available:
        lcd.print("OUTPUT 3:AVAILABLE");
        break;
      case Preparing:
        lcd.print("OUTPUT 3:PREPARING");
        break;
      case Charging:
        lcd.print("OUTPUT 3:CHARGING");
        break;
      case Finishing: // not supported by this client
        Serial.print("Finishing");
        lcd.print("OUTPUT 3:FINISHING");
        break;
      case Reserved: // Implemented reserve now
        Serial.print("Reserved");
        lcd.print("OUTPUT 3:RESERVED");
        break;
      case Unavailable: // Implemented Change Availability
        lcd.print("OUTPUT 3:UNAVAILABLE");
        break;
      default:
        lcd.print("OUTPUT 3:AVAILABLE");
        break;
      }
    }
    else
    {
      switch (inferencedStatus)
      {
      case Faulted: // Implemented Faulted.
        disp_fault_code_C();
        break;
      default:
        break;
      }
    }
  
  // else
  // {
    
  //   lcd.clear();
  //   lcd.setCursor(13, 0);
  //   lcd.print("OFFLINE");
  //   if (notFaulty_A && notFaulty_B && notFaulty_C)
  //   {
  //     lcd.setCursor(0, 1);
  //     lcd.print("CHARGER UNAVAILABLE");
  //   }
  //   else if (fault_code_A || fault_code_B || fault_code_C)
  //   {
  //     if (fault_code_A)
  //     {
  //       disp_fault_code_A();
  //     }
  //     if (fault_code_B)
  //     {
  //       disp_fault_code_B();
  //     }
  //     if (fault_code_C)
  //     {
  //       disp_fault_code_C();
  //     }
  //   }
  // }
}

void charging_disp_A(void)
{
  float instantCurrrent_A = eic.GetLineCurrentA();
  int instantVoltage_A = eic.GetLineVoltageA();
  float instantPower_A = 0.0f;

  if (instantCurrrent_A < 0.15)
  {
    instantPower_A = 0;
  }
  else
  {
    instantPower_A = (instantCurrrent_A * instantVoltage_A) / 1000.0;
  }
  if (disp_evse_A)
  {
    Serial.println("charging_disp_A");
    lcd.clear();
    lcd.setCursor(0, 0); // Or setting the cursor in the desired position.
    if (notFaulty_A)
    {
      instantPower_A = (instantCurrrent_A * instantVoltage_A) / 1000.0;
      // session_ongoing_flag = true;
      lcd.print("OUTPUT 1 CHARGING"); // You can make spaces using well... spaces
      lcd.setCursor(0, 1);            // Or setting the cursor in the desired position.
      lcd.print("CHARGING @ ");
      lcd.setCursor(11, 1); // Or setting the cursor in the desired position.
      lcd.print(String(instantPower_A));
      lcd.setCursor(16, 1);
      lcd.print("kW");
      lcd.setCursor(0, 2);
      lcd.print("ENERGY(kWh):  ");
      lcd.setCursor(12, 2); // Or setting the cursor in the desired position.
      if (!offline_charging_A)
      {

        if ((online_charging_Enargy_A - globalmeterstartA) > 0)
        {

          lcd.print(String(discurrEnergy_A));
        }
        else
        {
          lcd.print("0");
        }
        if (gu8_online_flag == 1)
        {
          Serial.println(F("[DISP_METERVALUES]******* online charging***************"));
          Serial.println("online_charging_Enargy_A");
          Serial.println(online_charging_Enargy_A);
        }
        else
        {
          Serial.println(F("[DISP_METERVALUES]******* offline charging***************"));
          Serial.print("offline_charging_Enargy_A");
          Serial.println(online_charging_Enargy_A);
        }
      }
      else
      {
        lcd.print("0.00");
      }
      unsigned long stop_time = millis();
      lcd.setCursor(0, 3);
      lcd.print("TIME       :");
      lcd.setCursor(13, 3); // Or setting the cursor in the desired position.
      unsigned long seconds = (stop_time - st_timeA) / 1000;
      int hr = seconds / 3600;                                                 // Number of seconds in an hour
      int mins = (seconds - hr * 3600) / 60;                                   // Remove the number of hours and calculate the minutes.
      int sec = seconds - hr * 3600 - mins * 60;                               // Remove the number of hours and minutes, leaving only seconds.
      String hrMinSec = (String(hr) + ":" + String(mins) + ":" + String(sec)); // Converts to HH:MM:SS string. This can be returned to the calling function.
      lcd.print(String(hrMinSec));
      // gu8_lcd_chargingflag = 0;
    }
    else
    {
      disp_fault_code_A();
    }
  }
  else if (getChargePointStatusService_A()->inferenceStatus() == ChargePointStatus::Preparing)
  {
    lcd.clear();
    if (isInternetConnected)
    {

      if (wifi_connect)
      {
        // lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
        // lcd.print("                    "); // Clear the line
        lcd.setCursor(15, 0);
        lcd.print("WI-FI");
      }
      if (gsm_connect && client.connected())
      {
        // lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
        // lcd.print("                    "); // Clear the line
        lcd.setCursor(18, 0);
        lcd.print("4G");
      }
      if (ethernet_connect)
      {
        // lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
        // lcd.print("                    "); // Clear the line
        lcd.setCursor(12, 0);
        lcd.print("ETHERNET");
      }
    }
    lcd.setCursor(2, 1);
    lcd.print("AUTHENTICATING");
    lcd.setCursor(5, 2);
    lcd.print("OUTPUT 1");
  }
  else if (getChargePointStatusService_A()->inferenceStatus() == ChargePointStatus::Available)
  {
    if (gu8_Remote_start_A)
    {
      lcd.clear();
      if (isInternetConnected)
      {

        if (wifi_connect)
        {
          // lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
          // lcd.print("                    "); // Clear the line
          lcd.setCursor(15, 0);
        lcd.print("WI-FI");
        }
        if (gsm_connect && client.connected())
        {
          // lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
          // lcd.print("                    "); // Clear the line
          lcd.setCursor(18, 0);
          lcd.print("4G");
        }
        if (ethernet_connect)
        {
          // lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
          // lcd.print("                    "); // Clear the line
          lcd.setCursor(12, 0);
          lcd.print("ETHERNET");
        }
      }
      lcd.setCursor(2, 1);
      lcd.print("AUTHENTICATION");
      lcd.setCursor(2, 2);
      lcd.print("SUCCESSFUL 1");
    }
  }
}

void charging_disp_B(void)
{
  float instantCurrrent_B = eic.GetLineCurrentB();
  int instantVoltage_B = eic.GetLineVoltageB();
  float instantPower_B = 0.0f;

  if (instantCurrrent_B < 0.15)
  {
    instantPower_B = 0;
  }
  else
  {
    instantPower_B = (instantCurrrent_B * instantVoltage_B) / 1000.0;
  }

  if (disp_evse_B)
  {
    Serial.println("charging_disp_B.........##############################");
    lcd.clear();
    lcd.setCursor(0, 0); // Or setting the cursor in the desired position.
    if (notFaulty_B)
    {
      instantPower_B = (instantCurrrent_B * instantVoltage_B) / 1000.0;
      // session_ongoing_flag = true;
      lcd.print("OUTPUT 2 CHARGING"); // You can make spaces using well... spaces
      lcd.setCursor(0, 1);            // Or setting the cursor in the desired position.
      lcd.setCursor(0, 1);            // Or setting the cursor in the desired position.
      lcd.print("CHARGING @ ");
      lcd.setCursor(11, 1); // Or setting the cursor in the desired position.
      lcd.print(String(instantPower_B));
      lcd.setCursor(16, 1);
      lcd.print("kW");
      lcd.setCursor(0, 2);
      lcd.print("ENERGY(kWh):  ");
      lcd.setCursor(12, 2); // Or setting the cursor in the desired position.

      if (!offline_charging_B)
      {

        if ((online_charging_Enargy_B - globalmeterstartB) > 0)
        {

          lcd.print(String(discurrEnergy_B));
        }
        else
        {
          lcd.print("0");
        }
        if (gu8_online_flag == 1)
        {
          Serial.println(F("[DISP_METERVALUES]******* online charging***************"));
          Serial.println("online_charging_Enargy_B");
          Serial.println(online_charging_Enargy_B);
        }
        else
        {
          Serial.println(F("[DISP_METERVALUES]******* offline charging***************"));
          Serial.print("offline_charging_Enargy_B");
          Serial.println(online_charging_Enargy_B);
        }
      }
      else
      {
        lcd.print("0.00");
      }
      unsigned long stop_time = millis();
      lcd.setCursor(0, 3);
      lcd.print("TIME       :");
      lcd.setCursor(13, 3); // Or setting the cursor in the desired position.
      unsigned long seconds = (stop_time - st_timeB) / 1000;
      int hr = seconds / 3600;                                                 // Number of seconds in an hour
      int mins = (seconds - hr * 3600) / 60;                                   // Remove the number of hours and calculate the minutes.
      int sec = seconds - hr * 3600 - mins * 60;                               // Remove the number of hours and minutes, leaving only seconds.
      String hrMinSec = (String(hr) + ":" + String(mins) + ":" + String(sec)); // Converts to HH:MM:SS string. This can be returned to the calling function.
      lcd.print(String(hrMinSec));
      // gu8_lcd_chargingflag = 0;
    }
    else
    {

      disp_fault_code_B();
    }
  }
  else if (getChargePointStatusService_B()->inferenceStatus() == ChargePointStatus::Preparing)
  {
    lcd.clear();
    if (isInternetConnected)
    {

      if (wifi_connect)
      {
        // lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
        // lcd.print("                    "); // Clear the line
        lcd.setCursor(15, 0);
        lcd.print("WI-FI");
      }
      if (gsm_connect && client.connected())
      {
        // lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
        // lcd.print("                    "); // Clear the line
        lcd.setCursor(18, 0);
        lcd.print("4G");
      }
      if (ethernet_connect)
      {
        // lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
        // lcd.print("                    "); // Clear the line
        lcd.setCursor(12, 0);
        lcd.print("ETHERNET");
      }
    }
    lcd.setCursor(2, 1);
    lcd.print("AUTHENTICATING");
    lcd.setCursor(5, 2);
    lcd.print("OUTPUT 2");
  }
  else if (getChargePointStatusService_B()->inferenceStatus() == ChargePointStatus::Available)
  {
    if (gu8_Remote_start_B)
    {
      lcd.clear();
      if (isInternetConnected)
      {

        if (wifi_connect)
        {
          // lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
          // lcd.print("                    "); // Clear the line
          lcd.setCursor(15, 0);
        lcd.print("WI-FI");
        }
        if (gsm_connect && client.connected())
        {
          // lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
          // lcd.print("                    "); // Clear the line
          lcd.setCursor(18, 0);
          lcd.print("4G");
        }
        if (ethernet_connect)
        {
          // lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
          // lcd.print("                    "); // Clear the line
          lcd.setCursor(12, 0);
          lcd.print("ETHERNET");
        }
      }
      lcd.setCursor(2, 1);
      lcd.print("AUTHENTICATION");
      lcd.setCursor(2, 2);
      lcd.print("SUCCESSFUL 2");
    }
  }
}

void charging_disp_C(void)
{

  float instantCurrrent_C = eic.GetLineCurrentC();
  int instantVoltage_C = eic.GetLineVoltageC();
  float instantPower_C = 0.0f;

  if (instantCurrrent_C < 0.15)
  {
    instantPower_C = 0;
  }
  else
  {
    instantPower_C = (instantCurrrent_C * instantVoltage_C) / 1000.0;
  }

  if (disp_evse_C)
  {
    Serial.println("charging_disp_C");
    lcd.clear();
    lcd.setCursor(0, 0); // Or setting the cursor in the desired position.
    if (notFaulty_C)
    {
      instantPower_C = (instantCurrrent_C * instantVoltage_C) / 1000.0;
      // session_ongoing_flag = true;
      lcd.print("OUTPUT 3 CHARGING"); // You can make spaces using well... spaces
      lcd.setCursor(0, 1);            // Or setting the cursor in the desired position.
      lcd.print("CHARGING @ ");
      lcd.setCursor(11, 1); // Or setting the cursor in the desired position.
      lcd.print(String(instantPower_C));
      lcd.setCursor(16, 1);
      lcd.print("kW");
      lcd.setCursor(0, 2);
      lcd.print("ENERGY(kWh):  ");
      lcd.setCursor(12, 2); // Or setting the cursor in the desired position.
      if (!offline_charging_C)
      {
        if ((online_charging_Enargy_C - globalmeterstartC) > 0)
        {
          lcd.print(String(discurrEnergy_C));
        }
        else
        {
          lcd.print("0");
        }
        if (gu8_online_flag == 1)
        {
          Serial.println("online_charging_Enargy_C");
          Serial.println(online_charging_Enargy_C);
        }
        else
        {
          Serial.print("offline_charging_Enargy_C");
          Serial.println(online_charging_Enargy_C);
        }
      }
      else
      {
        lcd.print("0.00");
      }
      unsigned long stop_time = millis();
      lcd.setCursor(0, 3);
      lcd.print("TIME       :");
      lcd.setCursor(13, 3); // Or setting the cursor in the desired position.
      unsigned long seconds = (stop_time - st_timeC) / 1000;
      int hr = seconds / 3600;                                                 // Number of seconds in an hour
      int mins = (seconds - hr * 3600) / 60;                                   // Remove the number of hours and calculate the minutes.
      int sec = seconds - hr * 3600 - mins * 60;                               // Remove the number of hours and minutes, leaving only seconds.
      String hrMinSec = (String(hr) + ":" + String(mins) + ":" + String(sec)); // Converts to HH:MM:SS string. This can be returned to the calling function.
      lcd.print(String(hrMinSec));
      // gu8_lcd_chargingflag = 0;
    }

    else
    {

      disp_fault_code_C();
    }
  }
  else if (getChargePointStatusService_C()->inferenceStatus() == ChargePointStatus::Preparing)
  {
    lcd.clear();
    if (isInternetConnected)
    {

      if (wifi_connect)
      {
        // lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
        // lcd.print("                    "); // Clear the line
        lcd.setCursor(15, 0);
        lcd.print("WI-FI");
      }
      if (gsm_connect && client.connected())
      {
        // lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
        // lcd.print("                    "); // Clear the line
        lcd.setCursor(18, 0);
        lcd.print("4G");
      }
      if (ethernet_connect)
      {
        // lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
        // lcd.print("                    "); // Clear the line
        lcd.setCursor(12, 0);
        lcd.print("ETHERNET");
      }
    }
    lcd.setCursor(2, 1);
    lcd.print("AUTHENTICATING");
    lcd.setCursor(5, 2);
    lcd.print("OUTPUT 3");
  }
  else if (getChargePointStatusService_C()->inferenceStatus() == ChargePointStatus::Available)
  {
    if (gu8_Remote_start_C)
    {
      lcd.clear();
      if (isInternetConnected)
      {

        if (wifi_connect)
        {
          // lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
          // lcd.print("                    "); // Clear the line
          lcd.setCursor(15, 0);
        lcd.print("WI-FI");
        }
        if (gsm_connect && client.connected())
        {
          // lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
          // lcd.print("                    "); // Clear the line
          lcd.setCursor(18, 0);
          lcd.print("4G");
        }
        if (ethernet_connect)
        {
          // lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
          // lcd.print("                    "); // Clear the line
          lcd.setCursor(12, 0);
          lcd.print("ETHERNET");
        }
      }
      lcd.setCursor(2, 1);
      lcd.print("AUTHENTICATION");
      lcd.setCursor(2, 2);
      lcd.print("SUCCESSFUL 3");
    }
  }
}

void disp_fault_code_A(void)
{
  int instantVoltage_A = eic.GetLineVoltageA();
  switch (fault_code_A)
  {
  case -1:
    break; // It means default.
  case 0:
    lcd.setCursor(0, 1);
    lcd.print("OUTPUT 1:");
    lcd.setCursor(9, 1);
    lcd.print("O.VOLTAGE");
    break;
  case 1:
    if (instantVoltage_A > 20)
    {
      lcd.setCursor(0, 1);
      lcd.print("OUTPUT 1:");
      lcd.setCursor(9, 1);
      lcd.print("U.VOLTAGE");
    }

    else
    {
      lcd.setCursor(0, 1);
      lcd.print(" CHARGER FAULTED    ");
      lcd.setCursor(0, 2);
      lcd.print("   POWER FAILURE    ");
      lcd.setCursor(0,3);
    lcd.print("                    ");
    }

    break;
  case 2:
    lcd.setCursor(0, 1);
    lcd.print("OUTPUT 1:");
    lcd.setCursor(9, 1);
    lcd.print("O.CURRENT");
    break;
  case 3:
    lcd.setCursor(0, 1);
    lcd.print("OUTPUT 1:");
    lcd.setCursor(9, 1);
    lcd.print("U.CURRENT");
    break;
  case 4:
    lcd.setCursor(0, 1);
    lcd.print("OUTPUT 1:");
    lcd.setCursor(9, 1);
    lcd.print("OVER TEMP");
    break;
  case 5:
    lcd.setCursor(0, 1);
    lcd.print("OUTPUT 1:");
    lcd.setCursor(9, 1);
    lcd.print("UNDER TEMP");
    break;
  case 6:
    lcd.setCursor(0, 1);
    lcd.print("OUTPUT 1:");
    lcd.setCursor(9, 1);
    lcd.print("GFCI"); // Not implemented in AC001
    break;
  case 7:
    lcd.setCursor(0, 1);
    lcd.print(" CHARGER FAULTED    ");
    lcd.setCursor(0, 2);
    lcd.print("EARTH DISCONNECTED");
    lcd.setCursor(0,3);
    lcd.print("                    ");
    break;
  case 8:
    lcd.setCursor(0, 1);
    lcd.print(" CHARGER FAULTED    ");
    lcd.setCursor(0, 2);
    lcd.print("     EMERGENCY      ");
    lcd.setCursor(0,3);
    lcd.print("                    ");
    break;
  case 9:
    lcd.setCursor(0, 1);
    lcd.print(" CHARGER FAULTED    ");
    lcd.setCursor(0, 2);
    lcd.print("   POWER FAILURE    ");
    lcd.setCursor(0,3);
    lcd.print("                    ");
    break;
  }
}

void disp_fault_code_B(void)
{
    int instantVoltage_B = eic.GetLineVoltageB();

  switch (fault_code_B)
  {
  case -1:
    break; // It means default.
  case 0:
    lcd.setCursor(0, 2);
    lcd.print("OUTPUT 2:");
    lcd.setCursor(9, 2);
    lcd.print("O.VOLTAGE");
    break;
  case 1:
    if (instantVoltage_B > 20)
    {
      lcd.setCursor(0, 2);
      lcd.print("OUTPUT 2:");
      lcd.setCursor(9, 2);
      lcd.print("U.VOLTAGE");
    }

    else
    {

      lcd.setCursor(0, 1);
      lcd.print(" CHARGER FAULTED    ");
      lcd.setCursor(0, 2);
      lcd.print("   POWER FAILURE    ");
      lcd.setCursor(0,3);
    lcd.print("                    ");
    }

    break;
  case 2:
    lcd.setCursor(0, 2);
    lcd.print("OUTPUT 2:");
    lcd.setCursor(9, 2);
    lcd.print("O.CURRENT");
    break;
  case 3:
    lcd.setCursor(0, 2);
    lcd.print("OUTPUT 2:");
    lcd.setCursor(9, 2);
    lcd.print("U.CURRENT");
    break;
  case 4:
    lcd.setCursor(0, 2);
    lcd.print("OUTPUT 2:");
    lcd.setCursor(9, 2);
    lcd.print("OVER TEMP");
    break;
  case 5:
    lcd.setCursor(0, 2);
    lcd.print("OUTPUT 2:");
    lcd.setCursor(9, 2);
    lcd.print("UNDER TEMP");
    break;
  case 6:
    lcd.setCursor(0, 2);
    lcd.print("OUTPUT 2:");
    lcd.setCursor(9, 2);
    lcd.print("GFCI"); // Not implemented in AC001
    break;
  case 7:

    lcd.setCursor(0, 1);
    lcd.print(" CHARGER FAULTED    ");
    lcd.setCursor(0, 2);
    lcd.print("EARTH DISCONNECTED");
    lcd.setCursor(0,3);
    lcd.print("                    ");
    break;
  case 8:
    lcd.setCursor(0, 1);
    lcd.print(" CHARGER FAULTED    ");
    lcd.setCursor(0, 2);
    lcd.print("     EMERGENCY      ");
    lcd.setCursor(0,3);
    lcd.print("                    ");
    break;
  case 9:
    lcd.setCursor(0, 1);
   lcd.print(" CHARGER FAULTED    ");
    lcd.setCursor(0, 2);
    lcd.print("   POWER FAILURE    ");
    lcd.setCursor(0,3);
    lcd.print("                    ");
    break;
  }
}

void disp_fault_code_C(void)
{
    int instantVoltage_C = eic.GetLineVoltageC();

  switch (fault_code_C)
  {
  case -1:
    break; // It means default.
  case 0:
    lcd.setCursor(0, 3);
    lcd.print("OUTPUT 3:");
    lcd.setCursor(9, 3);
    lcd.print("O.VOLTAGE");
    break;
  case 1:
    if (instantVoltage_C > 20)
    {
      lcd.setCursor(0, 3);
      lcd.print("OUTPUT 3:");
      lcd.setCursor(9, 3);
      lcd.print("U.VOLTAGE");
    }

    else
    {
      lcd.setCursor(0, 1);
      lcd.print(" CHARGER FAULTED    ");
      lcd.setCursor(0, 2);
      lcd.print("   POWER FAILURE    ");
      lcd.setCursor(0,3);
    lcd.print("                    ");
    }

    break;
  case 2:
    lcd.setCursor(0, 3);
    lcd.print("OUTPUT 3:");
    lcd.setCursor(9, 3);
    lcd.print("O.CURRENT");
    break;
  case 3:
    lcd.setCursor(0, 3);
    lcd.print("OUTPUT 3:");
    lcd.setCursor(9, 3);
    lcd.print("U.CURRENT");
    break;
  case 4:
    lcd.setCursor(0, 3);
    lcd.print("OUTPUT 3:");
    lcd.setCursor(9, 3);
    lcd.print("OVER TEMP");
    break;
  case 5:
    lcd.setCursor(0, 3);
    lcd.print("OUTPUT 3:");
    lcd.setCursor(9, 3);
    lcd.print("UNDER TEMP");
    break;
  case 6:
    lcd.setCursor(0, 3);
    lcd.print("OUTPUT 3:");
    lcd.setCursor(9, 3);
    lcd.print("GFCI"); // Not implemented in AC001
    break;
  case 7:

    lcd.setCursor(0, 1);
    lcd.print(" CHARGER FAULTED    ");
    lcd.setCursor(0, 2);
    lcd.print("EARTH DISCONNECTED");
    lcd.setCursor(0,3);
    lcd.print("                    ");
    break;
  case 8:
    lcd.setCursor(0, 1);
    lcd.print(" CHARGER FAULTED    ");
    lcd.setCursor(0, 2);
    lcd.print("     EMERGENCY      ");
    lcd.setCursor(0,3);
    lcd.print("                    ");
    break;
  case 9:
    lcd.setCursor(0, 1);
    lcd.print(" CHARGER FAULTED    ");
    lcd.setCursor(0, 2);
    lcd.print("   POWER FAILURE    ");
    lcd.setCursor(0,3);
    lcd.print("                    ");
    break;
  }
}

#endif