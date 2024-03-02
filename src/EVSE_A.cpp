// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

/**
Edited by Pulkit Agrawal.
*/

#include "EVSE_A.h"
#include "Master.h"

#include "ControlPilot.h"
#include "display_meterValues.h"
#include "LCD_I2C.h"

#if DISPLAY_ENABLED
#include "display.h"
extern bool flag_tapped;
#endif

extern int8_t fault_code_A;
extern int8_t fault_code_B;
extern int8_t fault_code_C;

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
// new variable names defined by @Pulkit. might break the build.
OnBoot_A onBoot_A;
OnReadUserId_A onReadUserId_A;
OnSendHeartbeat_A onSendHeartbeat_A;
OnAuthentication_A onAuthentication_A;
OnStartTransaction_A onStartTransaction_A;
OnStopTransaction_A onStopTransaction_A;
OnUnauthorizeUser_A onUnauthorizeUser_A;

uint8_t currentCounterThreshold_A = 60;
float strgf32mincurr = 0;
int gs32lowcurcnt = 0;

bool disp_evse_A = false;

ulong timerHb = 0;
unsigned int heartbeatInterval = 50;

bool notFaulty_A = false;

extern bool flag_send_stop_A;

int prevTxnId_A = -1;
uint8_t fault_occured_A = 0;

// timeout for heartbeat signal.
ulong T_SENDHEARTBEAT = 60000;
bool timeout_active_A = false;
bool timer_initialize_A = false;
ulong timeout_start_A = 0;
// Reason for stop
extern bool disp_evse_C;
extern bool disp_evse_B;
extern uint8_t reasonForStop_A;
// new flag names. replace them with old names.
bool evIsPlugged_A;
bool flag_evseIsBooted_A;
bool flag_evseReadIdTag_A;
bool flag_evseAuthenticate_A;
bool flag_evseStartTransaction_A;
bool flag_evRequestsCharge_A;
bool flag_evseStopTransaction_A;
bool flag_evseUnauthorise_A;
bool flag_rebootRequired_A;
bool flag_evseSoftReset_A; // added by @Wamique
// bool flag_stop_finishing_A;
bool flag_evseReserveNow_A = false;		   // added by @mkrishna
bool flag_evseCancelReservation_A = false; // added by @mkrishna
bool flag_localAuthenication_A = false;	   // added by @mkrishna
bool gu8_send_status_flag_A = false;	   // added by @mkrishna
bool reservation_start_flag_A = false;	   // added by @mkrishna

extern int reservationId_A;
extern int reserve_connectorId_A;
extern time_t reservation_start_time_A;
extern time_t reserveDate_A;
extern time_t reservedDuration_A; // 15 minutes slot Duration 15 * (1 * 60)
extern bool reserve_state_A;

float minCurr = 0.25; // For LB nagar hubs
// extern float mincurr;
extern ATM90E36 eic;
extern bool flag_rebootRequired_B;
extern bool flag_rebootRequired_C;

// not used. part of Smart Charging System.
float chargingLimit_A = 32.0f;
String Ext_currentIdTag_A = "";
extern WebSocketsClient webSocket;

ulong timerDisplay;
bool EMGCY_FaultOccured = false;

extern LCD_I2C lcd;

extern MFRC522 mfrc522;
extern String currentIdTag;
long int blinckCounter_A = 0;
int counter1_A = 0;

ulong t;
int connectorDis_counter_A = 0;

extern String reserve_currentIdTag_A;

String currentIdTag_A = "";
extern EVSE_states_enum EVSE_state;
extern Preferences preferences;

short int fault_counter_A = 0;
bool flag_faultOccured_A = false;

short int counter_drawingCurrent_A = 0;
float drawing_current_A = 0;

extern bool webSocketConncted;
extern bool isInternetConnected;
short counter_faultstate_A = 0;

// metering Flag
extern bool flag_MeteringIsInitialised;
extern MeteringService *meteringService;
extern bool wifi_connect;
extern bool gsm_connect;

int transactionId_A = -1;

Preferences resumeTxn_A;
extern Preferences resumeTxn_B;
extern Preferences resumeTxn_C;
extern TinyGsmClient client;

bool ongoingTxn_A;
String idTagData_A = "";

ulong relay_timer_A;
ulong faultTimer_A = 0;

const ulong TIMEOUT_EMERGENCY_RELAY_CLOSE_A = 120000;

extern Preferences energymeter;

extern bool ethernet_enable;
extern bool ethernet_connect;

extern bool ongoingTxn_B;
extern bool ongoingTxn_C;

extern bool flag_stop_once_A;

extern int globalmeterstartA;

extern bool flag_start_txn_A;

uint8_t online_session_ongoing_A = 0;

extern volatile bool session_ongoing_flag;
extern volatile bool session_on_offline_txn_completed;

extern uint8_t gu8_online_flag;

extern int Sendstart_time_A;
extern int Sendstop_time_A;

extern uint8_t session_ongoing_A_count;

extern String Tag_Id_A;
extern String Transaction_Id_A;
extern String Start_Meter_Value_A;

extern int gs32_offlinestarttxnId;
extern int gs32_offlinestarttxn_update_flag;
extern int gs32_offlinestartStoptxn_update_flag;
// extern volatile int gs32_fetch_offline_txn_count;

extern uint8_t gu8_fetch_offline_txn_flag;
extern uint8_t gu8_clear_offline_txn_flag;
// extern volatile int gs32_fetch_offline_txn_count;
extern int gs32_offlineAuthorizeIdTag;

extern volatile int gs32_fetch_offline_txnlist_count;
extern volatile int gs32_fetch_offline_txn_count;
extern int gs32_offlinestartStoptxn_update_guard_Interval;

extern ChargePointStatusService *chargePointStatusService_A;
extern ChargePointStatusService *chargePointStatusService_B;
extern ChargePointStatusService *chargePointStatusService_C;

evse_boot_stat_t evse_boot_state;
uint8_t gu8_state_change = 0;
extern uint8_t gu8_bootsuccess;
extern uint8_t gu8_Remote_start_A;
uint8_t start_session_A = 0;
 uint8_t Stop_Session_A = 0;
 extern float current_energy_A;
extern float discurrEnergy_A;
float LastPresentEnergy_A = 0;

 bool EMGCY_status_B = 0;
 bool EMGCY_status_C = 0;
 volatile bool session_ongoing_flag_A = false;

bool stopoffline_A = false;
extern bool stopoffline_B;
extern bool stopoffline_C;
extern bool ongoingTxnoff_A;
extern uint8_t offline_charging_A;

// uint8_t ongoingTxn_A_stop = 0;

// initialize function. called when EVSE is booting.
// NOTE: It should be also called when there is a reset or reboot required. create flag to control that. @Pulkit
/**********************************************************/
void EVSE_A_StopSession()
{
	if (getChargePointStatusService_A()->getEvDrawsEnergy() == true)
	{
		getChargePointStatusService_A()->stopEvDrawsEnergy();
	}

	// digitalWrite(32, LOW);
    session_ongoing_flag_A = false;
	session_ongoing_flag = false;
	online_session_ongoing_A = 0;
	session_on_offline_txn_completed = true;
	disp_evse_A = false;
	gu8_Remote_start_A = 0;
    Stop_Session_A = 0;
    Display_Clear(Stop_Session_A);
	// requestForRelay(STOP, 1);
	if (webSocketConncted == 0)
	{
		getChargePointStatusService_A()->setChargePointstatus(Available);
		requestForRelay(STOP, 1);
		if (!offline_charging_A)
		{
			resumeTxn_A.begin("resume_A", false); // opening preferences in R/W mode
			resumeTxn_A.putBool("ongoingTxn_A", false);
			resumeTxn_A.putBool("ongoingTxnoff_A", true);
			resumeTxn_A.end();
		}
	}
	delay(500);
	flag_evseReadIdTag_A = false;
	flag_evseAuthenticate_A = false;
	flag_evseStartTransaction_A = false;
	flag_evRequestsCharge_A = false;
	// ongoingTxn_A_stop = 0;
	flag_evseStopTransaction_A = true;
	flag_evseUnauthorise_A = false;
	Serial.println("[EVSE] Stopping Session : " + String(EVSE_state));
}
/**************************************************************************/

/**************************SetUp********************************************/
void EVSE_A_setup()
{
	resumeTxn_A.begin("resume_A", false); // opening preferences in R/W mode
	idTagData_A = resumeTxn_A.getString("idTagData_A", "");
	ongoingTxn_A = resumeTxn_A.getBool("ongoingTxn_A", false);
	transactionId_A = resumeTxn_A.getInt("TxnIdData_A", -1);
	globalmeterstartA = resumeTxn_A.getFloat("meterStart", false);
	stopoffline_A = resumeTxn_A.getBool("ongoingTxnoff_A", false);
    
	resumeTxn_A.end();

	Serial.println("Stored ID_A:" + String(idTagData_A));
	Serial.println("Ongoing Txn_A: " + String(ongoingTxn_A));
	Serial.println("Txn id_A is : " + String(transactionId_A));
	Serial.println("meterstart_A is: " + String(globalmeterstartA));
	Serial.println(" stopoffline_A is: " + String(stopoffline_A));

	energymeter.begin("MeterData", false);
	float lu_lastEnergy_A = energymeter.getFloat("currEnergy_A", 0);
	// placing energy value back in EEPROM
	energymeter.end();
	Serial.println("[EnergyASampler] lu_lastEnergy_A: " + String(lu_lastEnergy_A));
	globalmeterstartA = (int)lu_lastEnergy_A;
	Serial.println("[EnergyASampler] globalmeterstartA: " + String(globalmeterstartA));

	if (ongoingTxn_A)
	{
		flag_start_txn_A = true;
	}

	EVSE_A_setOnBoot([]()
					 {
		//this is not in loop, that is why we need not update the flag immediately to avoid multiple copies of bootNotification.
		OcppOperation *bootNotification = makeOcppOperation(&webSocket,	new BootNotification());
		initiateOcppOperation(bootNotification);
if (gu8_online_flag == 1)
    {
		bootNotification->setOnReceiveConfListener([](JsonObject payload) {

      	 if( flag_MeteringIsInitialised == false){
      	 	Serial.println(F("[SetOnBooT] Initializing metering services"));
      		meteringService->init(meteringService);
			   /*#if LCD_ENABLED
  				lcd.clear();
  				lcd.setCursor(0, 2);
  				lcd.print("TAP RFID/SCAN QR");
			   #endif*/
      	 }

      if (DEBUG_OUT) Serial.println("EVSE_setOnBoot Callback: Metering Services Initialization finished.\n");

			flag_evseIsBooted_A = true; //Exit condition for booting. 	
			flag_evseReadIdTag_A = true; //Entry condition for reading ID Tag.
			flag_evseAuthenticate_A = false;
			flag_evseStartTransaction_A = false;
			flag_evRequestsCharge_A = false;
			flag_evseStopTransaction_A = false;
			flag_evseUnauthorise_A = false;
			EvseDevStatus_connector_1 = flag_EVSE_Read_Id_Tag;



      if (DEBUG_OUT) Serial.println("EVSE_setOnBoot Callback: Closing Relays.\n");

			if (DEBUG_OUT) Serial.println("EVSE_setOnBoot Callback: Boot successful. Calling Read User ID Block.\n");
		});
		
 }
 else
    {
      if ( flag_MeteringIsInitialised == false) {
        Serial.println("[SetOnBooT] Initializing metering services");
        meteringService->init(meteringService);
      }

      if (DEBUG_OUT) Serial.print(F("EVSE_setOnBoot Callback: Metering Services Initialization finished.\n"));
 
      flag_evseIsBooted_A = true; //Exit condition for booting.
      flag_evseReadIdTag_A = true; //Entry condition for reading ID Tag.
      flag_evseAuthenticate_A = false;
      flag_evseStartTransaction_A = false;
	  EvseDevStatus_connector_1 = flag_EVSE_Read_Id_Tag;

      //flag_evRequestsCharge = false;
      if(ongoingTxn_A)
      {
      flag_evRequestsCharge_A = true;
      if(DEBUG_OUT) Serial.println("****[EVSE_setOnBoot] on going transaction is true*****");
      }
      else
      {
        flag_evRequestsCharge_A = false;
        if(DEBUG_OUT) Serial.println("****[EVSE_setOnBoot] on going transaction is false*****");
      }
      flag_evseStopTransaction_A = false;
      flag_evseUnauthorise_A = false;

    } });

	EVSE_A_setOnReadUserId([]()
						   {
		if (DEBUG_OUT) Serial.print("EVSE_A:RFID\n");
		static ulong timerForRfid = millis();
		currentIdTag_A = "";  
		resumeTxn_A.begin("resume_A", false);		
		idTagData_A = resumeTxn_A.getString("idTagData_A","");
  		ongoingTxn_A = resumeTxn_A.getBool("ongoingTxn_A",false);
		stopoffline_A = resumeTxn_A.getBool("ongoingTxnoff_A", false);
		transactionId_A = resumeTxn_A.getInt("TxnIdData_A", -1);

		int connector = getChargePointStatusService_A()->getConnectorId();
		if(stopoffline_A == 1)
		{
			stopoffline_A = 0;

			OcppOperation *stopTransaction = makeOcppOperation(&webSocket, new StopTransaction(currentIdTag_A, transactionId_A, connector));
			initiateOcppOperation(stopTransaction);
			// prevTxnId_A = transactionId_A;

			if(webSocketConncted)
			{
				resumeTxn_A.begin("resume_A", false); // opening preferences in R/W mode
				// resumeTxn_B.putBool("ongoingTxn_B", false);
				resumeTxn_A.putString("idTagData_A", "");
				resumeTxn_A.putInt("TxnIdData_A", -1);
				resumeTxn_A.putBool("ongoingTxnoff_A", false);
				resumeTxn_A.end();
			}
		}
		/*
		* @brief : When ethernet is enabled, it has to be working.
		* G. Raja Sumant 27/08/2022
		*/

		if(ethernet_connect && 														//ethernet Block
  			(getChargePointStatusService_A()->getEmergencyRelayClose() == false)&&
  			(webSocketConncted == true) && getChargePointStatusService_A()->getUnavailable() == false)
		{
if((idTagData_A != "") && (ongoingTxn_A == 1)){
  				currentIdTag_A = resumeTxn_A.getString("idTagData_A", "");

#if SEEMA_CONNECT
				getChargePointStatusService_A()->startTransaction(transactionId_A);
				reasonForStop_A = 5;
    			//fault_code_A = Power_Loss;
				EVSE_A_StopSession();
				Serial.println("[EVSE_A] Stop Session");
#if DISPLAY_ENABLED
      			  connAvail(3,"SESSION RESUME");
  				  checkForResponse_Disp();
#endif

#else
				Serial.println("EVSE_A:Resume session");
#if DISPLAY_ENABLED
				connAvail(1, "SESSION A RESUME");
				checkForResponse_Disp();
#endif
#if LCD_ENABLED
				lcd.clear();
				lcd.setCursor(3, 2);
				lcd.print("SESSION A RESUME");
#endif
				getChargePointStatusService_A()->authorize(currentIdTag_A); // so that Starttxn can easily fetch this data
				// requestLed(BLUE, START, 1);
				getChargePointStatusService_A()->startTransaction(transactionId_A);        //new block for Three connector
				getChargePointStatusService_A()->startEnergyOffer();
				resumeTxn_A.putString("idTagData_A",getChargePointStatusService_A()->getIdTag());
				resumeTxn_A.putBool("ongoingTxn_A",true);
				resumeTxn_A.putInt("TxnIdData_A", transactionId_A);

#endif

  			}else{
				if(millis() - timerForRfid > 8000){ //timer for sending led request
	    		  	
					/*
					* @brief : Check unavailable condition
					*/
				
					// requestLed(GREEN, START, 1);
	    		  	timerForRfid = millis();
// add lcd print to take it back to available and online via wifi
#if 0
#if LCD_ENABLED
					//lcd.clear();
					if(disp_evse_B)
					{
					lcd.setCursor(0, 1);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 1);               // Or setting the cursor in the desired position.
      				lcd.print("B: CHARGING"); // Clear the line
					}
					else
					{
					lcd.setCursor(0, 1);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 1);               // Or setting the cursor in the desired position.
      				lcd.print("B: AVAILABLE"); // Clear the line
					}
					if(disp_evse_C)
					{
					lcd.setCursor(0, 2);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 2);               // Or setting the cursor in the desired position.
      				lcd.print("C: CHARGING"); // Clear the line
					}
					else
					{
					lcd.setCursor(0, 2);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 2);               // Or setting the cursor in the desired position.
      				lcd.print("C: AVAILABLE"); // Clear the line
					}
					lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 0); // Or setting the cursor in the desired position.
					lcd.print("A: AVAILABLE");
					//lcd.setCursor(0, 1);
					//lcd.print("TAP RFID/SCAN QR");
					//lcd.setCursor(0, 2);
					//lcd.print("CONNECTION");
					lcd.setCursor(0, 3);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 3);
					lcd.print("CLOUD: ETH. TAP RFID");
#endif
#endif
    		  	}
			
				currentIdTag_A = EVSE_A_getCurrnetIdTag(&mfrc522);
				Serial.println("[Wifi]RFID-A");
  			}

		}

  		else if((wifi_connect == true)&& 														//Wifi Block
  			(getChargePointStatusService_A()->getEmergencyRelayClose() == false)&&
  			(webSocketConncted == true)&&
  			(WiFi.status() == WL_CONNECTED)&&
  			(isInternetConnected == true)&& getChargePointStatusService_A()->getUnavailable() == false)
			{
  			if((idTagData_A != "") && (ongoingTxn_A == 1)){
  				currentIdTag_A = resumeTxn_A.getString("idTagData_A", "");
#if SEEMA_CONNECT
				getChargePointStatusService_A()->startTransaction(transactionId_A);
				reasonForStop_A = 5;
    			//fault_code_A = Power_Loss;
				EVSE_A_StopSession();
				Serial.println("[EVSE_A]Stop Session");
#if DISPLAY_ENABLED
      			  connAvail(3,"SESSION RESUME");
  				  checkForResponse_Disp();
#endif

#else
				Serial.println("[EVSE_A] Resuming Session");
#if DISPLAY_ENABLED
				connAvail(1, "SESSION A RESUME");
				checkForResponse_Disp();
#endif
#if LCD_ENABLED
				lcd.clear();
				lcd.setCursor(3, 2);
				lcd.print("SESSION A RESUME");
#endif
				getChargePointStatusService_A()->authorize(currentIdTag_A); // so that Starttxn can easily fetch this data
				// requestLed(BLUE, START, 1);
				getChargePointStatusService_A()->startTransaction(transactionId_A);        //new block for Three connector
				getChargePointStatusService_A()->startEnergyOffer();

				resumeTxn_A.putString("idTagData_A",getChargePointStatusService_A()->getIdTag());
				resumeTxn_A.putBool("ongoingTxn_A",true);
				resumeTxn_A.putInt("TxnIdData_A", transactionId_A);
#endif
  			}else{
				if(millis() - timerForRfid > 8000){ //timer for sending led request
	    		  	// requestLed(GREEN, START, 1);
	    		  	timerForRfid = millis();
// add lcd print to take it back to available and online via wifi
#if 0
#if LCD_ENABLED
					//lcd.clear();
					if(disp_evse_B)
					{
					lcd.setCursor(0, 1);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 1);               // Or setting the cursor in the desired position.
      				lcd.print("B: CHARGING"); // Clear the line
					}
					else
					{
					lcd.setCursor(0, 1);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 1);               // Or setting the cursor in the desired position.
      				lcd.print("B: AVAILABLE"); // Clear the line
					}
					if(disp_evse_C)
					{
					lcd.setCursor(0, 2);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 2);               // Or setting the cursor in the desired position.
      				lcd.print("C: CHARGING"); // Clear the line
					}
					else
					{
					lcd.setCursor(0, 2);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 2);               // Or setting the cursor in the desired position.
      				lcd.print("C: AVAILABLE"); // Clear the line
					}
					lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 0); // Or setting the cursor in the desired position.
					lcd.print("A: AVAILABLE");
					//lcd.setCursor(0, 1);
					//lcd.print("TAP RFID/SCAN QR");
					//lcd.setCursor(0, 2);
					//lcd.print("CONNECTION");
					lcd.setCursor(0, 3);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 3);
					lcd.print("CLOUD: WIFI.TAP RFID");
#endif
#endif
    		  	}
			
				currentIdTag_A = EVSE_A_getCurrnetIdTag(&mfrc522);
				Serial.println("[Wifi]-RFID A");
  			}

  		}else if((gsm_connect == true)&&													//GSM Block
  				(getChargePointStatusService_A()->getEmergencyRelayClose() == false)&&
  				(client.connected() == true) && getChargePointStatusService_A()->getUnavailable() == false){

  			if((idTagData_A != "") && (ongoingTxn_A == 1)){
  				currentIdTag_A = resumeTxn_A.getString("idTagData_A", "");
#if SEEMA_CONNECT
				getChargePointStatusService_A()->startTransaction(transactionId_A);
				reasonForStop_A = 5;
    			//fault_code_A = Power_Loss;
				EVSE_A_StopSession();
				Serial.println("[EVSE_A] Stop Session");
#if DISPLAY_ENABLED
      			  connAvail(3,"SESSION RESUME");
  				  checkForResponse_Disp();
#endif

#else
				Serial.println("[EVSE_A]Resuming Session");
#if DISPLAY_ENABLED
				connAvail(1, "SESSION A RESUME");
				checkForResponse_Disp();
#endif
#if LCD_ENABLED
				lcd.clear();
				lcd.setCursor(3, 2);
				lcd.print("SESSION A RESUME");
#endif
				getChargePointStatusService_A()->authorize(currentIdTag_A); // so that Starttxn can easily fetch this data
				// requestLed(BLUE, START, 1);
				getChargePointStatusService_A()->startTransaction(transactionId_A);        //new block for Three connector
				getChargePointStatusService_A()->startEnergyOffer();
				resumeTxn_A.putString("idTagData_A",getChargePointStatusService_A()->getIdTag());
				resumeTxn_A.putBool("ongoingTxn_A",true);
				resumeTxn_A.putInt("TxnIdData_A", transactionId_A);
#endif
  			}else{
				if(millis() - timerForRfid > 8000){ //timer for sending led request
	    		  	// requestLed(GREEN, START, 1);
	    		  	timerForRfid = millis();
// add lcd print to take it back to available and online via 4G
#if 0
#if LCD_ENABLED
					if(disp_evse_B)
					{
					lcd.setCursor(0, 1);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 1);               // Or setting the cursor in the desired position.
      				lcd.print("B: CHARGING"); // Clear the line
					}
					else
					{
					lcd.setCursor(0, 1);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 1);               // Or setting the cursor in the desired position.
      				lcd.print("B: AVAILABLE"); // Clear the line
					}
					if(disp_evse_C)
					{
					lcd.setCursor(0, 2);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 2);               // Or setting the cursor in the desired position.
      				lcd.print("C: CHARGING"); // Clear the line
					}
					else
					{
					lcd.setCursor(0, 2);              // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 2);               // Or setting the cursor in the desired position.
      				lcd.print("C: AVAILABLE"); // Clear the line
					}
					lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 0); // Or setting the cursor in the desired position.
					lcd.print("A: AVAILABLE");
					//lcd.setCursor(0, 1);
					//lcd.print("TAP RFID/SCAN QR");
					//lcd.setCursor(0, 2);
					//lcd.print("CONNECTION");
					lcd.setCursor(0, 3);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 3);
					lcd.print("CLOUD: 4G. TAP RFID");
#endif
#endif

    		  	}
			
				currentIdTag_A = EVSE_A_getCurrnetIdTag(&mfrc522);
				Serial.println("[GSM]-RFID A");
  			}


  		}
		resumeTxn_A.end();
  		/*
		if((ongoingTxn_m == 1) && (idTagData_m != "") && 
		      (getChargePointStatusService_A()->getEmergencyRelayClose() == false) &&
		      (WiFi.status() == WL_CONNECTED)&&
		      (webSocketConncted == true)&&
		      (isInternetConnected == true)){   //giving priority to stored data
			currentIdTag_A = resumeTxn.getString("idTagData","");
			Serial.println("[EVSE_setOnReadUserId] Resuming Session");
      		requestLed(BLUE,START,1);
         
		}else 
		if((getChargePointStatusService_A()->getEmergencyRelayClose() == false) &&
		          (WiFi.status() == WL_CONNECTED) &&
		          (webSocketConncted == true) && 
		          (isInternetConnected == true)){
			  #if LED_ENABLED
			  if(millis() - timerForRfid > 10000){ //timer for sending led request
    		  requestLed(GREEN,START,1);
    		  timerForRfid = millis();
    		  }
    		  #endif
			currentIdTag_A = EVSE_A_getCurrnetIdTag(&mfrc522);
			Serial.println("********RFID A**********");
		}*/

		if (currentIdTag_A.equals("") == true) {
			//Serial.println(F("Looping back read block as no ID tag present"));
			flag_evseReadIdTag_A = true; //Looping back read block as no ID tag present.
			flag_evseAuthenticate_A = false;
			flag_evseStartTransaction_A = false;
			flag_evRequestsCharge_A = false;
			flag_evseStopTransaction_A = false;
			flag_evseUnauthorise_A = false;
		} else {
			flag_evseReadIdTag_A = false;
			flag_evRequestsCharge_A = false;
			flag_evseUnauthorise_A = false;
			
#if SEEMA_CONNECT
			    if(ongoingTxn_A == 1)
				{
					flag_evseAuthenticate_A = false; //Entry condition for authentication block.
					flag_evseStartTransaction_A = false;
					flag_evseStopTransaction_A = true;
					Serial.println("EVSE_A_ ongoingTxn_A. \n");
				}
				else 
				{
					// flag_evseAuthenticate_A = false; //Entry condition for authentication block.
					// flag_evseStartTransaction_A = true;
					flag_evseAuthenticate_A = true; //Entry condition for authentication block.
					flag_evseStartTransaction_A = false;
					flag_evseStopTransaction_A = false;
					Serial.println("EVSE_A_ startTxn_A. \n");
				}

#else
			    if(ongoingTxn_A == 1)
				{

					flag_evseAuthenticate_A = false; //Entry condition for authentication block.
					flag_evseStartTransaction_A = false;
					// flag_evseStopTransaction_A = true;
                    flag_evRequestsCharge_A = true;    // to be check the functionality
					Serial.println("EVSE_A_ ongoingTxn_A. \n");
	
				}
				else
				{
					flag_evseAuthenticate_A = true; //Entry condition for authentication block.
					flag_evseStartTransaction_A = false;
					flag_evseStopTransaction_A = false;
					if (DEBUG_OUT) Serial.println("Calling Auth\n");
				}
#endif			
			
			
			
		} });

	EVSE_A_setOnsendHeartbeat([]()
							  {
    if (DEBUG_OUT) Serial.println("Sending Heartbeat\n");
    if(gu8_online_flag == 1)
	{
    OcppOperation *heartbeat = makeOcppOperation(&webSocket, new Heartbeat());
    initiateOcppOperation(heartbeat); 
    heartbeat->setOnReceiveConfListener([](JsonObject payload) {
        const char* currentTime = payload["currentTime"] | "Invalid";
        if (strcmp(currentTime, "Invalid")) {
          if (setTimeFromJsonDateString(currentTime)) {
            if (DEBUG_OUT) Serial.println("HeartBeat Accepted\n");
          } else {
            Serial.println("Heartbeat accepted not in UTC timestamp\n");
          }
        } else {
          Serial.println("Heartbeat Denied\n");
        }
    });
	} });

	EVSE_A_setOnAuthentication([]()
							   {
		if (DEBUG_OUT) Serial.println("[EVSE_A]Auth");
		flag_evseAuthenticate_A = false;
		OcppOperation *authorize = makeOcppOperation(&webSocket, new Authorize(currentIdTag_A));
		initiateOcppOperation(authorize);
		/*chargePointStatusService->authorize(currentIdTag_A, connectorId_A);  */    //have to edit
		authorize->setOnReceiveConfListener([](JsonObject payload) {
			const char* status = payload["idTagInfo"]["status"] | "Invalid";
			if (!strcmp(status, "Accepted")) {
				getChargePointStatusService_A()->setChargePointstatus(Preparing);
				flag_evseReadIdTag_A = false;
				flag_evseAuthenticate_A = false;
				flag_evseStartTransaction_A = true; //Entry condition for starting transaction.
				flag_evRequestsCharge_A = false;
				flag_evseStopTransaction_A = false;
				flag_evseUnauthorise_A = false;
				if (DEBUG_OUT) Serial.println("[EVSE_A]Auth-Accepted");
				//requestLed(BLUE,START,1);
				// requestLed(BLINKYBLUE,START,1);
				currentIdTag = currentIdTag_A;  /* Assinging currentIdTag_A to currentIdTag for Set to Auth cache  */
				// requestSendAuthCache();//DISABLED Not checking the authentication cache
#if CP_ACTIVE 
				flag_controlPAuthorise = true;
#endif

			} else {
				flag_evseReadIdTag_A = false;
				flag_evseAuthenticate_A = false;
				flag_evseStartTransaction_A = false;
				flag_evRequestsCharge_A = false;
				flag_evseStopTransaction_A = false;
				flag_evseUnauthorise_A = true; //wrong ID tag received, so clearing the global current ID tag variable and setting up to read again.
				if (DEBUG_OUT) Serial.println("[EVSE_A]AuthDenied");
			}  
		}); });

	EVSE_A_setOnStartTransaction([]()
								 {
		flag_evseStartTransaction_A = false;
		String idTag = "";
		int connectorId = 0;

		if (getChargePointStatusService_A() != NULL) {
			if (!getChargePointStatusService_A()->getIdTag().isEmpty()) {
				idTag = String(getChargePointStatusService_A()->getIdTag());
				connectorId = getChargePointStatusService_A()->getConnectorId();
			}

		}
		OcppOperation *startTransaction = makeOcppOperation(&webSocket, new StartTransaction(idTag,connectorId));
		initiateOcppOperation(startTransaction);
		startTransaction->setOnReceiveConfListener([](JsonObject payload) {
    		const char* status = payload["idTagInfo"]["status"] | "Invalid";
      if (!strcmp(status, "Accepted") || !strcmp(status, "ConcurrentTx")) { 

      flag_evseReadIdTag_A = false;
      flag_evseAuthenticate_A = false;
      flag_evseStartTransaction_A = false;
      flag_evRequestsCharge_A = true;
      flag_evseStopTransaction_A = false;
      flag_evseUnauthorise_A = false;
      if (DEBUG_OUT) Serial.println("[EVSE_A]:Started");
#if DISPLAY_ENABLED
      	connAvail(1,"SESSION A STARTED");
  		checkForResponse_Disp();
#endif
#if LCD_DISPLAY_ENABLED
      lcd.clear();
      lcd.setCursor(3, 1);
      lcd.print("AUTHENTICATION");
	  lcd.setCursor(3, 2);
      lcd.print("SUCCESSFUL - 1");
	  delay(1500);
#endif
      /************************************/
      int transactionId = payload["transactionId"] | -1;
      getChargePointStatusService_A()->startTransaction(transactionId);        //new block for Three connector
      getChargePointStatusService_A()->startEnergyOffer();

	  resumeTxn_A.begin("resume_A", false);	
      resumeTxn_A.putString("idTagData_A",getChargePointStatusService_A()->getIdTag());
      resumeTxn_A.putBool("ongoingTxn_A",true);
      resumeTxn_A.putInt("TxnIdData_A", transactionId);  
	  resumeTxn_A.putFloat("meterStart", globalmeterstartA);
	  resumeTxn_A.end();

	  Tag_Id_A = getChargePointStatusService_A()->getIdTag();
      Transaction_Id_A = String(transactionId);
      Start_Meter_Value_A = String(globalmeterstartA);
      Sendstart_time_A = now();

	  session_ongoing_flag_A = true;
	  session_ongoing_flag = true;
	  session_on_offline_txn_completed = false;
	  session_ongoing_A_count = 1;
      Serial.println("[EVSE_A]:Charging");
	  getChargePointStatusService_A()->setChargePointstatus(Charging);
			
      //*****Storing tag data in EEPROM****//
      /*
	  resumeTxn.putString("idTagData",currentIdTag);
      resumeTxn.putBool("ongoingTxn",true);*/
      //***********************************//

      } else {
        flag_evseReadIdTag_A = false;
        flag_evseAuthenticate_A = false;
        flag_evseStartTransaction_A = false;
        flag_evRequestsCharge_A = false;
        flag_evseStopTransaction_A = false;
        flag_evseUnauthorise_A = true; //wrong ID tag received, so clearing the global current ID tag variable and setting up to read again.
        if (DEBUG_OUT) Serial.println("[EVSE_A]-StartDenied");
#if LCD_DISPLAY_ENABLED
      lcd.clear();
      lcd.setCursor(3, 1);
      lcd.print("AUTHENTICATION");
	  lcd.setCursor(3, 2);
      lcd.print("DENIED - 1");
	  delay(1500);
#endif

#if CP_ACTIVE
		requestforCP_OUT(STOP);  //stop pwm
#endif
		/*resume namespace
		resumeTxn.putBool("ongoingTxn",false);
		resumeTxn.putString("idTagData","");
		*/

      }
		}); });

	EVSE_A_setOnStopTransaction([]()
								{
		flag_evseStopTransaction_A = false;
		getChargePointStatusService_A()->setFinishing(true);
		// getChargePointStatusService_A()->setChargePointstatus(Finishing);
		// if(notFaulty_A)
		// {
		// 	getChargePointStatusService_A()->setChargePointstatus(Available);
		// }
		
		if (getChargePointStatusService_A() != NULL) {
          getChargePointStatusService_A()->stopEnergyOffer();
        }
        int txnId = getChargePointStatusService_A()->getTransactionId();
        int connector = getChargePointStatusService_A()->getConnectorId();
	if(txnId !=prevTxnId_A)
	{
		OcppOperation *stopTransaction = makeOcppOperation(&webSocket, new StopTransaction(currentIdTag_A, txnId, connector));
		initiateOcppOperation(stopTransaction);
		prevTxnId_A = txnId;	
    	if (DEBUG_OUT) Serial.println("[EVSE_A]:ClosingRelays");
		#if LCD_DISPLAY_ENABLED
  			lcd.clear();
  			lcd.setCursor(2, 1);
  			lcd.print("OUTPUT 1 STOPPED");
#endif
LastPresentEnergy_A = (discurrEnergy_A)*1000 + LastPresentEnergy_A;

    	/**********************Until Offline functionality is implemented***********/
    	//Resume namespace(Preferences)
    	if(getChargePointStatusService_A()->getEmergencyRelayClose() == false){
    		//requestLed(GREEN,START,1);   //temp fix     // it is affecting offline led functionality
    	}
		#if 1
		resumeTxn_A.begin("resume_A", false);
    	resumeTxn_A.putBool("ongoingTxn_A", false);
    	// resumeTxn_A.putString("idTagData_A","");
		// resumeTxn_A.putInt("TxnIdData_A", -1);
		resumeTxn_A.end();
		#endif
		
		if(ethernet_connect)
		{
			if(!webSocketConncted)
			{
				 flag_evseReadIdTag_A = true;
		        flag_evseAuthenticate_A = false;
		        flag_evseStartTransaction_A = false;
		        flag_evRequestsCharge_A = false;
		        flag_evseStopTransaction_A = false;
		        flag_evseUnauthorise_A = false;
	    		// Serial.println("[EVSE_A]IdTag-cleared");
			}
		}
    	else if(wifi_connect == true)
		{
	    	if(!webSocketConncted || WiFi.status() != WL_CONNECTED || isInternetConnected == false){
		        getChargePointStatusService_A()->stopTransaction();
		    	getChargePointStatusService_A()->unauthorize();  //can be buggy 
		        flag_evseReadIdTag_A = true;
		        flag_evseAuthenticate_A = false;
		        flag_evseStartTransaction_A = false;
		        flag_evRequestsCharge_A = false;
		        flag_evseStopTransaction_A = false;
		        flag_evseUnauthorise_A = false;
	    		// Serial.println("[EVSE_A]IdTag-cleared");
	    	}
    	}
		else if(gsm_connect == true)
		{
    		 if(client.connected() == false){
    		 	getChargePointStatusService_A()->stopTransaction();
		    	getChargePointStatusService_A()->unauthorize();  //can be buggy 
		        flag_evseReadIdTag_A = true;
		        flag_evseAuthenticate_A = false;
		        flag_evseStartTransaction_A = false;
		        flag_evRequestsCharge_A = false;
		        flag_evseStopTransaction_A = false;
		        flag_evseUnauthorise_A = false;
	    		// Serial.println("[EVSE_A]IdTag-cleared");
    		 }
    	}
		Serial.println("[EVSE_A]IdTag-cleared");
    	// requestForRelay(STOP, 1);
		online_session_ongoing_A = 0;
    	delay(500);
		stopTransaction->setOnReceiveConfListener([](JsonObject payload) {
#if LCD_ENABLED
  			lcd.clear();
  			lcd.setCursor(3, 2);
  			lcd.print("STOP A SUCCESS");
#endif
			flag_evseReadIdTag_A = false;
			flag_evseAuthenticate_A = false;
			flag_evseStartTransaction_A = false;
			flag_evRequestsCharge_A = false;
			flag_evseStopTransaction_A = false;
			flag_evseUnauthorise_A = true;
			getChargePointStatusService_A()->stopTransaction();
      		// if (DEBUG_OUT) Serial.print("[EVSE_A]ClosingRelay");
			if (DEBUG_OUT) Serial.println("[EVSE_A]StopSuccesful,Relayclose");
			// if (DEBUG_OUT) Serial.print(F("EVSE_setOnStopTransaction Callback: Reinitializing for new transaction. \n"));
			
		});
		
	}
	else
	{
		Serial.println("EVSE_A]:AlreadyStopped");
	} });

	EVSE_A_setOnUnauthorizeUser([]()
								{
		if(flag_evseSoftReset_A == true){
			//This 'if' block is developed by @Wamique.
			flag_evseReadIdTag_A = false;
			flag_evseAuthenticate_A = false;
			flag_evseStartTransaction_A = false;
			flag_evRequestsCharge_A = false;
			flag_evseStopTransaction_A = false;
			flag_evseUnauthorise_A = false;
			flag_rebootRequired_A = true;
			getChargePointStatusService_A()->unauthorize();
			if(fault_occured_A != 1)
			{
				getChargePointStatusService_A()->setChargePointstatus(Available);
			}
			if (DEBUG_OUT) Serial.println("[EVSE_A]-UnAuthorized");
		} else if(flag_evseSoftReset_A == false){
			flag_evseReadIdTag_A = true;
			flag_evseAuthenticate_A = false;
			flag_evseStartTransaction_A = false;
			flag_evRequestsCharge_A = false;
			flag_evseStopTransaction_A = false;
			flag_evseUnauthorise_A = false;
			if (DEBUG_OUT) Serial.println("[EVSE_A]:Unauthorizing");
			getChargePointStatusService_A()->unauthorize();
            if(fault_occured_A != 1)
			{
			getChargePointStatusService_A()->setChargePointstatus(Available);
			}
			
		} });

	/*EVSE_A_setOnTriggerMeterValue([] ()
	{

	});*/
}

/*********************************************************************/
void EVSE_A_initialize()
{
	if (DEBUG_OUT)
		Serial.println("[EVSE_A]:Starting Boot");
	// onBoot_A();
	faultTimer_A = millis();
	 evse_boot_state = EVSE_BOOT_INITIATED;
}

// This is the main loop function which is controlling the whole charfing process. All the flags created are used to control the flow of the program.

void EVSE_A_loop()
{
	if (flag_evseIsBooted_A == false)
	{
		#if 0
		if (DEBUG_OUT)
			Serial.println(F("[EVSE] Booting..."));
		delay(1000);
		// onBoot();
		t = millis();
		/*
		@brief : To send the heartbeat repeatedly until it gets accepted.
		By G. Raja Sumant 19/10/2022
		*/
		if (onBoot_A != NULL)
		{
			if (millis() - timerHb > (heartbeatInterval * 1000))
			{
				timerHb = millis();
				onBoot_A();
			}
		}
		return;
		#endif
		switch (evse_boot_state)
        {
        case EVSE_BOOT_INITIATED:
        {
            Serial.println("CP LOOP: boot notification ");
            OcppOperation* bootNotification = makeOcppOperation(&webSocket, new BootNotification());
            initiateOcppOperation(bootNotification);
            meteringService->init(meteringService);
#if 0
            bootNotification->setOnReceiveConfListener([](JsonObject payload)
                {
                    if (DEBUG_OUT)
                        Serial.print(F("EVSE_setOnBoot Callback: boot massge recived.\n"));
                    // gu8_evse_change_state = EVSE_READ_RFID;
                    meteringService->init(meteringService); });
#endif
            evse_boot_state = EVSE_BOOT_SENT;
            Serial.print("\r\nevse_boot_state  :EVSE_BOOT_INITIATED\r\n");
            break;
        }
        case EVSE_BOOT_SENT:
        {
            if (gu8_state_change == 0)
            {
                gu8_state_change = 1;
                Serial.print("\r\nevse_boot_state :EVSE_BOOT_SENT\r\n");
            }
            break;
        }
        case EVSE_BOOT_ACCEPTED:
        {
            evse_boot_state = EVSE_BOOT_DEFAULT;
 
			flag_evseIsBooted_A = true; //Exit condition for booting. 	
			flag_evseReadIdTag_A = true; //Entry condition for reading ID Tag.
			flag_evseAuthenticate_A = false;
			flag_evseStartTransaction_A = false;
			flag_evRequestsCharge_A = false;
			flag_evseStopTransaction_A = false;
			flag_evseUnauthorise_A = false;
			EvseDevStatus_connector_1 = flag_EVSE_Read_Id_Tag;
			
			// if(ongoingTxn_A)
			// {
			// flag_evRequestsCharge_A = true;
			// ongoingTxn_A_stop = 1;
			// if(DEBUG_OUT) Serial.println(F("****[EVSE_setOnBoot] on going transaction is true*****"));
			// }
			// else
			// {
			// 	flag_evRequestsCharge_A = false;
			// 	if(DEBUG_OUT) Serial.println(F("****[EVSE_setOnBoot] on going transaction is false*****"));
			// }

			#if !SEEMA_CONNECT
			if((ongoingTxn_A == 1) && (stopoffline_A == 0))
			{
				getChargePointStatusService_A()->setChargePointstatus(Charging);
			}
			if((ongoingTxn_B == 1) && (stopoffline_B == 0))
			{
				getChargePointStatusService_B()->setChargePointstatus(Charging);
			}
			if((ongoingTxn_C == 1) && (stopoffline_C == 0))
			{
				getChargePointStatusService_C()->setChargePointstatus(Charging);
			}
			#endif

            gu8_state_change = 0;
			gu8_bootsuccess = 1;
            Serial.print("\r\nevse_boot_state  :EVSE_BOOT_ACCEPTED\r\n");
            break;
        }
        case EVSE_BOOT_REJECTED:
        {
            evse_boot_state = EVSE_BOOT_INITIATED;
            Serial.print("\r\nevse_boot_state  :EVSE_BOOT_REJECTED\r\n");
            break;
        }
        default:
            break;
        }
	}

	else if (flag_evseIsBooted_A == true)
	{
		if (flag_evseReadIdTag_A == true)
		{
			
			if (onReadUserId_A != NULL)
			{
				onReadUserId_A();
				// if(millis() - t > 5000){
				// t= millis();
				// Added by G. Raja Sumant - 14/05/2022
				if (millis() - timerHb > (heartbeatInterval * 1000))
				{
					timerHb = millis();
					// onSendHeartbeat();
					onSendHeartbeat_A();
				}
			}
			return;
		}
		else if (flag_evseAuthenticate_A == true)
		{
			if (onAuthentication_A != NULL)
			{

				// Add condition by checking if available or unavailable
				bool un = false;
				un = getChargePointStatusService_A()->getUnavailable();
				currentIdTag_A = getChargePointStatusService_A()->getIdTag();
				currentIdTag = currentIdTag_A; /* Assinging currentIdTag_A to currentIdTag for check Auth cache  */
				if (!un)
				{
					if (flag_evseReserveNow_A)
					{
						Serial.println("EVSE_A:ReserveIdTag");
						Serial.println(reserve_currentIdTag_A);

						Serial.println("currentIdTag_A :");
						Serial.println(currentIdTag_A);

						if (reserve_currentIdTag_A == currentIdTag_A)
						{
							if (DEBUG_OUT)
								Serial.println("reservation tag matched\n");

							bool auth_checker = false;
							auth_checker = requestGetAuthCache();

							if (auth_checker)
							{
								getChargePointStatusService_A()->authorize(currentIdTag_A);
								flag_evseReadIdTag_A = false;
								flag_evseAuthenticate_A = false;
								flag_evseStartTransaction_A = true; // Entry condition for starting transaction.
								flag_evRequestsCharge_A = false;
								flag_evseStopTransaction_A = false;
								flag_evseUnauthorise_A = false;

								if (DEBUG_OUT)
									Serial.println("[EVSE_A]AuthAccepted");
								// requestLed(BLINKYBLUE, START, 1);
							}
							else
							{
								onAuthentication_A();
							}
						}
						else
						{

							if (DEBUG_OUT)
								Serial.println("EVSE_A:Reserved Tag Not matched");
							return;
						}
					}
					else
					{
						// if (requestGetAuthCache())
						if(0)
						{
							getChargePointStatusService_A()->authorize(currentIdTag_A);
							flag_evseReadIdTag_A = false;
							flag_evseAuthenticate_A = false;
							flag_evseStartTransaction_A = true; // Entry condition for starting transaction.
							flag_evRequestsCharge_A = false;
							flag_evseStopTransaction_A = false;
							flag_evseUnauthorise_A = false;

							if (DEBUG_OUT)
								Serial.println("EVSE_A:AuthAccepted");
							// requestLed(BLINKYBLUE, START, 1);
						}
						else
						{
							onAuthentication_A();
						}
					}
				}
			}
			return;
		}
		else if (flag_evseStartTransaction_A == true)
		{
			if (onStartTransaction_A != NULL)
			{
#if CP_ACTIVE
				if ((EVSE_state == STATE_C || EVSE_state == STATE_D) && getChargePointStatusService_A()->getEmergencyRelayClose() == false)
				{
					onStartTransaction_A();
				}
				else
				{
					Serial.println(F("Connect the Connector to EV / Or fault exist")); // here have to add timeout of 30 sec
					connectorDis_counter_A++;
					// EVSE_stopTransactionByRfid();
					if (connectorDis_counter_A > 25)
					{
						connectorDis_counter_A = 0;

						EVSE_A_StopSession();
					}
				}
#endif

#if !CP_ACTIVE
				onStartTransaction_A();
#endif
			}
		}
		else if (flag_evRequestsCharge_A == true)
		{

#if CP_ACTIVE
			// flag_evRequestsCharge = false;
			if (getChargePointStatusService_A() != NULL && getChargePointStatusService_A()->getEvDrawsEnergy() == false)
			{

				/***********************Control Pilot @Wamique******************/
				if (EVSE_state == STATE_C || EVSE_state == STATE_D)
				{
					if (getChargePointStatusService_A()->getEmergencyRelayClose() == false)
					{
						EVSE_A_StartCharging();
					}
					else if (getChargePointStatusService_A()->getEmergencyRelayClose() == true)
					{
						Serial.println(F("The voltage / current / Temp is out or range. FAULTY CONDITION DETECTED."));
					}
				}
				else if (EVSE_state == STATE_SUS)
				{
					EVSE_A_Suspended();
					Serial.println(counter1);
					if (counter1++ > 25)
					{ // Have to implement proper timeout
						counter1 = 0;
						EVSE_A_StopSession();
					}
				}
				else if (EVSE_state == STATE_DIS || EVSE_state == STATE_E || EVSE_state == STATE_B || EVSE_state == STATE_A)
				{

					//	EVSE_StopSession();     // for the very first time cable can be in disconnected state

					// if(txn == true){           // can implement counter > 10 just to remove noise
					EVSE_A_StopSession();
					//	}
				}
				else
				{

					Serial.println("[EVSE] STATE Error" + String(EVSE_state));
					delay(2000);

					//	requestLed(RED,START,1);
				}
			}
			if (getChargePointStatusService_A()->getEvDrawsEnergy() == true)
			{

				//	txn = true;

				if (EVSE_state == STATE_C || EVSE_state == STATE_D)
				{

					if (DEBUG_OUT)
						Serial.println(F("[EVSE_CP] Drawing Energy"));

					if (millis() - t > 10000)
					{
						if (getChargePointStatusService_A()->getEmergencyRelayClose() == false)
						{
							requestLed(BLINKYGREEN, START, 1);
							t = millis();
						}
					}
					/*
					if(blinckCounter++ % 2 == 0){
						requestLed(GREEN,START,1);
					}else{
						requestLed(GREEN,STOP,1);
					}*/
				}
				else if (EVSE_state == STATE_A || EVSE_state == STATE_E || EVSE_state == STATE_B)
				{ // Although CP Inp will never go to A,B state
					if (counter_faultstate++ > 5)
					{
						EVSE_StopSession_A();
						counter_faultstate = 0;
					}
				}
				else if (EVSE_state == STATE_SUS)
				{
					EVSE_Suspended_A(); // pause transaction :update suspended state is considered in charging state
				}
				else if (EVSE_state == STATE_DIS)
				{

					Serial.println(F("[EVSE] Connect the Connector with EV and Try again"));
					EVSE_StopSession_A();
				}
			}

			/***Implemented Exit Feature with RFID @Wamique****/
//	  EVSE_stopTransactionByRfid_A();
#endif

#if !CP_ACTIVE
			if (getChargePointStatusService_A() != NULL && getChargePointStatusService_A()->getEvDrawsEnergy() == false)
			{
				if (getChargePointStatusService_A()->getEmergencyRelayClose() == false)
				{
					getChargePointStatusService_A()->startEvDrawsEnergy();

					if (DEBUG_OUT)
						Serial.println("EVSE_A:OpeningRelays");
					reasonForStop_A = 3; // Local
					requestForRelay(START, 1);
#if DISPLAY_ENABLED
					flag_tapped = true;
#endif
					// No need of such an action
					/*requestLed(ORANGE,START,1);
					delay(1200);
					requestLed(WHITE,START,1);
					delay(1200);
					requestLed(GREEN,START,1);
					delay(1000);*/
					if (DEBUG_OUT)
						Serial.println("EVSE_A:DrawingEnergy");
				}
				else if (getChargePointStatusService_A()->getEmergencyRelayClose() == true)
				{
					Serial.println("EVSE_A:FaultDetected");
				}
			}
			if (getChargePointStatusService_A()->getEvDrawsEnergy() == true)
			{
				// delay(250);
				// displayMeterValues();
				disp_evse_A = true;
				start_session_A = 0;
				Display_Set(start_session_A);
				if (DEBUG_OUT)
					Serial.println("[EVSE_A] Drawing Energy");

				// blinking green Led
				if (millis() - t > 10000) // Change from 5 to 10 for independent emergency.
				{
					// if((WiFi.status() == WL_CONNECTED) && (webSocketConncted == true) && (isInternetConnected == true)&& getChargePointStatusService()->getEmergencyRelayClose() == false){
					// 	requestLed(BLINKYGREEN_EINS,START,1);
					// 	t = millis();
					// }
					//	onSendHeartbeat_A();
					if (getChargePointStatusService_A()->getEmergencyRelayClose() == false)
					{
						// requestLed(BLINKYGREEN, START, 1);
						online_session_ongoing_A = 1;
						t = millis();

						if (millis() - relay_timer_A > 15000)
						{

							// requestForRelay(START, 1);
							relay_timer_A = millis();
						}
					}
				}
				#if 0
				// Current check
				drawing_current_A = eic.GetLineCurrentA();
				Serial.println("Current A: " + String(drawing_current_A));
				if (drawing_current_A <= minCurr)
				{
					Serial.println("Min current_A");
					if (notFaulty_A)
					{
						counter_drawingCurrent_A++;
						 Serial.println("counter_drawingCurrent_A online: " + (String)counter_drawingCurrent_A);
					}
					// if(counter_drawingCurrent_A > 120){
					if (counter_drawingCurrent_A > currentCounterThreshold_A)
					{
						counter_drawingCurrent_A = 0;
						if (reasonForStop_A != 3 || reasonForStop_A != 4)
							reasonForStop_A = 1; // EV disconnected
						Serial.println(F("Stopping session due to No current"));
#if LCD_ENABLED
						if (notFaulty_A)
						{
							lcd.clear();
							if (currentCounterThreshold_A == 2)
							{
								lcd.setCursor(0, 1);
								lcd.print("A: EV DISCONNECTED!");
							}
							else
							{
								lcd.setCursor(3, 0);
								lcd.print("A: NO POWER DRAWN");
							}
						}
#endif
						EVSE_A_StopSession();
					}
				}
				else
				{
					counter_drawingCurrent_A = 0;
					if (notFaulty_A)
					{
						// currentCounterThreshold_A = 2;
						// reasonForStop_A = 1;
					}
					// currentCounterThreshold_A = 60; //ARAI expects 2
					Serial.println(F("counter_drawing Current Reset"));
				}
				#endif
			}
			// Implemented Exit Feature with RFID @Wamique//
// EVSE_A_stopTransactionByRfid();
#endif
			// this is the only 'else if' block which is calling next else if block. the control is from this file itself. the control is not changed from any other file. but the variables are required to be present as extern in other file to decide calling of other functions.
			return;
		}
		else if (flag_evseStopTransaction_A == true)
		{
			if (getChargePointStatusService_A() != NULL)
			{
				getChargePointStatusService_A()->stopEvDrawsEnergy();
			}
			if (onStopTransaction_A != NULL)
			{
				onStopTransaction_A();
#if CP_ACTIVE
				requestforCP_OUT(STOP); // stop pwm
#endif
			}
			return;
		}
		else if (flag_evseUnauthorise_A == true)
		{
			if (onUnauthorizeUser_A != NULL)
			{
				onUnauthorizeUser_A();
			}
			return;
		}
		else if (flag_rebootRequired_A == true && flag_rebootRequired_B == true && flag_rebootRequired_C == true)
		{
			// soft reset execution.
			//  flag_evseIsBooted_A = false;
			//  flag_rebootRequired_A = false;
			//  flag_evseSoftReset_A = false;
			if (getChargePointStatusService_A()->inferenceStatus() != ChargePointStatus::Charging &&
				getChargePointStatusService_B()->inferenceStatus() != ChargePointStatus::Charging &&
				getChargePointStatusService_C()->inferenceStatus() != ChargePointStatus::Charging)
			{
				if (DEBUG_OUT)
					Serial.println("[EVSE_A]Rebooting\n");
				delay(5000);
				ESP.restart();
			}
		}
		else
		{
			if (DEBUG_OUT)
				Serial.println("[EVSE_A] waiting for response...\n");
			// delay(100);
		}
	}
}

short EMGCY_counter_A = 0;
extern short EMGCY_counter_B;
extern short EMGCY_counter_C;
extern short EMGCY_counter_B;
extern short EMGCY_counter_C;
bool EMGCY_FaultOccured_A = false;
extern bool EMGCY_FaultOccured_B;
extern bool EMGCY_FaultOccured_C;

void emergencyRelayClose_Loop_A()
{
	if (millis() - faultTimer_A > 2000)
	{ // This was 2000 earlier

		// Added a new condition to check the toggling of relays in no earth state.
		// G. Raja Sumant - 06/05/2022
		if (getChargePointStatusService_A()->getOverCurrent() == true)
		{
			// getChargePointStatusService_A()->stopEvDrawsEnergy();
			// getChargePointStatusService_A()->setEmergencyRelayClose(true);
			if (getChargePointStatusService_A()->getTransactionId() != -1)
			{
// flag_evRequestsCharge_A = false;
// flag_evseStopTransaction_A = true;
#if LCD_ENABLED
				lcd.clear();
				lcd.setCursor(0, 0); // Or setting the cursor in the desired position.
				lcd.print("STATUS: FAULTED");
				lcd.setCursor(0, 1);
				lcd.print("A: OVER CURRENT");
#endif
				EVSE_A_StopSession();
#if LED_ENABLED
				// requestLed(RED, START, 1);
				// delay(2000);
#endif
			}
		}
		bool EMGCY_status_A = requestEmgyStatus();
		Serial.println("EMGCY_Status_A: " + String(EMGCY_status_A));
		if (EMGCY_status_A == true)
		{
			EMGCY_status_B = true;
			EMGCY_status_C = true;
			if (EMGCY_counter_A++ > 0)
			{
				// if(EMGCY_counter_A == 0){
				notFaulty_A = false;
				fault_code_A = 8;
				fault_code_B = 8;
				fault_code_C = 8;

				requestForRelay(STOP, 0); // Zero means all!
				// requestForRelay(STOP,2);
				// requestForRelay(STOP,3);
				reasonForStop_A = 0;
				disp_evse_A = false;
				gu8_Remote_start_A = 0;
      			Stop_Session_A = 0;
                Display_Clear(Stop_Session_A);
				// requestLed(BLINKYRED, START, 0); //commented on 09112023 by shiva
// requestLed(BLINKYRED,START,2);
// requestLed(BLINKYRED,START,3);
#if DWIN_ENABLED
				uint8_t err = 0;
				// fault_emgy[4] = 0X51; // In the first page.
				// err = DWIN_SET(fault_emgy,sizeof(fault_emgy)/sizeof(fault_emgy[0]));
				fault_emgy[4] = 0X66; // In the fourth page.
				err = DWIN_SET(fault_emgy, sizeof(fault_emgy) / sizeof(fault_emgy[0]));
				fault_emgy[4] = 0X71; // In the fourth page.
				err = DWIN_SET(fault_emgy, sizeof(fault_emgy) / sizeof(fault_emgy[0]));
				fault_emgy[4] = 0X7B; // In the fourth page.
				err = DWIN_SET(fault_emgy, sizeof(fault_emgy) / sizeof(fault_emgy[0]));
#endif
#if DISPLAY_ENABLED
				setHeader("RFID UNAVAILABLE");
				checkForResponse_Disp();
				connAvail(1, "FAULTED EMGY");
				checkForResponse_Disp();
#endif

#if LCD_ENABLED
				lcd.clear();
				// lcd.setCursor(0, 0); // Or setting the cursor in the desired position.
				// lcd.print("                    ");//Clear the line
				lcd.setCursor(0, 0); // Or setting the cursor in the desired position.
				lcd.print("FAULTED: EMERGENCY");
#endif

				getChargePointStatusService_A()->setEmergencyRelayClose(true);
				EMGCY_FaultOccured_A = true;
				EMGCY_counter_A = 0;
				getChargePointStatusService_A()->setChargePointstatus(Faulted);
				fault_occured_A = 1;
				/*getChargePointStatusService_B()->setEmergencyRelayClose(true);
				EMGCY_FaultOccured_B = true;
				EMGCY_counter_B = 0;

				getChargePointStatusService_C()->setEmergencyRelayClose(true);
				EMGCY_FaultOccured_C = true;
				EMGCY_counter_C = 0;*/
			}
		}
		else
		{
			EMGCY_status_B = false;
			EMGCY_status_C = false;
			notFaulty_A = true;
			EMGCY_FaultOccured_A = false;
			EMGCY_counter_A = 0;
			
			getChargePointStatusService_A()->setEmergencyRelayClose(false);
			// if (getChargePointStatusService_A()->getChargePointstatus() != Available &&
			// getChargePointStatusService_A()->getChargePointstatus() != Charging &&
			// getChargePointStatusService_A()->getChargePointstatus() != Preparing &&
			// getChargePointStatusService_A()->getChargePointstatus() != Finishing )
			// {
			//     getChargePointStatusService_A()->setChargePointstatus(Available);
			// }
			if ((fault_occured_A == 1))
			{
				if ((flag_evRequestsCharge_A == false) && (ongoingTxn_A == false))
				{
					fault_occured_A = 0;
					getChargePointStatusService_A()->setChargePointstatus(Available);
				}
				else if ((flag_evRequestsCharge_A == true))
				{
					fault_occured_A = 0;
					getChargePointStatusService_A()->setChargePointstatus(Charging);
				}
			}
		}

		if (EMGCY_FaultOccured_A == true && getChargePointStatusService_A()->getTransactionId() != -1)
		{
			Serial.println("[EVSE_A]:EmergencyStop");
			// flag_evseReadIdTag_A = false;
			// flag_evseAuthenticate_A = false;
			// flag_evseStartTransaction_A = false;
			// flag_evRequestsCharge_A = false;
			// flag_evseStopTransaction_A = true;
			// session_ongoing_flag = false;
			// session_ongoing_flag_A = false;
			if (getChargePointStatusService_A()->getEvDrawsEnergy() == true)
			{
			    EVSE_A_StopSession();
			}
			online_session_ongoing_A = 0;
		}
		else if (EMGCY_FaultOccured_A == false)
		{
			// float volt = eic.GetLineVoltageA();
			// float current = eic.GetLineCurrentA();
			// float temp = eic.GetTemperature();
			// Serial.println("Voltage_A: " + String(volt) + ", Current_A: " + String(current) + ", Temperature: " + String(temp));
			if (getChargePointStatusService_A() != NULL)
			{
				if (getChargePointStatusService_A()->getOverVoltage() == true ||
					getChargePointStatusService_A()->getUnderVoltage() == true ||
					getChargePointStatusService_A()->getUnderTemperature() == true ||
					getChargePointStatusService_A()->getOverTemperature() == true ||
					getChargePointStatusService_A()->getEarthDisconnect() == true || // New fault added.
					getChargePointStatusService_A()->getOverCurrent() == true)
				{
					Serial.println("[EVSE_A] Fault Occurred.");
					notFaulty_A = false;
#if 0
#if LCD_ENABLED
					lcd.setCursor(0, 0);			   // Or setting the cursor in the desired position.
					lcd.print("                    "); // Clear the line
					lcd.print("A: FAULTED");
#endif
#endif
					getChargePointStatusService_A()->setEmergencyRelayClose(true);
					getChargePointStatusService_A()->setChargePointstatus(Faulted);
					fault_occured_A = 1;
					/*
					 * @brief  Unnecessary!
					 */
					EMGCY_FaultOccured = true;
					// getChargePointStatusService_A()->setEmergencyRelayClose(true);
					if (!timer_initialize_A)
					{
						timeout_start_A = millis();
						timer_initialize_A = true;
					}
				}
				else if (getChargePointStatusService_A()->getOverVoltage() == false &&
						 getChargePointStatusService_A()->getUnderVoltage() == false &&
						 getChargePointStatusService_A()->getUnderTemperature() == false &&
						 getChargePointStatusService_A()->getOverTemperature() == false &&
						 getChargePointStatusService_A()->getEarthDisconnect() == false && // New fault added.
						 getChargePointStatusService_A()->getOverCurrent() == false)
				{
					Serial.println("[EVSE_A] Not Faulty.");
					notFaulty_A = true;
					getChargePointStatusService_A()->setEmergencyRelayClose(false);

					// if (getChargePointStatusService_A()->getChargePointstatus() != Available &&
					// getChargePointStatusService_A()->getChargePointstatus() != Charging &&
					// getChargePointStatusService_A()->getChargePointstatus() != Preparing &&
					// getChargePointStatusService_A()->getChargePointstatus() != Finishing )
					// {
					//     getChargePointStatusService_A()->setChargePointstatus(Available);
					// }
			if ((fault_occured_A == 1))
			{
				if ((flag_evRequestsCharge_A == false) && (ongoingTxn_A == false))
				{
					fault_occured_A = 0;
					getChargePointStatusService_A()->setChargePointstatus(Available);
				}
				else if ((flag_evRequestsCharge_A == true))
				{
					fault_occured_A = 0;
					getChargePointStatusService_A()->setChargePointstatus(Charging);
				}
			}

					// if (!timer_initialize){
					timeout_start_A = 0;
					timer_initialize_A = false;
					//}
				}

				if (getChargePointStatusService_A()->getEmergencyRelayClose() == true)
				{
					timeout_active_A = true;
					requestForRelay(STOP, 1);
					online_session_ongoing_A = 0;
					delay(50);
#if LED_ENABLED
					// requestLed(RED, START, 1);
#endif
					flag_faultOccured_A = true;
				}
				else if (getChargePointStatusService_A()->getEmergencyRelayClose() == false && flag_faultOccured_A == true)
				{
					timeout_active_A = false;
					if ((getChargePointStatusService_A()->getTransactionId() != -1))
					{ // can be buggy
						if (fault_counter_A++ > 1)
						{
							fault_counter_A = 0;
							// requestForRelay(START,1);
							// delay(50);
							Serial.println("[EmergencyRelay_A] Starting Txn");
							flag_faultOccured_A = false;
						}
					}
				}

				if (timeout_active_A && getChargePointStatusService_A()->getTransactionId() != -1)
				{
					if (millis() - timeout_start_A >= TIMEOUT_EMERGENCY_RELAY_CLOSE_A)
					{
						Serial.println("EVSE_A:stoptxn");
						flag_evRequestsCharge_A = false;
						flag_evseStopTransaction_A = true;
						timeout_active_A = false;
						timer_initialize_A = false;
					}
				}
			}
		}
	}
}

/*
 * @param limit: expects current in amps from 0.0 to 32.0
 */
void EVSE_A_setChargingLimit(float limit)
{
	if (DEBUG_OUT)
		Serial.print(F("[EVSE] New charging limit set. Got "));
	if (DEBUG_OUT)
		Serial.print(limit);
	if (DEBUG_OUT)
		Serial.print(F("\n"));
	chargingLimit_A = limit;
}

bool EVSE_A_EvRequestsCharge()
{
	return flag_evRequestsCharge_A;
}

bool EVSE_A_EvIsPlugged()
{
	return evIsPlugged_A;
}

void EVSE_A_setOnBoot(OnBoot_A onBt_A)
{
	onBoot_A = onBt_A;
}

void EVSE_A_setOnReadUserId(OnReadUserId_A onReadUsrId_A)
{
	onReadUserId_A = onReadUsrId_A;
}

void EVSE_A_setOnsendHeartbeat(OnSendHeartbeat_A onSendHeartbt_A)
{
	onSendHeartbeat_A = onSendHeartbt_A;
}

void EVSE_A_setOnAuthentication(OnAuthentication_A onAuthenticatn_A)
{
	onAuthentication_A = onAuthenticatn_A;
}

void EVSE_A_setOnStartTransaction(OnStartTransaction_A onStartTransactn_A)
{
	onStartTransaction_A = onStartTransactn_A;
}

void EVSE_A_setOnStopTransaction(OnStopTransaction_A onStopTransactn_A)
{
	onStopTransaction_A = onStopTransactn_A;
}

void EVSE_A_setOnUnauthorizeUser(OnUnauthorizeUser_A onUnauthorizeUsr_A)
{
	onUnauthorizeUser_A = onUnauthorizeUsr_A;
}

// void EVSE_A_getSsid(String &out) {
// 	out += "Pied Piper";
// }
// void EVSE_getPass(String &out) {
// 	out += "plmzaq123";
// }

void EVSE_A_getChargePointSerialNumber(String &out)
{

	out += preferences.getString("chargepoint", "");

	/*
	#if STEVE
	out += "dummyCP002";
	#endif

	#if EVSECEREBRO
	out += "testpodpulkit";
	#endif
	*/
}

char *EVSE_A_getChargePointVendor()
{
	return "Amplify Mobility";
}

char *EVSE_A_getChargePointModel()
{
	return "AC001";
}

char *EVSE_A_getFirmwareVersion()
{
	return "AC001_4_7";
}

String EVSE_A_getCurrnetIdTag(MFRC522 *mfrc522)
{
	String currentIdTag = "";
	//	currentIdTag = EVSE_A_readRFID(mfrc522);    // masking rfid direct read from EVSE_A

	if (getChargePointStatusService_A()->getIdTag().isEmpty() == false)
	{
		if (DEBUG_OUT)
			Serial.println("[EVSE] Reading from Charge Point Station Service ID Tag stored.");
		currentIdTag = getChargePointStatusService_A()->getIdTag();
		if (DEBUG_OUT)
			Serial.print("[EVSE] ID Tag: ");
		if (DEBUG_OUT)
			Serial.println(currentIdTag);
		Serial.flush();
	}

	return currentIdTag;
}

String EVSE_A_readRFID(MFRC522 *mfrc522)
{
	String currentIdTag_A;
	currentIdTag_A = readRfidTag(true, mfrc522);
	return currentIdTag_A;
}

/********Added new funtion @Wamique***********************/

// void EVSE_A_stopTransactionByRfid(){

// 	Ext_currentIdTag_A = EVSE_A_readRFID(&mfrc522);
// 	if(currentIdTag_A.equals(Ext_currentIdTag_A) == true){
// 		flag_evRequestsCharge_A = false;
// 		flag_evseStopTransaction_A = true;
// 	}else{
// 			if(Ext_currentIdTag_A.equals("")==false)
// 			if(DEBUG_OUT) Serial.println("\n[EVSE_A] Incorrect ID tag\n");
// 		}
// }

#if CP_ACTIVE
/**************CP Implementation @mwh*************/
void EVSE_A_StartCharging()
{

	if (getChargePointStatusService_A()->getEvDrawsEnergy() == false)
	{
		getChargePointStatusService_A()->startEvDrawsEnergy();
	}
	if (DEBUG_OUT)
		Serial.print(F("[EVSE_A] Opening Relays.\n"));
	//   pinMode(32,OUTPUT);
	//  digitalWrite(32, HIGH); //RELAY_1
	// digitalWrite(RELAY_2, RELAY_HIGH);
	requestForRelay(START, 1);
	requestLed(ORANGE, START, 1);
	delay(1200);
	requestLed(WHITE, START, 1);
	delay(1200);
	requestLed(GREEN, START, 1);
	delay(1000);
	Serial.println(F("[EVSE_A] EV is connected and Started charging"));
	if (DEBUG_OUT)
		Serial.println(F("[EVSE_A] Started Drawing Energy"));
	delay(500);
}

void EVSE_A_Suspended()
{

	if (getChargePointStatusService_A()->getEvDrawsEnergy() == true)
	{
		getChargePointStatusService_A()->stopEvDrawsEnergy();
	}
	requestLed(BLUE, START, 1);
	requestForRelay(STOP, 1);
	//	delay(1000);
	Serial.printf(F("[EVSE_A] EV Suspended"));
}

/**************************************************/

#endif

void displayMeterValues()
{
	if (millis() - timerDisplay > 10000)
	{
		timerDisplay = millis();
		float instantCurrrent_A = eic.GetLineCurrentA();
		int instantVoltage_A = eic.GetLineVoltageA();
		float instantPower_A = 0.0f;

		if (instantCurrrent_A < minCurr)
		{
			instantPower_A = 0;
		}
		else
		{
			instantPower_A = (instantCurrrent_A * instantVoltage_A) / 1000.0;
		}

		/*float instantCurrrent_B = eic.GetLineCurrentB();
		int instantVoltage_B  = eic.GetLineVoltageB();
		float instantPower_B = 0.0f;

		if(instantCurrrent_B < 0.15){
			instantPower_B = 0;
		}else{
			instantPower_B = (instantCurrrent_B * instantVoltage_B)/1000.0;
		}

		float instantCurrrent_C = eic.GetLineCurrentC();
		int instantVoltage_C = eic.GetLineVoltageC();
		float instantPower_C = 0.0f;

		if(instantCurrrent_C < 0.15){
			instantPower_C = 0;
		}else{
			instantPower_C = (instantCurrrent_C * instantVoltage_C)/1000.0;
		}*/
		delay(100); // just for safety
#if LCD_ENABLED
		lcd.clear();
		lcd.setCursor(0, 0); // Or setting the cursor in the desired position.
		if (notFaulty_A)
		{
			lcd.print("*****CHARGING 1*****"); // You can make spaces using well... spaces
		}
		else
		{

			switch (fault_code_A)
			{
			case -1:
				break; // It means default.
			case 0:
				lcd.print("Connector1-Over Voltage");
				break;
			case 1:
				lcd.print("Connector1-Under Voltage");
				break;
			case 2:
				lcd.print("Connector1-Over Current");
				break;
			case 3:
				lcd.print("Connector1-Under Current");
				break;
			case 4:
				lcd.print("Connector1-Over Temp");
				break;
			case 5:
				lcd.print("Connector1-Under Temp");
				break;
			case 6:
				lcd.print("Connector1-GFCI"); // Not implemented in AC001
				break;
			case 7:
				lcd.print("Connector1-Earth Disc");
				break;
			default:
				lcd.print("*****FAULTED 1*****"); // You can make spaces using well... spacesbreak;
			}
		}
		/*
		lcd.setCursor(0, 1); // Or setting the cursor in the desired position.
		lcd.print("V:");
		lcd.setCursor(4, 1); // Or setting the cursor in the desired position.
		lcd.print(String(instantVoltage_A));
		lcd.setCursor(9, 1); // Or setting the cursor in the desired position.
		lcd.print(String(instantVoltage_B));
		lcd.setCursor(15, 1); // Or setting the cursor in the desired position.
		lcd.print(String(instantVoltage_C));
		lcd.setCursor(0, 2);
		lcd.print("I:");
		lcd.setCursor(4, 2); // Or setting the cursor in the desired position.
		lcd.print(String(instantCurrrent_A));
		lcd.setCursor(9, 2); // Or setting the cursor in the desired position.
		lcd.print(String(instantCurrrent_B));
		lcd.setCursor(15, 2); // Or setting the cursor in the desired position.
		lcd.print(String(instantCurrrent_C));
		 lcd.setCursor(0, 3);
		lcd.print("P:");
		lcd.setCursor(4, 3); // Or setting the cursor in the desired position.
		lcd.print(String(instantPower_A));
		lcd.setCursor(9, 3); // Or setting the cursor in the desired position.
		lcd.print(String(instantPower_B));
		lcd.setCursor(15, 3); // Or setting the cursor in the desired position.
		lcd.print(String(instantPower_C));*/

		lcd.setCursor(0, 1); // Or setting the cursor in the desired position.
		lcd.print("VOLTAGE(v):");
		lcd.setCursor(12, 1); // Or setting the cursor in the desired position.
		lcd.print(String(instantVoltage_A));
		lcd.setCursor(0, 2);
		lcd.print("CURRENT(A):");
		lcd.setCursor(12, 2); // Or setting the cursor in the desired position.
		lcd.print(String(instantCurrrent_A));
		lcd.setCursor(0, 3);
		lcd.print("POWER(KW) :");
		lcd.setCursor(12, 3); // Or setting the cursor in the desired position.
		lcd.print(String(instantPower_A));
#endif

#if DWIN_ENABLED
		uint8_t err = 0;
		change_page[9] = 4;
		v1[4] = 0X6A;
		instantVoltage_A = instantVoltage_A * 10;
		v1[6] = instantVoltage_A >> 8;
		v1[7] = instantVoltage_A & 0xff;
		/*v2[6] = instantVoltage_B >> 8;
		v2[7] = instantVoltage_B & 0xff;
		v3[6] = instantVoltage_C >> 8;
		v3[7] = instantVoltage_C & 0xff;*/
		i1[4] = 0X6C;
		i1[7] = instantCurrrent_A * 10;
		/*i2[7] = instantCurrrent_B*10;
		i3[7] = instantCurrrent_C*10;*/
		e1[4] = 0X6E;
		e1[7] = instantPower_A * 10;
		// e2[7] = instantPower_B*10;
		// e3[7] = instantPower_C*10;
		err = DWIN_SET(cid1, sizeof(cid1) / sizeof(cid1[0]));
		// delay(50);
		err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0])); // page 0
		// delay(50);
		if (notFaulty_A)
		{
			charging[4] = 0X66;
			err = DWIN_SET(charging, sizeof(charging) / sizeof(charging[0]));
		}
		// delay(50);
		err = DWIN_SET(v1, sizeof(v1) / sizeof(v1[0]));
		// err = DWIN_SET(v2,sizeof(v2)/sizeof(v2[0]));
		// err = DWIN_SET(v3,sizeof(v3)/sizeof(v3[0]));
		err = DWIN_SET(i1, sizeof(i1) / sizeof(i1[0]));
		// err = DWIN_SET(i2,sizeof(i2)/sizeof(i2[0]));
		// err = DWIN_SET(i3,sizeof(i3)/sizeof(i3[0]));
		err = DWIN_SET(e1, sizeof(e1) / sizeof(e1[0]));
		// err = DWIN_SET(e2,sizeof(e2)/sizeof(e2[0]));
		// err = DWIN_SET(e3,sizeof(e3)/sizeof(e3[0]));
		delay(500);

#endif
	}
}

/*************************************************EVSE A Reservation loop***********************************************************/
void EVSE_A_Reservation_loop()
{
#if 0
	int i = 0;
	while (i < ReserveNowSlotList.size()){
		reserveNow_slot *el = ReserveNowSlotList.get(i);
		boolean success = el->isAccepted;	
		if (success){
			flag_evseReserveNow = true;	
			currentIdTag = el->idTag;
			connectorId = el->connectorId ;
			reservationId = el->reservationId;
			reservation_start_time = el->start_time;
			reservation_expiry_time  = el->expiry_time; 
			
		ReserveNowSlotList.remove(i);
		//TODO Review: are all recources freed here?
		delete el;
		//go on with the next element in the queue, which is now at ReserveNowSlotList[i]
		} else {
		//There will be another attempt to send this conf message in a future loop call.
		//Go on with the next element in the queue, which is now at ReserveNowSlotList[i+1]
		i++;
		}
	}
#endif

	if (flag_evseReserveNow_A)
	{
		// Reserve Now execution.
		if (DEBUG_OUT)
			Serial.print("[EVSE_A] Reserve Now ...\n");

		// time_t start_reserve_time_delta_A = reservation_start_time_A - now();
		time_t reserve_time_delta_A = reserveDate_A - now();

		if ((reserve_time_delta_A <= reservedDuration_A) && (reserve_time_delta_A > 0))
		{
/*
 * @brief : Trigger a status notification of Reserved only once.
 */
#if 0
			//fire StatusNotification
		//TODO check for online condition: Only inform CS about status change if CP is online
		//TODO check for too short duration condition: Only inform CS about status change if it lasted for longer than MinimumStatusDuration
		OcppOperation *statusNotification = makeOcppOperation(webSocket,
		new StatusNotification(currentStatus));
		initiateOcppOperation(statusNotification);
#endif

			if (gu8_send_status_flag_A && reservation_start_flag_A)
			{
				gu8_send_status_flag_A = false;
				getChargePointStatusService_A()->setReserved(true);
			}

			if (!reservation_start_flag_A)
			{

				if (getChargePointStatusService_A()->getTransactionId() != -1)
				{
					EVSE_A_StopSession();
				}
				// requestLed(BLUE, START, 1);
				reservation_start_flag_A = true;
				flag_evseReadIdTag_A = true; // Entry condition for reading ID Tag.
				flag_evseAuthenticate_A = false;
				gu8_send_status_flag_A = true;
				if (DEBUG_OUT)
					Serial.print("[EVSE_A] Reserve Now  2 1 ...\n");
			}
			if (DEBUG_OUT)
				Serial.print("[EVSE_A] Reserve Now  2 ...\n");
			Serial.print("[EVSE_A] reserve time delta ");
			Serial.println(reserve_time_delta_A);
			if (getChargePointStatusService_A()->getEvDrawsEnergy() == false && notFaulty_A && getChargePointStatusService_A()->getEmergencyRelayClose() == false)
			{
				// requestLed(BLUE, START, 1);
			}
		}
		else
		{
			if ((reserve_time_delta_A <= 0))
			{
				flag_evseReserveNow_A = false;
				getChargePointStatusService_A()->setReserved(false);

#if 0
				/*
				* @bug : Session should not stop just because the reservation expired.
				Resolved by G. Raja Sumant 20/07/2022
				if (getChargePointStatusService_A()->getTransactionId() != -1)
				{
					EVSE_StopSession();
				}*/
#endif

				if (DEBUG_OUT)
					Serial.print("[EVSE_A] stopped due to Reservation timeout.....!\n");
				reservation_start_flag_A = false;
			}
		}

		if (flag_evseCancelReservation_A)
		{
			flag_evseCancelReservation_A = false;
			flag_evseReserveNow_A = false;
			getChargePointStatusService_A()->setReserved(false);
			// if(getChargePointStatusService_A()->getTransactionId() != -1)
			// {
			// 	EVSE_StopSession();
			// }
			if (DEBUG_OUT)
				Serial.print("[EVSE_A]stopped due to Cancel Reservation.....!\n");
			reservation_start_flag_A = false;
		}
	}
}
/*************************************************EVSE A Reservation loop***********************************************************/

#if 1

int offlineAuthorizeTxn(String &idTag)
{
	OcppOperation *authorize = makeOcppOperation(&webSocket, new Authorize(idTag));
	initiateOcppOperation(authorize);
	int ret = 0;
	authorize->setOnReceiveConfListener([](JsonObject payload)
										{
			const char* status = payload["idTagInfo"]["status"] | "Invalid";
			if (!strcmp(status, "Accepted")) {
				if (DEBUG_OUT) Serial.print(F("EVSE_setOnAuthentication Callback: Authorize request has been accepted! Calling StartTransaction Block.\n"));
			} else {
				if (DEBUG_OUT) Serial.print(F("EVSE_setOnAuthentication Callback: Authorize request has been denied! Read new User ID. \n"));

				// gs32_offlinestartStoptxn_update_flag = 0;

				// Serial.print("gs32_fetch_offline_txnlist_count 1 : " + String(gs32_fetch_offline_txnlist_count));
				// gs32_fetch_offline_txnlist_count--; // Decrement the fetch offline transaction count
				// Serial.print("gs32_fetch_offline_txnlist_count 2 : " + String(gs32_fetch_offline_txnlist_count));

				// if (gs32_fetch_offline_txnlist_count > 0)
				// {
				// 	gu8_fetch_offline_txn_flag = 1;
				// 	gu8_clear_offline_txn_flag = 0;
				// 	gs32_offlinestartStoptxn_update_guard_Interval = OFFLINE_START_STOP_TXN_GUARD_INTERVAL;
				// }
				// else if (gs32_fetch_offline_txnlist_count <= 0)
				// {
				// 	gs32_fetch_offline_txnlist_count = 0;
				// 	gu8_fetch_offline_txn_flag = 0;
				// 	gu8_clear_offline_txn_flag = 1;
				// }
			} });
	return ret;
}

int offlineStratTxn(String &idTag, int &connectorId)
{
	gs32_offlinestarttxn_update_flag = 1;
	OcppOperation *startTransaction = makeOcppOperation(&webSocket, new StartTransaction(idTag, connectorId));
	initiateOcppOperation(startTransaction);
	int ltransactionId = 0;
	startTransaction->setOnReceiveConfListener([](JsonObject payload)
											   {
    const char* status = payload["idTagInfo"]["status"] | "Invalid";
    if (!strcmp(status, "Accepted") || !strcmp(status, "ConcurrentTx")) { 
      if (DEBUG_OUT) Serial.print(F("EVSE_setOnStartTransaction Callback: StartTransaction was successful\n"));
	  /************************************/
       //int ltransactionId = payload["transactionId"] | -1;
	   int ltransactionId = payload["transactionId"] | -1; 
	    
      } else {
        if (DEBUG_OUT) Serial.print(F("EVSE_setOnStartTransaction Callback: StartTransaction was unsuccessful\n"));
      } });
	return ltransactionId;
}

int offlineStopTxn(String &idTag, int &txnId, int &connectorId)
{
	OcppOperation *stopTransaction = makeOcppOperation(&webSocket, new StopTransaction(idTag, txnId, connectorId));
	initiateOcppOperation(stopTransaction);
	if (DEBUG_OUT)
		Serial.print(F("EVSE_A_setOnStopTransaction  Offline Txn Update ....!\n"));

	stopTransaction->setOnReceiveConfListener([](JsonObject payload)
											  {
												  if (DEBUG_OUT)
													  Serial.print(F("EVSE_setOnStopTransaction: Offline Txn StopTransaction was successful\n"));
												  gs32_offlinestartStoptxn_update_flag = 0;
												  gs32_offlineAuthorizeIdTag = 0;
#if OFFLINE_STORAGE_ENABLE
												  requestClearTxn(String(gs32_fetch_offline_txnlist_count));
#endif
												  Serial.print("gs32_fetch_offline_txnlist_count 1 : " + String(gs32_fetch_offline_txnlist_count));
												  gs32_fetch_offline_txnlist_count--; // Decrement the fetch offline transaction count
												  Serial.print("gs32_fetch_offline_txnlist_count 2 : " + String(gs32_fetch_offline_txnlist_count));

												  if (gs32_fetch_offline_txnlist_count > 0)
												  {
													  gu8_fetch_offline_txn_flag = 1;
													  gu8_clear_offline_txn_flag = 0;
													  gs32_offlinestartStoptxn_update_guard_Interval = OFFLINE_START_STOP_TXN_GUARD_INTERVAL;
												  }
												  else if (gs32_fetch_offline_txnlist_count <= 0)
												  {
													  gs32_fetch_offline_txnlist_count = 0;
													  gu8_fetch_offline_txn_flag = 0;
													  gu8_clear_offline_txn_flag = 1;
												  }

												  // Serial.print("gs32_fetch_offline_txnlist_count 2 : " +String(gs32_fetch_offline_txnlist_count));
											  });

	return 1;
}
#endif
