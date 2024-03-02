#include "EVSE_B.h"
#include "Master.h"
#include "ControlPilot.h"
#include "LCD_I2C.h"
#include "display_meterValues.h"
#if DISPLAY_ENABLED
#include "display.h"
extern bool flag_tapped;
#endif

#if DWIN_ENABLED
#include "dwin.h"
extern SoftwareSerial dwin;
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
extern unsigned char cid2[8];
#endif

 uint8_t currentCounterThreshold_B = 60;

bool disp_evse_B = false;
uint8_t fault_occured_B = 0;

OnBoot_B onBoot_B;
OnReadUserId_B onReadUserId_B;
OnSendHeartbeat_B onSendHeartbeat_B;
OnAuthentication_B onAuthentication_B;
OnStartTransaction_B onStartTransaction_B;
OnStopTransaction_B onStopTransaction_B;
OnUnauthorizeUser_B onUnauthorizeUser_B;

bool notFaulty_B = false;

bool evIsPlugged_B;
bool flag_evseIsBooted_B;
bool flag_evseReadIdTag_B;
bool flag_evseAuthenticate_B;
bool flag_evseStartTransaction_B;
bool flag_evRequestsCharge_B;
bool flag_evseStopTransaction_B;
bool flag_evseUnauthorise_B;
bool flag_rebootRequired_B;
bool flag_evseSoftReset_B;
bool flag_evseReserveNow_B = false;		   // added by @mkrishna
bool flag_evseCancelReservation_B = false; // added by @mkrishna
bool flag_localAuthenication_B = false;	   // added by @mkrishna
bool gu8_send_status_flag_B = false;
bool reservation_start_flag_B = false; // added by @mkrishna

extern int reservationId_B;
extern int reserve_connectorId_B;
extern time_t reservation_start_time_B;
extern time_t reserveDate_B;
extern time_t reservedDuration_B; // 15 minutes slot Duration 15 * (1 * 60)
extern bool reserve_state_B;

extern bool ongoingTxn_A;
extern bool ongoingTxn_C;

extern bool flag_rebootRequired_C;
extern bool flag_rebootRequired_A;

extern bool flag_ed_A;

extern bool flag_send_stop_B;

extern float minCurr; // For LB nagar hubs

// Reason for stop
extern uint8_t reasonForStop_B;

extern int8_t fault_code_A;
extern int8_t fault_code_B;
extern int8_t fault_code_C;

float chargingLimit_B = 32.0f;
String Ext_currentIdTag_B = "";

long int blinckCounter_B = 0;
int counter1_B = 0;

int prevTxnId_B = -1;

ulong t_B;
int connectorDis_counter_B = 0;
short int counter_drawingCurrent_B = 0;
float drawing_current_B = 0;

String currentIdTag_B = "";
extern WebSocketsClient webSocket;
extern EVSE_states_enum EVSE_state;
extern Preferences preferences;
extern MFRC522 mfrc522;
extern String currentIdTag;
extern bool webSocketConncted;
extern bool isInternetConnected;
extern bool wifi_connect;
extern bool gsm_connect;

Preferences resumeTxn_B;
extern TinyGsmClient client;

int transactionId_B = -1;
bool ongoingTxn_B;
String idTagData_B = "";

bool timeout_active_B = false;
bool timer_initialize_B = false;
ulong timeout_start_B = 0;
short int fault_counter_B = 0;
bool flag_faultOccured_B = false;
ulong relay_timer_B = 0;
ulong faultTimer_B = 0;

ulong timerDisplayB;

// extern const ulong TIMEOUT_EMERGENCY_RELAY_CLOSE;
const ulong TIMEOUT_EMERGENCY_RELAY_CLOSE_B = 120000;

extern Preferences energymeter;

extern LCD_I2C lcd;
extern bool ethernet_enable;
extern bool ethernet_connect;

extern bool disp_evse_A;
extern bool disp_evse_C;

extern bool flag_stop_once_B;

extern String reserve_currentIdTag_B;

extern int globalmeterstartB;

extern bool flag_start_txn_B;

uint8_t online_session_ongoing_B = 0;

extern volatile bool session_ongoing_flag;
extern volatile bool session_on_offline_txn_completed;
extern uint8_t session_ongoing_B_count;

extern uint8_t gu8_online_flag;

extern int Sendstart_time_B;
extern int Sendstop_time_B;

extern String Tag_Id_B;
extern String Transaction_Id_B;
extern String Start_Meter_Value_B;

extern ChargePointStatusService *chargePointStatusService_A;
extern ChargePointStatusService *chargePointStatusService_B;
extern ChargePointStatusService *chargePointStatusService_C;
extern uint8_t gu8_Remote_start_B;
uint8_t start_session_B = 0;
 uint8_t Stop_Session_B = 0;
 extern float current_energy_B;
extern float discurrEnergy_B;
float LastPresentEnergy_B = 0;
extern uint8_t gu8_bootsuccess;
extern bool flag_evseIsBooted_A;

extern bool EMGCY_status_B;
volatile bool session_ongoing_flag_B = false;
bool stopoffline_B = false;
extern bool ongoingTxnoff_B;
extern uint8_t offline_charging_B;

/**********************************************************/
void EVSE_B_StopSession()
{

	if (getChargePointStatusService_B()->getEvDrawsEnergy() == true)
	{
		getChargePointStatusService_B()->stopEvDrawsEnergy();
	}

	// digitalWrite(32, LOW);
	session_ongoing_flag = false;
		session_ongoing_flag_B = false;
	online_session_ongoing_B = 0;
	disp_evse_B = false;
	gu8_Remote_start_B = 0;
      Stop_Session_B = 1;
      Display_Clear(Stop_Session_B);
	// requestForRelay(STOP, 2);
	  if (webSocketConncted == 0)
	  {
		  getChargePointStatusService_B()->setChargePointstatus(Available);
		  requestForRelay(STOP, 2);
		  if (!offline_charging_B)
		  {
			  resumeTxn_B.begin("resume_B", false); // opening preferences in R/W mode
			  resumeTxn_B.putBool("ongoingTxn_B", false);
			  resumeTxn_B.putBool("ongoingTxnoff_B", true);
			  resumeTxn_B.end();
		  }
	  }
	delay(500);
	flag_evseReadIdTag_B = false;
	flag_evseAuthenticate_B = false;
	flag_evseStartTransaction_B = false;
	flag_evRequestsCharge_B = false;
	flag_evseStopTransaction_B = true;
	flag_evseUnauthorise_B = false;
	Serial.println("[EVSE] Stopping Session : " + String(EVSE_state));
}
/**************************************************************************/

/**************************************************************************/

void EVSE_B_initialize()
{
	if (DEBUG_OUT)
		Serial.print(F("[EVSE_B] EVSE is powered on or reset. Starting Boot.\n"));
	// onBoot_B();
	flag_evseIsBooted_B = true;	 // Exit condition for booting.
	flag_evseReadIdTag_B = true; // Entry condition for reading ID Tag.
	faultTimer_B = millis();
}

/**************************SetUp********************************************/
void EVSE_B_setup()
{

	resumeTxn_B.begin("resume_B", false); // opening preferences in R/W mode
	idTagData_B = resumeTxn_B.getString("idTagData_B", "");
	ongoingTxn_B = resumeTxn_B.getBool("ongoingTxn_B", false);
	transactionId_B = resumeTxn_B.getInt("TxnIdData_B", -1);
	globalmeterstartB = resumeTxn_B.getFloat("meterStart", false);
	stopoffline_B = resumeTxn_B.getBool("ongoingTxnoff_B", false);
	resumeTxn_B.end();

	Serial.println("Stored ID_B:" + String(idTagData_B));
	Serial.println("Ongoing Txn_B: " + String(ongoingTxn_B));
	Serial.println("Txn id_B is : " + String(transactionId_B));
	Serial.println("meterstart_B is: " + String(globalmeterstartB));
	Serial.println(" stopoffline_B is: " + String(stopoffline_B));

	energymeter.begin("MeterData", false);
	float lu_lastEnergy_B = energymeter.getFloat("currEnergy_B", 0);
	// placing energy value back in EEPROM
	energymeter.end();
	Serial.println("[EnergyASampler] lu_lastEnergy_B: " + String(lu_lastEnergy_B));
	globalmeterstartB = (int)lu_lastEnergy_B;
	Serial.println("[EnergyASampler] globalmeterstartB: " + String(globalmeterstartB));

	if (ongoingTxn_B)
	{
		flag_start_txn_B = true;
	}

	EVSE_B_setOnBoot([]()
					 {
		//this is not in loop, that is why we need not update the flag immediately to avoid multiple copies of bootNotification.
		OcppOperation *bootNotification = makeOcppOperation(&webSocket,	new BootNotification());
		initiateOcppOperation(bootNotification);
		if (gu8_online_flag == 1)
    	{
			bootNotification->setOnReceiveConfListener([](JsonObject payload)
													   {

      	// if( flag_MeteringIsInitialised == false){
      	// 	Serial.println("[SetOnBooT] Initializing metering services");
      	// //	meteringService->init(meteringService);
      	// }

      if (DEBUG_OUT) Serial.print(F("EVSE_setOnBoot Callback: Metering Services Initialization finished.\n"));

			flag_evseIsBooted_B = true; //Exit condition for booting. 	
			flag_evseReadIdTag_B = true; //Entry condition for reading ID Tag.
			flag_evseAuthenticate_B = false;
			flag_evseStartTransaction_B = false;
			flag_evRequestsCharge_B = false;
			flag_evseStopTransaction_B = false;
			flag_evseUnauthorise_B = false;
						// EvseDevStatus_connector_1 = flag_EVSE_Read_Id_Tag;
			// getChargePointStatusService_B()->setChargePointstatus(Available);



      if (DEBUG_OUT) Serial.print(F("EVSE_setOnBoot Callback: Closing Relays.\n"));

			if (DEBUG_OUT) Serial.print(F("EVSE_setOnBoot Callback: Boot successful. Calling Read User ID Block.\n")); });
		}
		else
		{
			// if (flag_MeteringIsInitialised == false)
			// {
			// 	Serial.println("[SetOnBooT] Initializing metering services");
			// 	meteringService->init(meteringService);
			// }

			if (DEBUG_OUT)
				Serial.print(F("EVSE_setOnBoot Callback: Metering Services Initialization finished.\n"));

			flag_evseIsBooted_B = true;	 // Exit condition for booting.
			flag_evseReadIdTag_B = true; // Entry condition for reading ID Tag.
			flag_evseAuthenticate_B = false;
			flag_evseStartTransaction_B = false;
			// flag_evRequestsCharge = false;
						// EvseDevStatus_connector_1 = flag_EVSE_Read_Id_Tag;

			if (ongoingTxn_B)
			{
				flag_evRequestsCharge_B = true;
				if (DEBUG_OUT)
					Serial.println(F("****[EVSE_setOnBoot] on going transaction is true*****"));
			}
			else
			{
				flag_evRequestsCharge_B = false;
				if (DEBUG_OUT)
					Serial.println(F("****[EVSE_setOnBoot] on going transaction is false*****"));
			}
			flag_evseStopTransaction_B = false;
			flag_evseUnauthorise_B = false;
		} });

	EVSE_B_setOnReadUserId([]()
						   {
		if (DEBUG_OUT) Serial.print("EVSE_B:RFID\n");
		static ulong timerForRfid = millis();
		currentIdTag_B = "";  
		resumeTxn_B.begin("resume_B", false);
		idTagData_B = resumeTxn_B.getString("idTagData_B","");
  		ongoingTxn_B = resumeTxn_B.getBool("ongoingTxn_B",false);
		transactionId_B = resumeTxn_B.getInt("TxnIdData_B", -1);

		int connector = getChargePointStatusService_B()->getConnectorId();
		if(stopoffline_B == 1)
		{
			stopoffline_B = 0;

			OcppOperation *stopTransaction = makeOcppOperation(&webSocket, new StopTransaction(currentIdTag_B, transactionId_B, connector));
			initiateOcppOperation(stopTransaction);
			// prevTxnId_B = transactionId_B;

			if(webSocketConncted)
			{
				resumeTxn_B.begin("resume_B", false); // opening preferences in R/W mode
				// resumeTxn_B.putBool("ongoingTxn_B", false);
				resumeTxn_B.putString("idTagData_B", "");
				resumeTxn_B.putInt("TxnIdData_B", -1);
				resumeTxn_B.putBool("ongoingTxnoff_B", false);
				resumeTxn_B.end();
			}
		}

		/*
		* @brief : When ethernet is enabled, it has to be working.if(flag_stop_once_C) // To stop only once.
		{
		* G. Raja Sumant 27/08/2022
		*/


		if(ethernet_connect && 														//ethernet Block
  			(getChargePointStatusService_B()->getEmergencyRelayClose() == false)&&
  			(webSocketConncted == true) && getChargePointStatusService_B()->getUnavailable() == false)
		{
			if((idTagData_B != "") && (ongoingTxn_B == 1)){
  				currentIdTag_B = resumeTxn_B.getString("idTagData_B", "");

#if SEEMA_CONNECT

				getChargePointStatusService_B()->startTransaction(transactionId_B);
				reasonForStop_B = 5;
    			//fault_code_A = Power_Loss;
				EVSE_B_StopSession();
				Serial.println("[EVSE_B] Stop Session");
#if DISPLAY_ENABLED
      			  connAvail(3,"SESSION RESUME");
  				  checkForResponse_Disp();
#endif

#else

				Serial.println("EVSE_A:Resume session");
#if LCD_ENABLED
  				lcd.clear();
  				lcd.setCursor(3, 2);
  				lcd.print("SESSION B RESUME");
#endif
#if DISPLAY_ENABLED
      			  connAvail(2,"SESSION B RESUME");
  				  checkForResponse_Disp();
#endif`
  				getChargePointStatusService_B()->authorize(currentIdTag_B); // so that Starttxn can easily fetch this data
  				// requestLed(BLUE, START, 2);
				getChargePointStatusService_B()->startTransaction(transactionId_B);        //new block for Three connector
				getChargePointStatusService_B()->startEnergyOffer();
				resumeTxn_B.putString("idTagData_B",getChargePointStatusService_B()->getIdTag());
				resumeTxn_B.putBool("ongoingTxn_B",true);
				resumeTxn_B.putInt("TxnIdData_B", transactionId_B);

#endif

  			}else{
				if(millis() - timerForRfid > 10000){ //timer for sending led request
	    		  	// requestLed(GREEN, START, 2);
	    		  	timerForRfid = millis();
#if 0
#if LCD_ENABLED
					//lcd.clear();
#if 0
					if(ongoingTxn_A)
					{
					lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 2);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 3);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					}
					if(ongoingTxn_C)
					{
					lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 2);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 3);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					}
					lcd.setCursor(0, 1);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 1); // Or setting the cursor in the desired position.
					lcd.print("B: AVAILABLE");
					//lcd.setCursor(0, 1);
					//lcd.print("TAP RFID/SCAN QR");
					//lcd.setCursor(0, 2);
					//lcd.print("CONNECTION");
					lcd.setCursor(0, 3);
					lcd.print("CLOUD: ETH. TAP RFID");
#endif
					if(disp_evse_A)
					{
					lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      				lcd.print("A: CHARGING"); // Clear the line
					}
					else
					{
					lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      				lcd.print("A: AVAILABLE"); // Clear the line
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
					lcd.setCursor(0, 1);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 1); // Or setting the cursor in the desired position.
					lcd.print("B: AVAILABLE");
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
			
				currentIdTag_B = EVSE_B_getCurrnetIdTag(&mfrc522);
				Serial.println("[Wifi]RFID-B");
  			}

		}

  		if((wifi_connect == true)&& 														//Wifi Block
  			(getChargePointStatusService_B()->getEmergencyRelayClose() == false)&&
  			(webSocketConncted == true)&&
  			(WiFi.status() == WL_CONNECTED)&&
  			(isInternetConnected == true) && getChargePointStatusService_B()->getUnavailable() == false){

  			if((idTagData_B != "") && (ongoingTxn_B == 1)){
  				currentIdTag_B = resumeTxn_B.getString("idTagData_B", "");

#if SEEMA_CONNECT
				getChargePointStatusService_B()->startTransaction(transactionId_B);
				reasonForStop_B = 5;
    			//fault_code_A = Power_Loss;
				EVSE_B_StopSession();
				Serial.println("[EVSE_B]Stop Session");
#if DISPLAY_ENABLED
      			  connAvail(3,"SESSION RESUME");
  				  checkForResponse_Disp();
#endif

#else

				Serial.println("[EVSE_B]Resume Session");
#if LCD_ENABLED
  				lcd.clear();
  				lcd.setCursor(3, 2);
  				lcd.print("SESSION B RESUME");
#endif
#if DISPLAY_ENABLED
      			  connAvail(2,"SESSION B RESUME");
  				  checkForResponse_Disp();
#endif
  				getChargePointStatusService_B()->authorize(currentIdTag_B); // so that Starttxn can easily fetch this data
  				// requestLed(BLUE, START, 2);		
				getChargePointStatusService_B()->startTransaction(transactionId_B);        //new block for Three connector
				getChargePointStatusService_B()->startEnergyOffer();
				resumeTxn_B.putString("idTagData_B",getChargePointStatusService_B()->getIdTag());
				resumeTxn_B.putBool("ongoingTxn_B",true);
				resumeTxn_B.putInt("TxnIdData_B", transactionId_B);

#endif

  			}else{
				if(millis() - timerForRfid > 10000){ //timer for sending led request
	    		  	// requestLed(GREEN, START, 2);
	    		  	timerForRfid = millis();
#if 0
#if LCD_ENABLED
					//lcd.clear();
#if 0
					if(ongoingTxn_A)
					{
					lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 2);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 3);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					}
					if(ongoingTxn_C)
					{
					lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 2);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 3);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					}
					lcd.setCursor(0, 1);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 1); // Or setting the cursor in the desired position.
					lcd.print("B: AVAILABLE");
					//lcd.setCursor(0, 1);
					//lcd.print("TAP RFID/SCAN QR");
					//lcd.setCursor(0, 2);
					//lcd.print("CONNECTION");
					lcd.setCursor(0, 3);
					lcd.print("CLOUD: WIFI.TAP RFID");
#endif
					if(disp_evse_A)
					{
					lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      				lcd.print("A: CHARGING"); // Clear the line
					}
					else
					{
					lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      				lcd.print("A: AVAILABLE"); // Clear the line
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
					lcd.setCursor(0, 1);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 1); // Or setting the cursor in the desired position.
					lcd.print("B: AVAILABLE");
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
			
				currentIdTag_B = EVSE_B_getCurrnetIdTag(&mfrc522);
				Serial.println("[Wifi]-RFID B");
  			}

  		}else if((gsm_connect == true)&&													//GSM Block
  				(getChargePointStatusService_B()->getEmergencyRelayClose() == false)&&
  				(client.connected() == true) && getChargePointStatusService_B()->getUnavailable() == false){

  			if((idTagData_B != "") && (ongoingTxn_B == 1)){
  				currentIdTag_B = resumeTxn_B.getString("idTagData_B", "");
#if SEEMA_CONNECT
				getChargePointStatusService_B()->startTransaction(transactionId_B);
				reasonForStop_B = 5;
    			//fault_code_A = Power_Loss;
				EVSE_B_StopSession();
				Serial.println("[EVSE_B]Stop Session");
#if DISPLAY_ENABLED
      			  connAvail(3,"SESSION RESUME");
  				  checkForResponse_Disp();
#endif

#else

				Serial.println("[EVSE_B]Resuming Session");
#if LCD_ENABLED
  				lcd.clear();
  				lcd.setCursor(3, 2);
  				lcd.print("SESSION B RESUME");
#endif
#if DISPLAY_ENABLED
      			  connAvail(2,"SESSION B RESUME");
  				  checkForResponse_Disp();
#endif
  				getChargePointStatusService_B()->authorize(currentIdTag_B); // so that Starttxn can easily fetch this data
  				// requestLed(BLUE, START, 2);	
				getChargePointStatusService_B()->startTransaction(transactionId_B);        //new block for Three connector
				getChargePointStatusService_B()->startEnergyOffer();
				resumeTxn_B.putString("idTagData_B",getChargePointStatusService_B()->getIdTag());
				resumeTxn_B.putBool("ongoingTxn_B",true);
				resumeTxn_B.putInt("TxnIdData_B", transactionId_B);

#endif
  			}else{
				if(millis() - timerForRfid > 10000){ //timer for sending led request
	    		  	// requestLed(GREEN, START, 2);
	    		  	timerForRfid = millis();
#if 0
#if LCD_ENABLED
					if(disp_evse_A)
					{
					lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      				lcd.print("A: CHARGING"); // Clear the line
					}
					else
					{
					lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      				lcd.print("A: AVAILABLE"); // Clear the line
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
					lcd.setCursor(0, 1);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 1); // Or setting the cursor in the desired position.
					lcd.print("B: AVAILABLE");
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
			
				currentIdTag_B = EVSE_B_getCurrnetIdTag(&mfrc522);
				Serial.println("[GSM]RFID B");
  			}


  		}
		resumeTxn_B.end();

		/*	
		idTagData_m = resumeTxn.getString("idTagData","");
  		ongoingTxn_m = resumeTxn.getBool("ongoingTxn",false);

		if((ongoingTxn_m == 1) && (idTagData_m != "") && 
		      (getChargePointStatusService_B()->getEmergencyRelayClose() == false) &&
		      (WiFi.status() == WL_CONNECTED)&&
		      (webSocketConncted == true)&&
		      (isInternetConnected == true)){   //giving priority to stored data
			currentIdTag_B = resumeTxn.getString("idTagData","");
			Serial.println("[EVSE_setOnReadUserId] Resuming Session");
      		requestLed(BLUE,START,1);
         
		}else*/ 
		// if((getChargePointStatusService_B()->getEmergencyRelayClose() == false) &&
		//           (WiFi.status() == WL_CONNECTED) &&
		//           (webSocketConncted == true) && 
		//           (isInternetConnected == true)){
		// 	  #if LED_ENABLED
		// 	  if(millis() - timerForRfid > 5000){ //timer for sending led request
  //   		  requestLed(GREEN,START,2);
  //   		  timerForRfid = millis();
  //   		  }
  //   		  #endif
		// 	currentIdTag_B = EVSE_B_getCurrnetIdTag(&mfrc522);
		// 	Serial.println("********RFID B**********");
		// }

		if (currentIdTag_B.equals("") == true) {
			flag_evseReadIdTag_B = true; //Looping back read block as no ID tag present.
			flag_evseAuthenticate_B = false;
			flag_evseStartTransaction_B = false;
			flag_evRequestsCharge_B = false;
			flag_evseStopTransaction_B = false;
			flag_evseUnauthorise_B = false;
		} else {
			flag_evseReadIdTag_B = false;
			flag_evRequestsCharge_B = false;
			flag_evseStopTransaction_B = false;
			flag_evseUnauthorise_B = false;


#if SEEMA_CONNECT
			    if(ongoingTxn_B == 1)
				{
					flag_evseAuthenticate_B = false; //Entry condition for authentication block.
					flag_evseStartTransaction_B = false;
					flag_evseStopTransaction_B = true;
					Serial.print("EVSE_B_ ongoingTxn_B. \n");
				}
				else
				{
					// flag_evseAuthenticate_B = false; //Entry condition for authentication block.
					// flag_evseStartTransaction_B = true;
					flag_evseAuthenticate_B = true; //Entry condition for authentication block.
					flag_evseStartTransaction_B = false;
					flag_evseStopTransaction_B = false;
					Serial.print("EVSE_B_ StartTxn_B. \n");
				}

#else
			    if(ongoingTxn_B == 1)
				{

					flag_evseAuthenticate_B = false; //Entry condition for authentication block.
					flag_evseStartTransaction_B = false;
					// flag_evseStopTransaction_B = true;
                    flag_evRequestsCharge_B = true;    // to be check the functionality
					Serial.println("EVSE_B_ ongoingTxn_B. \n");

				}
				else
				{
					flag_evseAuthenticate_B = true; //Entry condition for authentication block.
					flag_evseStartTransaction_B = false;
					flag_evseStopTransaction_B = false;
					if (DEBUG_OUT) Serial.println("[EVSE_B]Calling Auth\n"); 			
				}
#endif
	
			
			
		} });

	EVSE_B_setOnsendHeartbeat([]()
							  {
    if (DEBUG_OUT) Serial.println("Sending Heartbeat\n");
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
    }); });

	EVSE_B_setOnAuthentication([]()
							   {
		if (DEBUG_OUT) Serial.println("[EVSE_B]Auth");
		flag_evseAuthenticate_B = false;
		OcppOperation *authorize = makeOcppOperation(&webSocket, new Authorize(currentIdTag_B));
		initiateOcppOperation(authorize);
		/*chargePointStatusService->authorize(currentIdTag_B, connectorId_B);  */    //have to edit
		authorize->setOnReceiveConfListener([](JsonObject payload) {
			const char* status = payload["idTagInfo"]["status"] | "Invalid";
			if (!strcmp(status, "Accepted")) {
				getChargePointStatusService_B()->setChargePointstatus(Preparing);
				flag_evseReadIdTag_B = false;
				flag_evseAuthenticate_B = false;
				flag_evseStartTransaction_B = true; //Entry condition for starting transaction.
				flag_evRequestsCharge_B = false;
				flag_evseStopTransaction_B = false;
				flag_evseUnauthorise_B = false;
				//requestLed(BLUE,START,2);
				// requestLed(BLINKYBLUE,START,2);
				currentIdTag = currentIdTag_B;   /* Assinging currentIdTag_B to currentIdTag for Set to Auth cache  */
				// requestSendAuthCache();//DISABLED Not checking the authentication cache
				
				if (DEBUG_OUT) Serial.println("[EVSE_B]Auth-Accepted");
#if CP_ACTIVE 
				flag_controlPAuthorise = true;
#endif

			} else {
				flag_evseReadIdTag_B = false;
				flag_evseAuthenticate_B = false;
				flag_evseStartTransaction_B = false;
				flag_evRequestsCharge_B = false;
				flag_evseStopTransaction_B = false;
				flag_evseUnauthorise_B = true; //wrong ID tag received, so clearing the global current ID tag variable and setting up to read again.
				if (DEBUG_OUT) Serial.println("[EVSE_B]AuthDenied");
			}  
		}); });

	EVSE_B_setOnStartTransaction([]()
								 {
		flag_evseStartTransaction_B = false;
		String idTag = "";
		int connectorId = 0;

		if (getChargePointStatusService_B() != NULL) {
			if (!getChargePointStatusService_B()->getIdTag().isEmpty()) {
				idTag = String(getChargePointStatusService_B()->getIdTag());
				connectorId = getChargePointStatusService_B()->getConnectorId();
			}
		}
		OcppOperation *startTransaction = makeOcppOperation(&webSocket, new StartTransaction(idTag,connectorId));
		initiateOcppOperation(startTransaction);
		startTransaction->setOnReceiveConfListener([](JsonObject payload) {
    		const char* status = payload["idTagInfo"]["status"] | "Invalid";
      if (!strcmp(status, "Accepted") || !strcmp(status, "ConcurrentTx")) { 

      flag_evseReadIdTag_B = false;
      flag_evseAuthenticate_B = false;
      flag_evseStartTransaction_B = false;
      flag_evRequestsCharge_B = true;
      flag_evseStopTransaction_B = false;
      flag_evseUnauthorise_B = false;
      if (DEBUG_OUT)  Serial.println("[EVSE_B]:Started");
#if DISPLAY_ENABLED
      	connAvail(2,"SESSION B STARTED");
  		checkForResponse_Disp();
#endif
#if LCD_DISPLAY_ENABLED
      lcd.clear();
      lcd.setCursor(3, 1);
      lcd.print("AUTHENTICATION");
	  lcd.setCursor(3, 2);
      lcd.print("SUCCESSFUL - 2");
	  delay(1500);
#endif
      /************************************/
      int transactionId = payload["transactionId"] | -1;
      getChargePointStatusService_B()->startTransaction(transactionId);        //new block for Three connector
      getChargePointStatusService_B()->startEnergyOffer();

	  resumeTxn_B.begin("resume_B", false);
      resumeTxn_B.putString("idTagData_B",getChargePointStatusService_B()->getIdTag());
      resumeTxn_B.putBool("ongoingTxn_B",true);
	  resumeTxn_B.putInt("TxnIdData_B", transactionId);
	  resumeTxn_B.putFloat("meterStart", globalmeterstartB);
	  resumeTxn_B.end();

	  Tag_Id_B = getChargePointStatusService_B()->getIdTag();
      Transaction_Id_B = String(transactionId);
      Start_Meter_Value_B = String(globalmeterstartB);

		  session_ongoing_flag_B = true;
      session_ongoing_flag = true;
	  session_on_offline_txn_completed = false;
      session_ongoing_B_count = 1;
	   Serial.println("[EVSE_B]:Charging");
	  getChargePointStatusService_B()->setChargePointstatus(Charging);

      //*****Storing tag data in EEPROM****//
      /*
	  resumeTxn.putString("idTagData",currentIdTag);
      resumeTxn.putBool("ongoingTxn",true);*/
      //***********************************//

      } else {
        flag_evseReadIdTag_B = false;
        flag_evseAuthenticate_B = false;
        flag_evseStartTransaction_B = false;
        flag_evRequestsCharge_B = false;
        flag_evseStopTransaction_B = false;
        flag_evseUnauthorise_B = true; //wrong ID tag received, so clearing the global current ID tag variable and setting up to read again.

        if (DEBUG_OUT) Serial.println("[EVSE_B]:AuthDenied");
#if LCD_DISPLAY_ENABLED
      lcd.clear();
      lcd.setCursor(3, 1);
      lcd.print("AUTHENTICATION");
	  lcd.setCursor(3, 2);
      lcd.print("DENIED - 2");
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

	EVSE_B_setOnStopTransaction([]()
								{
		flag_evseStopTransaction_B = false;
		getChargePointStatusService_B()->setFinishing(true);
		// getChargePointStatusService_B()->setChargePointstatus(Finishing);
		// if(notFaulty_B)
		// {
		// 	getChargePointStatusService_B()->setChargePointstatus(Available);
		// }
		if (getChargePointStatusService_B() != NULL) {
          getChargePointStatusService_B()->stopEnergyOffer();
        }
        int txnId = getChargePointStatusService_B()->getTransactionId();
        int connector = getChargePointStatusService_B()->getConnectorId();
		if(txnId !=prevTxnId_B)
	{
		OcppOperation *stopTransaction = makeOcppOperation(&webSocket, new StopTransaction(currentIdTag_B, txnId, connector));
		initiateOcppOperation(stopTransaction);
		prevTxnId_B = txnId;
    	if (DEBUG_OUT )Serial.println("[EVSE_B]:ClosingRelays");
#if LCD_DISPLAY_ENABLED
  			lcd.clear();
  			lcd.setCursor(2, 1);
  			lcd.print("OUTPUT 2 STOPPED");
#endif
LastPresentEnergy_B = (discurrEnergy_B)*1000 + LastPresentEnergy_B;
    	/**********************Until Offline functionality is implemented***********/
    	//Resume namespace(Preferences)
    	if(getChargePointStatusService_B()->getEmergencyRelayClose() == false){
    		//requestLed(GREEN,START,2);   //temp fix  // it is affecting offline led functionality
    	}
		#if 1
		resumeTxn_B.begin("resume_B", false);
    	resumeTxn_B.putBool("ongoingTxn_B", false);
    	// resumeTxn_B.putString("idTagData_B","");
		// resumeTxn_B.putInt("TxnIdData_B", -1);
		resumeTxn_B.end();
		#endif

		if(ethernet_connect)
		{
			if(!webSocketConncted)
			{
				flag_evseReadIdTag_B = true;
		        flag_evseAuthenticate_B = false;
		        flag_evseStartTransaction_B = false;
		        flag_evRequestsCharge_B = false;
		        flag_evseStopTransaction_B = false;
		        flag_evseUnauthorise_B = false;
	    		// Serial.println("Clearing Stored ID tag in StopTransaction()");
			}
		}
    	else if(wifi_connect == true){
	    	if(!webSocketConncted || WiFi.status() != WL_CONNECTED || isInternetConnected == false ){
		        getChargePointStatusService_B()->stopTransaction();
		    	getChargePointStatusService_B()->unauthorize();  //can be buggy 
		        flag_evseReadIdTag_B = true;
		        flag_evseAuthenticate_B = false;
		        flag_evseStartTransaction_B = false;
		        flag_evRequestsCharge_B = false;
		        flag_evseStopTransaction_B = false;
		        flag_evseUnauthorise_B = false;
    			// Serial.println(F("Clearing Stored ID tag in StopTransaction()"));
    		}
    	}else if(gsm_connect == true){
    		if(client.connected() == false){
    			getChargePointStatusService_B()->stopTransaction();
		    	getChargePointStatusService_B()->unauthorize();  //can be buggy 
		        flag_evseReadIdTag_B = true;
		        flag_evseAuthenticate_B = false;
		        flag_evseStartTransaction_B = false;
		        flag_evRequestsCharge_B = false;
		        flag_evseStopTransaction_B = false;
		        flag_evseUnauthorise_B = false;
    			// Serial.println(F("Clearing Stored ID tag in StopTransaction()"));
    		}
    	}
	Serial.println("[EVSE_B]IdTag-cleared");
    	// requestForRelay(STOP, 2);
		online_session_ongoing_B = 0;
    	delay(500);
		stopTransaction->setOnReceiveConfListener([](JsonObject payload) {
#if LCD_ENABLED
  			lcd.clear();
  			lcd.setCursor(3, 2);
  			lcd.print("STOP B SUCCESS");
#endif
			flag_evseReadIdTag_B = false;
			flag_evseAuthenticate_B = false;
			flag_evseStartTransaction_B = false;
			flag_evRequestsCharge_B = false;
			flag_evseStopTransaction_B = false;
			flag_evseUnauthorise_B = true;
			getChargePointStatusService_B()->stopTransaction();
      		if (DEBUG_OUT) Serial.println("[EVSE_B]StopSuccesful,Relayclose");
		});
	}else
	{
		Serial.println("EVSE_B]:AlreadyStopped");
	} });

	EVSE_B_setOnUnauthorizeUser([]()
								{
		if(flag_evseSoftReset_B == true){
			//This 'if' block is developed by @Wamique.
			flag_evseReadIdTag_B = false;
			flag_evseAuthenticate_B = false;
			flag_evseStartTransaction_B = false;
			flag_evRequestsCharge_B = false;
			flag_evseStopTransaction_B = false;
			flag_evseUnauthorise_B = false;
			flag_rebootRequired_B = true;
			getChargePointStatusService_B()->unauthorize();
			if(fault_occured_B != 1)
			{
				getChargePointStatusService_B()->setChargePointstatus(Available);
			}
			
			if (DEBUG_OUT) 		
			Serial.println("EVSE_B]:Unauthorized");

		} else if(flag_evseSoftReset_B == false){
			flag_evseReadIdTag_B = true;
			flag_evseAuthenticate_B = false;
			flag_evseStartTransaction_B = false;
			flag_evRequestsCharge_B = false;
			flag_evseStopTransaction_B = false;
			flag_evseUnauthorise_B = false;
			if (DEBUG_OUT) Serial.println("EVSE_B]:Unauthorizing");
			getChargePointStatusService_B()->unauthorize();
			if(fault_occured_B != 1)
			{
			getChargePointStatusService_B()->setChargePointstatus(Available);
			}
			
		} });
}

/*********************************************************************/

void EVSE_B_loop()
{
	if (flag_evseIsBooted_B == false)
	{
		if (DEBUG_OUT)
			Serial.println("[EVSE] Booting...");
		delay(1000);
		// onBoot();
		t_B = millis();
		return;
	}
	// else if (flag_evseIsBooted_A == true)
	else if (flag_evseIsBooted_B == true)
	{
		if (flag_evseReadIdTag_B == true)
		{
			if (onReadUserId_B != NULL)
			{
				onReadUserId_B();
			}
			return;
		}
		else if (flag_evseAuthenticate_B == true)
		{
			if (onAuthentication_B != NULL)
			{

				// Add condition by checking if available or unavailable
				bool un = false;
				un = getChargePointStatusService_B()->getUnavailable();
				currentIdTag_B = getChargePointStatusService_B()->getIdTag();
				currentIdTag = currentIdTag_B; /* Assinging currentIdTag_B to currentIdTag for check Auth cache  */
				if (!un)
				{
					if (flag_evseReserveNow_B)
					{
						Serial.println(" reserve_currentIdTag_B :");
						Serial.println(reserve_currentIdTag_B);

						Serial.println(" currentIdTag_B :");
						Serial.println(currentIdTag_B);

						if (reserve_currentIdTag_B == currentIdTag_B)
						{
							if (DEBUG_OUT)
								Serial.println("EVSE_B]:Resrvation Tag Matched");

							bool auth_checker = false;
							auth_checker = requestGetAuthCache();

							if (auth_checker)
							{
								getChargePointStatusService_B()->authorize(currentIdTag_B);
								flag_evseReadIdTag_B = false;
								flag_evseAuthenticate_B = false;
								flag_evseStartTransaction_B = true; // Entry condition for starting transaction.
								flag_evRequestsCharge_B = false;
								flag_evseStopTransaction_B = false;
								flag_evseUnauthorise_B = false;

								if (DEBUG_OUT)
									Serial.println("EVSE_B]:AuthAccepted");
								// requestLed(BLUE, START, 2);
								// requestLed(BLINKYBLUE, START, 2);
							}
							else
							{
								onAuthentication_B();
							}
						}
						else
						{
							if (DEBUG_OUT)
							Serial.println("EVSE_B]:Resrvation Tag not Matched");
							return;
						}
					}
					else
					{
						// bool auth_checker = false;
						// auth_checker = requestGetAuthCache();
						// if (auth_checker)
						// if (requestGetAuthCache())
						if(0)
						{
							getChargePointStatusService_B()->authorize(currentIdTag_B);
							flag_evseReadIdTag_B = false;
							flag_evseAuthenticate_B = false;
							flag_evseStartTransaction_B = true; // Entry condition for starting transaction.
							flag_evRequestsCharge_B = false;
							flag_evseStopTransaction_B = false;
							flag_evseUnauthorise_B = false;

							if (DEBUG_OUT)
							Serial.println("EVSE_B:AuthAccepted");
							// requestLed(BLUE, START, 2);
							// requestLed(BLINKYBLUE, START, 2);
						}
						else
						{
							onAuthentication_B();
						}
					}
				}
			}
			return;
		}
		else if (flag_evseStartTransaction_B == true)
		{
			if (onStartTransaction_B != NULL)
			{
#if CP_ACTIVE
				if ((EVSE_state == STATE_C || EVSE_state == STATE_D) && getChargePointStatusService()->getEmergencyRelayClose() == false)
				{
					onStartTransaction_B();
				}
				else
				{
					Serial.println(F("Connect the Connector to EV / Or fault exist")); // here have to add timeout of 30 sec
					connectorDis_counter_B++;
					// EVSE_stopTransactionByRfid();
					if (connectorDis_counter_B > 25)
					{
						connectorDis_counter_B = 0;

						EVSE_StopSession_B();
					}
				}
#endif

#if !CP_ACTIVE
				onStartTransaction_B(); // can add check for fault
#endif
			}
		}
		else if (flag_evRequestsCharge_B == true)
		{

#if CP_ACTIVE
			// flag_evRequestsCharge = false;
			if (getChargePointStatusService_B() != NULL && getChargePointStatusService_B()->getEvDrawsEnergy() == false)
			{

				/***********************Control Pilot @Wamique******************/
				if (EVSE_state == STATE_C || EVSE_state == STATE_D)
				{
					if (getChargePointStatusService_B()->getEmergencyRelayClose() == false)
					{
						EVSE_StartCharging_B();
					}
					else if (getChargePointStatusService_B()->getEmergencyRelayClose() == true)
					{
						Serial.println(F("The voltage / current / Temp is out or range. FAULTY CONDITION DETECTED."));
					}
				}
				else if (EVSE_state == STATE_SUS)
				{
					EVSE_Suspended_B();
					Serial.println(counter1);
					if (counter1_B++ > 25)
					{ // Have to implement proper timeout
						counter1_B = 0;
						EVSE_B_StopSession();
					}
				}
				else if (EVSE_state == STATE_DIS || EVSE_state == STATE_E || EVSE_state == STATE_B || EVSE_state == STATE_A)
				{

					//	EVSE_StopSession();     // for the very first time cable can be in disconnected state

					// if(txn == true){           // can implement counter > 10 just to remove noise
					EVSE_B_StopSession();
					//	}
				}
				else
				{

					Serial.println("[EVSE] STATE Error" + String(EVSE_state));
					delay(2000);

					//	requestLed(RED,START,1);
				}
			}
			if (getChargePointStatusService_B()->getEvDrawsEnergy() == true)
			{

				//	txn = true;

				if (EVSE_state == STATE_C || EVSE_state == STATE_D)
				{

					if (DEBUG_OUT)
						Serial.println(F("[EVSE_CP] Drawing Energy"));

					if (millis() - t_B > 10000)
					{
						if (getChargePointStatusService_B()->getEmergencyRelayClose() == false)
						{
							requestLed(BLINKYGREEN, START, 2);
							t_B = millis();
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
					if (counter_faultstate_B++ > 5)
					{
						EVSE_B_StopSession();
						counter_faultstate_B = 0;
					}
				}
				else if (EVSE_state == STATE_SUS)
				{
					EVSE_B_Suspended(); // pause transaction :update suspended state is considered in charging state
				}
				else if (EVSE_state == STATE_DIS)
				{

					Serial.println(F("[EVSE] Connect the Connector with EV and Try again"));
					EVSE_B_StopSession();
				}
			}

			/***Implemented Exit Feature with RFID @Wamique****/
//	  EVSE_B_stopTransactionByRfid();
#endif

#if !CP_ACTIVE
			if (getChargePointStatusService_B() != NULL && getChargePointStatusService_B()->getEvDrawsEnergy() == false)
			{
				if (getChargePointStatusService_B()->getEmergencyRelayClose() == false)
				{
					getChargePointStatusService_B()->startEvDrawsEnergy();
					reasonForStop_B = 3; // Local
					if (DEBUG_OUT)
						Serial.println("[EVSE_B] Opening Relays.\n");
					requestForRelay(START, 2);
#if DISPLAY_ENABLED
					flag_tapped = true;
#endif
					/*requestLed(ORANGE,START,2);
					delay(1200);
					requestLed(WHITE,START,2);
					delay(1200);
					requestLed(GREEN,START,2);
					delay(1000);*/
					if (DEBUG_OUT)
						Serial.println("[EVSE_B]Drawing Energy");
				}
				else if (getChargePointStatusService_B()->getEmergencyRelayClose() == true)
				{
					Serial.println("EVSE_B:Faulty");;
				}
			}
			if (getChargePointStatusService_B()->getEvDrawsEnergy() == true)
			{
				// delay(250);

				if (DEBUG_OUT)
					Serial.println("EVSE_B:DrawingEnergy");
				disp_evse_B = true;
				start_session_B = 1;
				Display_Set(start_session_B);
				// displayMeterValuesB();
				// blinking green Led
				if (millis() - t_B > 10000) // Change from 5 to 10 for independent emergency.
				{
					// if((WiFi.status() == WL_CONNECTED) && (webSocketConncted == true) && (isInternetConnected == true)&& getChargePointStatusService()->getEmergencyRelayClose() == false){
					// 	requestLed(BLINKYGREEN_EINS,START,1);
					// 	t = millis();
					// }

					if (getChargePointStatusService_B()->getEmergencyRelayClose() == false)
					{
						// requestLed(BLINKYGREEN, START, 2);
						online_session_ongoing_B = 1;
						t_B = millis();

						if (millis() - relay_timer_B > 15000)
						{

							// requestForRelay(START, 2);
							relay_timer_B = millis();
						}
					}
				}
				// Current check
				#if 0
				drawing_current_B = eic.GetLineCurrentB();
				Serial.println("Current B: " + String(drawing_current_B));
				if (drawing_current_B <= minCurr)
				{
					Serial.println("Min current_B");

					if (notFaulty_B)
					{
						counter_drawingCurrent_B++;
						Serial.println("counter_drawingCurrent_B Online: " + (String)counter_drawingCurrent_B);
					}
					// if(counter_drawingCurrent_B > 120){
					if (counter_drawingCurrent_B > currentCounterThreshold_B)
					{
						counter_drawingCurrent_B = 0;
						// Check for the case where reasonForStop is not Local , Other
						if (reasonForStop_B != 3 || reasonForStop_B != 4)
							reasonForStop_B = 1; // EV disconnected
#if LCD_ENABLED
						if (notFaulty_B)
						{
							lcd.clear();
							if (currentCounterThreshold_B == 2)
							{
								lcd.setCursor(0, 1);
								lcd.print("B: EV DISCONNECTED!");
							}
							else
							{
								lcd.setCursor(3, 0);
								lcd.print("B: NO POWER DRAWN");
							}
						}
#endif
						Serial.println(F("Stopping session due to No current"));
						EVSE_B_StopSession();
					}
				}
				else
				{
					counter_drawingCurrent_B = 0;
					if (notFaulty_B)
					{
						currentCounterThreshold_B = 2;
					}
					Serial.println(F("counter_drawing Current Reset"));
				}
				#endif
			}
			// Implemented Exit Feature with RFID @Wamique//
			//	 EVSE_B_stopTransactionByRfid();
#endif
			// this is the only 'else if' block which is calling next else if block. the control is from this file itself. the control is not changed from any other file. but the variables are required to be present as extern in other file to decide calling of other functions.
			return;
		}
		else if (flag_evseStopTransaction_B == true)
		{
			if (getChargePointStatusService_B() != NULL)
			{
				getChargePointStatusService_B()->stopEvDrawsEnergy();
			}
			if (onStopTransaction_B != NULL)
			{
				onStopTransaction_B();
#if CP_ACTIVE
				requestforCP_OUT(STOP); // stop pwm
#endif
			}
			return;
		}
		else if (flag_evseUnauthorise_B == true)
		{
			if (onUnauthorizeUser_B != NULL)
			{
				onUnauthorizeUser_B();
			}
			return;
		}
		else if (flag_rebootRequired_B == true && flag_rebootRequired_A == true && flag_rebootRequired_C == true)
		{
			// soft reset execution.
			//  flag_evseIsBooted_B = false;
			//  flag_rebootRequired_B = false;
			//  flag_evseSoftReset_B = false;
			if (getChargePointStatusService_A()->inferenceStatus() != ChargePointStatus::Charging &&
				getChargePointStatusService_B()->inferenceStatus() != ChargePointStatus::Charging &&
				getChargePointStatusService_C()->inferenceStatus() != ChargePointStatus::Charging)
			{
				if (DEBUG_OUT)
					Serial.println("EVSE:ReBooting...");
				delay(5000);
				ESP.restart();
			}
		}
		else
		{
			if (DEBUG_OUT)
				Serial.println("[EVSE_B] waiting for response...\n");
			// delay(100);
		}
	}
}

short EMGCY_counter_B = 0;
extern short EMGCY_counter_A;
extern short EMGCY_counter_C;
bool EMGCY_FaultOccured_B = false;
extern bool EMGCY_FaultOccured_A;
extern bool EMGCY_FaultOccured_C;

void emergencyRelayClose_Loop_B()
{
	if (millis() - faultTimer_B > 2000)
	{
		if (getChargePointStatusService_B()->getOverCurrent() == true)
		{
			getChargePointStatusService_B()->setEmergencyRelayClose(true);
			getChargePointStatusService_B()->setChargePointstatus(Faulted);
			if (getChargePointStatusService_B()->getTransactionId() != -1)
			{
#if LCD_ENABLED
				lcd.clear();
				lcd.setCursor(0, 0); // Or setting the cursor in the desired position.
				lcd.print("STATUS: FAULTED");
				lcd.setCursor(0, 1);
				lcd.print("B: OVER CURRENT");
#endif
				EVSE_B_StopSession();
#if LED_ENABLED
				// requestLed(RED, START, 2);
				delay(2000);
#endif
			}
		}

		faultTimer_B = millis();
		// bool EMGCY_status_B = requestEmgyStatus();
		// Serial.println("EMGCY_Status_B: "+String(EMGCY_status_B));
		if (EMGCY_status_B == true)
		{
			if (EMGCY_counter_B++ > 0)
			{
				notFaulty_B = false;
				// if(EMGCY_counter_B == 0){
				fault_code_A = 8;
				fault_code_B = 8;
				fault_code_C = 8;
				// requestForRelay(STOP,2); // zero means all!
				// requestForRelay(STOP,2);
				// requestForRelay(STOP,3);
				disp_evse_B = false;
				gu8_Remote_start_B = 0;
      			Stop_Session_B = 1;
      			Display_Clear(Stop_Session_B);
				reasonForStop_B = 0;
				// requestLed(BLINKYRED,START,2);
				// requestLed(BLINKYRED,START,2);
				// requestLed(BLINKYRED,START,3);
#if DISPLAY_ENABLED
				setHeader("RFID UNAVAILABLE");
				checkForResponse_Disp();
				connAvail(2, "FAULTED EMGY");
				checkForResponse_Disp();
#endif

#if LCD_ENABLED
				// lcd.clear();
				// lcd.setCursor(0, 0); // Or setting the cursor in the desired position.
				// lcd.print("                    ");//Clear the line
				lcd.setCursor(0, 0); // Or setting the cursor in the desired position.
				lcd.print("FAULTED: EMERGENCY");
#endif

				/*getChargePointStatusService_A()->setEmergencyRelayClose(true);
				EMGCY_FaultOccured_A = true;
				EMGCY_counter_A = 0;*/

				getChargePointStatusService_B()->setEmergencyRelayClose(true);
				getChargePointStatusService_B()->setChargePointstatus(Faulted);
				fault_occured_B = 1;
				EMGCY_FaultOccured_B = true;
				EMGCY_counter_B = 0;

				/*getChargePointStatusService_C()->setEmergencyRelayClose(true);
				EMGCY_FaultOccured_C = true;
				EMGCY_counter_C = 0;*/
			}
		}
		else
		{
			notFaulty_B = true;
			EMGCY_FaultOccured_B = false;
			EMGCY_counter_B = 0;
			getChargePointStatusService_B()->setEmergencyRelayClose(false);
			// if (getChargePointStatusService_B()->getChargePointstatus() != Available &&
			// getChargePointStatusService_B()->getChargePointstatus() != Charging &&
			// getChargePointStatusService_B()->getChargePointstatus() != Preparing &&
			// getChargePointStatusService_B()->getChargePointstatus() != Finishing )
			// {
			//     getChargePointStatusService_B()->setChargePointstatus(Available);
			// }
			if ((fault_occured_B == 1))
			{
				if ((flag_evRequestsCharge_B == false) && (ongoingTxn_B == false))
				{
					fault_occured_B = 0;
					getChargePointStatusService_B()->setChargePointstatus(Available);
				}

				else if ((flag_evRequestsCharge_B == true))
				{
					fault_occured_B = 0;
					getChargePointStatusService_B()->setChargePointstatus(Charging);
				}
			}
		}

		if (EMGCY_FaultOccured_B == true && getChargePointStatusService_B()->getTransactionId() != -1)
		{
			Serial.println("[EVSE_B]EMERGENCY STOP REQUESTED");
			// flag_evseReadIdTag_B = false;
			// flag_evseAuthenticate_B = false;
			// flag_evseStartTransaction_B = false;
			// flag_evRequestsCharge_B = false;
			// flag_evseStopTransaction_B = true;
			// session_ongoing_flag = false;
			// session_ongoing_flag_B = false;
			if (getChargePointStatusService_B()->getEvDrawsEnergy() == true)
			{
			    EVSE_B_StopSession();
			}
			online_session_ongoing_B = 0;
		}
		else if (EMGCY_FaultOccured_B == false)
		{

			// float volt = eic.GetLineVoltageB();
			// float current = eic.GetLineCurrentB();
			// float temp = eic.GetTemperature();
			// Serial.println("Voltage_B: " + String(volt) + ", Current_B: " + String(current) + ", Temperature: " + String(temp));
			if (getChargePointStatusService_B() != NULL)
			{
				if (getChargePointStatusService_B()->getOverVoltage() == true ||
					getChargePointStatusService_B()->getUnderVoltage() == true ||
					getChargePointStatusService_B()->getUnderTemperature() == true ||
					getChargePointStatusService_B()->getOverTemperature() == true ||
					(flag_ed_A) ||
					getChargePointStatusService_B()->getOverCurrent() == true)
				{
					Serial.println("[EVSE_B] Fault Occurred.");
					notFaulty_B = false;
					getChargePointStatusService_B()->setEmergencyRelayClose(true);
					getChargePointStatusService_B()->setChargePointstatus(Faulted);
					fault_occured_B = 1;
					if (!timer_initialize_B)
					{
						timeout_start_B = millis();
						timer_initialize_B = true;
					}
				}
				else if (getChargePointStatusService_B()->getOverVoltage() == false &&
						 getChargePointStatusService_B()->getUnderVoltage() == false &&
						 getChargePointStatusService_B()->getUnderTemperature() == false &&
						 getChargePointStatusService_B()->getOverTemperature() == false &&
						 (!flag_ed_A) &&
						 getChargePointStatusService_B()->getOverCurrent() == false)
				{
					Serial.println("[EVSE_B]Not Faulty.");
					notFaulty_B = true;
					getChargePointStatusService_B()->setEmergencyRelayClose(false);
					// if (getChargePointStatusService_B()->getChargePointstatus() != Available &&
					// getChargePointStatusService_B()->getChargePointstatus() != Charging &&
					// getChargePointStatusService_B()->getChargePointstatus() != Preparing &&
					// getChargePointStatusService_B()->getChargePointstatus() != Finishing )
					// {
					//     getChargePointStatusService_B()->setChargePointstatus(Available);
					// }
			if ((fault_occured_B == 1))
			{
				if ((flag_evRequestsCharge_B == false) && (ongoingTxn_B == false))
				{
					fault_occured_B = 0;
					getChargePointStatusService_B()->setChargePointstatus(Available);
				}

				else if ((flag_evRequestsCharge_B == true))
				{
					fault_occured_B = 0;
					getChargePointStatusService_B()->setChargePointstatus(Charging);
				}
			}
					// if (!timer_initialize){
					timeout_start_B = 0;
					timer_initialize_B = false;
					//}
				}

				if (getChargePointStatusService_B()->getEmergencyRelayClose() == true)
				{
					timeout_active_B = true;
					requestForRelay(STOP, 2);
					online_session_ongoing_B = 0;
					delay(50);
#if LED_ENABLED
					// requestLed(RED, START, 2);
#endif

					flag_faultOccured_B = true;
				}
				else if (getChargePointStatusService_B()->getEmergencyRelayClose() == false && flag_faultOccured_B == true)
				{
					timeout_active_B = false;
					if ((getChargePointStatusService_B()->getTransactionId() != -1))
					{ // can be buggy
						if (fault_counter_B++ > 1)
						{
							fault_counter_B = 0;
							// requestForRelay(START,2);
							// delay(50);
							Serial.println("[EmergencyRelay_B] Starting Txn");
							flag_faultOccured_B = false;
						}
					}
				}

				if (timeout_active_B && getChargePointStatusService_B()->getTransactionId() != -1)
				{
					if (millis() - timeout_start_B >= TIMEOUT_EMERGENCY_RELAY_CLOSE_B)
					{
						Serial.println("[EVSE_B] Fault timer expired. Sending stop transaction");
						flag_evRequestsCharge_B = false;
						flag_evseStopTransaction_B = true;
						timeout_active_B = false;
						timer_initialize_B = false;
					}
				}
			}
		}
	}
}

/*
 * @param limit: expects current in amps from 0.0 to 32.0
 */
void EVSE_B_setChargingLimit(float limit)
{
	if (DEBUG_OUT)
		Serial.print(F("[EVSE] New charging limit set. Got "));
	if (DEBUG_OUT)
		Serial.print(limit);
	if (DEBUG_OUT)
		Serial.print(F("\n"));
	chargingLimit_B = limit;
}

bool EVSE_B_EvRequestsCharge()
{
	return flag_evRequestsCharge_B;
}

bool EVSE_B_EvIsPlugged()
{
	return evIsPlugged_B;
}

void EVSE_B_setOnBoot(OnBoot_B onBt_B)
{
	onBoot_B = onBt_B;
}

void EVSE_B_setOnReadUserId(OnReadUserId_B onReadUsrId_B)
{
	onReadUserId_B = onReadUsrId_B;
}

void EVSE_B_setOnsendHeartbeat(OnSendHeartbeat_B onSendHeartbt_B)
{
	onSendHeartbeat_B = onSendHeartbt_B;
}

void EVSE_B_setOnAuthentication(OnAuthentication_B onAuthenticatn_B)
{
	onAuthentication_B = onAuthenticatn_B;
}

void EVSE_B_setOnStartTransaction(OnStartTransaction_B onStartTransactn_B)
{
	onStartTransaction_B = onStartTransactn_B;
}

void EVSE_B_setOnStopTransaction(OnStopTransaction_B onStopTransactn_B)
{
	onStopTransaction_B = onStopTransactn_B;
}

void EVSE_B_setOnUnauthorizeUser(OnUnauthorizeUser_B onUnauthorizeUsr_B)
{
	onUnauthorizeUser_B = onUnauthorizeUsr_B;
}

// void EVSE_getSsid(String &out) {
// 	out += "Pied Piper";
// }
// void EVSE_getPass(String &out) {
// 	out += "plmzaq123";
// }

// void EVSE_getChargePointSerialNumber(String &out) {

// 	out += preferences.getString("chargepoint","");

// 	/*
// 	#if STEVE
// 	out += "dummyCP002";
// 	#endif

// 	#if EVSECEREBRO
// 	out += "testpodpulkit";
// 	#endif
// 	*/
// }

// char *EVSE_getChargePointVendor() {
// 	return "Amplify Mobility";
// }

// char *EVSE_getChargePointModel() {
// 	return "Wx2";
// }

String EVSE_B_getCurrnetIdTag(MFRC522 *mfrc522)
{
	String currentIdTag = "";
	// currentIdTag_B = EVSE_B_readRFID(mfrc522);

	if (getChargePointStatusService_B()->getIdTag().isEmpty() == false)
	{
		if (DEBUG_OUT)
			Serial.println("[EVSE_B] Reading from Charge Point Station Service ID Tag stored.");
		currentIdTag = getChargePointStatusService_B()->getIdTag();
		if (DEBUG_OUT)
			Serial.print("[EVSE_B] ID Tag: ");
		if (DEBUG_OUT)
			Serial.println(currentIdTag);
		Serial.flush();
	}

	return currentIdTag;
}

String EVSE_B_readRFID(MFRC522 *mfrc522)
{
	String currentIdTag_B;
	currentIdTag_B = readRfidTag(true, mfrc522);
	return currentIdTag_B;
}

/********Added new funtion @Wamique***********************/

// void EVSE_B_stopTransactionByRfid(){

// 	Ext_currentIdTag_B = EVSE_B_readRFID(&mfrc522);
// 	if(currentIdTag_B.equals(Ext_currentIdTag_B) == true){
// 		flag_evRequestsCharge_B = false;
// 		flag_evseStopTransaction_B = true;
// 	}else{
// 			if(Ext_currentIdTag_B.equals("") == false)
// 			if(DEBUG_OUT) Serial.println("\n[EVSE_B] Incorrect ID tag\n");
// 		}
// }

#if CP_ACTIVE
/**************CP Implementation @mwh*************/
void EVSE_B_StartCharging()
{

	if (getChargePointStatusService_B()->getEvDrawsEnergy() == false)
	{
		getChargePointStatusService_B()->startEvDrawsEnergy();
	}
	if (DEBUG_OUT)
		Serial.print(F("[EVSE_B] Opening Relays.\n"));
	//   pinMode(32,OUTPUT);
	//  digitalWrite(32, HIGH); //RELAY_1
	// digitalWrite(RELAY_2, RELAY_HIGH);
	requestForRelay(START, 2);
	requestLed(ORANGE, START, 2);
	delay(1200);
	requestLed(WHITE, START, 2);
	delay(1200);
	requestLed(GREEN, START, 2);
	delay(1000);
	Serial.println("[EVS_B] EV is connected and Started charging");
	if (DEBUG_OUT)
		Serial.println("[EVSE_B] Started Drawing Energy");
	delay(500);
}

void EVSE_B_Suspended()
{

	if (getChargePointStatusService_B()->getEvDrawsEnergy() == true)
	{
		getChargePointStatusService_B()->stopEvDrawsEnergy();
	}
	requestLed(BLUE, START, 2); // replace 1 with connector ID
	requestForRelay(STOP, 2);
	//	delay(1000);
	Serial.printf("[EVSE_B] EV Suspended");
}

/**************************************************/

#endif

void displayMeterValuesB()
{
	if (millis() - timerDisplayB > 10000)
	{
		timerDisplayB = millis();
		/*float instantCurrrent_A = eic.GetLineCurrentA();
		float instantVoltage_A  = eic.GetLineVoltageA();
		float instantPower_A = 0.0f;

		if(instantCurrrent_A < minCurr){
			instantPower_A = 0;
		}else{
			instantPower_A = (instantCurrrent_A * instantVoltage_A)/1000.0;
		}*/

		float instantCurrrent_B = eic.GetLineCurrentB();
		int instantVoltage_B = eic.GetLineVoltageB();
		float instantPower_B = 0.0f;

		if (instantCurrrent_B < minCurr)
		{
			instantPower_B = 0;
		}
		else
		{
			instantPower_B = (instantCurrrent_B * instantVoltage_B) / 1000.0;
		}

		/*float instantCurrrent_C = eic.GetLineCurrentC();
		int instantVoltage_C = eic.GetLineVoltageC();
		float instantPower_C = 0.0f;

		if(instantCurrrent_C < minCurr){
			instantPower_C = 0;
		}else{
			instantPower_C = (instantCurrrent_C * instantVoltage_C)/1000.0;
		}*/
		delay(100); // just for safety
#if LCD_ENABLED
		lcd.clear();
		lcd.setCursor(0, 0); // Or setting the cursor in the desired position.
		if (notFaulty_B)
		{
			lcd.print("*****CHARGING 2*****"); // You can make spaces using well... spaces
		}
		else
		{
			switch (fault_code_B)
			{
			case -1:
				break; // It means default.
			case 0:
				lcd.print("Connector2-Over Voltage");
				break;
			case 1:
				lcd.print("Connector2-Under Voltage");
				break;
			case 2:
				lcd.print("Connector2-Over Current");
				break;
			case 3:
				lcd.print("Connector2-Under Current");
				break;
			case 4:
				lcd.print("Connector2-Over Temp");
				break;
			case 5:
				lcd.print("Connector2-Over Temp");
				break;
			case 6:
				lcd.print("Connector2-GFCI"); // Not implemented in AC001
				break;
			case 7:
				lcd.print("Connector2-Earth Disc");
				break;
			default:
				lcd.print("*****FAULTED 2*****"); // You can make spaces using well... spacesbreak;
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
		lcd.print(String(instantVoltage_B));
		lcd.setCursor(0, 2);
		lcd.print("CURRENT(A):");
		lcd.setCursor(12, 2); // Or setting the cursor in the desired position.
		lcd.print(String(instantCurrrent_B));
		lcd.setCursor(0, 3);
		lcd.print("POWER(KW) :");
		lcd.setCursor(12, 3); // Or setting the cursor in the desired position.
		lcd.print(String(instantPower_B));
#endif

#if DWIN_ENABLED
		uint8_t err = 0;
		change_page[9] = 5;
		// v1[6] = instantVoltage_A >> 8;
		// v1[7] = instantVoltage_A & 0xff;
		v2[4] = 0X75;
		instantVoltage_B = instantVoltage_B * 10;
		v2[6] = instantVoltage_B >> 8;
		v2[7] = instantVoltage_B & 0xff;
		// v3[6] = instantVoltage_C >> 8;
		// v3[7] = instantVoltage_C & 0xff;
		// i1[7] = instantCurrrent_A*10;
		i2[4] = 0X77;
		i2[7] = instantCurrrent_B * 10;
		// i3[7] = instantCurrrent_C*10;
		// e1[7] = instantPower_A*10;
		e2[4] = 0X79;
		e2[7] = instantPower_B * 10;
		// e3[7] = instantPower_C*10;
		err = DWIN_SET(cid2, sizeof(cid2) / sizeof(cid2[0]));
		// delay(50);
		err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0])); // page 0
		if (notFaulty_B)
		{
			charging[4] = 0X71;
			err = DWIN_SET(charging, sizeof(charging) / sizeof(charging[0]));
		}
		// delay(50);
		// err = DWIN_SET(charging,sizeof(charging)/sizeof(charging[0]));
		// delay(50);
		// err = DWIN_SET(v1,sizeof(v1)/sizeof(v1[0]));
		err = DWIN_SET(v2, sizeof(v2) / sizeof(v2[0]));
		// err = DWIN_SET(v3,sizeof(v3)/sizeof(v3[0]));
		// err = DWIN_SET(i1,sizeof(i1)/sizeof(i1[0]));
		err = DWIN_SET(i2, sizeof(i2) / sizeof(i2[0]));
		// err = DWIN_SET(i3,sizeof(i3)/sizeof(i3[0]));
		// err = DWIN_SET(e1,sizeof(e1)/sizeof(e1[0]));
		err = DWIN_SET(e2, sizeof(e2) / sizeof(e2[0]));
		// err = DWIN_SET(e3,sizeof(e3)/sizeof(e3[0]));
		delay(500);
#endif
	}
}

/*************************************************EVSE B Reservation loop***********************************************************/
void EVSE_B_Reservation_loop()
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

	if (flag_evseReserveNow_B)
	{
		// Reserve Now execution.
		if (DEBUG_OUT)
			Serial.print("[EVSE_B] Reserve Now\n");

		// time_t start_reserve_time_delta_B = reservation_start_time_B - now();
		time_t reserve_time_delta_B = reserveDate_B - now();

		if ((reserve_time_delta_B <= reservedDuration_B) && (reserve_time_delta_B > 0))
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

			if (gu8_send_status_flag_B && reservation_start_flag_B)
			{
				gu8_send_status_flag_B = false;
				getChargePointStatusService_B()->setReserved(true);
			}

			if (!reservation_start_flag_B)
			{

				if (getChargePointStatusService_B()->getTransactionId() != -1)
				{
					EVSE_B_StopSession();
				}
				// requestLed(BLUE, START, 2);
				reservation_start_flag_B = true;
				flag_evseReadIdTag_B = true; // Entry condition for reading ID Tag.
				flag_evseAuthenticate_B = false;
				gu8_send_status_flag_B = true;
				if (DEBUG_OUT)
					Serial.print("[EVSE_B] Reserve Now  2 1\n");
			}
			if (DEBUG_OUT)
				Serial.print("[EVSE_B] Reserve Now  2\n");
			Serial.print("[EVSE_B] reserve time delta ");
			Serial.println(reserve_time_delta_B);
			if (getChargePointStatusService_B()->getEvDrawsEnergy() == false && notFaulty_B && getChargePointStatusService_B()->getEmergencyRelayClose() == false)
			{
				// requestLed(BLUE, START, 2);
			}
		}
		else
		{
			if ((reserve_time_delta_B <= 0))
			{
				flag_evseReserveNow_B = false;
				getChargePointStatusService_B()->setReserved(false);

#if 0
				/*
				* @bug : Session should not stop just because the reservation expired.
				Resolved by G. Raja Sumant 20/07/2022
				if (getChargePointStatusService_B()->getTransactionId() != -1)
				{
					EVSE_StopSession();
				}*/
#endif

				if (DEBUG_OUT)
					Serial.print("[EVSE_B]stopped due to Reservation timeout!\n");
				reservation_start_flag_B = false;
			}
		}

		if (flag_evseCancelReservation_B)
		{
			flag_evseCancelReservation_B = false;
			flag_evseReserveNow_B = false;
			getChargePointStatusService_B()->setReserved(false);
			// if(getChargePointStatusService_B()->getTransactionId() != -1)
			// {
			// 	EVSE_StopSession();
			// }
			if (DEBUG_OUT)
				Serial.print("[EVSE_B]stopped due to Cancel Reservation!\n");
			reservation_start_flag_B = false;
		}
	}
}
/*************************************************EVSE B Reservation loop***********************************************************/