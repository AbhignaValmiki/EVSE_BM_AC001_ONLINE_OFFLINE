// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#include "Variants.h"

#include "RemoteStartTransaction.h"
#include "OcppEngine.h"
#include "display_meterValues.h"
extern bool flag_freeze;
extern bool flag_unfreeze;

extern String reserve_currentIdTag_A;
extern String reserve_currentIdTag_B;
extern String reserve_currentIdTag_C;

extern bool reservation_start_flag_A;
extern bool reservation_start_flag_B;
extern bool reservation_start_flag_C;

extern bool flag_evseReadIdTag_A;
extern bool flag_evseAuthenticate_A;
extern bool flag_evseStartTransaction_A; // Entry condition for starting transaction.
extern bool flag_evRequestsCharge_A;
extern bool flag_evseStopTransaction_A;
extern bool flag_evseUnauthorise_A;

extern bool flag_evseReadIdTag_B;
extern bool flag_evseAuthenticate_B;
extern bool flag_evseStartTransaction_B; // Entry condition for starting transaction.
extern bool flag_evRequestsCharge_B;
extern bool flag_evseStopTransaction_B;
extern bool flag_evseUnauthorise_B;

extern bool flag_evseReadIdTag_C;
extern bool flag_evseAuthenticate_C;
extern bool flag_evseStartTransaction_C; // Entry condition for starting transaction.
extern bool flag_evRequestsCharge_C;
extern bool flag_evseStopTransaction_C;
extern bool flag_evseUnauthorise_C;

extern bool flag_AuthorizeRemoteTxRequests;

extern uint8_t start_session_A;
extern uint8_t start_session_B;
extern uint8_t start_session_C;

extern uint8_t  Stop_Session_A;
extern uint8_t  Stop_Session_B;
extern uint8_t  Stop_Session_C;

uint8_t gu8_Remote_start_A = 0;
uint8_t gu8_Remote_start_B = 0;
uint8_t gu8_Remote_start_C = 0;


RemoteStartTransaction::RemoteStartTransaction()
{
}

const char *RemoteStartTransaction::getOcppOperationType()
{
	return "RemoteStartTransaction";
}

void RemoteStartTransaction::processReq(JsonObject payload)
{
	idTag = String(payload["idTag"].as<String>());
	connectorId = payload["connectorId"].as<int>();
	Serial.println("Connector ID: " + String(connectorId));
	// Serial.println(String(getChargePointStatusService_A()->inferenceStatus());
	if (connectorId == 1 && (getChargePointStatusService_A()->inferenceStatus() == ChargePointStatus::Available || getChargePointStatusService_A()->inferenceStatus() == ChargePointStatus::Reserved))
	{
#if DISPLAY_ENABLED
		flag_freeze = true;
#endif
		Serial.println("[RemoteStart]-1");
		if (reservation_start_flag_A)
		{
			if (idTag.equals(reserve_currentIdTag_A))
			{
				getChargePointStatusService_A()->authorize(idTag, connectorId);
				accepted = true;
			}
			else
				accepted = false;
		}
		else
		{
			getChargePointStatusService_A()->authorize(idTag, connectorId);
			accepted = true;
			start_session_A = 0;
			Display_Set(start_session_A);
			gu8_Remote_start_A = 1;
		}
	}
	else if (connectorId == 2 && (getChargePointStatusService_B()->inferenceStatus() == ChargePointStatus::Available || getChargePointStatusService_B()->inferenceStatus() == ChargePointStatus::Reserved))
	{
#if DISPLAY_ENABLED
		flag_freeze = true;
#endif
		if (reservation_start_flag_B)
		{
			if (idTag.equals(reserve_currentIdTag_B))
			{
				getChargePointStatusService_B()->authorize(idTag, connectorId);
				accepted = true;
			}
			else
				accepted = false;
		}
		else
		{
			getChargePointStatusService_B()->authorize(idTag, connectorId);
			accepted = true;
			start_session_B = 1;
			Display_Set(start_session_B);
			gu8_Remote_start_B = 1;
		}
	}
	else if (connectorId == 3 && (getChargePointStatusService_C()->inferenceStatus() == ChargePointStatus::Available || getChargePointStatusService_C()->inferenceStatus() == ChargePointStatus::Reserved))
	{
#if DISPLAY_ENABLED
		flag_freeze = true;
#endif

		if (reservation_start_flag_C)
		{
			if (idTag.equals(reserve_currentIdTag_C))
			{
				getChargePointStatusService_C()->authorize(idTag, connectorId);
				accepted = true;
			}
			else
				accepted = false;
		}
		else
		{
			getChargePointStatusService_C()->authorize(idTag, connectorId);
			accepted = true;
			start_session_C = 2;
			Display_Set(start_session_C);
			gu8_Remote_start_C = 1;
		}
	}
	else
	{
#if DISPLAY_ENABLED
		flag_freeze = false;
		flag_unfreeze = true;
#endif
		Serial.println("Unable to start txn Connector is busy");
		accepted = false;
		switch(connectorId)
      {
        case 1:
         Stop_Session_A = 0;
		Display_Set(Stop_Session_A);
        break;
        case 2:
         Stop_Session_B = 1;
		Display_Set(Stop_Session_B);
        break;
        case 3:
                 Stop_Session_C = 2;
		Display_Set(Stop_Session_C);
	
        break;
      }
	}
	/*
	if(getChargePointStatusService_A()->transactionId == -1){
		if(getChargePointStatusService_B()->connectorId != connectorId && getChargePointStatusService_C()->connectorId != connectorId){
			getChargePointStatusService_A()->authorize(idTag,connectorId);    //authorizing twice needed to be improvise
		}else{
			Serial.println("[A] Unable to start txn as Connector is busy");
		}
	}else if(getChargePointStatusService_B->transactionId == -1){
		if(getChargePointStatusService_A()->connectorId != connectorId && getChargePointStatusService_C()->connectorId != connectorId){
			getChargePointStatusService_B()->authorize(idTag,connectorId);    //authorizing twice needed to be improvise
		}else{
			Serial.println("[B] Unable to start txn as Connector is busy");
		}
	}else if(getChargePointStatusService_C()->transactionId == -1){
		if(getChargePointStatusService_A()->connectorId != connectorId && getChargePointStatusService_B()->connectorId != connectorId){
			getChargePointStatusService_C()->authorize(idTag,connectorId);    //authorizing twice needed to be improvise
		}else{
			Serial.println("[B] Unable to start txn as Connector is busy");
		}
	}else{
		Serial.println("ALL connectors are busy");
	}*/
	//	getChargePointStatusService()->authorize(idTag, connectorId);
}

DynamicJsonDocument *RemoteStartTransaction::createConf()
{
	DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1));
	JsonObject payload = doc->to<JsonObject>();
	if (accepted)
	{
		payload["status"] = "Accepted";
		// if (!flag_AuthorizeRemoteTxRequests)
		// {
			switch (connectorId)
			{
			case 1:
				flag_evseReadIdTag_A = false;
				flag_evseAuthenticate_A = false;
				flag_evseStartTransaction_A = true; // Entry condition for starting transaction.
				flag_evRequestsCharge_A = false;
				flag_evseStopTransaction_A = false;
				flag_evseUnauthorise_A = false;
				break;
			case 2:
				flag_evseReadIdTag_B = false;
				flag_evseAuthenticate_B = false;
				flag_evseStartTransaction_B = true; // Entry condition for starting transaction.
				flag_evRequestsCharge_B = false;
				flag_evseStopTransaction_B = false;
				flag_evseUnauthorise_B = false;
				break;
			case 3:
				flag_evseReadIdTag_C = false;
				flag_evseAuthenticate_C = false;
				flag_evseStartTransaction_C = true; // Entry condition for starting transaction.
				flag_evRequestsCharge_C = false;
				flag_evseStopTransaction_C = false;
				flag_evseUnauthorise_C = false;
				break;
			}
		// }
	}
	else
		payload["status"] = "Rejected";
	return doc;
}

DynamicJsonDocument *RemoteStartTransaction::createReq()
{
	DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1));
	JsonObject payload = doc->to<JsonObject>();

	payload["idTag"] = "fefed1d19876";
	payload["connectorId"] = 1;

	return doc;
}

void RemoteStartTransaction::processConf(JsonObject payload)
{
	String status = payload["status"] | "Invalid";

	if (status.equals("Accepted"))
	{
		if (DEBUG_OUT)
			Serial.print("[RemoteStartTxn]accepted!\n");
	}
	else
	{
		Serial.print("[RemoteStartTxn] denied!");
	}
}
