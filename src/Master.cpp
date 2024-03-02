#include "Master.h"
#include "Variants.h"
#include <Preferences.h>
#include "OcppEngine.h"
SoftwareSerial masterSerial(25,33); //25 Rx, 33 Tx
//StaticJsonDocument<250> txM_doc;
//StaticJsonDocument<250> rxM_doc;
DynamicJsonDocument txM_doc(600);
DynamicJsonDocument rxM_doc(600);

DynamicJsonDocument get_txn_doc(600);

DynamicJsonDocument txA_doc(200);
DynamicJsonDocument rxA_doc(200);

extern String currentIdTag;
extern String expiry_time;

bool flag_GFCI_set_here = false;

extern int readConnectorVal;

extern bool flag_noVehicle;
bool Offline_idTag_is_valid = false;
bool Offline_idTagStatus_is_valid = false;
uint8_t gu8_lla_updateType_is_valid = false;

uint16_t gu16_locallist_ver = 0;
bool authCache_idTag_is_valid = false;
bool ClearCache_status = false;

bool SendStartTxn_status = false;
bool SendStopTxn_status = false;
bool GetTxn_status = false;
bool ClearTxn_status = false;
bool GetTxnList_status = false;
bool SetTxn_status = false;
bool ClearTxnList_status = false;

String Tag_Id_A = "1111abcd";
String Transaction_Id_A = "111111";
String Start_Meter_Value_A = "1000";

String Tag_Id_B = "2222abcd";
String Transaction_Id_B = "222222";
String Start_Meter_Value_B = "2000";

String Tag_Id_C = "3333abcd";
String Transaction_Id_C = "333333";
String Start_Meter_Value_C = "3000";

int Sendstart_time_A;
int Sendstop_time_A;
int Sendstart_time_B;
int Sendstop_time_B;
int Sendstart_time_C;
int Sendstop_time_C;

extern int globalmeterstartA;
extern int globalmeterstartB;
extern int globalmeterstartC;

extern uint8_t reasonForStop_A; // Local is the default value
extern uint8_t reasonForStop_B; // Local is the default value
extern uint8_t reasonForStop_C; // Local is the default value

extern int8_t button;

extern Preferences resumeTxn_A;
extern Preferences resumeTxn_B;
extern Preferences resumeTxn_C;

extern bool webSocketConncted;

void customflush(){
  while(masterSerial.available() > 0)
    masterSerial.read();
}

#if 0
/*
  @brief : Read the touch display
*/
#if DWIN_ENABLED
int8_t dwin_input()
{

  button = DWIN_read();
  Serial.printf("Button pressed : %d", button);
  //delay(50);
  return button;

}
#endif
#endif 

int requestConnectorStatus(){
  txM_doc.clear();
  rxM_doc.clear();

  
  const char* type = "";
  int connectorId = 0;
  int startTime = 0;
  bool success = false;
  
  Serial.println("Master: ");
  txM_doc["type"] = "request";
  txM_doc["object"] = "connector";
  
  serializeJson(txM_doc,masterSerial);     //data send to slave   
  serializeJson(txM_doc, Serial);
  delay(100);
  
  startTime = millis();
  while(millis() - startTime < 20000/*WAIT_TIMEOUT*/){ // waiting for response from slave
  
    if(masterSerial.available()){
      
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code()) {
        case DeserializationError::Ok:
          success = true;
          break;
        case DeserializationError::InvalidInput:
          Serial.print(F(" Invalid input! Not a JSON\n"));
          break;
        case DeserializationError::NoMemory:
          Serial.print(F("Error: Not enough memory\n"));
          break;
        default:
          Serial.print(F("Deserialization failed\n"));
          break;
     }

     if (!success) {
      //propably the payload just wasn't a JSON message
      rxM_doc.clear();
      delay(200);
      customflush();
      Serial.println("***Sending request again***");
      delay(100);
      serializeJson(txM_doc, masterSerial);     // data send to slave again
      serializeJson(txM_doc, Serial);
      //return false;
    }else{
      type = rxM_doc["type"] | "Invalid";
      connectorId = rxM_doc["connectorId"];
      if(strcmp(type , "response") == 0){
        Serial.println("Received connectorId from slave--->" + String(connectorId));
        return connectorId;
      }
    }
   }
   
 }
  Serial.println(F("No response from Slave"));
  return connectorId;  //returns 0 if connectorId (push button is not pressed)
  
}


bool requestForRelay(int action , int connectorId){
  
  if(connectorId == 0 || connectorId > 3){
    return false;
  }

  // DynamicJsonDocument txM_doc(200);
  // DynamicJsonDocument rxM_doc(200);

  txM_doc.clear();
  txM_doc.clear();

  const char* type = "";
  const char* object = "";
  
  //const char* statusRelay = "";
  int statusRelay=0;
  bool success = false;
  int startTime = 0;
  
  Serial.println("Relay:");
  txM_doc["type"] = "request";
  txM_doc["object"] = "relay";
  
  txM_doc["action"] = action;
  txM_doc["connectorId"] = connectorId;

  serializeJson(txM_doc,masterSerial);     //data send to slave   
  serializeJson(txM_doc, Serial);
  delay(2000);

  startTime = millis();
  while(millis() - startTime < 6000){ // waiting for response from slave
    if(masterSerial.available()){ 
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code()) {
        case DeserializationError::Ok:
          success = true;
          break;
        case DeserializationError::InvalidInput:
          Serial.print(F(" Invalid input! Not a JSON\n"));
          break;
        case DeserializationError::NoMemory:
          Serial.print(F("Error: Not enough memory\n"));
          break;
        default:
          Serial.print(F("Deserialization failed\n"));
          break;
      }

     if (!success) {
      //propably the payload just wasn't a JSON message
      rxM_doc.clear();
      delay(200);
      customflush();
      Serial.println("***Sending request again***");
      delay(100);
      serializeJson(txM_doc, masterSerial);     // data send to slave again
      //return false;
    }else{
      type = rxM_doc["type"] | "Invalid";
      statusRelay = rxM_doc["status"];
      connectorId = rxM_doc["connectorId"];
      if((strcmp(type , "response") == 0) && (action == statusRelay)){
        Serial.println("Received Status--->" +String(statusRelay)+" for ConnectorId: "+ String(connectorId));
        return true;
      }
    }
   }//end of .available 
  }//end of while loop
  Serial.println(F("No response from Slave"));
  return false;
}

bool requestLed(int colour, int action , int connectorId){
  
 /*if(connectorId == 0 || connectorId > 4){
    return false;
  }*/

  if(connectorId > 4){
    return false;
  }

  // DynamicJsonDocument txM_doc(200);
  // DynamicJsonDocument rxM_doc(200);
  txM_doc.clear();
  rxM_doc.clear();

  const char* type = "";
  const char* object = "";
  
  //const char* statusRelay = "";
  int statusLed=0;
  bool success = false;
  int startTime = 0;
  
  Serial.println("Led:");
  txM_doc["type"] = "request";
  if(connectorId == 4){
    txM_doc["object"] = "rfid";
  }else{
    txM_doc["object"] = "led";
  }
  txM_doc["colour"] = colour;
  txM_doc["action"] = action;
  txM_doc["connectorId"] = connectorId;

  serializeJson(txM_doc,masterSerial);     //data send to slave   
  serializeJson(txM_doc, Serial);
  delay(100);

  startTime = millis();
  while(millis() - startTime < WAIT_TIMEOUT){ // waiting for response from slave
    if(masterSerial.available()){ 
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code()) {
        case DeserializationError::Ok:
          success = true;
          break;
        case DeserializationError::InvalidInput:
          Serial.print(F(" Invalid input! Not a JSON\n"));
          break;
        case DeserializationError::NoMemory:
          Serial.print(F("Error: Not enough memory\n"));
          break;
        default:
          Serial.print(F("Deserialization failed\n"));
          break;
      }

     if (!success) {
      //propably the payload just wasn't a JSON message
      rxM_doc.clear();
      delay(200);
      customflush();
      Serial.println("***Sending request again***");
      delay(100);
      serializeJson(txM_doc, masterSerial);     // data send to slave again
      //return false;
    }else{
      type = rxM_doc["type"] | "Invalid";
      connectorId = rxM_doc["connectorId"];
      statusLed = rxM_doc["status"];
      if((strcmp(type , "response") == 0) && (action == statusLed)){
        Serial.println("Received Status--->" +String(statusLed)+" for ConnectorId: "+ String(connectorId));
        return true;
      }
    }
   }//end of .available 
  }//end of while loop
  Serial.println(F("TimeOut"));
  return false;
}

bool requestEmgyStatus(){

  // DynamicJsonDocument txM_doc(100);
  // DynamicJsonDocument rxM_doc(100);
  txM_doc.clear();
  rxM_doc.clear();

  const char* type = "";
  const char* object = "";
  bool success = false;
  int startTime = 0;
  bool statusE = 0;
  Serial.println("EMGY:");
  txM_doc["type"] = "request";
  txM_doc["object"] = "emgy";

  serializeJson(txM_doc,masterSerial);     //data send to slave   
  serializeJson(txM_doc, Serial);
  delay(100);

  startTime = millis();
  while(millis() - startTime < WAIT_TIMEOUT){ // waiting for response from slave
    if(masterSerial.available()){ 
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code()) {
        case DeserializationError::Ok:
          success = true;
          break;
        case DeserializationError::InvalidInput:
          Serial.print(F(" Invalid input! Not a JSON\n"));
          break;
        case DeserializationError::NoMemory:
          Serial.print(F("Error: Not enough memory\n"));
          break;
        default:
          Serial.print(F("Deserialization failed\n"));
          break;
      }

     if (!success) {
      //propably the payload just wasn't a JSON message
      rxM_doc.clear();
      delay(200);
      customflush();
      Serial.println("***Sending request again***");
      delay(100);
      serializeJson(txM_doc, masterSerial);     // data send to slave again
      //return false;
    }else{
      type = rxM_doc["type"] | "Invalid";
      object = rxM_doc["object"] | "Invalid";
      statusE = rxM_doc["status"];
      if((strcmp(type , "response") == 0) && (strcmp(object,"emgy") == 0)){
        Serial.println("Received Status--->" +String(statusE));
        if(statusE == true){ 
          return true;
        }else if(statusE ==false){
          return false;
        }
      }
     }
   }//end of .available 
  }//end of while loop
  Serial.println(F("TimeOut"));
  return false;
}


bool requestEarthDiscStatus()
{
  // DynamicJsonDocument txM_doc(100);
  // DynamicJsonDocument rxM_doc(100);
  txM_doc.clear();
  rxM_doc.clear();

  const char* type = "";
  const char* object = "";
  bool success = false;
  int startTime = 0;
  bool statusE = 0;
  //Serial.println(F("EARTH DISCONNECT:"));
  txM_doc["type"] = "request";
  txM_doc["object"] = "earthdisc";

  serializeJson(txM_doc,masterSerial);     //data send to slave   
  serializeJson(txM_doc, Serial);
  delay(100);

  startTime = millis();
  while(millis() - startTime < WAIT_TIMEOUT){ // waiting for response from slave
    if(masterSerial.available()){ 
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code()) {
        case DeserializationError::Ok:
          success = true;
          break;
        case DeserializationError::InvalidInput:
          Serial.print(F(" Invalid input! Not a JSON\n"));
          break;
        case DeserializationError::NoMemory:
          Serial.print(F("Error: Not enough memory\n"));
          break;
        default:
          Serial.print(F("Deserialization failed\n"));
          break;
      }

     if (!success) {
      //propably the payload just wasn't a JSON message
      rxM_doc.clear();
      delay(50);
      customflush();
      Serial.println("***Sending request again***");
      delay(50);
      serializeJson(txM_doc, masterSerial);     // data send to slave again
      //return false;
    }else{
      type = rxM_doc["type"] | "Invalid";
      object = rxM_doc["object"] | "Invalid";
      statusE = rxM_doc["status"];
      if((strcmp(type , "response") == 0) && (strcmp(object,"earthdisc") == 0)){
        Serial.println("Received Status--->" +String(statusE));
        if(statusE == true){ 
          return true;
        }else if(statusE ==false){
          return false;
        }
      }
     }
   }//end of .available 
  }//end of while loop
  Serial.println(F("TimeOut"));
  return false;
}


//CP
bool requestforCP_OUT(int action){

  // DynamicJsonDocument txM_doc(200);
  // DynamicJsonDocument rxM_doc(200);
  txM_doc.clear();
  rxM_doc.clear();

  const char* type = "";
  const char* object = "";
  
  bool success = false;
  int startTime = 0;
  bool status = 0;
  
  Serial.println("Control Pilot:");
  txM_doc["type"] = "request";
  txM_doc["object"] = "cpout";
  
  txM_doc["action"] = action;
//  txM_doc["connectorId"] = connectorId;   future Implemnetation

  serializeJson(txM_doc,masterSerial);     //data send to slave   
  serializeJson(txM_doc, Serial);
  delay(100);

  startTime = millis();
  while(millis() - startTime < WAIT_TIMEOUT){ // waiting for response from slave
    if(masterSerial.available()){ 
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code()) {
        case DeserializationError::Ok:
          success = true;
          break;
        case DeserializationError::InvalidInput:
          Serial.print(F(" Invalid input! Not a JSON\n"));
          break;
        case DeserializationError::NoMemory:
          Serial.print(F("Error: Not enough memory\n"));
          break;
        default:
          Serial.print(F("Deserialization failed\n"));
          break;
      }

     if (!success) {
      //propably the payload just wasn't a JSON message
      rxM_doc.clear();
      delay(200);
      customflush();
      Serial.println("***Sending request again***");
      delay(100);
      serializeJson(txM_doc, masterSerial);     // data send to slave again
      //return false;
    }else{
      type = rxM_doc["type"] | "Invalid";
      object = rxM_doc["object"] | "Invalid";
      status = rxM_doc["status"];
      if((strcmp(type , "response") == 0) && (strcmp(object,"cpout") == 0)){
        Serial.println("Received Status--->" +String(status));
        return true;
      }
    }
   }//end of .available 
  }//end of while loop
  Serial.println(F("No response from Slave"));
  return false;
}

int requestforCP_IN(){

  // DynamicJsonDocument txM_doc(200);
  // DynamicJsonDocument rxM_doc(200);
  txM_doc.clear();
  rxM_doc.clear();

  const char* type = "";
  const char* object = "";
  
  bool success = false;
  int startTime = 0;
  int value = 0;
  
  Serial.println("Control Pilot:");
  txM_doc["type"] = "request";
  txM_doc["object"] = "cpin";
  
//  txM_doc["connectorId"] = connectorId;   future Implemnetation

  serializeJson(txM_doc,masterSerial);     //data send to slave   
  serializeJson(txM_doc, Serial);
  delay(100);

  startTime = millis();
  while(millis() - startTime < WAIT_TIMEOUT){ // waiting for response from slave
    if(masterSerial.available()){ 
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code()) {
        case DeserializationError::Ok:
          success = true;
          break;
        case DeserializationError::InvalidInput:
          Serial.print(F(" Invalid input! Not a JSON\n"));
          break;
        case DeserializationError::NoMemory:
          Serial.print(F("Error: Not enough memory\n"));
          break;
        default:
          Serial.print(F("Deserialization failed\n"));
          break;
      }

     if (!success) {
      //propably the payload just wasn't a JSON message
      rxM_doc.clear();
      delay(200);
      customflush();
      serializeJson(txM_doc, masterSerial);     // data send to slave again
      //return false;
    }else{
      type = rxM_doc["type"] | "Invalid";
      object = rxM_doc["object"] | "Invalid";
      value = rxM_doc["value"];
      if((strcmp(type , "response") == 0) && (strcmp(object,"cpin") == 0)){
        Serial.println("Received value--->" +String(value));
        return value;
      }
    }
   }//end of .available 
  }//end of while loop
  Serial.println(F("No response from Slave"));
  return false;
}

//For ARAI
#if 0
bool requestGfciStatus()
{
return false;
}
#endif
/*
 * @brief: requestGfciStatus - this will request the status from the secondary controller.
 * Added by G. Raja Sumant to fix the bug of POD generic code missign GFCI.
 */
#if 1
bool requestGfciStatus(){

  // DynamicJsonDocument txM_doc(100);
  // DynamicJsonDocument rxM_doc(100);
  txM_doc.clear();
  rxM_doc.clear();

  const char* type = "";
  const char* object = "";
  bool success = false;
  int startTime = 0;
  bool statusE = 0;
  Serial.println("GFCI:");
  txM_doc["type"] = "request";
  txM_doc["object"] = "gfci";

  serializeJson(txM_doc,masterSerial);     //data send to slave   
  serializeJson(txM_doc, Serial);
  delay(100);

  startTime = millis();
  while(millis() - startTime < WAIT_TIMEOUT){ // waiting for response from slave
    if(masterSerial.available()){ 
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code()) {
        case DeserializationError::Ok:
          success = true;
          break;
        case DeserializationError::InvalidInput:
          Serial.print(F(" Invalid input! Not a JSON\n"));
          break;
        case DeserializationError::NoMemory:
          Serial.print(F("Error: Not enough memory\n"));
          break;
        default:
          Serial.print(F("Deserialization failed\n"));
          break;
      }

     if (!success) {
      //propably the payload just wasn't a JSON message
      rxM_doc.clear();
      delay(200);
      customflush();
      Serial.println("***Sending request again***");
      delay(100);
      serializeJson(txM_doc, masterSerial);     // data send to slave again
      //return false;
    }else{
      type = rxM_doc["type"] | "Invalid";
      object = rxM_doc["object"] | "Invalid";
      statusE = rxM_doc["status"];
      if((strcmp(type , "response") == 0) && (strcmp(object,"gfci") == 0)){
        Serial.println("Received Status--->" +String(statusE));
        if(statusE == true){ 
          flag_GFCI_set_here = true;
          return true;
        }else if(statusE ==false){
          return false;
        }
      }
     }
   }//end of .available 
  }//end of while loop
  Serial.println("TimeOut");
  return false;
}
#endif


#if ALPR_ENABLED
void alprRead_loop(){
  rxA_doc.clear();
  txA_doc.clear();

  bool success = false;

  const char* licenseData = "";
  char s[500] = {0};
  int i=0;
  String payload;

if(alprSerial.available()){ 

  // Serial.println(alprSerial.readLine().decode("utf-8").rstrip());
  //while(alprSerial.available()>0){
  payload = alprSerial.readString();
  //  Serial.println(s);
  //}

  Serial.println(payload);
      delay(1000);
     // ReadLoggingStream loggingStream(alprSerial, Serial);
      DeserializationError err = deserializeJson(rxA_doc, payload);
      //  Serial.print(rxM_doc);
      switch (err.code()) {
        case DeserializationError::Ok:
          success = true;
          break;
        case DeserializationError::InvalidInput:
          Serial.print(F(" Invalid input! Not a JSON\n"));
          break;
        case DeserializationError::NoMemory:
          Serial.print(F("Error: Not enough memory\n"));
          break;
        default:
          Serial.print(F("Deserialization failed\n"));
          break;
      }

      if (!success) {
        //propably the payload just wasn't a JSON message
        rxA_doc.clear();
        delay(1000);
      }else{
        //licenseData = rxA_doc["LicensePlate"];
        licenseData = rxA_doc["license"];
        
        String data = String(licenseData);

        if((strcmp(licenseData , "novehicle") == 0)) 
        {
          txA_doc["ESP_Status"] = "OK";
          flag_noVehicle = true;
          serializeJson(txA_doc,alprSerial);     //data send to slave   
          serializeJson(txA_doc, Serial);
          delay(100);
          return;
        }

        if((strcmp(licenseData , "") != 0)) {

          
          Serial.println("Received value--->" +String(licenseData));

          getChargePointStatusService()->authorize(data);
          
          txA_doc["ESP_Status"] = "OK";
          serializeJson(txA_doc,alprSerial);     //data send to slave   
          serializeJson(txA_doc, Serial);
          delay(100);


        }
      }
  }

}


void alprAuthorized() {
    txA_doc.clear();  
    txA_doc["Authorization"] = 1;
    serializeJson(txA_doc, alprSerial);
    serializeJson(txA_doc, Serial);
    Serial.println();
    delay(1000);
}

void alprunAuthorized() {
    txA_doc.clear();  
    txA_doc["Authorization"] = 0;
    serializeJson(txA_doc, alprSerial);
    serializeJson(txA_doc, Serial);
    Serial.println();
    delay(1000);
}


void alprTxnStarted() {
    txA_doc.clear(); 
    txA_doc["Txn_Started"] = 1;
    serializeJson(txA_doc, alprSerial);
    serializeJson(txA_doc, Serial);
    Serial.println();
    delay(1000);
}

void alprTxnStopped() {
    txA_doc.clear();
    txA_doc["Txn_Stopped"] = 0;
    serializeJson(txA_doc, alprSerial);
    serializeJson(txA_doc, Serial);
    Serial.println();
    delay(1000);
}
#endif 


/************************************************* Authentication cache  (SQL DataBase)************************************/
  bool requestSendAuthCache(void)
{

  txM_doc.clear();
  rxM_doc.clear();

  const char *type = "";
  const char *object = "";

  int statusLL = 0;
  bool success = false;
  int startTime = 0;
  time_t expiryDate_epoch = 0;

  txM_doc["type"] = "request";
  txM_doc["object"] = "SendAuthCache";

  Serial.println("SendAuthCache:");
  
  // Read the idTag from Authentication/Start/Stop confirmation packet
  txM_doc["idTag"] = currentIdTag;

	if (expiry_time.equals("Invalid") == true) 
  {
    txM_doc["expiryTime"] = (19800 + now());
  }
  else
  {
    bool success = getTimeFromJsonDateString(expiry_time.c_str(), &expiryDate_epoch);
		if(success)
		{
      txM_doc["expiryTime"] = expiryDate_epoch;
    }
  }

  serializeJson(txM_doc, masterSerial); // data send to slave
  serializeJson(txM_doc, Serial);
  delay(100);

  startTime = millis();
  while (millis() - startTime < WAIT_TIMEOUT)
  { // waiting for response from slave
    if (masterSerial.available())
    {
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code())
      {
      case DeserializationError::Ok:
        success = true;
        break;
      case DeserializationError::InvalidInput:
        Serial.print(F(" Invalid input! Not a JSON\n"));
        break;
      case DeserializationError::NoMemory:
        Serial.print(F("Error: Not enough memory\n"));
        break;
      default:
        Serial.print(F("Deserialization failed\n"));
        break;
      }

      if (!success)
      {
        // propably the payload just wasn't a JSON message
        rxM_doc.clear();
        Serial.println("***Sending request again***");
        delay(1000);
        serializeJson(txM_doc, masterSerial); // data send to slave again
        // return false;
      }
      else
      {
        type = rxM_doc["type"];
        statusLL = rxM_doc["status"];
        object = rxM_doc["object"];
        
        if ((strcmp(type, "response") == 0) && (strcmp(object, "SendAuthCache") == 0))
        {
          Serial.println("Received Status--->" + String(statusLL));
          return true;
        }
      }
    } // end of .available
  }   // end of while loop
  Serial.println("TimeOut");
  return false;
}    

bool requestGetAuthCache(void)
{
  txM_doc.clear();
  rxM_doc.clear();

  const char *type = "";
  const char *object = "";
  const char *status = "";

  int statusLL = 0;
  bool success = false;
  int startTime = 0;

  txM_doc["type"] = "request";
  txM_doc["object"] = "GetAuthCache";
  txM_doc["idTag"] = currentIdTag;
  Serial.println("GetidTag:");

  serializeJson(txM_doc, masterSerial); // data send to slave
  serializeJson(txM_doc, Serial);
  delay(100);

  startTime = millis();
  while (millis() - startTime < WAIT_TIMEOUT)
  { // waiting for response from slave
    if (masterSerial.available())
    {
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code())
      {
      case DeserializationError::Ok:
        success = true;
        break;
      case DeserializationError::InvalidInput:
        Serial.print(F(" Invalid input! Not a JSON\n"));
        break;
      case DeserializationError::NoMemory:
        Serial.print(F("Error: Not enough memory\n"));
        break;
      default:
        Serial.print(F("Deserialization failed\n"));
        break;
      }

      if (!success)
      {
        // propably the payload just wasn't a JSON message
        rxM_doc.clear();
        Serial.println("***Sending request again***");
        delay(1000);
        serializeJson(txM_doc, masterSerial); // data send to slave again
        // return false;
      }
      else
      {
        type = rxM_doc["type"];
        object = rxM_doc["object"];
        status = rxM_doc["status"];

        if ((strcmp(type, "response") == 0) && (strcmp(object, "GetAuthCache") == 0))
        {
          Serial.println("Received GetidTag Status--->" + String(status));
          if (strcmp(status, "Accepted") == 0)
          {
            authCache_idTag_is_valid = true;
            #if 1
            if(webSocketConncted)
            {
            switch (readConnectorVal)
            {
            case 1:
              getChargePointStatusService_A()->setChargePointstatus(Preparing);
              break;
            case 2:
              getChargePointStatusService_B()->setChargePointstatus(Preparing);
              break;
            case 3:
              getChargePointStatusService_C()->setChargePointstatus(Preparing);
              break;
            }
            }
            #endif
          }
          else
          {
            authCache_idTag_is_valid = false;
          }
          return authCache_idTag_is_valid;
        }
      }
    } // end of .available
  }   // end of while loop
  Serial.println("TimeOut");
  return false;
}

bool requestClearCache(void)
{

  txM_doc.clear();
  rxM_doc.clear();

  const char *type = "";
  const char *object = "";
  const char *status = "";

  int statusLL = 0;
  bool success = false;
  int startTime = 0;

  txM_doc["type"] = "request";
  txM_doc["object"] = "ClearCache";
  
  Serial.println("ClearCache");

  serializeJson(txM_doc, masterSerial); // data send to slave
  serializeJson(txM_doc, Serial);
  delay(100);

  startTime = millis();
  while (millis() - startTime < WAIT_TIMEOUT)
  { // waiting for response from slave
    if (masterSerial.available())
    {
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code())
      {
      case DeserializationError::Ok:
        success = true;
        break;
      case DeserializationError::InvalidInput:
        Serial.print(F(" Invalid input! Not a JSON\n"));
        break;
      case DeserializationError::NoMemory:
        Serial.print(F("Error: Not enough memory\n"));
        break;
      default:
        Serial.print(F("Deserialization failed\n"));
        break;
      }

      if (!success)
      {
        // propably the payload just wasn't a JSON message
        rxM_doc.clear();
        Serial.println("***Sending request again***");
        delay(1000);
        serializeJson(txM_doc, masterSerial); // data send to slave again
        // return false;
      }
      else
      {
        type = rxM_doc["type"];
        object = rxM_doc["object"];
        status = rxM_doc["status"];

        if ((strcmp(type, "response") == 0) && (strcmp(object, "ClearCache") == 0))
        {
          Serial.println("Received GetidTag Status--->" + String(status));
          if (strcmp(status, "Accepted") == 0)
          {
            ClearCache_status = true;
          }
          else
          {
            ClearCache_status = false;
          }
          return authCache_idTag_is_valid;
        }
      }
    } // end of .available
  }   // end of while loop
  Serial.println("TimeOut");
  return false;
}
/************************************************* Authentication cache  (SQL DataBase)************************************/
      

/************************************************* Local Authentication List (SQL DataBase) ************************************/

bool requestSendLocalList(JsonObject payload)
{

  txM_doc.clear();
  rxM_doc.clear();

  const char *type = "";
  const char *object = "";

  int statusLL = 0;
  bool success = false;
  int startTime = 0;

  txM_doc["type"] = "request";
  txM_doc["object"] = "SendLocalList";

  Serial.println("SendLocalList:");

  txM_doc["locallist"] = payload;

  serializeJson(txM_doc, masterSerial); // data send to slave
  serializeJson(txM_doc, Serial);
  delay(100);

  startTime = millis();
  while (millis() - startTime < WAIT_TIMEOUT)
  { // waiting for response from slave
    if (masterSerial.available())
    {
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code())
      {
      case DeserializationError::Ok:
        success = true;
        break;
      case DeserializationError::InvalidInput:
        Serial.print(F(" Invalid input! Not a JSON\n"));
        break;
      case DeserializationError::NoMemory:
        Serial.print(F("Error: Not enough memory\n"));
        break;
      default:
        Serial.print(F("Deserialization failed\n"));
        break;
      }

      if (!success)
      {
        // propably the payload just wasn't a JSON message
        rxM_doc.clear();
        Serial.println("***Sending request again***");
        delay(1000);
        serializeJson(txM_doc, masterSerial); // data send to slave again
        // return false;
      }
      else
      {
        type = rxM_doc["type"];
        statusLL = rxM_doc["status"];
        if ((strcmp(type, "response") == 0) && (strcmp(object, "sendLocallist") == 0))
        {
          Serial.println("Received Status--->" + String(statusLL));
          return true;
        }
      }
    } // end of .available
  }   // end of while loop
  Serial.println("TimeOut");
  return false;
}

bool requestGetLocalListVersion(void)
{

  txM_doc.clear();
  rxM_doc.clear();

  const char *type = "";
  const char *object = "";

  int statusLL = 0;
  bool success = false;
  int startTime = 0;

  txM_doc["type"] = "request";
  txM_doc["object"] = "GetLocalListVersion";

  Serial.println("GetLocalListVersion:");

  serializeJson(txM_doc, masterSerial); // data send to slave
  serializeJson(txM_doc, Serial);
  delay(100);

  startTime = millis();
  while (millis() - startTime < WAIT_TIMEOUT)
  { // waiting for response from slave
    if (masterSerial.available())
    {
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code())
      {
      case DeserializationError::Ok:
        success = true;
        break;
      case DeserializationError::InvalidInput:
        Serial.print(F(" Invalid input! Not a JSON\n"));
        break;
      case DeserializationError::NoMemory:
        Serial.print(F("Error: Not enough memory\n"));
        break;
      default:
        Serial.print(F("Deserialization failed\n"));
        break;
      }

      if (!success)
      {
        // propably the payload just wasn't a JSON message
        rxM_doc.clear();
        Serial.println("***Sending request again***");
        delay(1000);
        serializeJson(txM_doc, masterSerial); // data send to slave again
        // return false;
      }
      else
      {
        type = rxM_doc["type"];
        statusLL = rxM_doc["listVersion"];
        gu16_locallist_ver = statusLL;
        if ((strcmp(type, "response") == 0) && (strcmp(object, "GetLocalListVersion") == 0))
        {
          Serial.println("Received listVersion--->" + String(statusLL));
          return true;
        }
      }
    } // end of .available
  }   // end of while loop
  Serial.println("TimeOut");
  return false;
}

bool requestGetidTag(void)
{

  txM_doc.clear();
  rxM_doc.clear();

  const char *type = "";
  const char *object = "";
  const char *status = "";

  int statusLL = 0;
  bool success = false;
  int startTime = 0;

  txM_doc["type"] = "request";
  txM_doc["object"] = "GetidTag";
  txM_doc["idTag"] = currentIdTag;
  Serial.println("GetidTag:");

  serializeJson(txM_doc, masterSerial); // data send to slave
  serializeJson(txM_doc, Serial);
  delay(100);

  startTime = millis();
  while (millis() - startTime < WAIT_TIMEOUT)
  { // waiting for response from slave
    if (masterSerial.available())
    {
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code())
      {
      case DeserializationError::Ok:
        success = true;
        break;
      case DeserializationError::InvalidInput:
        Serial.print(F(" Invalid input! Not a JSON\n"));
        break;
      case DeserializationError::NoMemory:
        Serial.print(F("Error: Not enough memory\n"));
        break;
      default:
        Serial.print(F("Deserialization failed\n"));
        break;
      }

      if (!success)
      {
        // propably the payload just wasn't a JSON message
        rxM_doc.clear();
        Serial.println("***Sending request again***");
        delay(1000);
        serializeJson(txM_doc, masterSerial); // data send to slave again
        // return false;
      }
      else
      {
        type = rxM_doc["type"];
        object = rxM_doc["object"];
        status = rxM_doc["status"];

        if ((strcmp(type, "response") == 0) && (strcmp(object, "GetidTag") == 0))
        {
          Serial.println("Received GetidTag Status--->" + String(status));
          if (strcmp(status, "Accepted") == 0)
          {
            Offline_idTag_is_valid = true;
          }
          else
          {
            Offline_idTag_is_valid = false;
          }
          return Offline_idTag_is_valid;
        }
      }
    } // end of .available
  }   // end of while loop
  Serial.println("TimeOut");
  return false;
}

bool requestGetidTagStatus(void)
{

  txM_doc.clear();
  rxM_doc.clear();

  const char *type = "";
  const char *object = "";
  const char *status = "";

  int statusLL = 0;
  bool success = false;
  int startTime = 0;

  txM_doc["type"] = "request";
  txM_doc["object"] = "GetidTagStatus";
  txM_doc["idTag"] = currentIdTag;
  Serial.println("GetidTagStatus:");

  serializeJson(txM_doc, masterSerial); // data send to slave
  serializeJson(txM_doc, Serial);
  delay(100);

  startTime = millis();
  while (millis() - startTime < WAIT_TIMEOUT)
  { // waiting for response from slave
    if (masterSerial.available())
    {
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code())
      {
      case DeserializationError::Ok:
        success = true;
        break;
      case DeserializationError::InvalidInput:
        Serial.print(F(" Invalid input! Not a JSON\n"));
        break;
      case DeserializationError::NoMemory:
        Serial.print(F("Error: Not enough memory\n"));
        break;
      default:
        Serial.print(F("Deserialization failed\n"));
        break;
      }

      if (!success)
      {
        // propably the payload just wasn't a JSON message
        rxM_doc.clear();
        Serial.println("***Sending request again***");
        delay(1000);
        serializeJson(txM_doc, masterSerial); // data send to slave again
        // return false;
      }
      else
      {
        type = rxM_doc["type"];
        object = rxM_doc["object"];
        status = rxM_doc["status"];

        if ((strcmp(type, "response") == 0) && (strcmp(object, "GetidTagStatus") == 0))
        {
          Serial.println("Received GetidTag Status--->" + String(status));
        if(strcmp(status , "Accepted") == 0)
        {
            Offline_idTag_is_valid = true;
        }
        else
        {
            Offline_idTag_is_valid = false;
        }
        return true;
        }
      }
    } // end of .available
  }   // end of while loop
  Serial.println("TimeOut");
  return false;
}

bool requestGetupdateType(void)
{

  txM_doc.clear();
  rxM_doc.clear();

  const char *type = "";
  const char *object = "";
  const char *updateType = "";

  int statusLL = 0;
  bool success = false;
  int startTime = 0;

  txM_doc["type"] = "request";
  txM_doc["object"] = "GetupdateType";
  Serial.println("GetupdateType:");

  serializeJson(txM_doc, masterSerial); // data send to slave
  serializeJson(txM_doc, Serial);
  delay(100);

  startTime = millis();
  while (millis() - startTime < WAIT_TIMEOUT)
  { // waiting for response from slave
    if (masterSerial.available())
    {
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code())
      {
      case DeserializationError::Ok:
        success = true;
        break;
      case DeserializationError::InvalidInput:
        Serial.print(F(" Invalid input! Not a JSON\n"));
        break;
      case DeserializationError::NoMemory:
        Serial.print(F("Error: Not enough memory\n"));
        break;
      default:
        Serial.print(F("Deserialization failed\n"));
        break;
      }

      if (!success)
      {
        // propably the payload just wasn't a JSON message
        rxM_doc.clear();
        Serial.println("***Sending request again***");
        delay(1000);
        serializeJson(txM_doc, masterSerial); // data send to slave again
        // return false;
      }
      else
      {
        type = rxM_doc["type"];
        object = rxM_doc["object"];
        updateType = rxM_doc["updateType"];

        if ((strcmp(type, "response") == 0) && (strcmp(object, "GetupdateType") == 0))
        {
          Serial.println("Received GetidTag updateType--->" + String(updateType));
          if (strcmp(updateType, "Full") == 0)
          {
            gu8_lla_updateType_is_valid = 1;
          }
          else if (strcmp(updateType, "Differential") == 0)
          {
            gu8_lla_updateType_is_valid = 2;
          }
          else
          {
            gu8_lla_updateType_is_valid = 0;
          }
          return true;
        }
      }
    } // end of .available
  }   // end of while loop
  Serial.println("TimeOut");
  return false;
}
/************************************************* Local Authentication List (SQL DataBase) ************************************/


/************************************************* OFFLINE TRANSACTION STORAGE  (SQL DataBase)************************************/


#if OFFLINE_STORAGE_ENABLE
bool requestSendStartTxn(String tid, String Conn_Id )
{

  txM_doc.clear();
  rxM_doc.clear();

  const char *type = "";
  const char *object = "";
  const char *status = "";

  int statusLL = 0;
  bool success = false;
  int startTime = 0;
  time_t startDate_epoch = 0;
  String ltime_startTS;
  uint8_t lu8_Conn_Id = 0;

  txM_doc["type"] = "request";
  txM_doc["object"] = "SendStartTxn";
  txM_doc["tid"] = tid.toInt();
  Serial.println("SendStartTxn:");
 
  // Send the Conn_Id to Start confirmation packet
  txM_doc["conId"] = Conn_Id.toInt();
  lu8_Conn_Id = Conn_Id.toInt();
   
  

  switch(lu8_Conn_Id)
  {
    case 1:
    {
          // Send the idTag to Start confirmation packet
          txM_doc["idTag"] = String(Tag_Id_A);

          // Send the Txnid to Start confirmation packet
          txM_doc["txnid"] = String(Transaction_Id_A);
            
          // resumeTxn_A.begin("resume_A", false); // opening preferences in R/W mode
	        // float startmetervalue_A = resumeTxn_A.getFloat("meterStart", false);
	        // resumeTxn_A.end();
          float startmetervalue_A = globalmeterstartA;
          // Send the Meter Value to Start confirmation packet
          txM_doc["metervalue"] = (int)startmetervalue_A;
          Serial.println("startmetervalue_A :"+String(startmetervalue_A));
          
          txM_doc["units"] = 0;  
          // txM_doc["units"] = String(0);
          
          // Send the Start time stump to Start confirmation packet
          txM_doc["ts"] = Sendstart_time_A;

          // txM_doc["ros"] = "3";  
          txM_doc["ros"] = String(reasonForStop_A);  
          Serial.println("Conn_Id: 1");
  }   
    break;

    case 2:   
    {   
          // Send the idTag to Start confirmation packet
          txM_doc["idTag"] = String(Tag_Id_B);

          // Send the Txnid to Start confirmation packet
          txM_doc["txnid"] = String(Transaction_Id_B);
          
          // resumeTxn_B.begin("resume_B", false); // opening preferences in R/W mode
	        // float startmetervalue_B = resumeTxn_B.getFloat("meterStart", false);
	        // resumeTxn_B.end();
          float startmetervalue_B = globalmeterstartB;
          // Send the Meter Value to Start confirmation packet
          txM_doc["metervalue"] = (int)startmetervalue_B;
          Serial.println("startmetervalue_B :"+String(startmetervalue_B));
   
          txM_doc["units"] = 0;
          
          // Send the Start time stump to Start confirmation packet
          txM_doc["ts"] = Sendstart_time_B;

          // txM_doc["ros"] = "3";
          txM_doc["ros"] = String(reasonForStop_B);  

          Serial.println("Conn_Id: 2");
  }
    break;

    case 3:  
    {    
          // Send the idTag to Start confirmation packet
          txM_doc["idTag"] = String(Tag_Id_C);

          // Send the Txnid to Start confirmation packet
          txM_doc["txnid"] = String(Transaction_Id_C);

          // resumeTxn_C.begin("resume_C", false); // opening preferences in R/W mode
	        // float startmetervalue_C = resumeTxn_C.getFloat("meterStart", false);
	        // resumeTxn_C.end();
          
          float startmetervalue_C = globalmeterstartC;
          
          // Send the Meter Value to Start confirmation packet
          txM_doc["metervalue"] = (int)startmetervalue_C;
          Serial.println("startmetervalue_C :"+String(startmetervalue_C));

          txM_doc["units"] = 0;
          
          // Send the Start time stump to Start confirmation packet
          txM_doc["ts"] = Sendstart_time_C;
          
          // txM_doc["ros"] = "3"; 
          txM_doc["ros"] = String(reasonForStop_C);  
 
 
          Serial.println("Conn_Id: 3"); 
  }     
    break;

    default:

    break;
  }

  serializeJson(txM_doc, masterSerial); // data send to slave
  serializeJson(txM_doc, Serial);
  delay(100);

  startTime = millis();
  while (millis() - startTime < WAIT_TIMEOUT)
  { // waiting for response from slave
    if (masterSerial.available())
    {
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code())
      {
      case DeserializationError::Ok:
        success = true;
        break;
      case DeserializationError::InvalidInput:
        Serial.print(F(" Invalid input! Not a JSON\n"));
        break;
      case DeserializationError::NoMemory:
        Serial.print(F("Error: Not enough memory\n"));
        break;
      default:
        Serial.print(F("Deserialization failed\n"));
        break;
      }

      if (!success)
      {
        // propably the payload just wasn't a JSON message
        rxM_doc.clear();
        Serial.println("***Sending request again***");
        delay(1000);
        serializeJson(txM_doc, masterSerial); // data send to slave again
        // return false;
      }
      else
      {
        type = rxM_doc["type"];
        status = rxM_doc["status"];
        object = rxM_doc["object"];

        if ((strcmp(type, "response") == 0) && (strcmp(object, "SendStartTxn") == 0))
        {
          Serial.println("Received GetidTag Status--->" + String(status));
          if (strcmp(status, "Accepted") == 0)
          {
            SendStartTxn_status = true;
          }
          else
          {
            SendStartTxn_status = false;
          }
          return SendStartTxn_status;
        }
      }
    } // end of .available
  }   // end of while loop
  Serial.println("TimeOut");
  return false;
}



bool requestSendStopTxn(String tid,String Conn_Id, String meter_value )
{

  txM_doc.clear();
  rxM_doc.clear();

  const char *type = "";
  const char *object = "";
  const char *status = "";

  int statusLL = 0;
  bool success = false;
  int startTime = 0;
  time_t stopDate_epoch = 0;
  String ltime_stopTS;
  uint8_t lu8_Conn_Id = 0;
  float lfmeter_value = 0; 

  txM_doc["type"] = "request";
  txM_doc["object"] = "SendStopTxn";
  // txM_doc["idTag"] = Tag_Id;
  txM_doc["tid"] = tid.toInt();
  Serial.println("SendStopTxn:");
   // Send the Conn_Id to Stop confirmation packet
  txM_doc["conId"] = Conn_Id.toInt(); 

   lu8_Conn_Id = Conn_Id.toInt();
   lfmeter_value = meter_value.toFloat();
   Serial.println("meter_value:" +String(lfmeter_value));

  switch(lu8_Conn_Id)
  {
    case 1:
          // Send the idTag to Start confirmation packet
          txM_doc["idTag"] = String(Tag_Id_A);

          // Send the Txnid to Start confirmation packet
          txM_doc["txnid"] = String(Transaction_Id_A);

          // Send the Meter Value to Start confirmation packet
          // txM_doc["metervalue"] = String(Start_Meter_Value_A);
           
          // txM_doc["metervalue"] = meter_value.toInt();
          // txM_doc["metervalue"] = meter_value.toFloat();
          txM_doc["metervalue"] = (int)lfmeter_value;

          // txM_doc["units"] = String(1);
          txM_doc["units"] = 1;  

          // Send the Start time stump to Start confirmation packet
          // ltime_stopTS = String(Sendstop_time_A); 
          Sendstop_time_A = now();
          txM_doc["ts"] = Sendstop_time_A;
          
          // txM_doc["ros"] = "3";  
          txM_doc["ros"] = String(reasonForStop_A);

    break;

        case 2:
          // Send the idTag to Start confirmation packet
          txM_doc["idTag"] = String(Tag_Id_B);

          // Send the Txnid to Start confirmation packet
          txM_doc["txnid"] = String(Transaction_Id_B);

          // Send the Meter Value to Start confirmation packet
          // txM_doc["metervalue"] = String(Start_Meter_Value_B);

          // txM_doc["metervalue"] = meter_value.toInt();
          // txM_doc["metervalue"] = meter_value.toFloat();
          txM_doc["metervalue"] = (int)lfmeter_value;
          
          txM_doc["units"] = 1;

          // Send the Start time stump to Start confirmation packet
          Sendstop_time_B  = now();          
          txM_doc["ts"] = Sendstart_time_B;

          // txM_doc["ros"] = "3";  
          txM_doc["ros"] = String(reasonForStop_A);

                  
          

    break;

        case 3:
                      // Send the idTag to Start confirmation packet
          txM_doc["idTag"] = String(Tag_Id_C);

          // Send the Txnid to Start confirmation packet
          txM_doc["txnid"] = String(Transaction_Id_C);

          // Send the Meter Value to Start confirmation packet
          // txM_doc["metervalue"] = String(Start_Meter_Value_C);

          // txM_doc["metervalue"] = meter_value.toInt();
          // txM_doc["metervalue"] = meter_value.toFloat();
          txM_doc["metervalue"] = (int)lfmeter_value;

          txM_doc["units"] = 1;

          Sendstop_time_C  = now(); 
          txM_doc["ts"] = Sendstop_time_C;

          // txM_doc["ros"] = "3";  
          txM_doc["ros"] = String(reasonForStop_A);

          // Send the Start time stump to Start confirmation packet
          // ltime_stopTS = String(Sendstop_time_C); 
                  
    break;

    default:

    break;
  }
 


  serializeJson(txM_doc, masterSerial); // data send to slave
  serializeJson(txM_doc, Serial);
  delay(100);

  startTime = millis();
  while (millis() - startTime < WAIT_TIMEOUT)
  { // waiting for response from slave
    if (masterSerial.available())
    {
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code())
      {
      case DeserializationError::Ok:
        success = true;
        break;
      case DeserializationError::InvalidInput:
        Serial.print(F(" Invalid input! Not a JSON\n"));
        break;
      case DeserializationError::NoMemory:
        Serial.print(F("Error: Not enough memory\n"));
        break;
      default:
        Serial.print(F("Deserialization failed\n"));
        break;
      }

      if (!success)
      {
        // propably the payload just wasn't a JSON message
        rxM_doc.clear();
        Serial.println("***Sending request again***");
        delay(1000);
        serializeJson(txM_doc, masterSerial); // data send to slave again
        // return false;
      }
      else
      {
        type = rxM_doc["type"];
        object = rxM_doc["object"];
        status = rxM_doc["status"];

        if ((strcmp(type, "response") == 0) && (strcmp(object, "SendStopTxn") == 0))
        {
          Serial.println("Received GetidTag Status--->" + String(status));
          if (strcmp(status, "Accepted") == 0)
          {
            SendStopTxn_status = true;
          }
          else
          {
            SendStopTxn_status = false;
          }
          return SendStopTxn_status;
        }
      }
    } // end of .available
  }   // end of while loop
  Serial.println("TimeOut");
  return false;
}

#endif

#if 0
bool requestSendStopTxn(String tid,String Conn_Id)
{

  txM_doc.clear();
  rxM_doc.clear();

  const char *type = "";
  const char *object = "";
  const char *status = "";

  int statusLL = 0;
  bool success = false;
  int startTime = 0;
  time_t stopDate_epoch = 0;
  String ltime_stopTS;
  uint8_t lu8_Conn_Id = 0;

  txM_doc["type"] = "request";
  txM_doc["object"] = "SendStopTxn";
  // txM_doc["idTag"] = Tag_Id;
  txM_doc["tid"] = tid.toInt();
  Serial.println("SendStopTxn:");
   // Send the Conn_Id to Stop confirmation packet
  txM_doc["conId"] = Conn_Id.toInt(); 

   lu8_Conn_Id = Conn_Id.toInt();
   

  switch(lu8_Conn_Id)
  {
    case 1:
          // Send the idTag to Start confirmation packet
          txM_doc["idTag"] = String(Tag_Id_A);

          // Send the Txnid to Start confirmation packet
          txM_doc["txnid"] = String(Transaction_Id_A);

          // Send the Meter Value to Start confirmation packet
          // txM_doc["metervalue"] = String(Start_Meter_Value_A);
           
          txM_doc["metervalue"] = globalmeterstartA;
            
          // txM_doc["units"] = String(1);
          txM_doc["units"] = 1;  
          // Send the Start time stump to Start confirmation packet
          // ltime_stopTS = String(Sendstop_time_A); 
          txM_doc["ts"] = Sendstart_time_A;
          
          // txM_doc["ros"] = "3";  
          txM_doc["ros"] = String(reasonForStop_A);

    break;

        case 2:
          // Send the idTag to Start confirmation packet
          txM_doc["idTag"] = String(Tag_Id_B);

          // Send the Txnid to Start confirmation packet
          txM_doc["txnid"] = String(Transaction_Id_B);

          // Send the Meter Value to Start confirmation packet
          // txM_doc["metervalue"] = String(Start_Meter_Value_B);

          txM_doc["metervalue"] = globalmeterstartB;
          
          txM_doc["units"] = 1;

          txM_doc["ts"] = Sendstart_time_B;

          // txM_doc["ros"] = "3";  
          txM_doc["ros"] = String(reasonForStop_A);

          // Send the Start time stump to Start confirmation packet
          // ltime_stopTS = String(Sendstop_time_B); 
                  
          

    break;

        case 3:
                      // Send the idTag to Start confirmation packet
          txM_doc["idTag"] = String(Tag_Id_C);

          // Send the Txnid to Start confirmation packet
          txM_doc["txnid"] = String(Transaction_Id_C);

          // Send the Meter Value to Start confirmation packet
          // txM_doc["metervalue"] = String(Start_Meter_Value_C);

          txM_doc["metervalue"] = globalmeterstartC;

          txM_doc["units"] = 1;

          txM_doc["ts"] = Sendstart_time_B;

          // txM_doc["ros"] = "3";  
          txM_doc["ros"] = String(reasonForStop_A);

          // Send the Start time stump to Start confirmation packet
          // ltime_stopTS = String(Sendstop_time_C); 
                  
    break;

    default:

    break;
  }
 


  serializeJson(txM_doc, masterSerial); // data send to slave
  serializeJson(txM_doc, Serial);
  delay(100);

  startTime = millis();
  while (millis() - startTime < WAIT_TIMEOUT)
  { // waiting for response from slave
    if (masterSerial.available())
    {
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code())
      {
      case DeserializationError::Ok:
        success = true;
        break;
      case DeserializationError::InvalidInput:
        Serial.print(F(" Invalid input! Not a JSON\n"));
        break;
      case DeserializationError::NoMemory:
        Serial.print(F("Error: Not enough memory\n"));
        break;
      default:
        Serial.print(F("Deserialization failed\n"));
        break;
      }

      if (!success)
      {
        // propably the payload just wasn't a JSON message
        rxM_doc.clear();
        Serial.println("***Sending request again***");
        delay(1000);
        serializeJson(txM_doc, masterSerial); // data send to slave again
        // return false;
      }
      else
      {
        type = rxM_doc["type"];
        object = rxM_doc["object"];
        status = rxM_doc["status"];

        if ((strcmp(type, "response") == 0) && (strcmp(object, "SendStopTxn") == 0))
        {
          Serial.println("Received GetidTag Status--->" + String(status));
          if (strcmp(status, "Accepted") == 0)
          {
            SendStopTxn_status = true;
          }
          else
          {
            SendStopTxn_status = false;
          }
          return SendStopTxn_status;
        }
      }
    } // end of .available
  }   // end of while loop
  Serial.println("TimeOut");
  return false;
}
#endif

#if OFFLINE_STORAGE_ENABLE
int requestGetTxnList(void)
{

  txM_doc.clear();
  rxM_doc.clear();

  const char *type = "";
  const char *object = "";
  const char *status = "";

  int statusLL = 0;
  bool success = false;
  int startTime = 0;
  int s32_no_of_txn_list = 0;

  txM_doc["type"] = "request";
  txM_doc["object"] = "GetTxnList";

  Serial.println("GetTxnList");

  serializeJson(txM_doc, masterSerial); // data send to slave
  serializeJson(txM_doc, Serial);
  delay(100);

  startTime = millis();
  while (millis() - startTime < WAIT_TIMEOUT)
  { // waiting for response from slave
    if (masterSerial.available())
    {
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code())
      {
      case DeserializationError::Ok:
        success = true;
        break;
      case DeserializationError::InvalidInput:
        Serial.print(F(" Invalid input! Not a JSON\n"));
        break;
      case DeserializationError::NoMemory:
        Serial.print(F("Error: Not enough memory\n"));
        break;
      default:
        Serial.print(F("Deserialization failed\n"));
        break;
      }

      if (!success)
      {
        // propably the payload just wasn't a JSON message
        rxM_doc.clear();
        Serial.println("***Sending request again***");
        delay(1000);
        serializeJson(txM_doc, masterSerial); // data send to slave again
        // return false;
      }
      else
      {
        type = rxM_doc["type"];
        object = rxM_doc["object"];
        status = rxM_doc["status"];
        s32_no_of_txn_list = rxM_doc["txnlist"].as<int>();

        if ((strcmp(type, "response") == 0) && (strcmp(object, "GetTxnList") == 0))
        {
          Serial.println("Received GetidTag Status--->" + String(status));
          if (strcmp(status, "Accepted") == 0)
          {
            GetTxnList_status = true;
          }
          else
          {
            GetTxnList_status = false;
          }
          return s32_no_of_txn_list;
          // return GetTxnList_status;
        }
      }
    } // end of .available
  }   // end of while loop
  Serial.println("TimeOut");
  // return false;
  return s32_no_of_txn_list;
}

bool requestGetTxn(String txn_id)
{

  txM_doc.clear();
  rxM_doc.clear();
  get_txn_doc.clear();

  const char *type = "";
  const char *object = "";
  const char *status = "";
  const char *lidtag = "";
  const char *lconId = "";
  const char *ltxnid = "";
  const char *lstartDate = "";
  const char *lstart_metervalue = "";
  const char *lstopDate = "";
  const char *lstop_metervalue = "";
  const char *lunits = "";
  const char *lreason = "";

  int statusLL = 0;
  bool success = false;
  int startTime = 0;

  txM_doc["type"] = "request";
  txM_doc["object"] = "GetTxn";
  txM_doc["txn_id"] = txn_id;
  Serial.println("GetTxn");

  serializeJson(txM_doc, masterSerial); // data send to slave
  serializeJson(txM_doc, Serial);
  delay(100);

  startTime = millis();
  while (millis() - startTime < WAIT_TIMEOUT)
  { // waiting for response from slave
    if (masterSerial.available())
    {
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code())
      {
      case DeserializationError::Ok:
        success = true;
        break;
      case DeserializationError::InvalidInput:
        Serial.print(F(" Invalid input! Not a JSON\n"));
        break;
      case DeserializationError::NoMemory:
        Serial.print(F("Error: Not enough memory\n"));
        break;
      default:
        Serial.print(F("Deserialization failed\n"));
        break;
      }

      if (!success)
      {
        // propably the payload just wasn't a JSON message
        rxM_doc.clear();
        Serial.println("***Sending request again***");
        delay(1000);
        serializeJson(txM_doc, masterSerial); // data send to slave again
        // return false;
      }
      else
      {
        type = rxM_doc["type"];
        object = rxM_doc["object"];
        status = rxM_doc["status"];
        lidtag = rxM_doc["idTag"];
        lconId = rxM_doc["conId"];
        ltxnid = rxM_doc["txnid"];
        lstartDate = rxM_doc["startDate"];
        lstart_metervalue = rxM_doc["start_metervalue"];
        lstopDate = rxM_doc["stopDate"];
        lstop_metervalue = rxM_doc["stop_metervalue"];
        lunits = rxM_doc["units"];
        lreason = rxM_doc["reason"];

        get_txn_doc["gtxn"] =  rxM_doc;

        if ((strcmp(type, "response") == 0) && (strcmp(object, "GetTxn") == 0))
        {
          Serial.println("Received GetidTag Status--->" + String(status));
          if (strcmp(status, "Accepted") == 0)
          {
            GetTxn_status = true;
          }
          else
          {
            GetTxn_status = false;
          }
          return GetTxn_status;
        }
      }
    } // end of .available
  }   // end of while loop
  Serial.println("TimeOut");
  return false;
}

bool requestClearTxn(String txn_id)
{

  txM_doc.clear();
  rxM_doc.clear();

  const char *type = "";
  const char *object = "";
  const char *status = "";
  const char *lidtag = "";
  const char *lconId = "";
  const char *ltxnid = "";
  // const char *lstartDate = "";
  // const char *lstart_metervalue = "";
  // const char *lstopDate = "";
  // const char *lstop_metervalue = "";
  // const char *lunits = "";
  // const char *lreason = "";

  int statusLL = 0;
  bool success = false;
  int startTime = 0;

  txM_doc["type"] = "request";
  txM_doc["object"] = "ClearTxn";
  txM_doc["txn_id"] = txn_id;
  Serial.println("ClearTxn");

  serializeJson(txM_doc, masterSerial); // data send to slave
  serializeJson(txM_doc, Serial);
  delay(100);

  startTime = millis();
  while (millis() - startTime < WAIT_TIMEOUT)
  { // waiting for response from slave
    if (masterSerial.available())
    {
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code())
      {
      case DeserializationError::Ok:
        success = true;
        break;
      case DeserializationError::InvalidInput:
        Serial.print(F(" Invalid input! Not a JSON\n"));
        break;
      case DeserializationError::NoMemory:
        Serial.print(F("Error: Not enough memory\n"));
        break;
      default:
        Serial.print(F("Deserialization failed\n"));
        break;
      }

      if (!success)
      {
        // propably the payload just wasn't a JSON message
        rxM_doc.clear();
        Serial.println("***Sending request again***");
        delay(1000);
        serializeJson(txM_doc, masterSerial); // data send to slave again
        // return false;
      }
      else
      {
        type = rxM_doc["type"];
        object = rxM_doc["object"];
        status = rxM_doc["status"];
        // lidtag = rxM_doc["idTag"];
        // lconId = rxM_doc["conId"];
        // ltxnid = rxM_doc["txnid"];
        // lstartDate = rxM_doc["startDate"];
        // lstart_metervalue = rxM_doc["start_metervalue"];
        // lstopDate = rxM_doc["stopDate"];
        // lstop_metervalue = rxM_doc["stop_metervalue"];
        // lunits = rxM_doc["units"];
        // lreason = rxM_doc["reason"];

        // get_txn_doc["gtxn"] =  rxM_doc;

        if ((strcmp(type, "response") == 0) && (strcmp(object, "ClearTxn") == 0))
        {
          Serial.println("Received GetidTag Status--->" + String(status));
          if (strcmp(status, "Accepted") == 0)
          {
            ClearTxn_status = true;
          }
          else
          {
            ClearTxn_status = false;
          }
          return ClearTxn_status;
        }
      }
    } // end of .available
  }   // end of while loop
  Serial.println("TimeOut");
  return false;
}


bool requestClearTxnList(void)
{

  txM_doc.clear();
  rxM_doc.clear();

  const char *type = "";
  const char *object = "";
  const char *status = "";

  int statusLL = 0;
  bool success = false;
  int startTime = 0;

  txM_doc["type"] = "request";
  txM_doc["object"] = "ClearTxnList";

  Serial.println("ClearTxnList");

  serializeJson(txM_doc, masterSerial); // data send to slave
  serializeJson(txM_doc, Serial);
  delay(100);

  startTime = millis();
  while (millis() - startTime < WAIT_TIMEOUT)
  { // waiting for response from slave
    if (masterSerial.available())
    {
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code())
      {
      case DeserializationError::Ok:
        success = true;
        break;
      case DeserializationError::InvalidInput:
        Serial.print(F(" Invalid input! Not a JSON\n"));
        break;
      case DeserializationError::NoMemory:
        Serial.print(F("Error: Not enough memory\n"));
        break;
      default:
        Serial.print(F("Deserialization failed\n"));
        break;
      }

      if (!success)
      {
        // propably the payload just wasn't a JSON message
        rxM_doc.clear();
        Serial.println("***Sending request again***");
        delay(1000);
        serializeJson(txM_doc, masterSerial); // data send to slave again
        // return false;
      }
      else
      {
        type = rxM_doc["type"];
        object = rxM_doc["object"];
        status = rxM_doc["status"];

        if ((strcmp(type, "response") == 0) && (strcmp(object, "ClearTxnList") == 0))
        {
          Serial.println("Received GetidTag Status--->" + String(status));
          if (strcmp(status, "Accepted") == 0)
          {
            ClearTxnList_status = true;
          }
          else
          {
            ClearTxnList_status = false;
          }
          return ClearTxnList_status;
        }
      }
    } // end of .available
  }   // end of while loop
  Serial.println("TimeOut");
  return false;
}

#endif
/************************************************* OFFLINE TRANSACTION STORAGE  (SQL DataBase)************************************/

void Master_setup() {
  // put your setup code here, to run once:
  
 // Serial.begin(115200);
  masterSerial.begin(9600, SWSERIAL_8N1, 25,33, false,256);// Required to fix the deserialization error
  Serial.println(F("***VERSION HISTORY***"));
  // Serial.println(F("***Firmware_ver-----EVSE_7.4KW_V1.2.0***"));
  Serial.print(F("***Firmware_ver-----:"));
  Serial.println(String(VERSION) +"***");

  #if V_charge_lite1_4
  Serial.println(F("***Hardware_ver-----V_Charge_2.0***"));
  #else
  Serial.println(F("***Hardware_ver-----VL_1.3***"));
  #endif

  #if DWIN_ENABLED
  Serial.println(F("Integrated DWIN 3.5 Inch Display"));
  #endif

  #if LCD_DISPLAY_ENABLED
  Serial.println("Integrated LCD 20x4 Display");
  #endif
  Serial.println("Firmware version in the boot notification - Abhigna-01/02/2024");
  Serial.println("4G support - G. Raja Sumant 21/04/2022");
  Serial.println("Reason for stop - G. Raja Sumant 31/05/2022");
  Serial.println("20x4,DWIN, arduino uno - G. Raja Sumant 31/05/2022");
  Serial.println("1,2,3 Modified by Shiva . Date:20-06-2023");
  Serial.println("Added SEEMA CONNECT customer feature that is  skip the Authorization packet before start transaction to CMS.");
  Serial.println("Resume charging Session with same transaction ID after Power Cycle if any previous charing sessions are on going.");
  Serial.println("1. WiFi TAP RFID Print on LCD Modified");
  Serial.println("2. Remote Start RFID Stop Modified");
  Serial.println("3. Stop with any valid card removed.");
  Serial.println("Start charging with any RFID in offline is included");
  Serial.println("Updated by Shiva Poola & Abhigna Valmiki. Date:17-11-2023");
  Serial.println("Fixed Websocket reconnect Issue.");
  Serial.println("Fixed Connector are showing Preparing State after sessions terminated Issue.");
  Serial.println("Fixed Invalid Meter values starting from zero,after session resume on Power recycle Issue.");
  Serial.println("Fixed Session is stop through Emergency Switch,  Meter values are not updating in offline, sending stop with online last updated Watt-Hour Issue.");
  Serial.println("Fixed Low current count/timeout is not working in Off-line Issue.");
  Serial.println("Fixed Multiple status notification.");
  Serial.println("Fixed Relays are starting in offline once emergency pressed and released Issue.");
  Serial.println("Fixed LED's Indicating RED randomly Issue.");
  Serial.println("Fixed relays are toggling while charging Issue.");
  Serial.println("Fixed Low current count is not working Issue.");
  Serial.println("last Updated by Abhigna Valmiki. Date:01-02-2024");
  Serial.println("LCD Driver implementation");
  Serial.println("LED Driver Implemetation");
  Serial.println("Retry for Bootnotification till accpted if boot notification Timed out");
  Serial.println("Removed metervalues/energy consumption in offline");
  Serial.println("Removed finishing state");
  Serial.println("Configuration based for OTA, Low current count and Min Current");
  Serial.print("[MASTER] ESP32 Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
}

/*
void loop() {
  // put your main code here, to run repeatedly:
   //requestLed(RED,START,1);

  bool emgcy = requestEmgyStatus();
  Serial.println("Emergency Button Status--> " +String(emgcy));
  if(emgcy == 0){
    for(;;){
      
      Serial.println("EMGY is pressed");
       bool emgcy = requestEmgyStatus();
       if(emgcy ==1) break;
      }
  }
  int connector = requestConnectorStatus();
  Serial.println("Connector: " + String(connector)+ "\n");
  delay(1000);
  if(connector > 0){
   bool statusRelay = requestForRelay(START,connector);
   Serial.println("statusRelay--->" + String(statusRelay));
  
  delay(1000);

  bool ch1 = requestForRelay(STOP,connector);
  Serial.println("ch1 --->" +String(ch1));

  delay(1000);

  bool ch2 = requestLed(RED,START,connector);
  Serial.println("ch2--->" +String(ch2));

  delay(1000);

  requestLed(GREEN,START,connector);

  delay(1000);

    requestLed(BLUE,START,connector);

  delay(1000);
    requestLed(WHITE,START,connector);

  delay(1000);
    requestLed(RED,STOP,connector);

  //  delay(1000);
   // requestLed(RED,START,connector);
  }
}
*/