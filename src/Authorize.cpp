// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#include "Authorize.h"
#include "OcppEngine.h"
#include "Variants.h"

extern uint8_t gu8_fetch_offline_txn_flag;
extern uint8_t gu8_clear_offline_txn_flag;
extern int gs32_offlineAuthorizeIdTag;
extern int gs32_offlinestartStoptxn_update_flag;
extern volatile int gs32_fetch_offline_txnlist_count;
extern volatile int gs32_fetch_offline_txn_count;
extern int gs32_offlinestartStoptxn_update_guard_Interval;

#if DISPLAY_ENABLED
extern bool flag_freeze;
#endif

Authorize::Authorize()
{
	idTag = String("defaultCPIDTag"); // Use a default payload. In the typical use case of this library, you probably you don't even need Authorization at all
}

Authorize::Authorize(String &idTag)
{
	this->idTag = String(idTag);
	//	EEPROM.begin(sizeof(EEPROM_Data));
	//	EEPROM.put(28,idTag);
	//	EEPROM.commit();
	//	EEPROM.end();
}

const char *Authorize::getOcppOperationType()
{
	return "Authorize";
}

DynamicJsonDocument *Authorize::createReq()
{
	DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1) + (idTag.length() + 1));
	JsonObject payload = doc->to<JsonObject>();
	payload["idTag"] = idTag;
	return doc;
}

void Authorize::processConf(JsonObject payload)
{
	String idTagInfo = payload["idTagInfo"]["status"] | "Invalid";

	if (idTagInfo.equals("Accepted"))
	{
		if (DEBUG_OUT)
			Serial.println("[Authorize] Request has been accepted!\n");

		// ChargePointStatusService *cpStatusService = getChargePointStatusService();
		// if (cpStatusService != NULL){
		// 	cpStatusService->authorize();
		// }
		if (gs32_offlinestartStoptxn_update_flag == 1)
		{
			gs32_offlineAuthorizeIdTag = 1;
		}
	}
	else
	{
		Serial.println("[Authorize] Request has been denied!");
#if DISPLAY_ENABLED
		flag_freeze = false;
#endif
		if (gs32_offlinestartStoptxn_update_flag == 1)
		{
			gs32_offlineAuthorizeIdTag = 0;

			gs32_offlinestartStoptxn_update_flag = 0;

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
		}
	}
	// Serial.println("[Authorize] gs32_offlineAuthorizeIdTag : " + String(gs32_offlineAuthorizeIdTag));
}

void Authorize::processReq(JsonObject payload)
{
	/*
	 * Ignore Contents of this Req-message, because this is for debug purposes only
	 */
}

DynamicJsonDocument *Authorize::createConf()
{
	DynamicJsonDocument *doc = new DynamicJsonDocument(2 * JSON_OBJECT_SIZE(1));
	JsonObject payload = doc->to<JsonObject>();
	JsonObject idTagInfo = payload.createNestedObject("idTagInfo");
	idTagInfo["status"] = "Accepted";
	return doc;
}
