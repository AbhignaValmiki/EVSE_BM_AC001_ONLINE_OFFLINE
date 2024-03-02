// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#include "SendLocalList.h"
#include "OcppEngine.h"
#include "Variants.h"
#include "Master.h"

bool send_ll_status = false;

SendLocalList::SendLocalList() {
	
}


const char* SendLocalList::getOcppOperationType(){
	return "SendLocalList";
}

DynamicJsonDocument* SendLocalList::createReq() {
	
}

void SendLocalList::processConf(JsonObject payload){
	
	
	
}

void SendLocalList::processReq(JsonObject payload){
	
	//listVersion = payload["listVersion"];
	//localAuthorizationList[100] = payload["localAuthorizationList"];
	send_ll_status = requestSendLocalList(payload);
}

DynamicJsonDocument* SendLocalList::createConf(){
	DynamicJsonDocument* doc = new DynamicJsonDocument(2 * JSON_OBJECT_SIZE(1));
	JsonObject payload = doc->to<JsonObject>();
	payload["status"] = "Accepted";
	//JsonObject idTagInfo = payload.createNestedObject("idTagInfo");
	//idTagInfo["status"] = "Accepted";
	int ret = send_ll_status;
    if(!ret)
	{
		payload["status"] = "Accepted";
	}
	else
	{
		
		payload["status"] = "Rejected";
	}

	return doc;
}
