// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#include "Variants.h"

#include "StopTransaction.h"
#include "OcppEngine.h"
#include "MeteringService.h"
#include "display_meterValues.h"

extern bool flag_send_stop_A;
extern bool flag_send_stop_B;
extern bool flag_send_stop_C;

extern uint8_t Stop_Session_A;
extern uint8_t Stop_Session_B;
extern uint8_t Stop_Session_C;

#if DISPLAY_ENABLED
#include "display.h"
extern bool flag_unfreeze;
#endif
#if LCD_DISPLAY_ENABLED
#include "LCD_I2C.h"
extern LCD_I2C lcd;
#endif
#if DWIN_ENABLED
#include "dwin.h"

extern unsigned char kwh[8];
extern unsigned char change_page[10];
extern unsigned char HR[8];
extern unsigned char MINS[8];
extern unsigned char SEC[8];
extern unsigned char cid1[8];
extern unsigned char cid2[8];
extern unsigned char cid3[8];
#endif

extern bool disp_evse_A;
extern bool disp_evse_B;
extern bool disp_evse_C;

extern bool flag_start_txn_A;
extern bool flag_start_txn_B;
extern bool flag_start_txn_C;

extern int globalmeterstartA;
extern unsigned long st_timeA;
extern int globalmeterstartB;
extern unsigned long st_timeB;
extern int globalmeterstartC;
extern unsigned long st_timeC;

bool disp_evse_A_finished;
/*
 * @brief: Feature added by Raja
 * This feature will avoid hardcoding of messages.
 */
// typedef enum resonofstop { EmergencyStop, EVDisconnected , HardReset, Local , Other , PowerLoss, Reboot,Remote, Softreset,UnlockCommand,DeAuthorized};
extern uint8_t reasonForStop_A;
extern uint8_t reasonForStop_B;
extern uint8_t reasonForStop_C;
static const char *resonofstop_str[] = {"EmergencyStop", "EVDisconnected", "HardReset", "Local", "Other", "PowerLoss", "Reboot", "Remote", "SoftReset", "UnlockCommand", "DeAuthorized"};

// extern uint8_t currentCounterThreshold_A;
// extern uint8_t currentCounterThreshold_B;
// extern uint8_t currentCounterThreshold_C;

extern bool reservation_start_flag_A;
extern bool reservation_start_flag_B;
extern bool reservation_start_flag_C;

extern int gs32_offlinestarttxnId;
extern int gs32_offlinestarttxnId_A;
extern int gs32_offlinestarttxnId_B;
extern int gs32_offlinestarttxnId_C;

extern int gs32_offlinestartStoptxn_update_flag;
extern int gs32_offlinestartStop_connId;
extern int gs32_offlinestarttxn_update_flag;

extern String gc_offline_txn_startDate;
extern String gc_offline_txn_start_metervalue;
extern String gc_offline_txn_stopDate;
extern String gc_offline_txn_stop_metervalue;
extern String gc_offline_txn_reason;

extern uint8_t gu8_Remote_start_A;
extern uint8_t gu8_Remote_start_B;
extern uint8_t gu8_Remote_start_C;

extern Preferences resumeTxn_A;
extern Preferences resumeTxn_B;
extern Preferences resumeTxn_C;

extern Preferences energymeter;

extern bool ongoingTxnoff_A;
extern bool ongoingTxnoff_B;
extern bool ongoingTxnoff_C;

float meterStop = 0.0f;
unsigned long stop_time = 0;

StopTransaction::StopTransaction(String idTag, int transactionId, int connectorId)
{
  this->idTag = idTag;
  this->transactionId = transactionId;
  this->connectorId = connectorId;
}

StopTransaction::StopTransaction()
{
}

const char *StopTransaction::getOcppOperationType()
{
  return "StopTransaction";
}

DynamicJsonDocument *StopTransaction::createReq()
{

  // String idTag = String('\0');

  // if (getChargePointStatusService() != NULL) {
  //   idTag += getChargePointStatusService()->getIdTag();
  // }
  // JSON_OBJECT_SIZE = 5 to include reason for stop as well.

  DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(5) + (idTag.length() + 1) + (JSONDATE_LENGTH + 1));
  JsonObject payload = doc->to<JsonObject>();

  if (!idTag.isEmpty())
  { // if there is no idTag present, we shouldn't add a default one
    payload["idTag"] = idTag;
  }
#if 1
  if (getMeteringService() != NULL)
  {
    if (connectorId == 1)
    {
        meterStop = getMeteringService()->currentEnergy_A();
        if (meterStop > (20000000 - 100000)) // making metervalues '0' if the values increased by (20000000 - 100000) @Abhigna
        {
          energymeter.begin("MeterData", false);
          energymeter.putFloat("currEnergy_A", 0);
          energymeter.end();
          Serial.println("[Metering init] Reinitialized currEnergy_A");
        }
      if (flag_start_txn_A)
      {
        flag_start_txn_A = false;
        disp_evse_A_finished = true;
        // meterStop = getMeteringService()->currentEnergy_A();
        // if (meterStop > (20000000 - 100000)) // making metervalues '0' if the values increased by (20000000 - 100000) @Abhigna
        // {
        //   energymeter.begin("MeterData", false);
        //   energymeter.putFloat("currEnergy_A", 0);
        //   energymeter.end();
        //   Serial.println("[Metering init] Reinitialized currEnergy_A");
        // }
        // stop_time = millis();
        // currentCounterThreshold_A = 15;
// Add that stop lcd display over here
#if DISPLAY_ENABLED
        unsigned long seconds = (stop_time - st_timeA) / 1000;
        int hr = seconds / 3600;                                                 // Number of seconds in an hour
        int mins = (seconds - hr * 3600) / 60;                                   // Remove the number of hours and calculate the minutes.
        int sec = seconds - hr * 3600 - mins * 60;                               // Remove the number of hours and minutes, leaving only seconds.
        String hrMinSec = (String(hr) + ":" + String(mins) + ":" + String(sec)); // Converts to HH:MM:SS string. This can be returned to the calling function.
        thanks_Disp_AC("1", String(float((meterStop - globalmeterstartA) / 1000)), hrMinSec);
#endif
#if 1
#if 0
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
#endif
#if DWIN_ENABLED
        uint8_t err = 0;
        unsigned long seconds = (stop_time - st_timeA) / 1000;
        int hr = seconds / 3600;                   // Number of seconds in an hour
        int mins = (seconds - hr * 3600) / 60;     // Remove the number of hours and calculate the minutes.
        int sec = seconds - hr * 3600 - mins * 60; // Remove the number of hours and minutes, leaving only seconds.
        err = DWIN_SET(cid1, sizeof(cid1) / sizeof(cid1[0]));
        // Take to page 2.
        change_page[9] = 2;
        err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
        // kwh[7] = float((meterStop-globalmeterstartA)/1000);
        // kwh[7] = int((meterStop-globalmeterstartA)/1000);
        int jaf = meterStop - globalmeterstartA;
        kwh[6] = jaf >> 8;
        kwh[7] = jaf & 0xff;
        // Since Kwh is with 3 decimal points multiply by 1000
        // kwh[7]  = jaf/1000;
        err = DWIN_SET(kwh, sizeof(kwh) / sizeof(kwh[0]));
        HR[7] = hr;
        MINS[7] = mins;
        SEC[7] = sec;
        err = DWIN_SET(HR, sizeof(HR) / sizeof(HR[0]));
        err = DWIN_SET(MINS, sizeof(MINS) / sizeof(MINS[0]));
        err = DWIN_SET(SEC, sizeof(SEC) / sizeof(SEC[0]));
        delay(3000);
        // Take to page 2.
        change_page[9] = 0;
        err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
        delay(50);
#endif
      }
    }
    else if (connectorId == 2)
    {
              meterStop = getMeteringService()->currentEnergy_B();
        if (meterStop > (20000000 - 100000))
        {
          energymeter.begin("MeterData", false);
          energymeter.putFloat("currEnergy_B", 0);
          energymeter.end();
          Serial.println("[Metering init] Reinitialized currEnergy_B");
        }
      if (flag_start_txn_B)
      {
        flag_start_txn_B = false;
        // meterStop = getMeteringService()->currentEnergy_B();
        // if (meterStop > (20000000 - 100000))
        // {
        //   energymeter.begin("MeterData", false);
        //   energymeter.putFloat("currEnergy_B", 0);
        //   energymeter.end();
        //   Serial.println("[Metering init] Reinitialized currEnergy_B");
        // }
        // stop_time = millis();
        // currentCounterThreshold_B = 15;
// Add that stop lcd display over here
#if DISPLAY_ENABLED
        unsigned long seconds = (stop_time - st_timeB) / 1000;
        int hr = seconds / 3600;                                                 // Number of seconds in an hour
        int mins = (seconds - hr * 3600) / 60;                                   // Remove the number of hours and calculate the minutes.
        int sec = seconds - hr * 3600 - mins * 60;                               // Remove the number of hours and minutes, leaving only seconds.
        String hrMinSec = (String(hr) + ":" + String(mins) + ":" + String(sec)); // Converts to HH:MM:SS string. This can be returned to the calling function.
        thanks_Disp_AC("2", String(float((meterStop - globalmeterstartB) / 1000)), hrMinSec);
#endif
#if 0
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("OUTPUT 2 COMPLETED");
        lcd.setCursor(0, 1);
        lcd.print("THANKS FOR CHARGING");
        lcd.setCursor(0, 2);
        lcd.print("ENERGY(KWH):");
        lcd.setCursor(12, 2); // Or setting the cursor in the desired position.
        lcd.print(String(float((meterStop - globalmeterstartB) / 1000)));
        lcd.setCursor(0, 3);
        lcd.print("TIME      :");
        lcd.setCursor(12, 3); // Or setting the cursor in the desired position.
        unsigned long seconds = (stop_time - st_timeB) / 1000;
        int hr = seconds / 3600;                                                 // Number of seconds in an hour
        int mins = (seconds - hr * 3600) / 60;                                   // Remove the number of hours and calculate the minutes.
        int sec = seconds - hr * 3600 - mins * 60;                               // Remove the number of hours and minutes, leaving only seconds.
        String hrMinSec = (String(hr) + ":" + String(mins) + ":" + String(sec)); // Converts to HH:MM:SS string. This can be returned to the calling function.
        lcd.print(String(hrMinSec));
        delay(5000);
#endif

#if DWIN_ENABLED
        uint8_t err = 0;
        unsigned long seconds = (stop_time - st_timeA) / 1000;
        int hr = seconds / 3600;                   // Number of seconds in an hour
        int mins = (seconds - hr * 3600) / 60;     // Remove the number of hours and calculate the minutes.
        int sec = seconds - hr * 3600 - mins * 60; // Remove the number of hours and minutes, leaving only seconds.

        err = DWIN_SET(cid2, sizeof(cid2) / sizeof(cid2[0]));

        // Take to page 2.
        change_page[9] = 2;
        err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
        // kwh[7] = float((meterStop-globalmeterstartA)/1000);
        // int jaf = int((meterStop - globalmeterstartB) / 1000);
        int jaf = int(meterStop - globalmeterstartB);
        kwh[6] = jaf >> 8;
        kwh[7] = jaf & 0xff;
        err = DWIN_SET(kwh, sizeof(kwh) / sizeof(kwh[0]));
        HR[7] = hr;
        MINS[7] = mins;
        SEC[7] = sec;
        err = DWIN_SET(HR, sizeof(HR) / sizeof(HR[0]));
        err = DWIN_SET(MINS, sizeof(MINS) / sizeof(MINS[0]));
        err = DWIN_SET(SEC, sizeof(SEC) / sizeof(SEC[0]));
        delay(3000);
        // Take to page 2.
        change_page[9] = 0;
        err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
        delay(50);

#endif
      }
    }
    else if (connectorId == 3)
    {
              meterStop = getMeteringService()->currentEnergy_C();
        if (meterStop > (20000000 - 100000))
        // if(meterStop > 200) //testing
        {
          energymeter.begin("MeterData", false);
          energymeter.putFloat("currEnergy_C", 0);
          energymeter.end();
          Serial.println("[Metering init] Reinitialized currEnergy_C");
        }
      if (flag_start_txn_C)
      {
        flag_start_txn_C = false;
        // meterStop = getMeteringService()->currentEnergy_C();
        // if (meterStop > (20000000 - 100000))
        // // if(meterStop > 200) //testing
        // {
        //   energymeter.begin("MeterData", false);
        //   energymeter.putFloat("currEnergy_C", 0);
        //   energymeter.end();
        //   Serial.println("[Metering init] Reinitialized currEnergy_C");
        // }
        // stop_time = millis();
        // reset the counter.
        // currentCounterThreshold_C = 15;

#if DISPLAY_ENABLED
        unsigned long seconds = (stop_time - st_timeC) / 1000;
        int hr = seconds / 3600;                                                 // Number of seconds in an hour
        int mins = (seconds - hr * 3600) / 60;                                   // Remove the number of hours and calculate the minutes.
        int sec = seconds - hr * 3600 - mins * 60;                               // Remove the number of hours and minutes, leaving only seconds.
        String hrMinSec = (String(hr) + ":" + String(mins) + ":" + String(sec)); // Converts to HH:MM:SS string. This can be returned to the calling function.
        thanks_Disp_AC("3", String(float((meterStop - globalmeterstartC) / 1000)), hrMinSec);
#endif
        // Add that stop lcd display over here
#if 0
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("OUTPUT 3 COMPLETED");
        lcd.setCursor(0, 1);
        lcd.print("THANKS FOR CHARGING");
        lcd.setCursor(0, 2);
        lcd.print("ENERGY(KWH):");
        lcd.setCursor(12, 2); // Or setting the cursor in the desired position.
        lcd.print(String(float((meterStop - globalmeterstartC) / 1000)));
        lcd.setCursor(0, 3);
        lcd.print("TIME      :");
        lcd.setCursor(12, 3); // Or setting the cursor in the desired position.
        unsigned long seconds = (stop_time - st_timeC) / 1000;
        int hr = seconds / 3600;                                                 // Number of seconds in an hour
        int mins = (seconds - hr * 3600) / 60;                                   // Remove the number of hours and calculate the minutes.
        int sec = seconds - hr * 3600 - mins * 60;                               // Remove the number of hours and minutes, leaving only seconds.
        String hrMinSec = (String(hr) + ":" + String(mins) + ":" + String(sec)); // Converts to HH:MM:SS string. This can be returned to the calling function.
        lcd.print(String(hrMinSec));
        delay(5000);
#endif

#if DWIN_ENABLED
        uint8_t err = 0;
        unsigned long seconds = (stop_time - st_timeA) / 1000;
        int hr = seconds / 3600;                   // Number of seconds in an hour
        int mins = (seconds - hr * 3600) / 60;     // Remove the number of hours and calculate the minutes.
        int sec = seconds - hr * 3600 - mins * 60; // Remove the number of hours and minutes, leaving only seconds.

        // Take to page 2.
        err = DWIN_SET(cid3, sizeof(cid3) / sizeof(cid3[0]));
        change_page[9] = 2;
        err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
        // kwh[7] = float((meterStop-globalmeterstartA)/1000);
        // int jaf = int((meterStop - globalmeterstartC) / 1000);
        int jaf = int(meterStop - globalmeterstartC);
        kwh[6] = jaf >> 8;
        kwh[7] = jaf & 0xff;
        err = DWIN_SET(kwh, sizeof(kwh) / sizeof(kwh[0]));
        HR[7] = hr;
        MINS[7] = mins;
        SEC[7] = sec;
        err = DWIN_SET(HR, sizeof(HR) / sizeof(HR[0]));
        err = DWIN_SET(MINS, sizeof(MINS) / sizeof(MINS[0]));
        err = DWIN_SET(SEC, sizeof(SEC) / sizeof(SEC[0]));
        delay(3000);
        // Take to page 2.
        change_page[9] = 0;
        err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
        delay(50);

#endif
      }
    }
  }
#endif

  payload["meterStop"] = (int)meterStop; // TODO meterStart is required to be in Wh, but measuring unit is probably inconsistent in implementation
  payload["transactionId"] = transactionId;

  char timestamp[JSONDATE_LENGTH + 1] = {'\0'};

  // getJsonDateStringFromSystemTime(timestamp, JSONDATE_LENGTH);
  // getJsonDateStringFromGivenTime(timestamp, JSONDATE_LENGTH, time_t t);

  if (gs32_offlinestarttxn_update_flag == 2)
  {
    time_t given_time = gc_offline_txn_stopDate.toInt();
    Serial.println("gc_offline_txn_stopDate.toInt() : " + String(gc_offline_txn_stopDate.toInt()));
    getJsonDateStringFromGivenUnixTime(timestamp, JSONDATE_LENGTH + 1, given_time);

    connectorId = gs32_offlinestartStop_connId;
    payload["meterStop"] = gc_offline_txn_stop_metervalue.toInt();
  }
  // else
  // {
  // 	getJsonDateStringFromGivenTime(timestamp, JSONDATE_LENGTH + 1, now());
  // }
  // payload["timestamp"] = timestamp;

  if (gs32_offlinestartStoptxn_update_flag == 1)
  {
    payload["transactionId"] = gs32_offlinestarttxnId;

    time_t given_time = gc_offline_txn_stopDate.toInt();
    Serial.println("gc_offline_txn_stopDate.toInt() : " + String(gc_offline_txn_stopDate.toInt()));
    getJsonDateStringFromGivenUnixTime(timestamp, JSONDATE_LENGTH + 1, given_time);
    connectorId = gs32_offlinestartStop_connId;

    payload["meterStop"] = gc_offline_txn_stop_metervalue.toInt();

    // payload["meterStop"] = (int)meterStop; // TODO meterStart is required to be in Wh, but measuring unit is probably inconsistent in implementation
    // payload["timestamp"] = timestamp;
    // payload["reason"] = resonofstop_str[reasonForStop_C];
    Serial.println("[StopTransaction] transactionId : " + String(gs32_offlinestarttxnId));
  }
  else
  {
    getJsonDateStringFromGivenTime(timestamp, JSONDATE_LENGTH + 1, now());
    Serial.println("[StopTransaction] transactionId : " + String(transactionId));
  }

  payload["timestamp"] = timestamp;

  // if(gs32_offlinestartStoptxn_update_flag == 2)
  // {
  //   connectorId = gs32_offlinestartStop_connId;
  // }

  // int transactionId = -1;
  // if (getChargePointStatusService() != NULL) {
  //   transactionId = getChargePointStatusService()->getTransactionId();
  // }

#if DISPLAY_ENABLED
  flag_unfreeze = true;
#endif

  switch (connectorId)
  {
  case 1:
    Serial.printf("[StopTransaction] A reason for stop : %d", reasonForStop_A);
    payload["reason"] = resonofstop_str[reasonForStop_A];
    disp_evse_A = false;
    gu8_Remote_start_A = 0;
    Stop_Session_A = 0;
    Display_Clear(Stop_Session_A);
    // Clearing the reason.
    reasonForStop_A = 3;
    getChargePointStatusService_A()->setFinishing(false);
    flag_send_stop_A = false;
    if (gs32_offlinestartStoptxn_update_flag == 2)
    {
      payload["transactionId"] = gs32_offlinestarttxnId_A;
    }
    break;
  case 2:
    Serial.printf("[StopTransaction] B reason for stop : %d", reasonForStop_B);
    payload["reason"] = resonofstop_str[reasonForStop_B];
    disp_evse_B = false;
    gu8_Remote_start_B = 0;
    Stop_Session_B = 1;
    Display_Clear(Stop_Session_B);
    // Clearing the reason.
    reasonForStop_B = 3;
    getChargePointStatusService_B()->setFinishing(false);
    flag_send_stop_B = false;
    if (gs32_offlinestartStoptxn_update_flag == 2)
    {
      payload["transactionId"] = gs32_offlinestarttxnId_B;
    }
    break;
  case 3:
    Serial.printf("[StopTransaction] C reason for stop : %d", reasonForStop_C);
    payload["reason"] = resonofstop_str[reasonForStop_C];
    disp_evse_C = false;
    gu8_Remote_start_C = 0;
    Stop_Session_C = 2;
    Display_Clear(Stop_Session_C);
    // Clearing the reason.
    reasonForStop_C = 3;
    getChargePointStatusService_C()->setFinishing(false);
    flag_send_stop_C = false;
    if (gs32_offlinestartStoptxn_update_flag == 2)
    {
      payload["transactionId"] = gs32_offlinestarttxnId_C;
    }
    break;
  }

  // if(gs32_offlinestartStoptxn_update_flag == 1)
  // {
  //   payload["transactionId"] = gs32_offlinestarttxnId;
  //   payload["meterStop"] = (int)meterStop; // TODO meterStart is required to be in Wh, but measuring unit is probably inconsistent in implementation
  //   payload["timestamp"] = timestamp;
  //   //payload["reason"] = resonofstop_str[reasonForStop_C];
  // }

  // else if(gs32_offlinestartStoptxn_update_flag == 2)
  // {
  //   // payload["transactionId"] = gs32_offlinestarttxnId;
  //   payload["meterStop"] = (int)meterStop; // TODO meterStart is required to be in Wh, but measuring unit is probably inconsistent in implementation
  //   payload["timestamp"] = timestamp;
  //   payload["reason"] = resonofstop_str[reasonForStop_C];
  // }

  // if (getChargePointStatusService() != NULL) {
  //   getChargePointStatusService()->stopEnergyOffer();
  // }

  return doc;
}

void StopTransaction::processConf(JsonObject payload)
{

  // no need to process anything here

  // ChargePointStatusService *cpStatusService = getChargePointStatusService();
  // if (cpStatusService != NULL){
  //   //cpStatusService->stopEnergyOffer(); //No. This should remain in createReq
  //   cpStatusService->stopTransaction();
  //   cpStatusService->unauthorize();
  // }

  SmartChargingService *scService = getSmartChargingService();
  if (scService != NULL)
  {
    scService->endChargingNow();
  }

  /*
   * @brief : Feature added by G. Raja Sumant 29/07/2022
   * This will take the charge point to reserved state when ever it is available during the reservation loop
   */
  /*
   * @bug fix: G. Raja Sumant
   * When emergency is pressed multiple stop transactions are being sent despite it being accepted by the CMS.
   * This issue can be fixed by calling the stopTransaction and unauthorize function.
   */
#if 1
  switch (connectorId)
  {
  case 1:
    if (reservation_start_flag_A)
    {
      getChargePointStatusService_A()->setReserved(true);
    }
    requestForRelay(STOP, 1);
    resumeTxn_A.begin("resume_A", false); // opening preferences in R/W mode
    // resumeTxn_A.putBool("ongoingTxn_A", false);
    resumeTxn_A.putString("idTagData_A", "");
    resumeTxn_A.putInt("TxnIdData_A", -1);
    resumeTxn_A.putBool("ongoingTxnoff_A", false);
    resumeTxn_A.end();
    Serial.println("[StopTransaction] Connector 1 cleared");
    /* getChargePointStatusService_A()->stopTransaction();
     getChargePointStatusService_A()->unauthorize();*/
    break;
  case 2:
    if (reservation_start_flag_B)
    {
      getChargePointStatusService_B()->setReserved(true);
    }
    requestForRelay(STOP, 2);
    resumeTxn_B.begin("resume_B", false); // opening preferences in R/W mode
    // resumeTxn_B.putBool("ongoingTxn_B", false);
    resumeTxn_B.putString("idTagData_B", "");
    resumeTxn_B.putInt("TxnIdData_B", -1);
    resumeTxn_B.putBool("ongoingTxnoff_B", false);
    resumeTxn_B.end();

    Serial.println("[StopTransaction] Connector 2 cleared");
    /*getChargePointStatusService_B()->stopTransaction();
      getChargePointStatusService_B()->unauthorize();*/
    break;
  case 3:
    if (reservation_start_flag_C)
    {
      getChargePointStatusService_C()->setReserved(true);
    }
    requestForRelay(STOP, 3);
    resumeTxn_C.begin("resume_C", false); // opening preferences in R/W mode
    // resumeTxn_C.putBool("ongoingTxn_C", false);
    resumeTxn_C.putString("idTagData_C", "");
    resumeTxn_C.putInt("TxnIdData_C", -1);
    resumeTxn_C.putBool("ongoingTxnoff_C", false);
    resumeTxn_C.end();

    Serial.println("[StopTransaction] Connector 3 cleared");
    /*getChargePointStatusService_C()->stopTransaction();
      getChargePointStatusService_C()->unauthorize();*/
    break;
  }
#endif


  if (DEBUG_OUT)
    Serial.print("[EVSE]Stopaccepted!\n");

  if (connectorId == 1)
  {
    stop_time = millis();
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
  }
  else if (connectorId == 2)
  {
    stop_time = millis();
#if LCD_DISPLAY_ENABLED
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("OUTPUT 2 COMPLETED");
    lcd.setCursor(0, 1);
    lcd.print("THANKS FOR CHARGING");
    lcd.setCursor(0, 2);
    lcd.print("ENERGY(KWH):");
    lcd.setCursor(12, 2); // Or setting the cursor in the desired position.
    lcd.print(String(float((meterStop - globalmeterstartB) / 1000)));
    lcd.setCursor(0, 3);
    lcd.print("TIME      :");
    lcd.setCursor(12, 3); // Or setting the cursor in the desired position.
    unsigned long seconds = (stop_time - st_timeB) / 1000;
    int hr = seconds / 3600;                                                 // Number of seconds in an hour
    int mins = (seconds - hr * 3600) / 60;                                   // Remove the number of hours and calculate the minutes.
    int sec = seconds - hr * 3600 - mins * 60;                               // Remove the number of hours and minutes, leaving only seconds.
    String hrMinSec = (String(hr) + ":" + String(mins) + ":" + String(sec)); // Converts to HH:MM:SS string. This can be returned to the calling function.
    lcd.print(String(hrMinSec));
    delay(5000);
#endif
  }
  else if (connectorId == 3)
  {
    stop_time = millis();
#if LCD_DISPLAY_ENABLED
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("OUTPUT 3 COMPLETED");
    lcd.setCursor(0, 1);
    lcd.print("THANKS FOR CHARGING");
    lcd.setCursor(0, 2);
    lcd.print("ENERGY(KWH):");
    lcd.setCursor(12, 2); // Or setting the cursor in the desired position.
    lcd.print(String(float((meterStop - globalmeterstartC) / 1000)));
    lcd.setCursor(0, 3);
    lcd.print("TIME      :");
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

void StopTransaction::processReq(JsonObject payload)
{
  /**
   * Ignore Contents of this Req-message, because this is for debug purposes only
   */
}

DynamicJsonDocument *StopTransaction::createConf()
{
  DynamicJsonDocument *doc = new DynamicJsonDocument(2 * JSON_OBJECT_SIZE(1));
  JsonObject payload = doc->to<JsonObject>();

  JsonObject idTagInfo = payload.createNestedObject("idTagInfo");
  idTagInfo["status"] = "Accepted";

  return doc;
}
