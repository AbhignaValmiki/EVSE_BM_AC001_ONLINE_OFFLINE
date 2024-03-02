#include "EVSE_C.h"
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
extern unsigned char cid3[8];
#endif
uint8_t fault_occured_C = 0;
 uint8_t currentCounterThreshold_C = 60;
extern float minCurr;
extern bool flag_send_stop_C;
OnBoot_C onBoot_C;
OnReadUserId_C onReadUserId_C;
OnSendHeartbeat_C onSendHeartbeat_C;
OnAuthentication_C onAuthentication_C;
OnStartTransaction_C onStartTransaction_C;
OnStopTransaction_C onStopTransaction_C;
OnUnauthorizeUser_C onUnauthorizeUser_C;

bool evIsPlugged_C;
bool flag_evseIsBooted_C;
bool flag_evseReadIdTag_C;
bool flag_evseAuthenticate_C;
bool flag_evseStartTransaction_C;
bool flag_evRequestsCharge_C;
bool flag_evseStopTransaction_C;
bool flag_evseUnauthorise_C;
bool flag_rebootRequired_C;
bool flag_evseSoftReset_C;
bool flag_evseReserveNow_C = false;		   // added by @mkrishna
bool flag_evseCancelReservation_C = false; // added by @mkrishna
bool flag_localAuthenication_C = false;	   // added by @mkrishna
bool gu8_send_status_flag_C = false;
bool reservation_start_flag_C = false; // added by @mkrishna

extern int reservationId_C;
extern int reserve_connectorId_C;
extern time_t reservation_start_time_C;
extern time_t reserveDate_C;
extern time_t reservedDuration_C; // 15 minutes slot Duration 15 * (1 * 60)
extern bool reserve_state_C;

bool notFaulty_C = false;
extern bool flag_ed_A;

extern bool flag_rebootRequired_A;
extern bool flag_rebootRequired_B;

float chargingLimit_C = 32.0f;
String Ext_currentIdTag_C = "";

long int blinckCounter_C = 0;
int counter1_C = 0;

int prevTxnId_C = -1;

ulong timerDisplayC;

// Reason for stop
extern uint8_t reasonForStop_C;

extern int8_t fault_code_A;
extern int8_t fault_code_B;
extern int8_t fault_code_C;

extern LCD_I2C lcd;

ulong t_C;
int connectorDis_counter_C = 0;
short int counter_drawingCurrent_C = 0;
float drawing_current_C = 0;

String currentIdTag_C = "";
extern WebSocketsClient webSocket;

bool disp_evse_C = false;
extern bool disp_evse_A;
extern bool disp_evse_B;

extern EVSE_states_enum EVSE_state;
extern Preferences preferences;
extern MFRC522 mfrc522;
extern String currentIdTag;
extern bool webSocketConncted;
extern bool isInternetConnected;
extern bool wifi_connect;
extern bool gsm_connect;

Preferences resumeTxn_C;
extern TinyGsmClient client;

int transactionId_C = -1;

bool ongoingTxn_C;
extern bool ongoingTxn_A;
extern bool ongoingTxn_B;

String idTagData_C = "";

bool timeout_active_C = false;
bool timer_initialize_C = false;
ulong timeout_start_C = 0;
short int fault_counter_C = 0;
bool flag_faultOccured_C = false;
ulong relay_timer_C = 0;
ulong faultTimer_C = 0;

extern bool ethernet_enable;
extern bool ethernet_connect;
const ulong TIMEOUT_EMERGENCY_RELAY_CLOSE_C = 120000;

extern Preferences energymeter;

extern bool flag_stop_once_C;

extern String reserve_currentIdTag_C;

extern int globalmeterstartC;

extern bool flag_start_txn_C;

uint8_t online_session_ongoing_C = 0;

extern volatile bool session_ongoing_flag;
extern volatile bool session_on_offline_txn_completed;

extern uint8_t gu8_online_flag;

extern int Sendstart_time_C;
extern int Sendstop_time_C;

extern uint8_t session_ongoing_C_count;

extern String Tag_Id_C;
extern String Transaction_Id_C;
extern String Start_Meter_Value_C;

extern ChargePointStatusService *chargePointStatusService_A;
extern ChargePointStatusService *chargePointStatusService_B;
extern ChargePointStatusService *chargePointStatusService_C;
uint8_t start_session_C = 0;
extern uint8_t gu8_Remote_start_C;
uint8_t Stop_Session_C = 0;
extern float current_energy_C;
extern float discurrEnergy_C;
float LastPresentEnergy_C = 0;
extern uint8_t gu8_bootsuccess;
extern bool flag_evseIsBooted_A;

extern bool EMGCY_status_C;
volatile bool session_ongoing_flag_C = false;
bool stopoffline_C = false;
extern bool ongoingTxnoff_C;
extern uint8_t offline_charging_C;

/**********************************************************/
void EVSE_C_StopSession()
{

	if (getChargePointStatusService_C()->getEvDrawsEnergy() == true)
	{
		getChargePointStatusService_C()->stopEvDrawsEnergy();
	}

	// digitalWrite(32, LOW);
	session_ongoing_flag_C = false;
	session_ongoing_flag = false;
	online_session_ongoing_C = 0;
	disp_evse_C = false;
	gu8_Remote_start_C = 0;
	Stop_Session_C = 2;
	Display_Clear(Stop_Session_C);
	// requestForRelay(STOP, 3);
	if (webSocketConncted == 0)
	{
		getChargePointStatusService_C()->setChargePointstatus(Available);
		requestForRelay(STOP, 3);
		if (!offline_charging_C)
		{
			resumeTxn_C.begin("resume_C", false); // opening preferences in R/W mode
			resumeTxn_C.putBool("ongoingTxn_C", false);
			resumeTxn_C.putBool("ongoingTxnoff_C", true);
			resumeTxn_C.end();
		}
	}
	delay(500);
	flag_evseReadIdTag_C = false;
	flag_evseAuthenticate_C = false;
	flag_evseStartTransaction_C = false;
	flag_evRequestsCharge_C = false;
	flag_evseStopTransaction_C = true;
	flag_evseUnauthorise_C = false;
	Serial.println("[EVSE_C] Stopping Session : " + String(EVSE_state));
}
/**************************************************************************/

/**************************************************************************/

void EVSE_C_initialize()
{
	if (DEBUG_OUT)
		Serial.println("[EVSE_C]:Starting Boot");
	// onBoot_C();
	flag_evseIsBooted_C = true;	 // Exit condition for booting.
	flag_evseReadIdTag_C = true; // Entry condition for reading ID Tag.
	faultTimer_C = millis();
}

/**************************SetUp********************************************/
void EVSE_C_setup()
{

	resumeTxn_C.begin("resume_C", false); // opening preferences in R/W mode
	idTagData_C = resumeTxn_C.getString("idTagData_C", "");
	ongoingTxn_C = resumeTxn_C.getBool("ongoingTxn_C", false);
	transactionId_C = resumeTxn_C.getInt("TxnIdData_C", -1);
	globalmeterstartC = resumeTxn_C.getFloat("meterStart", false);
	stopoffline_C = resumeTxn_C.getBool("ongoingTxnoff_C", false);
	resumeTxn_C.end();

	Serial.println("Stored ID_C:" + String(idTagData_C));
	Serial.println("Ongoing Txn_C: " + String(ongoingTxn_C));
	Serial.println("Txn id_C is : " + String(transactionId_C));
	Serial.println("meterstart_C is: " + String(globalmeterstartC));
	Serial.println(" stopoffline_C is: " + String(stopoffline_C));

	energymeter.begin("MeterData", false);
	float lu_lastEnergy_C = energymeter.getFloat("currEnergy_C", 0);
	// placing energy value back in EEPROM
	energymeter.end();
	Serial.println("[EnergyASampler] lu_lastEnergy_C: " + String(lu_lastEnergy_C));
	globalmeterstartC = (int)lu_lastEnergy_C;
	Serial.println("[EnergyASampler] globalmeterstartC: " + String(globalmeterstartC));

	if (ongoingTxn_C)
	{
		flag_start_txn_C = true;
	}

	EVSE_C_setOnBoot([]()
					 {
		//this is not in loop, that is why we need not update the flag immediately to avoid multiple copies of bootNotification.
		OcppOperation *bootNotification = makeOcppOperation(&webSocket,	new BootNotification());
		initiateOcppOperation(bootNotification);
		if (gu8_online_flag == 1)
    	{
		bootNotification->setOnReceiveConfListener([](JsonObject payload) {

      	// if( flag_MeteringIsInitialised == false){
      	// 	Serial.println("[SetOnBooT] Initializing metering services");
      	// //	meteringService->init(meteringService);
      	// }

      if (DEBUG_OUT) Serial.print(F("EVSE_C_setOnBoot Callback: Metering Services Initialization finished.\n"));

			flag_evseIsBooted_C = true; //Exit condition for booting. 	
			flag_evseReadIdTag_C = true; //Entry condition for reading ID Tag.
			flag_evseAuthenticate_C = false;
			flag_evseStartTransaction_C = false;
			flag_evRequestsCharge_C = false;
			flag_evseStopTransaction_C = false;
			flag_evseUnauthorise_C = false;
			// EvseDevStatus_connector_1 = flag_EVSE_Read_Id_Tag;
			// getChargePointStatusService_C()->setChargePointstatus(Available);


      if (DEBUG_OUT) Serial.print(F("EVSE_C_setOnBoot Callback: Closing Relays.\n"));

			if (DEBUG_OUT) Serial.print(F("EVSE_setOnBoot Callback: Boot successful. Calling Read User ID Block.\n"));
		});
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

			flag_evseIsBooted_C = true;	 // Exit condition for booting.
			flag_evseReadIdTag_C = true; // Entry condition for reading ID Tag.
			flag_evseAuthenticate_C = false;
			flag_evseStartTransaction_C = false;
			// EvseDevStatus_connector_1 = flag_EVSE_Read_Id_Tag;

			// flag_evRequestsCharge = false;
			if (ongoingTxn_C)
			{
				flag_evRequestsCharge_C = true;
				if (DEBUG_OUT)
					Serial.println(F("****[EVSE_setOnBoot] on going transaction is true*****"));
			}
			else
			{
				flag_evRequestsCharge_C = false;
				if (DEBUG_OUT)
					Serial.println(F("****[EVSE_setOnBoot] on going transaction is false*****"));
			}
			flag_evseStopTransaction_C = false;
			flag_evseUnauthorise_C = false;
		} });

	EVSE_C_setOnReadUserId([]()
						   {
		if (DEBUG_OUT) Serial.println("[EVSE_C]:Read RFID");
		static ulong timerForRfid = millis();
		currentIdTag_C = ""; 
		resumeTxn_C.begin("resume_C", false);
		idTagData_C = resumeTxn_C.getString("idTagData_C","");
  		ongoingTxn_C = resumeTxn_C.getBool("ongoingTxn_C",false);
		transactionId_C = resumeTxn_C.getInt("TxnIdData_C", -1);

		int connector = getChargePointStatusService_C()->getConnectorId();
		if(stopoffline_C == 1)
		{
			stopoffline_C = 0;

			OcppOperation *stopTransaction = makeOcppOperation(&webSocket, new StopTransaction(currentIdTag_C, transactionId_C, connector));
			initiateOcppOperation(stopTransaction);
			// prevTxnId_C = transactionId_C;

			if(webSocketConncted)
			{
				resumeTxn_C.begin("resume_C", false); // opening preferences in R/W mode
				// resumeTxn_B.putBool("ongoingTxn_B", false);
				resumeTxn_C.putString("idTagData_C", "");
				resumeTxn_C.putInt("TxnIdData_C", -1);
				resumeTxn_C.putBool("ongoingTxnoff_C", false);
				resumeTxn_C.end();
			}
		}
		/*
		* @brief : When ethernet is enabled, it has to be working.
		* G. Raja Sumant 27/08/2022
		*/

		if(ethernet_connect && 														//ethernet Block
  			(getChargePointStatusService_B()->getEmergencyRelayClose() == false)&&
  			(webSocketConncted == true) && getChargePointStatusService_C()->getUnavailable() == false)
		{
			if((idTagData_C != "") && (ongoingTxn_C == 1)){
  				currentIdTag_C = resumeTxn_C.getString("idTagData_C","");

#if SEEMA_CONNECT
				getChargePointStatusService_C()->startTransaction(transactionId_C);
				reasonForStop_C = 5;
    			//fault_code_A = Power_Loss;
				EVSE_C_StopSession();
				Serial.println("[EVSE_C]:stop session");
#if DISPLAY_ENABLED
      			  connAvail(3,"SESSION RESUME");
  				  checkForResponse_Disp();
#endif

#else

				getChargePointStatusService_C()->authorize(currentIdTag_C); // so that Starttxn can easily fetch this data
  				Serial.println("[EVSE_C]:Resuming session");
#if DISPLAY_ENABLED
      			  connAvail(3,"SESSION RESUME");
  				  checkForResponse_Disp();
#endif
  				// requestLed(BLUE, START, 3);

				getChargePointStatusService_C()->startTransaction(transactionId_C);        //new block for Three connector
				getChargePointStatusService_C()->startEnergyOffer();
				resumeTxn_C.putString("idTagData_C",getChargePointStatusService_C()->getIdTag());
				resumeTxn_C.putBool("ongoingTxn_C",true);
				resumeTxn_C.putInt("TxnIdData_C", transactionId_C);

#endif


  			}else{
				if(millis() - timerForRfid > 10000){ //timer for sending led request
	    		  	// requestLed(GREEN, START, 3);
	    		  	timerForRfid = millis();
#if 0
#if LCD_ENABLED
					//lcd.clear();
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
					lcd.setCursor(0, 2);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 2); // Or setting the cursor in the desired position.
					lcd.print("C: AVAILABLE");
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
			
				currentIdTag_C = EVSE_C_getCurrnetIdTag(&mfrc522);
				Serial.println("WIFI-RFID-C");
  			}
		}

  		if((wifi_connect == true)&& 														//Wifi Block
  			(getChargePointStatusService_C()->getEmergencyRelayClose() == false)&&
  			(webSocketConncted == true)&&
  			(WiFi.status() == WL_CONNECTED)&&
  			(isInternetConnected == true) && getChargePointStatusService_C()->getUnavailable() == false){

  			if((idTagData_C != "") && (ongoingTxn_C == 1)){
  				currentIdTag_C = resumeTxn_C.getString("idTagData_C","");

#if SEEMA_CONNECT
				getChargePointStatusService_C()->startTransaction(transactionId_C);
				reasonForStop_C = 5;
    			//fault_code_A = Power_Loss;
				EVSE_C_StopSession();
				Serial.println("[EVSE_C]:Stop session");
#if DISPLAY_ENABLED
      			  connAvail(3,"SESSION RESUME");
  				  checkForResponse_Disp();
#endif

#else

				getChargePointStatusService_C()->authorize(currentIdTag_C); // so that Starttxn can easily fetch this data
  				Serial.println("[EVSE_C]:Resuming session");
#if DISPLAY_ENABLED
      			  connAvail(3,"SESSION RESUME");
  				  checkForResponse_Disp();
#endif
  				// requestLed(BLUE, START, 3);

				getChargePointStatusService_C()->startTransaction(transactionId_C);        //new block for Three connector
				getChargePointStatusService_C()->startEnergyOffer();
				resumeTxn_C.putString("idTagData_C",getChargePointStatusService_C()->getIdTag());
				resumeTxn_C.putBool("ongoingTxn_C",true);
				resumeTxn_C.putInt("TxnIdData_C", transactionId_C);
#endif


  			}else{
				if(millis() - timerForRfid > 10000){ //timer for sending led request
	    		  	// requestLed(GREEN, START, 3);
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
					lcd.setCursor(0, 2);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 2); // Or setting the cursor in the desired position.
					lcd.print("C: AVAILABLE");
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
			
				currentIdTag_C = EVSE_C_getCurrnetIdTag(&mfrc522);
				Serial.println("[Wifi]RFID C");
  			}

  		}else if((gsm_connect == true)&&													//GSM Block
  				(getChargePointStatusService_C()->getEmergencyRelayClose() == false)&&
  				(client.connected() == true) && getChargePointStatusService_C()->getUnavailable() == false){

  			if((idTagData_C != "") && (ongoingTxn_C == 1)){
  				currentIdTag_C = resumeTxn_C.getString("idTagData_C","");

#if SEEMA_CONNECT
				getChargePointStatusService_C()->startTransaction(transactionId_C);
				reasonForStop_C= 5;
    			//fault_code_A = Power_Loss;
				EVSE_C_StopSession();
				Serial.println("[EVSE_C]:Stop session");
#if DISPLAY_ENABLED
      			  connAvail(3,"SESSION RESUME");
  				  checkForResponse_Disp();
#endif

#else

				getChargePointStatusService_C()->authorize(currentIdTag_C); // so that Starttxn can easily fetch this data
  				Serial.println("[EVSE_C]:Resuming session");
#if DISPLAY_ENABLED
      			  connAvail(3,"SESSION RESUME");
  				  checkForResponse_Disp();
#endif
  				// requestLed(BLUE, START, 3);

				getChargePointStatusService_C()->startTransaction(transactionId_C);        //new block for Three connector
				getChargePointStatusService_C()->startEnergyOffer();
				resumeTxn_C.putString("idTagData_C",getChargePointStatusService_C()->getIdTag());
				resumeTxn_C.putBool("ongoingTxn_C",true);
				resumeTxn_C.putInt("TxnIdData_C", transactionId_C);

#endif


  			}else{
				if(millis() - timerForRfid > 10000){ //timer for sending led request
	    		  	// requestLed(GREEN, START, 3);
	    		  	timerForRfid = millis();
#if 0
#if LCD_ENABLED
					//lcd.clear();
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
					lcd.setCursor(0, 2);               // Or setting the cursor in the desired position.
      				lcd.print("                    "); // Clear the line
					lcd.setCursor(0, 2); // Or setting the cursor in the desired position.
					lcd.print("C: AVAILABLE");
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
			
				currentIdTag_C = EVSE_C_getCurrnetIdTag(&mfrc522);
				Serial.println("[GSM]RFID C");
  			}


  		} 
		resumeTxn_C.end();
		/*	
		idTagData_m = resumeTxn.getString("idTagData","");
  		ongoingTxn_m = resumeTxn.getBool("ongoingTxn",false);

		if((ongoingTxn_m == 1) && (idTagData_m != "") && 
		      (getChargePointStatusService_C()->getEmergencyRelayClose() == false) &&
		      (WiFi.status() == WL_CONNECTED)&&
		      (webSocketConncted == true)&&
		      (isInternetConnected == true)){   //giving priority to stored data
			currentIdTag_C = resumeTxn.getString("idTagData","");
			Serial.println("[EVSE_setOnReadUserId] Resuming Session");
      		requestLed(BLUE,START,1);
         
		}else*/ 
		// if((getChargePointStatusService_C()->getEmergencyRelayClose() == false) &&
		//           (WiFi.status() == WL_CONNECTED) &&
		//           (webSocketConncted == true) && 
		//           (isInternetConnected == true)){
		// 	  #if LED_ENABLED
		// 	  if(millis() - timerForRfid > 5000){ //timer for sending led request
  //   		  requestLed(GREEN,START,3);
  //   		  timerForRfid = millis();
  //   		  }
  //   		  #endif
		// 	currentIdTag_C = EVSE_C_getCurrnetIdTag(&mfrc522);
		// 	Serial.println("********RFID C**********");
		// }

		if (currentIdTag_C.equals("") == true) {
			flag_evseReadIdTag_C = true; //Looping back read block as no ID tag present.
			flag_evseAuthenticate_C = false;
			flag_evseStartTransaction_C = false;
			flag_evRequestsCharge_C = false;
			flag_evseStopTransaction_C = false;
			flag_evseUnauthorise_C = false;
		} else {
			flag_evseReadIdTag_C = false;		
			flag_evRequestsCharge_C = false;
			flag_evseStopTransaction_C = false;
			flag_evseUnauthorise_C = false;
#if SEEMA_CONNECT
			    if(ongoingTxn_C == 1)
				{
					flag_evseAuthenticate_C = false; //Entry condition for authentication block.
					flag_evseStartTransaction_C = false;
					flag_evseStopTransaction_C = true;
					Serial.print("EVSE_C_ ongoingTxn_C. \n");

				}
				else
				{
					// flag_evseAuthenticate_C = false; //Entry condition for authentication block.
					// flag_evseStartTransaction_C = true;
					flag_evseAuthenticate_C = true; 
					flag_evseStartTransaction_C = false;
					flag_evseStopTransaction_C = false;
					Serial.print("EVSE_C_ Start Txn_C. \n");
				}

#else

			    if(ongoingTxn_C == 1)
				{

					flag_evseAuthenticate_C = false; //Entry condition for authentication block.
					flag_evseStartTransaction_C = false;
					// flag_evseStopTransaction_C = true;
                    flag_evRequestsCharge_C = true;    // to be check the functionality
					Serial.print("EVSE_C_ ongoingTxn_C. \n");

				}
				else
				{
					flag_evseAuthenticate_C = true; //Entry condition for authentication block.
					flag_evseStartTransaction_C = false;
					flag_evseStopTransaction_C = false;
					if (DEBUG_OUT) Serial.println("[EVSE_C]:Calling Auth");
				}
#endif

			
		} });

	EVSE_C_setOnsendHeartbeat([]()
							  {
    if (DEBUG_OUT) Serial.println("[EVSE_C]:Calling HeartBeat");
    OcppOperation *heartbeat = makeOcppOperation(&webSocket, new Heartbeat());
    initiateOcppOperation(heartbeat); 
    heartbeat->setOnReceiveConfListener([](JsonObject payload) {
        const char* currentTime = payload["currentTime"] | "Invalid";
        if (strcmp(currentTime, "Invalid")) {
          if (setTimeFromJsonDateString(currentTime)) {
            if (DEBUG_OUT) Serial.println("[EVSE_C]:Heartbeat accepted");
          } else {
            Serial.println("Heartbeat accepted not in UTC timestamp\n");
          }
        } else {
          Serial.println("Heartbeat Denied\n");
        }
    }); });

	EVSE_C_setOnAuthentication([]()
							   {
		if (DEBUG_OUT) Serial.println("EVSE_C:Auth");
		flag_evseAuthenticate_C = false;
		OcppOperation *authorize = makeOcppOperation(&webSocket, new Authorize(currentIdTag_C));
		initiateOcppOperation(authorize);
		/*chargePointStatusService->authorize(currentIdTag_C, connectorId_C);  */    //have to edit
		authorize->setOnReceiveConfListener([](JsonObject payload) {
			const char* status = payload["idTagInfo"]["status"] | "Invalid";
			if (!strcmp(status, "Accepted")) {
				getChargePointStatusService_C()->setChargePointstatus(Preparing);
				flag_evseReadIdTag_C = false;
				flag_evseAuthenticate_C = false;
				flag_evseStartTransaction_C = true; //Entry condition for starting transaction.
				flag_evRequestsCharge_C = false;
				flag_evseStopTransaction_C = false;
				flag_evseUnauthorise_C = false;
				//requestLed(BLUE,START,3);
				// requestLed(BLINKYBLUE,START,3);
				currentIdTag = currentIdTag_C; 		/* Assinging currentIdTag_C to currentIdTag for Set to Auth cache  */
				// requestSendAuthCache();//DISABLED Not checking the authentication cache
					
				if (DEBUG_OUT) Serial.println("EVSE_C:Auth ACcepted");
#if CP_ACTIVE 
				flag_controlPAuthorise = true;
#endif

			} else {
				flag_evseReadIdTag_C = false;
				flag_evseAuthenticate_C = false;
				flag_evseStartTransaction_C = false;
				flag_evRequestsCharge_C = false;
				flag_evseStopTransaction_C = false;
				flag_evseUnauthorise_C = true; //wrong ID tag received, so clearing the global current ID tag variable and setting up to read again.
				if (DEBUG_OUT) Serial.println("EVSE_C:Auth Denied");
			}  
		}); });

	EVSE_C_setOnStartTransaction([]()
								 {
		flag_evseStartTransaction_C = false;
		String idTag = "";
		int connectorId = 0;
		if (getChargePointStatusService_C() != NULL) {
			if (!getChargePointStatusService_C()->getIdTag().isEmpty()) {
				idTag = String(getChargePointStatusService_C()->getIdTag());
				connectorId = getChargePointStatusService_C()->getConnectorId();
			}
		}
		OcppOperation *startTransaction = makeOcppOperation(&webSocket, new StartTransaction(idTag,connectorId));
		initiateOcppOperation(startTransaction);
		startTransaction->setOnReceiveConfListener([](JsonObject payload) {
    		const char* status = payload["idTagInfo"]["status"] | "Invalid";
      if (!strcmp(status, "Accepted") || !strcmp(status, "ConcurrentTx")) { 

      flag_evseReadIdTag_C = false;
      flag_evseAuthenticate_C = false;
      flag_evseStartTransaction_C = false;
      flag_evRequestsCharge_C = true;
      flag_evseStopTransaction_C = false;
      flag_evseUnauthorise_C = false;
      if (DEBUG_OUT) Serial.println("EVSE_C:Start succesful");
#if LCD_DISPLAY_ENABLED
      lcd.clear();
      lcd.setCursor(3, 1);
      lcd.print("AUTHENTICATION");
	  lcd.setCursor(3, 2);
      lcd.print("SUCCESSFUL - 3");
	  delay(1500);
#endif
      /************************************/
      int transactionId = payload["transactionId"] | -1;
      getChargePointStatusService_C()->startTransaction(transactionId);        //new block for Three connector
      getChargePointStatusService_C()->startEnergyOffer();
	  resumeTxn_C.begin("resume_C", false);
      resumeTxn_C.putString("idTagData_C",getChargePointStatusService_C()->getIdTag());
      resumeTxn_C.putBool("ongoingTxn_C",true);
	  resumeTxn_C.putInt("TxnIdData_C", transactionId);
	  resumeTxn_C.putFloat("meterStart", globalmeterstartC);
	  resumeTxn_C.end();

	  Tag_Id_C = getChargePointStatusService_C()->getIdTag();
      Transaction_Id_C = String(transactionId);
      Start_Meter_Value_C = String(globalmeterstartC);
	   session_ongoing_flag_C = true;
      session_ongoing_flag = true;
	  session_on_offline_txn_completed = false;
      session_ongoing_C_count = 1;
     Serial.println("EVSE_C:Charging");
	  getChargePointStatusService_C()->setChargePointstatus(Charging);
      //*****Storing tag data in EEPROM****//
      /*
	  resumeTxn.putString("idTagData",currentIdTag);
      resumeTxn.putBool("ongoingTxn",true);*/
      //***********************************//

      } else {
        flag_evseReadIdTag_C = false;
        flag_evseAuthenticate_C = false;
        flag_evseStartTransaction_C = false;
        flag_evRequestsCharge_C = false;
        flag_evseStopTransaction_C = false;
        flag_evseUnauthorise_C = true; //wrong ID tag received, so clearing the global current ID tag variable and setting up to read again.
        if (DEBUG_OUT) Serial.println("EVSE_C:Start iunsuccesful");
#if LCD_DISPLAY_ENABLED
      lcd.clear();
      lcd.setCursor(3, 1);
      lcd.print("AUTHENTICATION");
	  lcd.setCursor(3, 2);
      lcd.print("DENIED - 3");
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

	EVSE_C_setOnStopTransaction([]()
								{
		flag_evseStopTransaction_C = false;
		getChargePointStatusService_C()->setFinishing(true);
		// getChargePointStatusService_C()->setChargePointstatus(Finishing);
		// if(notFaulty_C)
		// {
		// 	getChargePointStatusService_C()->setChargePointstatus(Available);
		// }
		if (getChargePointStatusService_C() != NULL) {
          getChargePointStatusService_C()->stopEnergyOffer();
        }
        int txnId = getChargePointStatusService_C()->getTransactionId();
        int connector = getChargePointStatusService_C()->getConnectorId();
		if(txnId !=prevTxnId_C)
	{
		OcppOperation *stopTransaction = makeOcppOperation(&webSocket, new StopTransaction(currentIdTag_C, txnId, connector));
		initiateOcppOperation(stopTransaction);
		prevTxnId_C = txnId;
    	if (DEBUG_OUT) Serial.println("EVSE_C:ClosingRelays");
#if LCD_DISPLAY_ENABLED
  			lcd.clear();
  			lcd.setCursor(2, 1);
  			lcd.print("OUTPUT 3 STOPPED");
#endif
LastPresentEnergy_C = (discurrEnergy_C)*1000 + LastPresentEnergy_C;

    	/**********************Until Offline functionality is implemented***********/
    	//Resume namespace(Preferences)
    	if(getChargePointStatusService_C()->getEmergencyRelayClose() == false){
    		//requestLed(GREEN,START,3);   //temp fix  // it is affecting offline led functionality

    	}
		#if 1
		resumeTxn_C.begin("resume_C", false);
    	resumeTxn_C.putBool("ongoingTxn_C", false);
    	// resumeTxn_C.putString("idTagData_C","");
		// resumeTxn_C.putInt("TxnIdData_C", -1);
		resumeTxn_C.end();
		#endif
		
		if(ethernet_connect)
		{
			if(!webSocketConncted)
			{
				 flag_evseReadIdTag_C = true;
		        flag_evseAuthenticate_C = false;
		        flag_evseStartTransaction_C = false;
		        flag_evRequestsCharge_C = false;
		        flag_evseStopTransaction_C = false;
		        flag_evseUnauthorise_C = false;
	    		// Serial.println("Clearing Stored ID tag in StopTransaction()");
			}
		}
    	else if(wifi_connect == true){
	    	if(!webSocketConncted || WiFi.status() != WL_CONNECTED || isInternetConnected == false){
		        getChargePointStatusService_C()->stopTransaction();
		    	getChargePointStatusService_C()->unauthorize();  //can be buggy 
		        flag_evseReadIdTag_C = true;
		        flag_evseAuthenticate_C = false;
		        flag_evseStartTransaction_C = false;
		        flag_evRequestsCharge_C = false;
		        flag_evseStopTransaction_C = false;
		        flag_evseUnauthorise_C = false;
		    	// Serial.println(F("Clearing Stored ID tag in StopTransaction()"));
	    	}
    	}else if(gsm_connect == true){
    		if(client.connected() == false){
    			getChargePointStatusService_C()->stopTransaction();
		    	getChargePointStatusService_C()->unauthorize();  //can be buggy 
		        flag_evseReadIdTag_C = true;
		        flag_evseAuthenticate_C = false;
		        flag_evseStartTransaction_C = false;
		        flag_evRequestsCharge_C = false;
		        flag_evseStopTransaction_C = false;
		        flag_evseUnauthorise_C = false;
		    	// Serial.println(F("Clearing Stored ID tag in StopTransaction()"));
    		}
    	}
		Serial.println("[EVSE_C]IdTag-cleared");
    	// requestForRelay(STOP, 3);
		online_session_ongoing_C = 0;
    	delay(500);
		stopTransaction->setOnReceiveConfListener([](JsonObject payload) {
#if LCD_ENABLED
  			lcd.clear();
  			lcd.setCursor(3, 2);
  			lcd.print("STOP C SUCCESS");
#endif
			flag_evseReadIdTag_C = false;
			flag_evseAuthenticate_C = false;
			flag_evseStartTransaction_C = false;
			flag_evRequestsCharge_C = false;
			flag_evseStopTransaction_C = false;
			flag_evseUnauthorise_C = true;
			getChargePointStatusService_C()->stopTransaction();
      		if (DEBUG_OUT) Serial.println("[EVSE_C]StopSuccesful,Relayclose");

		});
		}
		    else
			{
				Serial.println("[EVSE_C]:AlreadyStopped");
			} });

	EVSE_C_setOnUnauthorizeUser([]()
								{
		if(flag_evseSoftReset_C == true){
			//This 'if' block is developed by @Wamique.
			flag_evseReadIdTag_C = false;
			flag_evseAuthenticate_C = false;
			flag_evseStartTransaction_C = false;
			flag_evRequestsCharge_C = false;
			flag_evseStopTransaction_C = false;
			flag_evseUnauthorise_C = false;
			flag_rebootRequired_C = true;
			getChargePointStatusService_C()->unauthorize();
			if(fault_occured_C != 1)
			{
			getChargePointStatusService_C()->setChargePointstatus(Available);
			}
			if (DEBUG_OUT)Serial.println("EVSE_C]:Unauthorizied");
		} else if(flag_evseSoftReset_C == false){
			flag_evseReadIdTag_C = true;
			flag_evseAuthenticate_C = false;
			flag_evseStartTransaction_C = false;
			flag_evRequestsCharge_C = false;
			flag_evseStopTransaction_C = false;
			flag_evseUnauthorise_C = false;
			if (DEBUG_OUT) Serial.println("EVSE_C]:Unauthoriziing");
			getChargePointStatusService_C()->unauthorize();
			if(fault_occured_C != 1)
			{
				getChargePointStatusService_C()->setChargePointstatus(Available);
			}
			
		} });
}

/*********************************************************************/

void EVSE_C_loop()
{

	if (flag_evseIsBooted_C == false)
	{
		if (DEBUG_OUT)
			Serial.println("[EVSE] Booting...");
		delay(1000);
		// onBoot();
		t_C = millis();
		return;
	}
	// else if (flag_evseIsBooted_A == true)
	else if (flag_evseIsBooted_C == true)
	{
		if (flag_evseReadIdTag_C == true)
		{

			if (onReadUserId_C != NULL)
			{
				onReadUserId_C();
			}
			return;
		}
		else if (flag_evseAuthenticate_C == true)
		{
			if (onAuthentication_C != NULL)
			{
				// Add condition by checking if available or unavailable
				bool un = false;
				un = getChargePointStatusService_C()->getUnavailable();
				currentIdTag_C = getChargePointStatusService_C()->getIdTag();
				currentIdTag = currentIdTag_C; /* Assinging currentIdTag_C to currentIdTag for check Auth cache  */
				if (!un)
				{
					if (flag_evseReserveNow_C)
					{
						Serial.println(" reserve_currentIdTag_C :");
						Serial.println(reserve_currentIdTag_C);

						Serial.println(" currentIdTag_C :");
						Serial.println(currentIdTag_C);

						if (reserve_currentIdTag_C == currentIdTag_C)
						{
							if (DEBUG_OUT)
								Serial.println("EVSE_C]:Reservation tag matched");

							bool auth_checker = false;
							auth_checker = requestGetAuthCache();

							if (auth_checker)
							{
								getChargePointStatusService_C()->authorize(currentIdTag_C);
								flag_evseReadIdTag_C = false;
								flag_evseAuthenticate_C = false;
								flag_evseStartTransaction_C = true; // Entry condition for starting transaction.
								flag_evRequestsCharge_C = false;
								flag_evseStopTransaction_C = false;
								flag_evseUnauthorise_C = false;

								if (DEBUG_OUT)
									Serial.println("EVSE_C]:AuthAcceptef");
								// requestLed(BLUE, START, 3);
								// requestLed(BLINKYBLUE, START, 3);
							}
							else
							{
								onAuthentication_C();
							}
						}
						else
						{
							if (DEBUG_OUT)
								Serial.println("EVSE_C]:Reservation tag not matched");
							return;
						}
					}
					else
					{
						// if (requestGetAuthCache())
						if (0)
						{
							getChargePointStatusService_C()->authorize(currentIdTag_C);
							flag_evseReadIdTag_C = false;
							flag_evseAuthenticate_C = false;
							flag_evseStartTransaction_C = true; // Entry condition for starting transaction.
							flag_evRequestsCharge_C = false;
							flag_evseStopTransaction_C = false;
							flag_evseUnauthorise_C = false;

							if (DEBUG_OUT)
								Serial.println("EVSE_C]:AuthAccepted");
							// requestLed(BLUE, START, 3);
							// requestLed(BLINKYBLUE, START, 3);
						}
						else
						{
							onAuthentication_C();
						}
					}
				}
			}
			return;
		}
		else if (flag_evseStartTransaction_C == true)
		{
			if (onStartTransaction_C != NULL)
			{
#if CP_CCTIVE
				if ((EVSE_state == STATE_C || EVSE_state == STATE_D) && getChargePointStatusService()->getEmergencyRelayClose() == false)
				{
					onStartTransaction_C();
				}
				else
				{
					Serial.println(F("Connect the Connector to EV / Or fault exist")); // here have to add timeout of 30 sec
					connectorDis_counter_C++;
					// EVSE_stopTransactionByRfid();
					if (connectorDis_counter_C > 25)
					{
						connectorDis_counter_C = 0;

						EVSE_C_StopSession();
					}
				}
#endif

#if !CP_ACTIVE
				onStartTransaction_C(); // can add check for fault
#endif
			}
		}
		else if (flag_evRequestsCharge_C == true)
		{

#if CP_ACTIVE
			// flag_evRequestsCharge = false;
			if (getChargePointStatusService_C() != NULL && getChargePointStatusService_C()->getEvDrawsEnergy() == false)
			{

				/***********************Control Pilot @Wamique******************/
				if (EVSE_state == STATE_C || EVSE_state == STATE_D)
				{
					if (getChargePointStatusService_C()->getEmergencyRelayClose() == false)
					{
						EVSE_C_StartCharging();
					}
					else if (getChargePointStatusService_C()->getEmergencyRelayClose() == true)
					{
						Serial.println(F("The voltage / current / Temp is out or range. FAULTY CONDITION DETECTED."));
					}
				}
				else if (EVSE_state == STATE_SUS)
				{
					EVSE_C_Suspended();
					Serial.println(counter1);
					if (counter1_C++ > 25)
					{ // Have to implement proper timeout
						counter1_C = 0;
						EVSE_C_StopSession();
					}
				}
				else if (EVSE_state == STATE_DIS || EVSE_state == STATE_E || EVSE_state == STATE_B || EVSE_state == STATE_A)
				{

					//	EVSE_StopSession();     // for the very first time cable can be in disconnected state

					// if(txn == true){           // can implement counter > 10 just to remove noise
					EVSE_C_StopSession();
					//	}
				}
				else
				{

					Serial.println(F("[EVSE] STATE Error" + String(EVSE_state)));
					delay(2000);

					//	requestLed(RED,START,1);
				}
			}
			if (getChargePointStatusService_C()->getEvDrawsEnergy() == true)
			{

				//	txn = true;

				if (EVSE_state == STATE_C || EVSE_state == STATE_D)
				{

					if (DEBUG_OUT)
						Serial.println(F("[EVSE_CP] Drawing Energy"));

					if (millis() - t_C > 10000)
					{
						if (getChargePointStatusService_C()->getEmergencyRelayClose() == false)
						{
							requestLed(BLINKYGREEN, START, 3);
							t_C = millis();
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
					if (counter_faultstate_C++ > 5)
					{
						EVSE_C_StopSession();
						counter_faultstate_C = 0;
					}
				}
				else if (EVSE_state == STATE_SUS)
				{
					EVSE_C_Suspended(); // pause transaction :update suspended state is considered in charging state
				}
				else if (EVSE_state == STATE_DIS)
				{

					Serial.println(F("[EVSE] Connect the Connector with EV and Try again"));
					EVSE_C_StopSession();
				}
			}

			/***Implemented Exit Feature with RFID @Wamique****/
			//  EVSE_C_stopTransactionByRfid();
#endif

#if !CP_ACTIVE
			if (getChargePointStatusService_C() != NULL && getChargePointStatusService_C()->getEvDrawsEnergy() == false)
			{
				if (getChargePointStatusService_C()->getEmergencyRelayClose() == false)
				{
					getChargePointStatusService_C()->startEvDrawsEnergy();

					if (DEBUG_OUT)
						Serial.println("EVSE_C]:opening relays");
					reasonForStop_C = 3; // Local
					requestForRelay(START, 3);
#if DISPLAY_ENABLED
					flag_tapped = true;
#endif
					/*requestLed(ORANGE,START,3);
					delay(1200);
					requestLed(WHITE,START,3);
					delay(1200);
					requestLed(GREEN, START, 3);
					delay(1000);*/
					if (DEBUG_OUT)
						Serial.println("EVSE_C]:Drawing energy");
					// displayMeterValuesC();
				}
				else if (getChargePointStatusService_C()->getEmergencyRelayClose() == true)
				{
					Serial.println("EVSE_C]:fault detected");
				}
			}
			if (getChargePointStatusService_C()->getEvDrawsEnergy() == true)
			{
				// delay(250);

				if (DEBUG_OUT)
					Serial.println("EVSE_C]:drawing energy");
				disp_evse_C = true;
				start_session_C = 2;
				Display_Set(start_session_C);
				// displayMeterValuesC();

				// blinking green Led
				if (millis() - t_C > 10000) // Change from 5 to 10 for independent emergency.
				{
					// if((WiFi.status() == WL_CONNECTED) && (webSocketConncted == true) && (isInternetConnected == true)&& getChargePointStatusService()->getEmergencyRelayClose() == false){
					// 	requestLed(BLINKYGREEN_EINS,START,1);
					// 	t = millis();
					// }

					if (getChargePointStatusService_C()->getEmergencyRelayClose() == false)
					{
						// requestLed(BLINKYGREEN, START, 3);
						online_session_ongoing_C = 1;
						t_C = millis();
						if (millis() - relay_timer_C > 15000)
						{

							requestForRelay(START, 3);
							relay_timer_C = millis();
						}
					}
				}
// Current check
#if 0
				drawing_current_C = eic.GetLineCurrentC();
				Serial.println("Current C: " + String(drawing_current_C));
				if (drawing_current_C <= minCurr)
				{
				Serial.println("Min current_C");

					if (notFaulty_C)
					{
						counter_drawingCurrent_C++;
						Serial.println("counter_drawingCurrent_C online: " + (String)counter_drawingCurrent_C);
					}
					// if(counter_drawingCurrent_C > 120){
					if (counter_drawingCurrent_C > currentCounterThreshold_C)
					{
						// Check for the case where reasonForStop is not Local , Other
						if (reasonForStop_C != 3 || reasonForStop_C != 4)
							reasonForStop_C = 1; // EV disconnected
						Serial.println(F("Stopping session due to No current"));
#if LCD_ENABLED
						if (notFaulty_C)
						{
							lcd.clear();
							if (currentCounterThreshold_C == 2)
							{
								lcd.setCursor(0, 1);
								lcd.print("C: EV DISCONNECTED!");
							}
							else
							{
								lcd.setCursor(3, 0);
								lcd.print("C: NO POWER DRAWN");
							}
						}
#endif
						counter_drawingCurrent_C = 0;
						EVSE_C_StopSession();
					}
				}
				else
				{
					counter_drawingCurrent_C = 0;
					if (notFaulty_C)
					{
						// currentCounterThreshold_C = 2; // 2 ideally.
					}
					Serial.println(F("counter_drawingCurrent Reset"));
				}
#endif
			}
			// Implemented Exit Feature with RFID @Wamique//
// EVSE_C_stopTransactionByRfid();
#endif
			// this is the only 'else if' block which is calling next else if block. the control is from this file itself. the control is not changed from any other file. but the variables are required to be present as extern in other file to decide calling of other functions.
			return;
		}
		else if (flag_evseStopTransaction_C == true)
		{
			if (getChargePointStatusService_C() != NULL)
			{
				getChargePointStatusService_C()->stopEvDrawsEnergy();
			}
			if (onStopTransaction_C != NULL)
			{
				onStopTransaction_C();
#if CP_ACTIVE
				requestforCP_OUT(STOP); // stop pwm
#endif
			}
			return;
		}
		else if (flag_evseUnauthorise_C == true)
		{
			if (onUnauthorizeUser_C != NULL)
			{
				onUnauthorizeUser_C();
			}
			return;
		}
		else if (flag_rebootRequired_C == true && flag_rebootRequired_A == true && flag_rebootRequired_B == true)
		{
			// soft reset execution.
			//  flag_evseIsBooted_C = false;
			//  flag_rebootRequired_C = false;
			//  flag_evseSoftReset_C = false;
			if (getChargePointStatusService_A()->inferenceStatus() != ChargePointStatus::Charging &&
				getChargePointStatusService_B()->inferenceStatus() != ChargePointStatus::Charging &&
				getChargePointStatusService_C()->inferenceStatus() != ChargePointStatus::Charging)
			{
				if (DEBUG_OUT)
					Serial.println("EVSE_C]:rebooting");
				delay(5000);
				ESP.restart();
			}
		}
		else
		{
			if (DEBUG_OUT)
				Serial.println("[EVSE_C] waiting for response...\n");
			// delay(100);
		}
	}
}

bool EMGCY_FaultOccured_C = false;
extern bool EMGCY_FaultOccured_A;
extern bool EMGCY_FaultOccured_B;
short EMGCY_counter_C = 0;
extern short EMGCY_counter_A;
extern short EMGCY_counter_B;

void emergencyRelayClose_Loop_C()
{
	if (millis() - faultTimer_C > 1000)
	{
		if (getChargePointStatusService_C()->getOverCurrent() == true)
		{
			if (getChargePointStatusService_C()->getTransactionId() != -1)
			{
#if LCD_ENABLED
				lcd.clear();
				lcd.setCursor(0, 0); // Or setting the cursor in the desired position.
				lcd.print("STATUS: FAULTED");
				lcd.setCursor(0, 1);
				lcd.print("C: OVER CURRENT");
#endif
				EVSE_C_StopSession();
#if LED_ENABLED
				// requestLed(RED, START, 3);
				delay(2000);
#endif
			}
		}
		faultTimer_C = millis();
		// bool EMGCY_status_C = requestEmgyStatus();
		// Serial.println("EMGCY_Status_C: "+String(EMGCY_Status_C));
		if (EMGCY_status_C == true)
		{
			if (EMGCY_counter_C++ > 0)
			{
				notFaulty_C = false;
				// if(EMGCY_counter_C == 0){
				fault_code_A = 8;
				fault_code_B = 8;
				fault_code_C = 8;
				// requestForRelay(STOP,3); // zero means all!
				// requestForRelay(STOP,2);
				// requestForRelay(STOP,3);
				reasonForStop_C = 0;
				disp_evse_C = false;
				gu8_Remote_start_C = 0;
				Stop_Session_C = 2;
				Display_Clear(Stop_Session_C);
// requestLed(BLINKYRED,START,3);
// requestLed(BLINKYRED,START,2);
// requestLed(BLINKYRED,START,3);
#if DISPLAY_ENABLED
				setHeader("RFID UNAVAILABLE");
				checkForResponse_Disp();
				connAvail(3, "FAULTED EMGY");
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
				EMGCY_counter_A = 0;

				getChargePointStatusService_B()->setEmergencyRelayClose(true);
				EMGCY_FaultOccured_B = true;
				EMGCY_counter_B = 0;*/

				getChargePointStatusService_C()->setEmergencyRelayClose(true);
				getChargePointStatusService_C()->setChargePointstatus(Faulted);
				fault_occured_C = 1;
				EMGCY_FaultOccured_C = true;
				EMGCY_counter_C = 0;
			}
		}
		else
		{
			notFaulty_C = true;
			EMGCY_FaultOccured_C = false;
			EMGCY_counter_C = 0;
			getChargePointStatusService_C()->setEmergencyRelayClose(false);
			// if (getChargePointStatusService_C()->getChargePointstatus() != Available &&
			// getChargePointStatusService_C()->getChargePointstatus() != Charging &&
			// getChargePointStatusService_C()->getChargePointstatus() != Preparing &&
			// getChargePointStatusService_C()->getChargePointstatus() != Finishing )
			// {
			//     getChargePointStatusService_C()->setChargePointstatus(Available);
			// }
			if ((fault_occured_C == 1))
			{
				if ((flag_evRequestsCharge_C == false) && (ongoingTxn_C == false))
				{
					fault_occured_C = 0;
					getChargePointStatusService_C()->setChargePointstatus(Available);
				}

				else if ((flag_evRequestsCharge_C == true))
				{
					fault_occured_C = 0;
					getChargePointStatusService_C()->setChargePointstatus(Charging);
				}
			}
		}

		/*if(getChargePointStatusService_C()->getTransactionId() == -1)
		{
			//getChargePointStatusService_C()->stopEvDrawsEnergy();
			requestForRelay(STOP, 3);
		}*/

		if (EMGCY_FaultOccured_C == true && getChargePointStatusService_C()->getTransactionId() != -1)
		{
			Serial.println("EVSE_C]:Emergency stop requested");
			// flag_evseReadIdTag_C = false;
			// flag_evseAuthenticate_C = false;
			// flag_evseStartTransaction_C = false;
			// flag_evRequestsCharge_C = false;
			// flag_evseStopTransaction_C = true;
			// session_ongoing_flag = false;
			// session_ongoing_flag_C = false;
			if (getChargePointStatusService_C()->getEvDrawsEnergy() == true)
			{
			    EVSE_C_StopSession();
			}
			online_session_ongoing_C = 0;
		}
		else if (EMGCY_FaultOccured_C == false)
		{

			// float volt = eic.GetLineVoltageC();
			// float current = eic.GetLineCurrentC();
			// float temp = eic.GetTemperature();
			// Serial.println("Voltage_C: " + String(volt) + ", Current_C: " + String(current) + ", Temperature: " + String(temp));
			if (getChargePointStatusService_C() != NULL)
			{
				if (getChargePointStatusService_C()->getOverVoltage() == true ||
					getChargePointStatusService_C()->getUnderVoltage() == true ||
					getChargePointStatusService_C()->getUnderTemperature() == true ||
					getChargePointStatusService_C()->getOverTemperature() == true ||
					(flag_ed_A) ||
					getChargePointStatusService_C()->getOverCurrent() == true)
				{
					Serial.println("EVSE_C]:fault occured");
					notFaulty_C = false;
					getChargePointStatusService_C()->setEmergencyRelayClose(true);
					getChargePointStatusService_C()->setChargePointstatus(Faulted);
					fault_occured_C = 1;
					if (!timer_initialize_C)
					{
						timeout_start_C = millis();
						timer_initialize_C = true;
					}
				}
				else if (getChargePointStatusService_C()->getOverVoltage() == false &&
						 getChargePointStatusService_C()->getUnderVoltage() == false &&
						 getChargePointStatusService_C()->getUnderTemperature() == false &&
						 getChargePointStatusService_C()->getOverTemperature() == false &&
						 (!flag_ed_A) &&
						 getChargePointStatusService_C()->getOverCurrent() == false)
				{
					Serial.println("EVSE_C]:not faulty ");
					notFaulty_C = true;
					getChargePointStatusService_C()->setEmergencyRelayClose(false);
					// if (getChargePointStatusService_C()->getChargePointstatus() != Available &&
					// getChargePointStatusService_C()->getChargePointstatus() != Charging &&
					// getChargePointStatusService_C()->getChargePointstatus() != Preparing &&
					// getChargePointStatusService_C()->getChargePointstatus() != Finishing )
					// {
					//     getChargePointStatusService_C()->setChargePointstatus(Available);
					// }
					if ((fault_occured_C == 1))
					{
						if ((flag_evRequestsCharge_C == false) && (ongoingTxn_C == false))
						{
							fault_occured_C = 0;
							getChargePointStatusService_C()->setChargePointstatus(Available);
						}

						else if ((flag_evRequestsCharge_C == true))
						{
							fault_occured_C = 0;
							getChargePointStatusService_C()->setChargePointstatus(Charging);
						}
					}
					// if (!timer_initialize){
					timeout_start_C = 0;
					timer_initialize_C = false;
					//}
				}

				if (getChargePointStatusService_C()->getEmergencyRelayClose() == true)
				{
					timeout_active_C = true;
					requestForRelay(STOP, 3);
					online_session_ongoing_C = 0;
					delay(50);
#if LED_ENABLED
					// requestLed(RED, START, 3);
#endif

					flag_faultOccured_C = true;
				}
				else if (getChargePointStatusService_C()->getEmergencyRelayClose() == false && flag_faultOccured_C == true)
				{
					timeout_active_C = false;
					if ((getChargePointStatusService_C()->getTransactionId() != -1))
					{ // can be buggy
						if (fault_counter_C++ > 1)
						{
							fault_counter_C = 0;
							// requestForRelay(START,3);
							// delay(50);
							Serial.println("[EmergencyRelay_C] Starting Txn");
							flag_faultOccured_C = false;
						}
					}
				}

				if (timeout_active_C && getChargePointStatusService_C()->getTransactionId() != -1)
				{
					if (millis() - timeout_start_C >= TIMEOUT_EMERGENCY_RELAY_CLOSE_C)
					{
						Serial.println("[EVSE_C]-stopping");
						flag_evRequestsCharge_C = false;
						flag_evseStopTransaction_C = true;
						timeout_active_C = false;
						timer_initialize_C = false;
					}
				}
			}
		}
	}
}

/*
 * @param limit: expects current in amps from 0.0 to 32.0
 */
void EVSE_C_setChargingLimit(float limit)
{
	if (DEBUG_OUT)
		Serial.print(F("[EVSE] New charging limit set. Got "));
	if (DEBUG_OUT)
		Serial.print(limit);
	if (DEBUG_OUT)
		Serial.print(F("\n"));
	chargingLimit_C = limit;
}

bool EVSE_C_EvRequestsCharge()
{
	return flag_evRequestsCharge_C;
}

bool EVSE_C_EvIsPlugged()
{
	return evIsPlugged_C;
}

void EVSE_C_setOnBoot(OnBoot_C onBt_C)
{
	onBoot_C = onBt_C;
}

void EVSE_C_setOnReadUserId(OnReadUserId_C onReadUsrId_C)
{
	onReadUserId_C = onReadUsrId_C;
}

void EVSE_C_setOnsendHeartbeat(OnSendHeartbeat_C onSendHeartbt_C)
{
	onSendHeartbeat_C = onSendHeartbt_C;
}

void EVSE_C_setOnAuthentication(OnAuthentication_C onAuthenticatn_C)
{
	onAuthentication_C = onAuthenticatn_C;
}

void EVSE_C_setOnStartTransaction(OnStartTransaction_C onStartTransactn_C)
{
	onStartTransaction_C = onStartTransactn_C;
}

void EVSE_C_setOnStopTransaction(OnStopTransaction_C onStopTransactn_C)
{
	onStopTransaction_C = onStopTransactn_C;
}

void EVSE_C_setOnUnauthorizeUser(OnUnauthorizeUser_C onUnauthorizeUsr_C)
{
	onUnauthorizeUser_C = onUnauthorizeUsr_C;
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

String EVSE_C_getCurrnetIdTag(MFRC522 *mfrc522)
{
	String currentIdTag = "";
	// currentIdTag_C = EVSE_C_readRFID(mfrc522);

	if (getChargePointStatusService_C()->getIdTag().isEmpty() == false)
	{
		if (DEBUG_OUT)
			Serial.println("[EVSE_C] Reading from Charge Point Station Service ID Tag stored.");
		currentIdTag = getChargePointStatusService_C()->getIdTag();
		if (DEBUG_OUT)
			Serial.println("[EVSE_C] ID Tag: ");
		if (DEBUG_OUT)
			Serial.println(currentIdTag);
		Serial.flush();
	}

	return currentIdTag;
}

String EVSE_C_readRFID(MFRC522 *mfrc522)
{
	String currentIdTag_C;
	currentIdTag_C = readRfidTag(true, mfrc522);
	return currentIdTag_C;
}

/********Added new funtion @Wamique***********************/

// void EVSE_C_stopTransactionByRfid(){

// 	Ext_currentIdTag_C = EVSE_C_readRFID(&mfrc522);
// 	if(currentIdTag_C.equals(Ext_currentIdTag_C) == true){
// 		flag_evRequestsCharge_C = false;
// 		flag_evseStopTransaction_C = true;
// 	}else{
// 			if(Ext_currentIdTag_C.equals("") == false)
// 			if(DEBUG_OUT) Serial.println("\n[EVSE_C] Incorrect ID tag\n");
// 		}
// }

#if CP_ACTIVE
/**************CP Implementation @mwh*************/
void EVSE_C_StartCharging()
{

	if (getChargePointStatusService_C()->getEvDrawsEnergy() == false)
	{
		getChargePointStatusService_C()->startEvDrawsEnergy();
	}
	if (DEBUG_OUT)
		Serial.print(F("[EVSE_C] Opening Relays.\n"));
	//   pinMode(32,OUTPUT);
	//  digitalWrite(32, HIGH); //RELAY_1
	// digitalWrite(RELAY_2, RELAY_HIGH);
	requestForRelay(START, 3);
	requestLed(ORANGE, START, 3);
	delay(1200);
	requestLed(WHITE, START, 3);
	delay(1200);
	requestLed(GREEN, START, 3);
	delay(1000);
	Serial.println("[EVSE_C] EV is connected and Started charging");
	if (DEBUG_OUT)
		Serial.println("[EVSE_C] Started Drawing Energy");
	delay(500);
}

void EVSE_C_Suspended()
{

	if (getChargePointStatusService_C()->getEvDrawsEnergy() == true)
	{
		getChargePointStatusService_C()->stopEvDrawsEnergy();
	}
	requestLed(BLUE, START, 3); // replace 1 with connector ID
	requestForRelay(STOP, 3);
	//	delay(1000);
	Serial.printf("[EVSE_C] EV Suspended");
}

/**************************************************/

#endif

void displayMeterValuesC()
{
	if (millis() - timerDisplayC > 10000)
	{
		timerDisplayC = millis();
		/*float instantCurrrent_A = eic.GetLineCurrentA();
		float instantVoltage_A  = eic.GetLineVoltageA();
		float instantPower_A = 0.0f;

		if(instantCurrrent_A < minCurr){
			instantPower_A = 0;
		}else{
			instantPower_A = (instantCurrrent_A * instantVoltage_A)/1000.0;
		}

		float instantCurrrent_B = eic.GetLineCurrentB();
		int instantVoltage_B  = eic.GetLineVoltageB();
		float instantPower_B = 0.0f;

		if(instantCurrrent_B < minCurr){
			instantPower_B = 0;
		}else{
			instantPower_B = (instantCurrrent_B * instantVoltage_B)/1000.0;
		}*/

		float instantCurrrent_C = eic.GetLineCurrentC();
		int instantVoltage_C = eic.GetLineVoltageC();
		float instantPower_C = 0.0f;

		if (instantCurrrent_C < minCurr)
		{
			instantPower_C = 0;
		}
		else
		{
			instantPower_C = (instantCurrrent_C * instantVoltage_C) / 1000.0;
		}
		delay(100); // just for safety
#if LCD_ENABLED
		lcd.clear();
		lcd.setCursor(0, 0); // Or setting the cursor in the desired position.
		if (notFaulty_C)
		{
			lcd.print("*****CHARGING 3*****"); // You can make spaces using well... spaces
		}
		else
		{
			switch (fault_code_C)
			{
			case -1:
				break; // It means default.
			case 0:
				lcd.print("Connector3-Over Voltage");
				break;
			case 1:
				lcd.print("Connector3-Under Voltage");
				break;
			case 2:
				lcd.print("Connector3-Over Current");
				break;
			case 3:
				lcd.print("Connector3-Under Current");
				break;
			case 4:
				lcd.print("Connector3-Over Temp");
				break;
			case 5:
				lcd.print("Connector3-Over Temp");
				break;
			case 6:
				lcd.print("Connector3-GFCI"); // Not implemented in AC001
				break;
			case 7:
				lcd.print("Connector3-Earth Disc");
				break;
			default:
				lcd.print("*****FAULTED 3*****"); // You can make spaces using well... spacesbreak;
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
		lcd.print(String(instantVoltage_C));
		lcd.setCursor(0, 2);
		lcd.print("CURRENT(A):");
		lcd.setCursor(12, 2); // Or setting the cursor in the desired position.
		lcd.print(String(instantCurrrent_C));
		lcd.setCursor(0, 3);
		lcd.print("POWER(KW) :");
		lcd.setCursor(12, 3); // Or setting the cursor in the desired position.
		lcd.print(String(instantPower_C));
#endif

#if DWIN_ENABLED
		uint8_t err = 0;
		change_page[9] = 6;
		/*v1[6] = instantVoltage_A >> 8;
		v1[7] = instantVoltage_A & 0xff;
		v2[6] = instantVoltage_B >> 8;
		v2[7] = instantVoltage_B & 0xff;*/
		v3[4] = 0X7F;
		instantVoltage_C = instantVoltage_C * 10;
		v3[6] = instantVoltage_C >> 8;
		v3[7] = instantVoltage_C & 0xff;
		// i1[7] = instantCurrrent_A*10;
		// i2[7] = instantCurrrent_B*10;
		i3[4] = 0X82;
		i3[7] = instantCurrrent_C * 10;
		// e1[7] = instantPower_A*10;
		// e2[7] = instantPower_B*10;
		e3[4] = 0X84;
		e3[7] = instantPower_C * 10;
		err = DWIN_SET(cid3, sizeof(cid3) / sizeof(cid3[0]));
		// delay(50);
		err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0])); // page 0
		if (notFaulty_C)
		{
			charging[4] = 0X7B;
			err = DWIN_SET(charging, sizeof(charging) / sizeof(charging[0]));
		}
		// delay(50);
		// err = DWIN_SET(charging,sizeof(charging)/sizeof(charging[0]));
		// delay(50);
		// err = DWIN_SET(v1,sizeof(v1)/sizeof(v1[0]));
		// err = DWIN_SET(v2,sizeof(v2)/sizeof(v2[0]));
		err = DWIN_SET(v3, sizeof(v3) / sizeof(v3[0]));
		// err = DWIN_SET(i1,sizeof(i1)/sizeof(i1[0]));
		// err = DWIN_SET(i2,sizeof(i2)/sizeof(i2[0]));
		err = DWIN_SET(i3, sizeof(i3) / sizeof(i3[0]));
		// err = DWIN_SET(e1,sizeof(e1)/sizeof(e1[0]));
		// err = DWIN_SET(e2,sizeof(e2)/sizeof(e2[0]));
		err = DWIN_SET(e3, sizeof(e3) / sizeof(e3[0]));

		delay(500);

#endif
	}
}

/*************************************************EVSE C Reservation loop***********************************************************/
void EVSE_C_Reservation_loop()
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

	if (flag_evseReserveNow_C)
	{
		// Reserve Now execution.
		if (DEBUG_OUT)
			Serial.print("[EVSE_C] Reserve Now ...\n");

		// time_t start_reserve_time_delta_C = reservation_start_time_C - now();
		time_t reserve_time_delta_C = reserveDate_C - now();

		if ((reserve_time_delta_C <= reservedDuration_C) && (reserve_time_delta_C > 0))
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

			if (gu8_send_status_flag_C && reservation_start_flag_C)
			{
				gu8_send_status_flag_C = false;
				getChargePointStatusService_C()->setReserved(true);
			}

			if (!reservation_start_flag_C)
			{

				if (getChargePointStatusService_C()->getTransactionId() != -1)
				{
					EVSE_C_StopSession();
				}
				// requestLed(BLUE, START, 3);
				reservation_start_flag_C = true;
				flag_evseReadIdTag_C = true; // Entry condition for reading ID Tag.
				flag_evseAuthenticate_C = false;
				gu8_send_status_flag_C = true;
				if (DEBUG_OUT)
					Serial.print("[EVSE_C] Reserve Now  2 1\n");
			}
			if (DEBUG_OUT)
				Serial.print("[EVSE_C] Reserve Now  2\n");
			Serial.print("[EVSE_C] reserve time delta ");
			Serial.println(reserve_time_delta_C);
			if (getChargePointStatusService_C()->getEvDrawsEnergy() == false && notFaulty_C && getChargePointStatusService_C()->getEmergencyRelayClose() == false)
			{
				// requestLed(BLUE, START, 3);
			}
		}
		else
		{
			if ((reserve_time_delta_C <= 0))
			{
				flag_evseReserveNow_C = false;
				getChargePointStatusService_C()->setReserved(false);

#if 0
				/*
				* @bug : Session should not stop just because the reservation expired.
				Resolved by G. Raja Sumant 20/07/2022
				if (getChargePointStatusService_C()->getTransactionId() != -1)
				{
					EVSE_StopSession();
				}*/
#endif

				if (DEBUG_OUT)
					Serial.print("[EVSE] stopped due to Reservation timeout.!\n");
				reservation_start_flag_C = false;
			}
		}

		if (flag_evseCancelReservation_C)
		{
			flag_evseCancelReservation_C = false;
			flag_evseReserveNow_C = false;
			getChargePointStatusService_C()->setReserved(false);
			// if(getChargePointStatusService_C()->getTransactionId() != -1)
			// {
			// 	EVSE_StopSession();
			// }
			if (DEBUG_OUT)
				Serial.print("[EVSE_C]stopped due to Cancel Reservation.!\n");
			reservation_start_flag_C = false;
		}
	}
}
/*************************************************EVSE C Reservation loop***********************************************************/