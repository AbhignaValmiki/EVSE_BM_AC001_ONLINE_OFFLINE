// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#include "GetLocalListVersion.h"
#include "OcppEngine.h"
#include "Variants.h"
#include "Master.h"

GetLocalListVersion::GetLocalListVersion() {
	
}


const char* GetLocalListVersion::getOcppOperationType(){
	return "GetLocalListVersion";
}

/*DynamicJsonDocument* GetLocalListVersion::createReq() {
	DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1));
	return doc;
}*/

/*void GetLocalListVersion::processConf(JsonObject payload){
	int listVersion = payload["listVersion"];
	this->localListVersionNumber = listVersion;
}*/

void GetLocalListVersion::processReq(JsonObject payload){
	/*int listVersion = payload["listVersion"];
	this->localListVersionNumber = listVersion;*/
	
	/*
	* Ignore Contents of this Req-message, because this is for debug purposes only
	*/
	
	/*
	 * @brief Get Local List Version request from CMS
	 */ 
	Serial.println(F("[GetLocalListVersion]*******Received GetLocalListVersion Request*******"));
    
	requestGetLocalListVersion();	

}

DynamicJsonDocument* GetLocalListVersion::createConf(){
	DynamicJsonDocument* doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1));
	JsonObject payload = doc->to<JsonObject>();
	payload["listVersion"] = gu16_locallist_ver;
	return doc;
}
