// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

/**
Edited by Pulkit Agrawal.
*/

#include "EVSE_A.h"
#include "Master.h"
#include "EVSE_A_Offline.h"
#include "MeteringService.h"
#include "OcppEngine.h"
#include <Preferences.h>
#include "display_meterValues.h"
#include "ControlPilot.h"

#include "LCD_I2C.h"

#if DISPLAY_ENABLED
#include "display.h"
extern bool flag_tapped;
#endif

extern int8_t fault_code_A;

#if DWIN_ENABLED
#include "dwin.h"

extern unsigned char v1[8];
extern unsigned char v2[8];
extern unsigned char v3[8];
extern unsigned char i1[8];
extern unsigned char i2[8];
extern unsigned char i3[8];
extern unsigned char e1[8];
extern unsigned char e2[8];
extern unsigned char e3[8];
extern unsigned char change_page[10];
extern unsigned char avail[22];
extern unsigned char charging[28];
extern unsigned char cid1[8];
extern unsigned char fault_emgy[28];
#endif

#if 0
//new variable names defined by @Pulkit. might break the build.
OnBoot_A onBoot_A;
OnReadUserId_A onReadUserId_A;
OnSendHeartbeat_A onSendHeartbeat_A;
OnAuthentication_A onAuthentication_A;
OnStartTransaction_A onStartTransaction_A;
OnStopTransaction_A onStopTransaction_A;
OnUnauthorizeUser_A onUnauthorizeUser_A;
#endif

extern bool disp_evse_A;

extern ulong timerHb;
extern unsigned int heartbeatInterval;

extern bool notFaulty_A;

// timeout for heartbeat signal.
extern ulong T_SENDHEARTBEAT;
extern bool timeout_active_A;
extern bool timer_initialize_A;
extern ulong timeout_start_A;
// Reason for stop
extern uint8_t reasonForStop_A;
// new flag names. replace them with old names.
extern bool evIsPlugged_A;
extern bool flag_evseIsBooted_A;
extern bool flag_evseReadIdTag_A;
extern bool flag_evseAuthenticate_A;
extern bool flag_evseStartTransaction_A;
extern bool flag_evRequestsCharge_A;
extern bool flag_evseStopTransaction_A;
extern bool flag_evseUnauthorise_A;
extern bool flag_rebootRequired_A;
extern bool flag_evseSoftReset_A; // added by @Wamique

extern float minCurr;

extern ATM90E36 eic;
extern bool flag_rebootRequired_B;
extern bool flag_rebootRequired_C;

// not used. part of Smart Charging System.
extern float chargingLimit_A;
String Ext_currentIdTag_A_Offl = "";
extern WebSocketsClient webSocket;

extern ulong timerDisplay;
extern bool EMGCY_FaultOccured;

extern LCD_I2C lcd;

extern MFRC522 mfrc522;
// extern String currentIdTag;
extern long int blinckCounter_A;
extern int counter1_A;
extern ulong t;
ulong t_A;
extern int connectorDis_counter_A;
extern String currentIdTag;
String currentIdTag_A_Offl = "";

extern EVSE_states_enum EVSE_state;
extern Preferences preferences;
extern short int fault_counter_A;
extern bool flag_faultOccured_A;
extern short int counter_drawingCurrent_A;
extern float drawing_current_A;

extern bool webSocketConncted;
extern bool isInternetConnected;
extern short counter_faultstate_A;

// metering Flag
extern bool flag_MeteringIsInitialised;
extern MeteringService *meteringService;
extern bool wifi_connect;
extern bool gsm_connect;
extern bool offline_connect;
extern bool flag_internet;
extern bool flag_offline;

extern Preferences resumeTxn_A;
extern TinyGsmClient client;

extern bool ongoingTxn_A;
extern String idTagData_A;

extern ulong relay_timer_A;
extern ulong faultTimer_A;

extern short EMGCY_counter_A;
extern bool EMGCY_FaultOccured_A;

unsigned long timer_green_offline = 0;
extern time_t lastsampledTimeA_off;

extern uint8_t currentCounterThreshold_A;
extern int globalmeterstartA;
extern unsigned long st_timeA;
const ulong TIMEOUT_EMERGENCY_RELAY_CLOSE_A = 120000;
uint8_t offline_charging_A = 0;
ulong timer_Offl_A;
bool flag_txndone_off_A = false;
extern bool flag_txndone_off_B;
extern bool flag_txndone_off_C;

float offline_charging_Enargy_A = 0;

extern unsigned long offline_t_A;

extern volatile bool session_ongoing_flag;

extern Preferences energymeter;

extern uint8_t online_session_ongoing_A;
extern uint8_t session_ongoing_A_count;

extern String Tag_Id_A;
extern String Transaction_Id_A;
extern String Start_Meter_Value_A;

extern String Tag_Id_B;
extern String Transaction_Id_B;
extern String Start_Meter_Value_B;

extern String Tag_Id_C;
extern String Transaction_Id_C;
extern String Start_Meter_Value_C;

extern float gf32meterStop_A;
extern float gf32meterStop_B;
extern float gf32meterStop_C;
extern uint8_t gu8_Remote_start_A;
extern uint8_t Stop_Session_A;
extern uint8_t start_session_A;
extern volatile bool session_ongoing_flag_A;

bool ongoingTxnoff_A = false;

// extern uint8_t ongoingTxn_A_stop;
/****************New Offline Functions********************************/

void EVSE_A_stopOfflineTxn()
{
  Serial.println("EVSE_A_stopOfflineTxn....!");
  disp_evse_A = false;
  gu8_Remote_start_A = 0;
  Stop_Session_A = 0;
  Display_Clear(Stop_Session_A);
  requestForRelay(STOP, 1);
  getChargePointStatusService_A()->setChargePointstatus(Available);	
  session_ongoing_flag = false;
  session_ongoing_A_count = 0;
if(session_ongoing_flag_A == true)
{
  flag_evseReadIdTag_A = false;
	flag_evseAuthenticate_A = false;
	flag_evseStartTransaction_A = false;
	flag_evRequestsCharge_A = false;
	flag_evseStopTransaction_A = true;
	flag_evseUnauthorise_A = false;
}
else
{
  flag_evseReadIdTag_A = true;
  flag_evseAuthenticate_A = false;
  flag_evseStartTransaction_A = false;
  flag_evRequestsCharge_A = false;
  flag_evseStopTransaction_A = false;
  flag_evseUnauthorise_A = false;
}

#if 0
  flag_evseReadIdTag_A = true;
  flag_evseAuthenticate_A = false;
  flag_evseStartTransaction_A = false;
  flag_evRequestsCharge_A = false;
  flag_evseStopTransaction_A = false;
  flag_evseUnauthorise_A = false;
#endif

  online_session_ongoing_A = 0;

  getChargePointStatusService_A()->stopEvDrawsEnergy();
  getChargePointStatusService_A()->unauthorize();
  // if (getChargePointStatusService_A()->getEmergencyRelayClose() == false)
  // {
  //   // requestLed(GREEN, START, 1);
  // }
  #if 0
  resumeTxn_A.begin("resume_A", false); // opening preferences in R/W mode
  resumeTxn_A.putBool("ongoingTxn_A", false);
  resumeTxn_A.putString("idTagData_A", "");
  resumeTxn_A.putInt("TxnIdData_A", -1);
  resumeTxn_A.end();
  #endif
  // if(!offline_charging_A)
  // {
  // resumeTxn_A.begin("resume_A", false); // opening preferences in R/W mode
  // resumeTxn_A.putBool("ongoingTxnoff_A", true);
  // resumeTxn_A.end();
  // }
  // Display transaction finished
  energymeter.begin("MeterData", false);
  float meterStop = energymeter.getFloat("currEnergy_A", 0);
  energymeter.end();
  unsigned long stop_time = millis();
  
  flag_txndone_off_A = true;
#if LCD_DISPLAY_ENABLED
   lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("OUTPUT 1 COMPLETED");
  lcd.setCursor(0, 1);
  lcd.print("THANKS FOR CHARGING");
  lcd.setCursor(0, 2);
  lcd.print("ENERGY(KWH):");
    lcd.setCursor(12, 2); // Or setting the cursor in the desired position.
  lcd.print(String(float((meterStop - globalmeterstartA) / 1000)));
  lcd.setCursor(0, 3);
  lcd.print("TIME      :");
        lcd.setCursor(12, 3); // Or setting the cursor in the desired position.
        unsigned long seconds = (stop_time - st_timeA) / 1000;
        int hr = seconds / 3600;                                                 // Number of seconds in an hour
        int mins = (seconds - hr * 3600) / 60;                                   // Remove the number of hours and calculate the minutes.
        int sec = seconds - hr * 3600 - mins * 60;                               // Remove the number of hours and minutes, leaving only seconds.
        String hrMinSec = (String(hr) + ":" + String(mins) + ":" + String(sec)); // Converts to HH:MM:SS string. This can be returned to the calling function.
        lcd.print(String(hrMinSec));
        delay(5000);
#endif
return;
}

#if 0
void showTxn_Finish()
{
  if(flag_txndone_off_A)
  {
    flag_txndone_off_A = false;
#if LCD_ENABLED
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TRANSACTION FINISHED");
  lcd.setCursor(0, 1);
  lcd.print("KWH       :");
  lcd.setCursor(12, 1); // Or setting the cursor in the desired position.
  lcd.print(String(float((meterStop - globalmeterstartA) / 1000)));
  lcd.setCursor(0, 3);
  lcd.print("DURATION  :");
  lcd.setCursor(12, 3); // Or setting the cursor in the desired position.
  unsigned long seconds = (stop_time - st_timeA) / 1000;
  int hr = seconds / 3600;                                                 // Number of seconds in an hour
  int mins = (seconds - hr * 3600) / 60;                                   // Remove the number of hours and calculate the minutes.
  int sec = seconds - hr * 3600 - mins * 60;                               // Remove the number of hours and minutes, leaving only seconds.
  String hrMinSec = (String(hr) + ":" + String(mins) + ":" + String(sec)); // Converts to HH:MM:SS string. This can be returned to the calling function.
  lcd.print(String(hrMinSec));
  delay(5000);
#endif
  }
  if(flag_txndone_off_B)
  {
    flag_txndone_off_B = false;
#if LCD_ENABLED
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TRANSACTION FINISHED");
  lcd.setCursor(0, 1);
  lcd.print("KWH       :");
  lcd.setCursor(12, 1); // Or setting the cursor in the desired position.
  lcd.print(String(float((meterStop-globalmeterstartB)/1000)));
  lcd.setCursor(0,3);
  lcd.print("DURATION  :");
  lcd.setCursor(12, 3); // Or setting the cursor in the desired position.
  unsigned long seconds = (stop_time - st_timeB) / 1000;
  int hr = seconds/3600;                                                        //Number of seconds in an hour
  int mins = (seconds-hr*3600)/60;                                              //Remove the number of hours and calculate the minutes.
  int sec = seconds-hr*3600-mins*60;                                            //Remove the number of hours and minutes, leaving only seconds.
  String hrMinSec = (String(hr) + ":" + String(mins) + ":" + String(sec));  //Converts to HH:MM:SS string. This can be returned to the calling function.
  lcd.print(String(hrMinSec));
   delay(5000);
#endif
  }
   if(flag_txndone_off_C)
  {
    flag_txndone_off_C = false;
#if LCD_ENABLED
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TRANSACTION FINISHED");
  lcd.setCursor(0, 1);
  lcd.print("KWH       :");
  lcd.setCursor(12, 1); // Or setting the cursor in the desired position.
  lcd.print(String(float((meterStop - globalmeterstartC) / 1000)));
  lcd.setCursor(0, 3);
  lcd.print("DURATION  :");
  lcd.setCursor(12, 3); // Or setting the cursor in the desired position.
  unsigned long seconds = (stop_time - st_timeC) / 1000;
  int hr = seconds / 3600;                                                 // Number of seconds in an hour
  int mins = (seconds - hr * 3600) / 60;                                   // Remove the number of hours and calculate the minutes.
  int sec = seconds - hr * 3600 - mins * 60;                               // Remove the number of hours and minutes, leaving only seconds.
  String hrMinSec = (String(hr) + ":" + String(mins) + ":" + String(sec)); // Converts to HH:MM:SS string. This can be returned to the calling function.
  lcd.print(String(hrMinSec));
  delay(5000);
#endif
  }
}
#endif

void EVSE_A_startOfflineTxn()
{
  offline_charging_A = 1;
  disp_evse_A = true;
  start_session_A = 0;
	Display_Set(start_session_A);
  requestForRelay(START, 1);
  getChargePointStatusService_A()->setChargePointstatus(Charging);
  session_ongoing_flag = true;
  Serial.println("Offline[EVSE_A] EV is connected and Started charging");
  if (DEBUG_OUT)
    Serial.println("[EVSE] Started Drawing Energy");
  st_timeA = millis();
  offline_t_A = millis();
  lastsampledTimeA_off = now();
  energymeter.begin("MeterData", false);
  globalmeterstartA = energymeter.getFloat("currEnergy_A", 0);
  energymeter.end();
  Serial.println("[EVSE] globalmeterstartA : " + String(globalmeterstartA));
  Transaction_Id_A = "0xFFFF";
  online_session_ongoing_A = 1;
  session_ongoing_A_count = 1;
}

void EVSE_A_offline_Loop()
{
  if (offline_charging_A)
  {
    drawing_current_A = eic.GetLineCurrentA();
    if (drawing_current_A <= minCurr)
    {
      Serial.println("count min current :" + counter_drawingCurrent_A);
      counter_drawingCurrent_A++;
      Serial.println("counter_drawingCurrent_A offline: " + (String)counter_drawingCurrent_A);

      if (counter_drawingCurrent_A > currentCounterThreshold_A)
      {
        counter_drawingCurrent_A = 0;
        offline_charging_A = 0;
        Serial.println("Stopping Offline Charging by low current");
        EVSE_A_stopOfflineTxn();
        disp_evse_A = false;
        gu8_Remote_start_A = 0;
      Stop_Session_A = 0;
      Display_Clear(Stop_Session_A);
#if LCD_ENABLED
        lcd.clear();
        lcd.setCursor(3, 0);
        lcd.print("No Power Drawn /");
        lcd.setCursor(3, 1);
        lcd.print("EV disconnected");
#endif
      }
    }
    else
    {
      counter_drawingCurrent_A = 0;
      // currentCounterThreshold_A = 10; // ARAI expects 2
      Serial.println("counter_drawingCurrent Reset");
    }

    emergencyRelayClose_Loop_A();

    if (DEBUG_OUT)
      Serial.println("[EVSE] Drawing Energy");

    if (millis() - t_A > 5000)
    {
      float instantCurrrent_A = eic.GetLineCurrentA();
      float instantVoltage_A = eic.GetLineVoltageA();

      if (getChargePointStatusService_A()->getEmergencyRelayClose() == true)
      {
        offline_charging_A = 0;
        Serial.println("Stopping Offline Charging by emergency");
        EVSE_A_stopOfflineTxn();
        disp_evse_A = false;
        gu8_Remote_start_A = 0;
      Stop_Session_A = 0;
      Display_Clear(Stop_Session_A);
      }

      if (millis() - relay_timer_A > 15000 && offline_charging_A == true)
      {
        // requestForRelay(START, 1);
        session_ongoing_flag = true;
        relay_timer_A = millis();
      }

      time_t sampledTimeA_off = now();
      time_t delta = sampledTimeA_off - lastsampledTimeA_off;
      energymeter.begin("MeterData", false);
      float lastEnergy_A = energymeter.getFloat("currEnergy_A", 0);
      #if 0
      float finalEnergy_A = lastEnergy_A + ((float)(instantVoltage_A * instantCurrrent_A * ((float)delta))) / 3600; // Whr
      // placing energy value back in EEPROM
      offline_charging_Enargy_A = finalEnergy_A;
      energymeter.putFloat("currEnergy_A", finalEnergy_A);
      Serial.println("[EnergyASampler] currEnergy_A: " + String(finalEnergy_A));
      #endif
      energymeter.end();

      lastsampledTimeA_off = sampledTimeA_off;
    }
  }
}

void EVSE_A_LED_loop()
{

  // if not faulted and not charging then take the led status to green once every 8 seconds

  if (getChargePointStatusService_A()->getEmergencyRelayClose() == false && offline_charging_A == false)
  {
    if (millis() - timer_green_offline > 8000)
    {
      timer_green_offline = millis();
      // requestLed(GREEN, START, 1);
#if LCD_ENABLED
      lcd.clear();
      lcd.setCursor(0, 0); // Or setting the cursor in the desired position.
      lcd.print("STATUS: AVAILABLE");
      lcd.setCursor(0, 1);
      lcd.print("TAP RFID/SCAN QR");
      lcd.setCursor(0, 2);
      lcd.print("CONNECTION");
      lcd.setCursor(0, 3);
      lcd.print("CLOUD: OFFLINE");
#endif
    }
  }
}
