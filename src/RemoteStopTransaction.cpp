// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#include "Variants.h"

#include "RemoteStopTransaction.h"
#include "OcppEngine.h"
#include "display_meterValues.h"

extern bool flag_evseReadIdTag_A;
extern bool flag_evseAuthenticate_A;
extern bool flag_evseStartTransaction_A;
extern bool flag_evRequestsCharge_A;
extern bool flag_evseStopTransaction_A;
extern bool flag_evseUnauthorise_A;

extern bool flag_evseReadIdTag_B;
extern bool flag_evseAuthenticate_B;
extern bool flag_evseStartTransaction_B;
extern bool flag_evRequestsCharge_B;
extern bool flag_evseStopTransaction_B;
extern bool flag_evseUnauthorise_B;

extern bool flag_evseReadIdTag_C;
extern bool flag_evseAuthenticate_C;
extern bool flag_evseStartTransaction_C;
extern bool flag_evRequestsCharge_C;
extern bool flag_evseStopTransaction_C;
extern bool flag_evseUnauthorise_C;

extern uint8_t reasonForStop_A;
extern uint8_t reasonForStop_B;
extern uint8_t reasonForStop_C;

extern bool disp_evse_A;
extern bool disp_evse_B;
extern bool disp_evse_C;

extern uint8_t Stop_Session_A;
extern uint8_t Stop_Session_B;
extern uint8_t Stop_Session_C;

extern uint8_t gu8_Remote_start_A;
extern uint8_t gu8_Remote_start_B;
extern uint8_t gu8_Remote_start_C;

RemoteStopTransaction::RemoteStopTransaction() {

}

const char* RemoteStopTransaction::getOcppOperationType(){
	return "RemoteStopTransaction";
}


// sample message: [2,"9f639cdf-8a81-406c-a77e-60dff3cb93eb","RemoteStopTransaction",{"transactionId":2042}]
void RemoteStopTransaction::processReq(JsonObject payload) {
	transactionId = payload["transactionId"];
	if(transactionId == getChargePointStatusService_A()-> getTransactionId()){
		reasonForStop_A = 7;
		disp_evse_A = false;
		  Stop_Session_A = 0;
		  Display_Clear(Stop_Session_A);
		gu8_Remote_start_A = 0;

		flag_evseReadIdTag_A = false;
		flag_evseAuthenticate_A = false;
		flag_evseStartTransaction_A = false;
		flag_evRequestsCharge_A = false;
		flag_evseStopTransaction_A = true;
		flag_evseUnauthorise_A = false;
	}else if(transactionId == getChargePointStatusService_B()->getTransactionId()){
		reasonForStop_B = 7;
		disp_evse_B = false;
		 gu8_Remote_start_B = 0;
		 Stop_Session_B = 1;
		 Display_Clear(Stop_Session_B);
		flag_evseReadIdTag_B = false;
		flag_evseAuthenticate_B = false;
		flag_evseStartTransaction_B = false;
		flag_evRequestsCharge_B = false;
		flag_evseStopTransaction_B = true;
		flag_evseUnauthorise_B = false;
	}else if(transactionId == getChargePointStatusService_C()->getTransactionId()){
		reasonForStop_C = 7;
		disp_evse_C = false;
		   gu8_Remote_start_C = 0;
		Stop_Session_C = 2;
		Display_Clear(Stop_Session_C);
		flag_evseReadIdTag_C = false;
		flag_evseAuthenticate_C = false;
		flag_evseStartTransaction_C = false;
		flag_evRequestsCharge_C = false;
		flag_evseStopTransaction_C = true;
		flag_evseUnauthorise_C = false;
	}else{
		Serial.println("Transaction ID is incorrect");
		Serial.println("Running Transaction Id: ");
		Serial.println(String(getChargePointStatusService_A()->getTransactionId()));
		Serial.println(String(getChargePointStatusService_B()->getTransactionId()));
		Serial.println(String(getChargePointStatusService_C()->getTransactionId()));
	}

/*	if (transactionId == getChargePointStatusService()->getTransactionId()){
		flag_evseReadIdTag = false;
		flag_evseAuthenticate = false;
		flag_evseStartTransaction = false;
		flag_evRequestsCharge = false;
		flag_evseStopTransaction = true;
		flag_evseUnauthorise = false;
		//getChargePointStatusService()->stopTransaction();
	}*/
	
}

DynamicJsonDocument* RemoteStopTransaction::createConf(){
	DynamicJsonDocument* doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1));
	JsonObject payload = doc->to<JsonObject>();
	payload["status"] = "Accepted";
	return doc;
}

DynamicJsonDocument* RemoteStopTransaction::createReq() {
	DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1));
	JsonObject payload = doc->to<JsonObject>();

	payload["idTag"] = "fefed1d19876";

	return doc;
}

void RemoteStopTransaction::processConf(JsonObject payload){
	String status = payload["status"] | "Invalid";

	if (status.equals("Accepted")) {
		if (DEBUG_OUT) Serial.print("[RemoteStopTxn]accepted!\n");
		} else {
			Serial.print("[RemoteStopTxn] denied!");
	}
}
