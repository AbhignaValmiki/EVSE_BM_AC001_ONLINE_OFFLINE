// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#include "Variants.h"

#include "StartTransaction.h"
#include "TimeHelper.h"
#include "OcppEngine.h"
#include "MeteringService.h"

int globalmeterstartA = 0;
unsigned long st_timeA = 0;
int globalmeterstartB = 0;
unsigned long st_timeB = 0;
int globalmeterstartC = 0;
unsigned long st_timeC = 0;


/*
* @brief: These flags are for display to show only one stop txn.
*/
bool flag_start_txn_A = false;
bool flag_start_txn_B = false;
bool flag_start_txn_C = false;

bool flag_send_stop_A = false;
bool flag_send_stop_B = false;
bool flag_send_stop_C = false;

extern int gs32_offlinestarttxnId;
extern int gs32_offlinestartStoptxn_update_flag;
extern int gs32_offlinestarttxn_update_flag;

extern String gc_offline_txn_startDate;
extern String gc_offline_txn_start_metervalue;
extern String gc_offline_txn_stopDate;
extern String gc_offline_txn_stop_metervalue;
extern String gc_offline_txn_reason;


StartTransaction::StartTransaction() {
	/*if (getChargePointStatusService() != NULL) {
		if (!getChargePointStatusService()->getIdTag().isEmpty()) {
			idTag = String(getChargePointStatusService()->getIdTag());
			connectorId = getChargePointStatusService()->getConnectorId();
		}
	}*/
	//if (idTag.isEmpty()) idTag = String("wrongIDTag"); //Use a default payload. In the typical use case of this library, you probably you don't even need Authorization at all
}

StartTransaction::StartTransaction(String &idTag) {
	this->idTag = String(idTag);
}

StartTransaction::StartTransaction(String &idTag, int &connectorId) {
	this->idTag = String(idTag);
	this->connectorId = connectorId;
}


const char* StartTransaction::getOcppOperationType(){
	return "StartTransaction";
}

DynamicJsonDocument* StartTransaction::createReq() {
	DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(5) + (JSONDATE_LENGTH + 1) + (idTag.length() + 1));
	JsonObject payload = doc->to<JsonObject>();

	payload["connectorId"] = connectorId;
	MeteringService* meteringService = getMeteringService();
	Serial.println("gs32_offlinestarttxn_update_flag : " +String(gs32_offlinestarttxn_update_flag));
	Serial.println("gc_offline_txn_startDate : " +String(gc_offline_txn_startDate));
	Serial.println("gc_offline_txn_start_metervalue : " +String(gc_offline_txn_start_metervalue));
	

	if (meteringService != NULL) {
		if(connectorId == 1){
			//payload["meterStart"] = (int) meteringService->currentEnergy_A();
			if(gs32_offlinestarttxn_update_flag == 1)
			{
				payload["meterStart"] = gc_offline_txn_start_metervalue.toInt();	
								
			}
			else
			{
				payload["meterStart"] = (int) meteringService->currentEnergy_A();
			}
			globalmeterstartA = payload["meterStart"];
			flag_start_txn_A = true;
			flag_send_stop_A = true;
		st_timeA = millis();
		}else if(connectorId ==2){
			// payload["meterStart"] = (int) meteringService->currentEnergy_B();
			if(gs32_offlinestarttxn_update_flag == 1)
			{
				payload["meterStart"] = gc_offline_txn_start_metervalue.toInt();					
			}
			else
			{
				payload["meterStart"] = (int) meteringService->currentEnergy_B();
			}
			globalmeterstartB = payload["meterStart"];
			flag_start_txn_B = true;
			flag_send_stop_B = true;
		st_timeB = millis();
		}else if(connectorId == 3){
			// payload["meterStart"] = (int) meteringService->currentEnergy_C();
			if(gs32_offlinestarttxn_update_flag == 1)
			{
				payload["meterStart"] = gc_offline_txn_start_metervalue.toInt();					
			}
			else
			{
				payload["meterStart"] = (int) meteringService->currentEnergy_C();
			}
			globalmeterstartC = payload["meterStart"];
			flag_start_txn_C = true;
			flag_send_stop_C = true;
		st_timeC = millis();
		}
	}

	Serial.println("gc_offline_txn_start_metervalue.toInt() : " +String(gc_offline_txn_start_metervalue.toInt()));
	char timestamp[JSONDATE_LENGTH + 1] = {'\0'};

	// getJsonDateStringFromGivenTime(timestamp, JSONDATE_LENGTH + 1, now());
    
	if(gs32_offlinestarttxn_update_flag == 1)
	{
		time_t given_time = gc_offline_txn_startDate.toInt();
		Serial.println("gc_offline_txn_startDate.toInt() : " +String(gc_offline_txn_startDate.toInt()));
		getJsonDateStringFromGivenUnixTime(timestamp, JSONDATE_LENGTH + 1, given_time);					
	}
	else
	{
		getJsonDateStringFromGivenTime(timestamp, JSONDATE_LENGTH + 1, now());
	}

    // getJsonDateStringFromGivenUnixTime(, JSONDATE_LENGTH + 1, given_time);

	payload["timestamp"] = timestamp;
	payload["idTag"] = idTag;

	return doc;
}

void StartTransaction::processConf(JsonObject payload) {

	const char* idTagInfoStatus = payload["idTagInfo"]["status"] | "Invalid";
	int transactionId = payload["transactionId"] | -1;
	if(gs32_offlinestartStoptxn_update_flag == 1)
	{
		gs32_offlinestarttxnId = transactionId;
		Serial.print("[StartTransaction] gs32_offlinestarttxnId Updated..!\n");
	}

	

	//EEPROM.begin(sizeof(EEPROM_Data));
	//EEPROM.put(68, transactionId);
	//EEPROM.commit();
	//EEPROM.end();

	if (!strcmp(idTagInfoStatus, "Accepted")) 
	{
		if (DEBUG_OUT) Serial.print("[StartTxn] accepted!\n");

		// ChargePointStatusService *cpStatusService = getChargePointStatusService();
		// if (cpStatusService != NULL){
		// 	cpStatusService->startTransaction(transactionId);
		// 	cpStatusService->startEnergyOffer();
		// }
		gs32_offlinestarttxn_update_flag = 0;
		SmartChargingService *scService = getSmartChargingService();
		if (scService != NULL){
			scService->beginChargingNow();
		}

	} else {
		Serial.print("[StartTxn]-denied!\n");
	}
}


void StartTransaction::processReq(JsonObject payload) {

	/**
	* Ignore Contents of this Req-message, because this is for debug purposes only
	*/

}

DynamicJsonDocument* StartTransaction::createConf(){
	DynamicJsonDocument* doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2));
	JsonObject payload = doc->to<JsonObject>();

	JsonObject idTagInfo = payload.createNestedObject("idTagInfo");
	idTagInfo["status"] = "Accepted";
	payload["transactionId"] = 123456; //sample data for debug purpose

	return doc;
}
