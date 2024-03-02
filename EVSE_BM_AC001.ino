/*
   4G support added - 21/04/2022
   Added DWIN display support.
   Fixed the status of NOT_SET 09
  <POD without Control Pilot Support>
  The following code is developed by Pulkit Agrawal & Wamique.
  Added Master-Slave files
  Added EnergyMeter Fix
*/

// ESP32 Libraries
#include <Arduino.h>
#include "src/libraries/arduinoWebSockets-master/src/WebSocketsClient.h"
// #include <ArduinoJson.h>
#include "src/Peripherals.h"
#include "esp32-hal-cpu.h"
#if WIFI_ENABLED
#include <WiFi.h>
// #define SSIDW   "Amplify Mobility_PD"
// #define PSSWD   "Amplify5"
#endif
// OCPP Message Generation Class
#include "src/OcppEngine.h"
// #include "src/SmartChargingService.h"
#include "src/ChargePointStatusService.h"
#include "src/MeteringService.h"
#include "src/GetConfiguration.h"
#include "src/TimeHelper.h"
#include "src/SimpleOcppOperationFactory.h"
#include "src/EVSE_A.h"
#include "src/EVSE_B.h"
#include "src/EVSE_C.h"
#include "src/LED.h"
#include "src/EVSE_A_Offline.h"
#include "src/EVSE_B_Offline.h"
#include "src/EVSE_C_Offline.h"

// Master Class
#include "src/Master.h"

// Power Cycle
#include "src/OTA.h"
#include "src/internet.h"

#include "esp32-hal-cpu.h"
#include "ESP32Time.h"
#include "Esp.h"
#include "core_version.h"
// #include "src/evse_addr_led.h"

#if CP_ACTIVE
// Control Pilot files
#include "src/ControlPilot.h"
#endif

#if DISPLAY_ENABLED
#include "src/display.h"
bool flag_tapped = false;
bool flag_freeze = false;
bool flag_unfreeze = false;
#endif

// Gsm Files

#include "src/CustomGsm.h"
extern TinyGsmClient client;

#include "src/urlparse.h"

#include "src/display_meterValues.h"

// 20x4 lcd display

#include <TinyGsmClient.h>
#include <CRC32.h>
#include "FFat.h"
#include "FS.h"
#include "SPIFFS.h"
#include "esp32-hal-cpu.h"
#include "ESP32Time.h"
#include "Esp.h"
#include "core_version.h"
#include <Update.h> //ADDED
#include "src/4G_OTA.h"
#include "src/evse_wifi.h"

#define TINY_GSM_MODEM_SIM7600

#define GSM_RXD2 16
#define GSM_TXD2 17

#define GSM_AUTOBAUD_MIN 9600
#define GSM_AUTOBAUD_MAX 115200

#define SerialAT Serial2

typedef enum wifi_connectivity_status
{
  wifi_connect_initiated,
  wifi_connecting,
  wifi_connected
};

typedef enum ws_connectivity_status
{
  ws_connect_initiated,
  ws_connecting,
  ws_connected
};

typedef enum EVSE_Connector_Status_t
{
  EVSE_A,
  EVSE_B,
  EVSE_C,
};
EVSE_Connector_Status_t EVSE_Connector_Status;
#if LCD_DISPLAY_ENABLED
#include "src/LCD_I2C.h"

LCD_I2C lcd(0x27, 20, 4); // Default address of most PCF8574 modules, change according
#endif

#if DWIN_ENABLED
#include "src/dwin.h"
int8_t button = 0;
extern unsigned char ct[22];        // connected
extern unsigned char nct[22];       // not connected
extern unsigned char et[22];        // ethernet
extern unsigned char wi[22];        // wifi
extern unsigned char tr[22];        // tap rfid
extern unsigned char utr[22];       // rfid unavailable
extern unsigned char g[22];         // 4g
extern unsigned char clu[22];       // connected
extern unsigned char clun[22];      // not connected
extern unsigned char avail[22];     // available
extern unsigned char not_avail[22]; // not available
extern unsigned char change_page[10];
extern unsigned char tap_rfid[30];
extern unsigned char clear_tap_rfid[30];
extern unsigned char CONN_UNAVAIL[28];
extern unsigned char clear_avail[28];
extern unsigned char select_connector[30];

extern bool flag_faultOccured_A;
extern bool flag_faultOccured_B;
extern bool flag_faultOccured_C;

extern unsigned char v1[8]; // 2];//not connected
extern unsigned char v2[8];
extern unsigned char v3[8];
extern unsigned char i1[8];
extern unsigned char i2[8];
extern unsigned char i3[8];
extern unsigned char e1[8];
extern unsigned char e2[8];
extern unsigned char e3[8];
extern unsigned char charging[28];
extern unsigned char cid1[8];
extern unsigned char cid2[8];
extern unsigned char cid3[8];
extern unsigned char unavail[30];
void display_avail();
int8_t dwin_input();
#endif

extern bool notFaulty_A;
extern bool notFaulty_B;
extern bool notFaulty_C;

void OTA_4G_setup_4G_OTA_get(void);
void OTA_4G_setup_4G_OTA(void);
void OTA_4G_setup4G(void);
uint8_t OTA_4G_waitForResp(uint8_t timeout);
void OTA_4G_printPercent(uint32_t readLength, uint32_t contentLength);
void OTA_4G_pu(const char *path);

extern bool EMGCY_FaultOccured_A;
extern bool EMGCY_FaultOccured_B;
extern bool EMGCY_FaultOccured_C;

extern bool flag_nopower;

extern int16_t counter_ethconnect;

int gs32_offlinestarttxnId_A = 0;
int gs32_offlinestarttxnId_B = 0;
int gs32_offlinestarttxnId_C = 0;

int gs32_offlinestartStop_connId = 0;

int gs32_offlinestarttxnId = 0;
int gs32_offlinestarttxn_update_flag = 0;
int gs32_offlinestartStoptxn_update_flag = 0;
int gs32_offlineAuthorizeIdTag = 0;

extern uint8_t reasonForStop_A;
extern uint8_t reasonForStop_B;
extern uint8_t reasonForStop_C;

extern int transactionId_A;
extern int transactionId_B;
extern int transactionId_C;

int gs32_offlinestartStoptxn_update_guard_Interval = OFFLINE_START_STOP_TXN_GUARD_INTERVAL;

String lidtag = "";
int lc_conId = 0;
String lc_txnid = "";
String lc_startDate = "";
String lc_start_metervalue = "";
String lc_stopDate = "";
String lc_stop_metervalue = "";
String lc_units = "";
String lc_reason = "";

String gc_offline_txn_startDate = "";
String gc_offline_txn_start_metervalue = "";
String gc_offline_txn_stopDate = "";
String gc_offline_txn_stop_metervalue = "";
String gc_offline_txn_reason = "";

extern bool flag_evRequestsCharge_A;
extern bool flag_evRequestsCharge_B;
extern bool flag_evRequestsCharge_C;
extern String currentIdTag_A;
extern String currentIdTag_B;
extern String currentIdTag_C;
extern Preferences resumeTxn_A;
extern Preferences resumeTxn_B;
extern Preferences resumeTxn_C;

String Stored_Tag_ID_A = "";
String Stored_Tag_ID_B = "";
String Stored_Tag_ID_C = "";

uint8_t Online_offline_stop_A = 0XFF;
uint8_t Online_offline_stop_B = 0XFF;
uint8_t Online_offline_stop_C = 0XFF;

extern bool disp_evse_A;
extern bool disp_evse_B;
extern bool disp_evse_C;

WebSocketsClient webSocket;

volatile int gsm_interruptCounter = 0; // for counting interrupt
uint8_t gu8_gsm_setup_counter = 10;

// SmartChargingService *smartChargingService;
ChargePointStatusService *chargePointStatusService_A;
ChargePointStatusService *chargePointStatusService_B;
ChargePointStatusService *chargePointStatusService_C;

// Mertering Service declarations
MeteringService *meteringService;
ATM90E36 eic(5);
#define SS_EIC 5 // GPIO 5 chip_select pin
// #define SS_EIC_earth 2
//  ATM90E36 eic_earth(SS_EIC_earth);

SPIClass *hspi = NULL;

extern bool flag_evseReserveNow_A; // added by @mkrishna
extern bool flag_evseReserveNow_B; // added by @mkrishna
extern bool flag_evseReserveNow_C; // added by @mkrishna

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length);
bool webSocketConncted = false;

void setup_WIFI_OTA_get_1(void);
void performUpdate_WiFi_1(WiFiClient &updateSource, size_t updateSize);
void setup_WIFI_OTA_1(void);
void printPercent_1(uint32_t readLength, uint32_t contentLength);
void setup_WIFI_OTA_getconfig_1(void);

#define EVSE_CHARGE_POINT_FIRMWARE_OTA_VERSION String("0.0.0\r\n")
#define DEVICE_ID String("evse_001")

// // CHANGED BY AV
// uint8_t waitForResp(uint8_t timeout);
// void printPercent(uint32_t readLength, uint32_t contentLength);
// void pu(fs::FS &fs, const char *path);

// bool gsm_net = false; // ADDED BY AV

uint8_t gu8_OTA_update_flag = 0;
size_t E_reason;

// RFID declarations
#define MFRC_RST 22
#define MFRC_SS 15
MFRC522 mfrc522(MFRC_SS, MFRC_RST); // Create MFRC522 instance
SPIClass *hspiRfid = NULL;
extern uint8_t gu8_ota_update_available;

String currentIdTag;
String expiry_time;
extern String CP_Id_m;

// Bluetooth
#include "src/bluetoothConfig.h"
#define TIMEOUT_BLE 10000
extern BluetoothSerial SerialBT;
bool isInternetConnected = true;

// Ethernet
#if ETHERNET_ENABLED
#include "src/CustomEthernet.h"
// C:\Users\Mr Cool\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.6\cores\esp32/Server.h edited this file @wamique
//  Enter a MAC address for your controller below.
// https://github.com/arduino-libraries/Ethernet/issues/88

// sudo nano /home/raja/.arduino15/packages/esp32/hardware/esp32/1.0.6/libraries/WiFi/src/WiFiServer.h
// virtual void begin() =0;

uint8_t counter_ether = 0;
#endif

extern String reserve_currentIdTag_A;
extern String reserve_currentIdTag_B;
extern String reserve_currentIdTag_C;

wifi_connectivity_status evse_cp_wifi_connection;
ws_connectivity_status evse_cp_ws_connection;

bool flagswitchoffBLE = false;
int startBLETime = 0;
String ws_url_prefix_m = "";
String host_m = "";
int port_m = 0;
// String protocol_m = "";
String protocol = "ocpp1.6";
String key_m = "";
String ssid_m = "";
String path_m;

extern Preferences preferences;
String url_m = "";
Preferences resumeTxn;
String idTagData_m = "";
bool ongoingTxn_m = false;

// Led timers
ulong timercloudconnect = 0;
ulong timer_dwin_avail = 0;
void wifi_Loop();

#define NUM_OF_CONNECTORS 3

bool flag_internet;
bool flag_offline;

// internet
bool wifi_enable = false;
bool gsm_enable = false;
bool ethernet_enable = false;
uint8_t counter_websock = 0;

bool wifi_connect = false;
bool gsm_connect = false;
bool ethernet_connect = false;
bool offline_connect = false;

extern uint8_t offline_charging_A;
extern uint8_t offline_charging_B;
extern uint8_t offline_charging_C;

extern bool reservation_start_flag_A;
extern bool reservation_start_flag_B;
extern bool reservation_start_flag_C;

extern bool flag_AuthorizeRemoteTxRequests;

extern ulong timer1;
extern Preferences preferences;
Preferences change_config;

extern unsigned int meterSampleInterval;
extern unsigned int heartbeatInterval;

bool evse_A_unavail = false;
bool evse_B_unavail = false;
bool evse_C_unavail = false;

int readConnectorVal = 0;

volatile bool online_to_offline_flag = false;
volatile bool session_ongoing_flag = false;
// volatile bool session_on_offline_txn_completed = false;
volatile bool session_on_offline_txn_completed = false;

uint8_t wifi_connection_available = 0;

uint8_t gu8_online_flag = 0;
uint8_t gu8_offline_to_online_detect_flag = 0;
uint8_t gu8_offline_storage_count_A = 0;
uint8_t gu8_offline_storage_count_B = 0;
uint8_t gu8_offline_storage_count_C = 0;
uint8_t gu8_fetch_offline_txn_flag = 0;
uint8_t gu8_clear_offline_txn_flag = 0;
volatile int gs32_fetch_offline_txnlist_count = 0;
volatile int gs32_fetch_offline_txn_count = 0;

static int get_txn_list_A = 0;
static int get_txn_list_B = 0;
static int get_txn_list_C = 0;

extern uint8_t online_session_ongoing_A;
extern uint8_t online_session_ongoing_B;
extern uint8_t online_session_ongoing_C;

uint8_t session_ongoing_A_count = 0;
uint8_t session_ongoing_B_count = 0;
uint8_t session_ongoing_C_count = 0;

float getMetervalue_online_offline_A = 0;
float getMetervalue_online_offline_B = 0;
float getMetervalue_online_offline_C = 0;

extern uint8_t gu8_Remote_start_A;
extern uint8_t gu8_Remote_start_B;
extern uint8_t gu8_Remote_start_C;

extern String Tag_Id_A;
extern String Transaction_Id_A;
extern String Start_Meter_Value_A;

extern String Tag_Id_B;
extern String Transaction_Id_B;
extern String Start_Meter_Value_B;

extern String Tag_Id_C;
extern String Transaction_Id_C;
extern String Start_Meter_Value_C;

float gf32meterStop_A = 0;
float gf32meterStop_B = 0;
float gf32meterStop_C = 0;

volatile uint8_t gu8_websocket_begin_once = 0xFF;

extern String idTagData_A;
extern String idTagData_B;
extern String idTagData_C;
void Offline_Loop();
void wifi_Loop();
void connectToWebsocket();
void connectivity_Loop();
void cloudConnectivityLed_Loop();
extern bool flag_evseIsBooted_A;
void setup();
void loop();
void EVSE_ReadInput(MFRC522 *mfrc522);
bool assignEvseToConnector(String readIdTag, int readConnectorVal);
bool assignEvseToConnector_Offl(String readIdTag, int readConnectorVal);

// extern CRGB leds;

extern uint8_t online_session_ongoing_A;
extern uint8_t online_session_ongoing_B;
extern uint8_t online_session_ongoing_C;

extern DynamicJsonDocument get_txn_doc;

extern float online_charging_Enargy_A;
extern float online_charging_Enargy_B;
extern float online_charging_Enargy_C;
extern int globalmeterstartA;
extern int globalmeterstartB;
extern int globalmeterstartC;

extern float offline_charging_Enargy_A;
extern float offline_charging_Enargy_B;
extern float offline_charging_Enargy_C;

extern ChargePointStatus evse_ChargePointStatus_A;
extern ChargePointStatus evse_ChargePointStatus_B;
extern ChargePointStatus evse_ChargePointStatus_C;

uint8_t gu8_online_trans_initiated = 0;

bool flag_ping_sent = false;
uint8_t gu8_bootsuccess = 0;

extern bool fota_available;
extern Preferences evse_preferences_fota;

extern uint8_t start_session_A;
extern uint8_t start_session_B;
extern uint8_t start_session_C;

extern uint8_t Stop_Session_A;
extern uint8_t Stop_Session_B;
extern uint8_t Stop_Session_C;

uint32_t ble_lastTime = 0;
uint32_t ble_startTime = 0;
uint32_t ble_timerDelay = 30000;

extern int gs32lowcurcnt;
extern uint8_t currentCounterThreshold_A;
extern uint8_t currentCounterThreshold_B;
extern uint8_t currentCounterThreshold_C;

float minCurr_A_B_C = 0.25;
int lowcurrent_A_B_C = 45;
extern float strgf32mincurr;
extern float minCurr;
// bool ongoingtxpor_1 = false ;
bool otaenable_1 = false;
// bool onlineoffline_1 = false ;
// uint8_t gu8_ota_update_available = 0;
bool offline_charging = false;
extern uint8_t evse_WifiConnected;
extern uint8_t evse_ConnectWebsocket;
hw_timer_t *timer = NULL; // H/W timer defining (Pointer to the Structure)
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer()
{ // Defining Inerrupt function with IRAM_ATTR for faster access
  portENTER_CRITICAL_ISR(&timerMux);
  gsm_interruptCounter++;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void setup()
{
  // Test LED
  //   pinMode(16,OUTPUT);
  Serial.begin(115200);
  Master_setup();
  // pinMode(23, INPUT); // Earth detection.
  pinMode(earth_disconnect_pin, INPUT); // Earth detection.
  // https://arduino-esp8266.readthedocs.io/en/latest/Troubleshooting/debugging.html
  Serial.setDebugOutput(true);

#if DISPLAY_ENABLED
  setupDisplay_Disp();
  cloudConnect_Disp(false);
  checkForResponse_Disp();
#endif

#if LCD_DISPLAY_ENABLED
  lcd.begin(true, 26, 27); // If you are using more I2C devices using the Wire library use lcd.begin(false)
                           // this stop the library(LCD_I2C) from calling Wire.begin()
  lcd.backlight();
#endif

#if LCD_DISPLAY_ENABLED
  lcd.setCursor(0, 1);               // Or setting the cursor in the desired position.
  lcd.print("CHARGER INITIALIZING"); // You can make spaces using well... spaces
#endif

  if (DEBUG_OUT)
    Serial.println();
  if (DEBUG_OUT)
    Serial.println();
  if (DEBUG_OUT)
    Serial.println();

#if DWIN_ENABLED
  uint8_t err = 0;
  dwin_setup();
  change_page[9] = 0;
  err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0])); // page 0
  delay(10);
  err = DWIN_SET(not_avail, sizeof(not_avail) / sizeof(not_avail[0])); // status not available
  delay(10);
  err = DWIN_SET(not_avail, sizeof(not_avail) / sizeof(not_avail[0])); // status not available
  delay(10);
  err = DWIN_SET(clun, sizeof(clun) / sizeof(clun[0])); // cloud: not connected
  delay(10);
  err = DWIN_SET(clun, sizeof(clun) / sizeof(clun[0])); // cloud: not connected
  delay(10);
  CONN_UNAVAIL[4] = 0X66;
  err = DWIN_SET(CONN_UNAVAIL, sizeof(CONN_UNAVAIL) / sizeof(CONN_UNAVAIL[0]));
  CONN_UNAVAIL[4] = 0X71;
  err = DWIN_SET(CONN_UNAVAIL, sizeof(CONN_UNAVAIL) / sizeof(CONN_UNAVAIL[0]));
  CONN_UNAVAIL[4] = 0X7B;
  err = DWIN_SET(CONN_UNAVAIL, sizeof(CONN_UNAVAIL) / sizeof(CONN_UNAVAIL[0]));
  // err = DWIN_SET(clear_tap_rfid, sizeof(clear_tap_rfid) / sizeof(clear_tap_rfid[0]));
  // delay(50);
#endif

  for (uint8_t t = 4; t > 0; t--)
  {
    if (DEBUG_OUT)
      Serial.print(F("[SETUP] BOOT WAIT...Reconnect fix & GSM fix: "));
    if (DEBUG_OUT)
      Serial.println(VERSION);
    Serial.flush();
    delay(500);
  }

  requestLed(BLINKYWHITE_ALL, START, 1);
  requestForRelay(STOP, 1);
  requestForRelay(STOP, 2);
  requestForRelay(STOP, 3);

  requestforCP_OUT(STOP);

  // Ethernet init

#if DISPLAY_ENABLED
  cloudConnect_Disp(false);
  checkForResponse_Disp();
  delay(10);

  // statusOfCharger_Disp("NOT AVAILABLE");
  connAvail(1, "NOT AVAILABLE");
  checkForResponse_Disp();
  connAvail(2, "NOT AVAILABLE");
  checkForResponse_Disp();
  connAvail(3, "NOT AVAILABLE");
  checkForResponse_Disp();
  setHeader("RFID UNAVAILABLE");
  checkForResponse_Disp();
  delay(10);

#endif
#if 0
  SerialMon.printf("[SETUP][AC001] ESP32 portTICK_PERIOD_MS %u Milli second \r\n", portTICK_PERIOD_MS);
  SerialMon.printf("[SETUP][AC001] ESP32 Freertos tick rate %d Hz\r\n", configTICK_RATE_HZ);
  SerialMon.printf("[SETUP][AC001] ESP32 Cpu Frequency  %d Mhz \r\n", getCpuFrequencyMhz());  // In MHz
  SerialMon.printf("[SETUP][AC001] ESP32 Xtal Frequency %d Mhz \r\n", getXtalFrequencyMhz()); // In MHz
  SerialMon.printf("[SETUP][AC001] ESP32 Apb Frequency  %d Hz \r\n", getApbFrequency());      // In Hz
  SerialMon.printf("[SETUP][AC001] ESP32 total heap size  %d \r\n", ESP.getHeapSize());
  SerialMon.printf("[SETUP][AC001] ESP32 available heap  %d \r\n", ESP.getFreeHeap());
  SerialMon.printf("[SETUP][AC001] ESP32 lowest level of free heap since boot  %d \r\n", ESP.getMinFreeHeap());
  SerialMon.printf("[SETUP][AC001] ESP32 largest block of heap that can be allocated at once  %d \r\n", ESP.getMaxAllocPsram());
  SerialMon.printf("[SETUP][AC001] ESP32 chip Reversion  %d \r\n", ESP.getChipRevision());
  SerialMon.printf("[SETUP][AC001] ESP32 chip Model  %s \r\n", ESP.getChipModel());
  SerialMon.printf("[SETUP][AC001] ESP32 chip Cores  %d \r\n", ESP.getChipCores());
  SerialMon.printf("[SETUP][AC001] ESP32 SDK Version  %s \r\n", ESP.getSdkVersion());
  SerialMon.printf("[SETUP][AC001] ESP32 Flash Chip Size  %d \r\n", ESP.getFlashChipSize());
  SerialMon.printf("[SETUP][AC001] ESP32 Flash Chip Speed  %d \r\n", ESP.getFlashChipSpeed());
  SerialMon.printf("[SETUP][AC001] ESP32 Sketch Size  %d \r\n", ESP.getSketchSize());
  SerialMon.printf("[SETUP][AC001] ESP32 Sketch MD5  %x \r\n", ESP.getSketchMD5());
  SerialMon.printf("[SETUP][AC001] ESP32 Free Sketch Space  %d \r\n", ESP.getFreeSketchSpace());

  SerialMon.print("[SETUP][AC001] CHARGE POINT VERSION : ");
  SerialMon.println(VERSION);
  SerialMon.printf("[SETUP][AC001] CHARGE POINT VENDOR : %s \r\n", CHARGE_POINT_VENDOR);
  SerialMon.printf("[SETUP][AC001] CHARGE POINT MODEL : %s \r\n", CHARGE_POINT_MODEL);
  // SerialMon.print("[SETUP][AC001] ARDUINO ESP32 GIT DESC RELEASE VERSION : ");
  // SerialMon.println(String(ARDUINO_ESP32_GIT_DESC));
  SerialMon.printf("[SETUP][AC001] ARDUINO ESP32 RELEASE VERSION : %s \r\n", ARDUINO_ESP32_RELEASE);
  SerialMon.printf("[SETUP][AC001] ARDUINO ESP32 GIT VERSION : %x \r\n", ARDUINO_ESP32_GIT_VER);
  SerialMon.println("[SETUP][AC001] Compiled: " __DATE__ ", " __TIME__ ", " __VERSION__);

  // SerialMon.println( F("[SETUP][AC001] FILE NAME : "__FILE__));
  SerialMon.print(F("[SETUP][AC001] Arduino IDE version: "));
  SerialMon.println(ARDUINO, DEC);
#endif
  Serial.println(F("*** EVSE Booting ......!***"));
  Serial.print(F("EVSE FIRMWARE VERSION:  "));
  Serial.print(F("[SETUP] ESP32 Freertos tick rate "));
  Serial.println(configTICK_RATE_HZ);
  Serial.print(F("[SETUP] ESP32 Freertos portTICK_PERIOD_MS 0"));
  Serial.println(portTICK_PERIOD_MS);
  Serial.print(F("[SETUP] ESP32 Cpu Frequency  "));
  Serial.println(getCpuFrequencyMhz());
  Serial.print(F("[SETUP] ESP32 Xtal Frequency "));
  Serial.println(getXtalFrequencyMhz());
  Serial.print(F("[SETUP] ESP32 Apb Frequency "));
  Serial.println(getApbFrequency());
  Serial.print(F("[SETUP] ESP32 total heap size "));
  Serial.println(ESP.getHeapSize());
  Serial.print(F("[SETUP] ESP32 available heap  "));
  Serial.println(ESP.getFreeHeap());
  Serial.print(F("[SETUP] ESP32 lowest level of free heap since boot "));
  Serial.println(ESP.getMinFreeHeap());
  Serial.print(F("[SETUP] ESP32 largest block of heap that can be allocated at once  "));
  Serial.println(ESP.getMaxAllocPsram());
  Serial.print(F("[SETUP] ESP32 chip Reversion  "));
  Serial.println(ESP.getChipRevision());
  Serial.print(F("[SETUP] ESP32 chip Model "));
  Serial.println(ESP.getChipModel());
  Serial.print(F("[SETUP] ESP32 chip Cores "));
  Serial.println(ESP.getChipCores());
  Serial.print(F("[SETUP] ESP32 SDK Version  "));
  Serial.println(ESP.getSdkVersion());
  Serial.print(F("[SETUP] ESP32 Flash Chip Size  "));
  Serial.println(ESP.getFlashChipSize());
  Serial.print(F("[SETUP] ESP32 Flash Chip Speed  "));
  Serial.println(ESP.getFlashChipSpeed());
  Serial.print(F("[SETUP] ESP32 Sketch Size  "));
  Serial.println(ESP.getSketchSize());
  Serial.print(F("[SETUP] ESP32 Sketch MD5"));
  Serial.println(ESP.getSketchMD5());
  Serial.print(F("[SETUP] ESP32 Free Sketch Space  "));
  Serial.println(ESP.getFreeSketchSpace());
  // ble_mac_add = WiFi.macAddress();
  Serial.print(F("[SETUP] ESP32 WIFI MAC "));
  // Serial.println(ble_mac_add);
  Serial.print(F("[SETUP]EVSE DEVICE VERSION :"));
  Serial.println(VERSION);
  Serial.print(F("[SETUP]EVSE CHARGE POINT VENDOR : "));
  Serial.println(CHARGE_POINT_VENDOR);
  Serial.print(F("[SETUP]EVSE CHARGE POINT MODEL : "));
  Serial.println(CHARGE_POINT_MODEL);
  // Serial.println("[SETUP]EVSE ARDUINO ESP32 GIT DESC RELEASE VERSION : " +String(ARDUINO_ESP32_GIT_DESC));
  // Serial.print(F("[SETUP]EVSE ARDUINO ESP32 RELEASE VERSION :"));
  // Serial.println(ARDUINO_ESP32_RELEASE);
  // Serial.print(F("[SETUP]EVSE ARDUINO ESP32 GIT VERSION : "));
  // Serial.println(ARDUINO_ESP32_GIT_VER);
  Serial.print(F("[SETUP]EVSE Compiled: "));
  Serial.print(__DATE__);
  Serial.println(__TIME__);
  // Serial.println(/* __VERSION__ */);
  Serial.print(F("[SETUP] FILE NAME : "));
  Serial.println(__FILE__);
  Serial.print(F("[SETUP] Arduino IDE version: "));
  Serial.print(ARDUINO, DEC);

  evse_preferences_fota.begin("fota_url", false);
  String evse_fota_uri = evse_preferences_fota.getString("fota_uri", "");
  uint32_t evse_fota_retries = evse_preferences_fota.getUInt("fota_retries", 0);
  String evse_fota_date = evse_preferences_fota.getString("fota_date", "");
  bool evse_fota_avail = evse_preferences_fota.getBool("fota_avial", false);
  evse_preferences_fota.end();

  Serial.println("FOTA UPDATE states ");
  Serial.println("evse_fota_uri :  " + String(evse_fota_uri));
  Serial.println("evse_fota_retries :  " + String(evse_fota_retries));
  Serial.println("evse_fota_date :  " + String(evse_fota_date));
  Serial.println("evse_fota_avail :  " + String(evse_fota_avail));

#if OFFLINE_TXN_TEST_CASE

  requestClearTxnList();
  volatile int lget_txn_list_x = requestGetTxnList();
  int get_txn_list_x = (++lget_txn_list_x);
  Serial.println("get_txn_list_B : " + String(get_txn_list_x));
  // requestSendStartTxn(String(get_txn_list_x ), String(2));
  // requestSendStopTxn(String(get_txn_list_x ), String(2));
  requestGetTxn(String(1));
  requestClearTxnList();
  SerialMon.println("Control Hold on ....!");
  while (1)
    ;

#endif

#if LCD_ENABLED
  lcd.clear();
  lcd.setCursor(6, 0);
  lcd.print("PREPARING");
  lcd.setCursor(5, 3);
  lcd.print("PLEASE WAIT");
#endif

  /************************Preferences***********************************************/
  /*  preferences.begin("credentials",false);

    ws_url_prefix_m = preferences.getString("ws_url_prefix",""); //characters
    if(ws_url_prefix_m.length() > 0){
    Serial.println("Fetched WS URL success: " + String(ws_url_prefix_m));
    }else{
    Serial.println("Unable to Fetch WS URL / Empty");
    }
    delay(100);

    host_m = preferences.getString("host","");
    if(host_m.length() > 0){
    Serial.println("Fetched host data success: "+String(host_m));
    }else{
    Serial.println("Unable to Fetch host data / Empty");
    }
    delay(100);

    port_m = preferences.getInt("port",0);
    if(port_m>0){
    Serial.println("Fetched port data success: "+String(port_m));
    }else{
    Serial.println("Unable to Fetch port Or port is 0000");
    }
    delay(100);

    protocol = preferences.getString("protocol","");
    if(protocol.length() > 0){
    Serial.println("Fetched protocol data success: "+String(protocol));
    }else{
    Serial.println("Unable to Fetch protocol");
    } */

  urlparser();
  // Added this not to break.
  preferences.begin("credentials", false);
  ssid_m = preferences.getString("ssid", "");
  if (ssid_m.length() > 0)
  {
    Serial.println("Fetched SSID: " + String(ssid_m));
  }
  else
  {
    Serial.println(F("Unable to Fetch SSID"));
  }

  key_m = preferences.getString("key", "");
  if (key_m.length() > 0)
  {
    Serial.println("Fetched Key: " + String(key_m));
  }
  else
  {
    Serial.println(F("Unable to Fetch key"));
  }

  wifi_enable = preferences.getBool("wifi", 0);
  Serial.println("Fetched Wifi data: " + String(wifi_enable));

  gsm_enable = preferences.getBool("gsm", 0);
  Serial.println("Fetched Gsm data: " + String(gsm_enable));

  offline_connect = preferences.getBool("offline", 0);
  Serial.println("Fetched Offline data: " + String(offline_connect));

  /*
   * @brief : ethernet - functionality
   * Added by G. Raja Sumant 30/08/2022
   */
  ethernet_enable = preferences.getBool("ethernet", 0);
  Serial.println("Fetched ethernet data: " + String(ethernet_enable));
   gs32lowcurcnt = preferences.getInt("lowcurcnt", 0);
   if(gs32lowcurcnt == 0)
   {
    preferences.putInt("lowcurcnt", lowcurrent_A_B_C);
    gs32lowcurcnt = preferences.getInt("lowcurcnt", 0);
   }
  Serial.println("gs32lowcurcnt " + String(gs32lowcurcnt));

   currentCounterThreshold_A = gs32lowcurcnt;
   currentCounterThreshold_B = gs32lowcurcnt;
   currentCounterThreshold_C = gs32lowcurcnt;

   Serial.println("currentCounterThreshold_A: " + String(currentCounterThreshold_A));
   Serial.println("currentCounterThreshold_B: " + String(currentCounterThreshold_B));
   Serial.println("currentCounterThreshold_C: " + String(currentCounterThreshold_C));
   strgf32mincurr = preferences.getFloat("mincurr", 0);
  if(strgf32mincurr == 0)
   {
      preferences.putFloat("mincurr", minCurr_A_B_C);
      strgf32mincurr = preferences.getFloat("mincurr", 0);
   }
   Serial.println("strgf32mincurr: " + String(strgf32mincurr));
   minCurr = strgf32mincurr;
   Serial.println("minCurr: " + String(minCurr));

  otaenable_1 = preferences.getBool("otaenable", 0);
  Serial.println("otaenable: " + String(otaenable_1));
  //  onlineoffline_1 = preferences.getBool("onlineoffline", 0);
  //  Serial.println("onlineoffline: " + String(onlineoffline_1));
  preferences.end();

  if (ethernet_enable)
  {
    wifi_enable = false;
  }

  // #if LCD_DISPLAY_ENABLED
  //   lcd.clear();
  //   lcd.setCursor(1, 1);
  //   if (wifi_enable)
  //   {
  //     lcd.print("CONNECTING TO WIFI");
  //   }
  //   else if (gsm_enable)
  //   {
  //     lcd.print("CONNECTING TO 4G");
  //   }
  // #endif

  /******************************************************************************/
  /*
   * @brief change_config: Feature added by G. Raja Sumant
   * 09/07/2022
   * The values stored using change configuration must
   * be restored after a reboot as well.
   */

  change_config.begin("configurations", false);
  meterSampleInterval = change_config.getInt("meterSampleInterval", 0);
  heartbeatInterval = change_config.getInt("heartbeatInterval", 50);
  flag_AuthorizeRemoteTxRequests = change_config.getBool("authRemoteStart", false); // skip auth by default
  change_config.end();
  /*             EVSE Offline Functionality  is Enabled                         */
  /*
   * @brief : Change made to accomodate ethernet
   */
  if ((wifi_enable == false) && (gsm_enable == false) && !ethernet_enable)
  {
    offline_connect = true; // commented by shiva
    Serial.println("Both WiFi and GSM is Disabled....!");
    Serial.println("EVSE Offline Functionality is Enabled ....!");
    requestLed(GREEN, START, 1);
    requestLed(GREEN, START, 2);
    requestLed(GREEN, START, 3);
  }
  /******************************************************************************/
  // Ideally preferences should be closed.
  // WiFi
  wifi_connect = wifi_enable;
  gsm_connect = gsm_enable;
  ethernet_connect = ethernet_enable;
  bool internet = false;
  int counter_wifiNotConnected = 0;
  int counter_gsmNotConnected = 0;
//Device ID based OTA implementation(WIFI & 4G) @Abhigna
  if (wifi_enable == true)
  {
    evse_cp_wifi_connection = wifi_connect_initiated;
    WiFi.begin(ssid_m.c_str(), key_m.c_str());
    evse_cp_wifi_connection = wifi_connecting;
    if (otaenable_1 == 1 || evse_fota_avail == 1) // checks for OTA from configuration based or DEVICE ID based @Abhigna
    {
      for (uint8_t otawaittime = 0; otawaittime <= 5; otawaittime++)
      {
        Serial.println("OTA WAIT");
      }
#if EVSE_FOTA_ENABLE_WIFI
      // ptr_leds->Yellow_led();
      setup_WIFI_OTA_get_1();
      requestLed(ORANGE, START, 1);
      requestLed(ORANGE, START, 2);
      requestLed(ORANGE, START, 3);

#if LCD_DISPLAY_ENABLED
      lcd.clear();
      lcd.setCursor(1, 1);
      lcd.print("CHARGER UPDATING");
#endif
      preferences.begin("credentials", false);
      preferences.putBool("otaenable", false); // making otaenable false, once checked for update @Abhigna
      otaenable_1 = preferences.getBool("otaenable", 0);
      Serial.println("OTA_ENABLE:" + String(otaenable_1));
      preferences.end();
      evse_preferences_fota.begin("fota_url", false);
      evse_preferences_fota.putString("fota_uri", "");
      evse_preferences_fota.putUInt("fota_retries", 0);
      evse_preferences_fota.putString("fota_date", "");
      evse_preferences_fota.putBool("fota_avial", false);
      evse_preferences_fota.end();

      switch (gu8_OTA_update_flag)
      {
      case 2:
        Serial.println("OTA update available, In Switch ...!");
        setup_WIFI_OTA_1();
        break;
      case 3:
        Serial.println("No OTA update available, In Switch ...!");
        break;
      default:
        Serial.println("default case OTA update, In Switch ...! \r\n gu8_OTA_update_flag :" + String(gu8_OTA_update_flag));
        break;
      }
#endif
    }
  }
  else if (gsm_enable == true)
  {
    if (!FFat.begin(true))
    {
      Serial.println("Mount Failed");
    }
    else
    {
      if (otaenable_1 == 1 || evse_fota_avail == 1) // checks for OTA from configuration based & every power on @Abhigna
      {
#if EVSE_FOTA_ENABLE_4G
        Serial.println("File system mounted");
        OTA_4G_setup4G();
        Serial.println("******checking for OTA******");
        requestLed(ORANGE, START, 1);
        requestLed(ORANGE, START, 2);
        requestLed(ORANGE, START, 3);
#if LCD_DISPLAY_ENABLED
        lcd.clear();
        lcd.setCursor(1, 1);
        lcd.print("CHARGER UPDATING");
#endif
        preferences.begin("credentials", false);
        preferences.putBool("otaenable", false);
        otaenable_1 = preferences.getBool("otaenable", 0);
        Serial.println("OTA_ENABLE:" + String(otaenable_1));
        preferences.end();

        OTA_4G_setup_4G_OTA_get();
        Serial.println("******OTA check done******");
        evse_preferences_fota.begin("fota_url", false);
        evse_preferences_fota.putString("fota_uri", "");
        evse_preferences_fota.putUInt("fota_retries", 0);
        evse_preferences_fota.putString("fota_date", "");
        evse_preferences_fota.putBool("fota_avial", false);
        evse_preferences_fota.end();
        delay(1000);
        if (gu8_ota_update_available == 1)
        {
          OTA_4G_setup_4G_OTA();
          Serial.println("******OTA update done******");
        }
      }
#endif
    }
  }
#if BLE_ENABLE // Time for Initialising and configuring bluetooth for 30 sec @Abhigna
  Serial.println("BLE Configuration...");
  // startingBTConfig();
  setupBLT();
  ble_startTime = millis();
  ble_lastTime = millis();
  while ((ble_lastTime - ble_startTime) <= ble_timerDelay)
  {
    ble_lastTime = millis();
    bluetooth_Loop();
  }
  Serial.println("Free Heap.....before Bluetooth");
  Serial.println(ESP.getFreeHeap());
  SerialBT.flush();
  SerialBT.disconnect();
  SerialBT.end();
  Serial.println("Free Heap.....after Bluetooth");
  Serial.println(ESP.getFreeHeap());
#endif
  // DEVICE INITIALIZING.................. STOPS FOR 30 SEC - In order to avoid disconnections with PulseCMS//@Abhigna
  int8_t gu8_DeviceInit = 30;
  while (gu8_DeviceInit > 0)
  {
    gu8_DeviceInit--;
    Serial.println("DEVICE INITIALIZING......" + String(gu8_DeviceInit));
    delay(1000);
  }

#if 1
  // WiFi
  if (!offline_connect)
  {
#if LCD_DISPLAY_ENABLED
    lcd.clear();
    lcd.setCursor(1, 1);
    if (wifi_enable)
    {
      wifi_init();
      delay(2000);
      connectToWebsocket();
      lcd.print("CONNECTING TO WIFI");
      delay(1000);
    }
    else if (gsm_enable)
    {
      lcd.print("CONNECTING TO 4G");
    }
#endif
// Serial.println("control stopped...........");
//     while (1)
//     {

//     }
    while ((internet == false) && (online_to_offline_flag == false))
    {
      Serial.println("Internet loop");
#if DWIN_ENABLED
      err = DWIN_SET(nct, sizeof(nct) / sizeof(nct[0]));
      delay(50);
      err = DWIN_SET(clun, sizeof(clun) / sizeof(clun[0]));
      delay(50);
      err = DWIN_SET(nct, sizeof(nct) / sizeof(nct[0]));
      delay(50);
      err = DWIN_SET(clun, sizeof(clun) / sizeof(clun[0]));
      delay(50);
#endif
      // bluetooth_Loop();
      #if 0
      if (wifi_enable == true && wifi_connect == true)
      {
        Serial.println(F("Waiting for WiFi Connction..."));

        if (WiFi.status() == WL_CONNECTED)
        {
          internet = true;
          gsm_connect = false;
          Serial.println(F("Connected via WiFi"));
          evse_cp_wifi_connection = wifi_connected;
#if DISPLAY_ENABLED
          cloudConnect_Disp(3);
          checkForResponse_Disp();
#endif
#if LCD_DISPLAY_ENABLED
          lcd.clear();
          lcd.setCursor(1, 1);
          // lcd.print("STATUS: WIFI");
          lcd.print("CONNECTED VIA WIFI");
#endif
#if DWIN_ENABLED
          // Cloud : WiFi
          err = DWIN_SET(wi, sizeof(wi) / sizeof(wi[0]));
          delay(50);
          err = DWIN_SET(wi, sizeof(wi) / sizeof(wi[0]));
          delay(50);
          // err = DWIN_SET(avail, sizeof(avail) / sizeof(avail[0]));
          // delay(50);
          // err = DWIN_SET(avail, sizeof(avail) / sizeof(avail[0]));
          // delay(50);
#endif
          delay(100);
          connectToWebsocket();
        }
        else if (WiFi.status() != WL_CONNECTED)
        {
          Serial.print(".");
          delay(100);
          // bluetooth_Loop();
          // wifi_Loop();
          Serial.println("Wifi Not Connected: " + String(counter_wifiNotConnected));
#if DWIN_ENABLED
          err = DWIN_SET(nct, sizeof(nct) / sizeof(nct[0]));
          delay(50);
          err = DWIN_SET(not_avail, sizeof(not_avail) / sizeof(not_avail[0]));
          delay(50);
#endif
          if (counter_wifiNotConnected++ > 50)
          {
            online_to_offline_flag = true;
            counter_wifiNotConnected = 0;

            if (gsm_enable == true)
            {
              WiFi.disconnect();
              wifi_connect = false;
              gsm_connect = true;
            }
          }
        }
      }
      #endif
      //Changed functionality of WIFI @Abhigna
      if (wifi_enable == true)
      {
        Serial.println("Waiting for WiFi Connction...");
        if(evse_WifiConnected)
        {
          internet = true;
          evse_cp_wifi_connection = wifi_connected;
          Serial.println(F("Connected via WiFi"));
          #if LCD_DISPLAY_ENABLED
            lcd.clear();
            lcd.setCursor(1, 1);
            // lcd.print("STATUS: WIFI");
            lcd.print("CONNECTED VIA WIFI");
          #endif
          #if DISPLAY_ENABLED
          cloudConnect_Disp(3);
          checkForResponse_Disp();
          #endif
          #if DWIN_ENABLED
          // Cloud : WiFi
          err = DWIN_SET(wi, sizeof(wi) / sizeof(wi[0]));
          delay(50);
          err = DWIN_SET(wi, sizeof(wi) / sizeof(wi[0]));
          delay(50);
          // err = DWIN_SET(avail, sizeof(avail) / sizeof(avail[0]));
          // delay(50);
          // err = DWIN_SET(avail, sizeof(avail) / sizeof(avail[0]));
          // delay(50);
          #endif
          // connectToWebsocket();
        }
        else
        {
          online_to_offline_flag = true;
        }
      }
      else if (ethernet_enable && ethernet_connect)
      {
#if ETHERNET_ENABLED
        // SPI.begin();
        ethernetSetup();
        while (Ethernet.linkStatus() != LinkON)
        {
          ethernetLoop();
          delay(200);
          Serial.println(".");
          if (counter_ether++ > 10)
          {
            break;
          }
        }
        if (counter_ether <= 10)
        {
          internet = true;
          gsm_connect = false;
          ethernet_connect = true;
          Serial.println("[Eth] Connected to Internet");
#if DWIN_ENABLED
          err = DWIN_SET(et, sizeof(et) / sizeof(et[0]));
          delay(10);
          err = DWIN_SET(et, sizeof(et) / sizeof(et[0]));
          delay(10);
#endif
          connectToWebsocket();
        }
        else
        {
          internet = false;
          if (gsm_enable == true)
          {
            ethernet_connect = false;
            gsm_connect = true;
#if DWIN_ENABLED
            err = DWIN_SET(nct, sizeof(nct) / sizeof(nct[0]));
            delay(50);
            err = DWIN_SET(not_avail, sizeof(not_avail) / sizeof(not_avail[0]));
            delay(50);
#endif
          }
        }
#endif
      }
      else if (gsm_enable == true && gsm_connect == true)
      {
        // SetupGsm();                                     //redundant @optimise
        // ConnectToServer();
        if (!client.connected())
        {
          gsm_Loop();
          // bluetooth_Loop();
          Serial.println("GSM not Connected: " + String(counter_gsmNotConnected));
#if DWIN_ENABLED
          err = DWIN_SET(nct, sizeof(nct) / sizeof(nct[0]));
          delay(50);
          err = DWIN_SET(not_avail, sizeof(not_avail) / sizeof(not_avail[0]));
          delay(50);
#endif
          if (counter_gsmNotConnected++ > 1)
          { // 2 == 5min
            counter_gsmNotConnected = 0;

            // offline_connect = true;
            // break;

            if (wifi_enable == true)
            {
              wifi_connect = true;
              gsm_connect = false;
            }
            if (ethernet_enable)
            {
              ethernet_connect = true;
              gsm_connect = false;
            }
          }
        }
        else if (client.connected())
        {
          internet = true;
          wifi_connect = false;
          Serial.println(F("connected via 4G"));
#if DISPLAY_ENABLED
          cloudConnect_Disp(2);
          checkForResponse_Disp();
#endif
#if LCD_DISPLAY_ENABLED
          lcd.clear();
          lcd.setCursor(1, 1);
          lcd.print("CONNECTED VIA 4G");

#endif
#if DWIN_ENABLED
          // err = DWIN_SET(avail, sizeof(avail) / sizeof(avail[0]));
          // delay(50);
          // err = DWIN_SET(avail, sizeof(avail) / sizeof(avail[0]));
          // delay(50);
          err = DWIN_SET(g, sizeof(g) / sizeof(g[0]));
          delay(50);
          err = DWIN_SET(g, sizeof(g) / sizeof(g[0]));
          delay(50);
#endif
        }
      }
      else if (offline_connect == true && gsm_connect == false && wifi_connect == false && ethernet_connect == false)
      { // redundous but for sake of safe coding testing all flags
        // Offline_Loop();
        Serial.println(F("[SETUP] ******OFFLINE loop enabled!*****"));
        break;
      }
    }
  }
#endif
  // SPI Enable for Energy Meter Read
  hspi = new SPIClass(HSPI); // Init SPI bus
  hspi->begin();
  pinMode(SS_EIC, OUTPUT); // HSPI SS Pin

  // ARAI testing
  // pinMode(SS_EIC_earth, OUTPUT); //HSPI SS Pin

  // SPI Enable for RFID
  hspiRfid = new SPIClass(HSPI);
  hspiRfid->begin();
  mfrc522.PCD_Init(); // Init MFRC522

  // Serial.println("closing preferences");
  // preferences.end();
  ocppEngine_initialize(&webSocket, 4096); // default JSON document size = 2048

  chargePointStatusService_A = new ChargePointStatusService(&webSocket);
  chargePointStatusService_B = new ChargePointStatusService(&webSocket);
  chargePointStatusService_C = new ChargePointStatusService(&webSocket);

  getChargePointStatusService_A()->setConnectorId(1);
  getChargePointStatusService_B()->setConnectorId(2);
  getChargePointStatusService_C()->setConnectorId(3);
  //  getChargePointStatusService_A()->authorize();

  EVSE_A_setup();
  EVSE_B_setup();
  EVSE_C_setup();

  meteringService = new MeteringService(&webSocket);

  // set system time to default value; will be without effect as soon as the BootNotification conf arrives
  setTimeFromJsonDateString("2021-22-12T11:59:55.123Z"); // use if needed for debugging
  uint16_t lu_websocket_count = 100;
  if (!offline_connect)
  {
    if (DEBUG_OUT)
      Serial.println(F("Web Socket Connction..."));
    // while ((!webSocketConncted && wifi_connect == true) || (!webSocketConncted && ETHERNET_ENABLED))
    while ((!webSocketConncted && wifi_connect == true && internet == true) && (lu_websocket_count))
    {
      lu_websocket_count--;

      Serial.print(F("*"));
      delay(50); // bit**
      webSocket.loop();
      // bluetooth_Loop();
      if (lu_websocket_count < 0)
      {
        lu_websocket_count = 0;
      }
    }
    if (lu_websocket_count > 0)
    {
      evse_cp_ws_connection = ws_connected;
    }
    // evse_cp_ws_connection = ws_connected;
  }

  EVSE_A_initialize();
  EVSE_B_initialize();
  EVSE_C_initialize();

  if (ethernet_enable)
  {
    if (DEBUG_OUT)
      Serial.println("Waiting for Web Socket Connction...");
    while (!webSocketConncted)
    {
      Serial.print("*");
      delay(50); // bit**
      webSocket.loop();
      // bluetooth_Loop();
      if (counter_websock++ > 50)
      {
        break;
      }
    }
  }
  // int8_t gu8_DeviceInit = 60;
  // while (gu8_DeviceInit >= 0)
  // {
  //   gu8_DeviceInit--;
  //   Serial.println("DEVICE INITIALIZING......" + String(gu8_DeviceInit));
  //   delay(1000);
  // }
  Serial.println(F("End of Setup"));
  // startBLETime = millis();
#if DWIN_ENABLED
  err = DWIN_SET(clear_tap_rfid, sizeof(clear_tap_rfid) / sizeof(clear_tap_rfid[0]));
  delay(50);
  err = DWIN_SET(tap_rfid, sizeof(tap_rfid) / sizeof(tap_rfid[0]));
  delay(50);

  // err = DWIN_SET(avail, sizeof(avail) / sizeof(avail[0]));
  // delay(50);
#endif

#if DISPLAY_ENABLED
  connAvail(1, "AVAILABLE");
  checkForResponse_Disp();
  connAvail(2, "AVAILABLE");
  checkForResponse_Disp();
  connAvail(3, "AVAILABLE");
  checkForResponse_Disp();
  setHeader("TAP RFID");
  checkForResponse_Disp();
  delay(500);
#endif
  timer = timerBegin(0, 80, true);             // timer 0, prescalar: 80, UP counting
  timerAttachInterrupt(timer, &onTimer, true); // Attach interrupt
  timerAlarmWrite(timer, 1000000, true);       // Match value= 1000000 for 1 sec. delay.
  timerAlarmEnable(timer);

  // EvseDevStatus_connector_1 = flag_EVSE_is_Booted;
  EVSE_Connector_Status = EVSE_A;
  getChargePointStatusService_A()->setChargePointstatus(NOT_SET);
  getChargePointStatusService_B()->setChargePointstatus(NOT_SET);
  getChargePointStatusService_C()->setChargePointstatus(NOT_SET);
}

void loop()
{
  Serial.println("*******S*******");
#if BLE_ENABLE & 0
  if (millis() - startBLETime < TIMEOUT_BLE)
  {
    // bluetooth_Loop();
    flagswitchoffBLE = true;
  }
  else
  {
    if (flagswitchoffBLE == true)
    {
      flagswitchoffBLE = false;
      Serial.println(F("Disconnecting BT"));
      // SerialBT.println("Wifi Connected");
      SerialBT.println(F("Disconnecting BT"));
      delay(100);
      SerialBT.flush();
      SerialBT.disconnect();
      SerialBT.end();
      Serial.println(ESP.getFreeHeap());
    }
  }
#endif

#if DWIN_ENABLED
  display_avail();
#endif

  /*
  * @brief By G. Raja Sumant who compared it with previous version.
  This is not necessary. It is being taken care inside wifi_gsm_connect loop.
  */
  /*
  if(wifi_connect){
      if((WiFi.status() == WL_CONNECTED) && (webSocketConncted == true) && (isInternetConnected == true)){
        Serial.print("[Wifi]");
        ocppEngine_loop();
        webSocket.loop();
      }

    }else if(gsm_connect){
      if(client.connected() == true){
        Serial.println("[GSM]");
        ocppEngine_loop();
        gsmOnEvent();
      }
    }*/
  if (webSocketConncted == 0 && gsm_enable == true)
  {
    if (gsm_interruptCounter > 0)
    {
      portENTER_CRITICAL(&timerMux);
      gsm_interruptCounter--;
      portEXIT_CRITICAL(&timerMux);
      gu8_gsm_setup_counter++; // counting total interrupt
      Serial.print("An interrupt as occurred. Total number: ");
      Serial.println(gu8_gsm_setup_counter);
    }
  }

  emergencyRelayClose_Loop_A();
  emergencyRelayClose_Loop_B();
  emergencyRelayClose_Loop_C();

  lowcurrent_loop_A_B_C(); // irresptive of offline online when device is charging and not taking current @Abhigna

  EVSE_ReadInput(&mfrc522);

  EVSE_Led_loop(); // Led implementation - Based on Chargepoint status @Abhigna

  if (offline_connect)
  {
    if (offline_charging_A)
    {
      EVSE_A_offline_Loop();
    }

    if (offline_charging_B)
    {
      EVSE_B_offline_Loop();
    }

    if (offline_charging_C)
    {
      EVSE_C_offline_Loop();
    }

    // EVSE_A_LED_loop();
    // EVSE_B_LED_loop();
    // EVSE_C_LED_loop();//Commented by Abhigna - implemented seperate function for LED's

    // if (online_session_ongoing_A == 1)
    // {
    //   // requestLed(BLINKYGREEN, START, 1);
    //   // online_session_ongoing_A++;
    // }

    // if (online_session_ongoing_B == 1)
    // {
    //   // requestLed(BLINKYGREEN, START, 2);
    //   // online_session_ongoing_B++;
    // }

    // if (online_session_ongoing_C == 1)
    // {
    //   // requestLed(BLINKYGREEN, START, 3);
    //   // online_session_ongoing_C++;
    // }
  }
  else
  {
    if (online_to_offline_flag == false)
    {
      if (webSocketConncted == 1)
      {
        ocppEngine_loop();
      }

#if 0
#if WIFI_ENABLED || ETHERNET_ENABLED
  webSocket.loop();
#if ETHERNET_ENABLED
    if(flag_evseIsBooted_A == true){
      Ethernet.maintain();  
    }
    ethernetLoop();
    //  webSocket.loop();
#endif
#endif
#endif

      EVSE_A_Reservation_loop();
      EVSE_B_Reservation_loop();
      EVSE_C_Reservation_loop();

     
#if 0
      switch (EVSE_Connector_Status)
      {
      case EVSE_A:
        Serial.println("Connector 1 Executing...!");
        EVSE_A_loop();
        // if (gu8_bootsuccess == 1)
        // {
        //   getChargePointStatusService_A()->loop();
        // }
        EVSE_Connector_Status = EVSE_B;
        break;
      case EVSE_B:
        Serial.println("Connector 2 Executing...!");
        EVSE_B_loop();
        // if (gu8_bootsuccess == 1)
        // {
        //   getChargePointStatusService_B()->loop();
        // }
        EVSE_Connector_Status = EVSE_C;
        break;
      case EVSE_C:
        Serial.println("Connector 3 Executing...!");
        EVSE_C_loop();
        // if (gu8_bootsuccess == 1)
        // {
        //   getChargePointStatusService_C()->loop();
        // }
        EVSE_Connector_Status = EVSE_A;
        break;
      }
#endif
      EVSE_A_loop();
      getChargePointStatusService_A()->loop();
      EVSE_B_loop();
      getChargePointStatusService_B()->loop();
      EVSE_C_loop();
      getChargePointStatusService_C()->loop();
      cloudConnectivityLed_Loop();
      internetLoop();
      webSocket.loop();
    if((getChargePointStatusService_A()->getChargePointstatus() == NOT_SET) &&
       (getChargePointStatusService_B()->getChargePointstatus() == NOT_SET) &&
       (getChargePointStatusService_C()->getChargePointstatus() == NOT_SET) && 
       (gu8_bootsuccess == 1))
     {
        getChargePointStatusService_A()->loop();
        getChargePointStatusService_B()->loop();
        getChargePointStatusService_C()->loop();
     }


#if OFFLINE_STORAGE_ENABLE
      if ((gu8_offline_to_online_detect_flag == 1) && (session_on_offline_txn_completed == true))
      {
        gu8_offline_to_online_detect_flag = 0;
        session_on_offline_txn_completed = false;
        requestLed(BLUE, START, 1);
        requestLed(BLUE, START, 2);
        requestLed(BLUE, START, 3);

        // Whenever this condition is true then LED indication as BLUE Indication
        Serial.println("In Main loop offline_to_online_detect_flag done...!");
// Get Transaction List from Slave offline Transaction Database i.e. Sqlite3 dB to Master.
#if OFFLINE_STORAGE_ENABLE
        int ls32get_txn_list = requestGetTxnList();
        gs32_fetch_offline_txnlist_count = ls32get_txn_list;

        if (ls32get_txn_list > 0)
        {
          gu8_fetch_offline_txn_flag = 1;
          gu8_clear_offline_txn_flag = 0;
        }
#endif
      }
      // Depending on the Get Transaction list, Execute the Offline Transaction send the loop to get Index base Offline Transaction
      gs32_fetch_offline_txn_count++;
      if (gs32_fetch_offline_txn_count >= OFFLINE_START_STOP_TXN_GUARD_INTERVAL)
      {
        gs32_fetch_offline_txn_count = 0;
        // requestLed(BLUE, START, 1);
        // requestLed(BLUE, START, 2);
        // requestLed(BLUE, START, 3);

        if (gu8_fetch_offline_txn_flag == 1)
        {
          gu8_fetch_offline_txn_flag = 0;

          int index = gs32_fetch_offline_txnlist_count;
#if OFFLINE_STORAGE_ENABLE
          requestGetTxn(String(index));
#endif
          // To Do
          //  check the charging info storage that is semi charging info or complete charging info
          //  i.e. semi offline charging info means ONLINE START TRANSACTION and OFFLINE STOP TRANSACTION
          //  i.e. complete offline charging info means OFFLINE START TRANSACTION and OFFLINE STOP TRANSACTION
          //  check the Transaction id
          //  if Transaction id is Unique Transaction id -> i.e. 0xFFFF then consider as complete offline charging info
          //    then Send Start Transaction Packet to CMS and LED indication as BLUE Indication

#if 0
          String lidtag = get_txn_doc["gtxn"]["idTag"].as<char *>();
          int    lc_conId = get_txn_doc["gtxn"]["conId"];
          String lc_txnid = get_txn_doc["gtxn"]["txnid"].as<char *>();
          String lc_startDate = get_txn_doc["gtxn"]["startDate"].as<char *>();
          String lc_start_metervalue = get_txn_doc["gtxn"]["start_metervalue"].as<char *>();
          String lc_stopDate = get_txn_doc["gtxn"]["stopDate"].as<char *>();
          String lc_stop_metervalue = get_txn_doc["gtxn"]["stop_metervalue"].as<char *>();
          String lc_units = get_txn_doc["gtxn"]["units"].as<char *>();
          String lc_reason
#endif

          // String lidtag = "";
          // int lc_conId = 0;
          // String lc_txnid = "";
          // String lc_startDate = "";
          // String lc_start_metervalue = "";
          // String lc_stopDate = "";
          // String lc_stop_metervalue = "";
          // String lc_units = "";
          // String lc_reason = "";

          lidtag = "";
          lc_conId = 0;
          lc_txnid = "";
          lc_startDate = "";
          lc_start_metervalue = "";
          lc_stopDate = "";
          lc_stop_metervalue = "";
          lc_units = "";
          lc_reason = "";

          lidtag = get_txn_doc["gtxn"]["idTag"].as<char *>();
          lc_conId = get_txn_doc["gtxn"]["conId"];
          lc_txnid = get_txn_doc["gtxn"]["txnid"].as<char *>();
          lc_startDate = get_txn_doc["gtxn"]["startDate"].as<char *>();
          lc_start_metervalue = get_txn_doc["gtxn"]["start_metervalue"].as<char *>();
          lc_stopDate = get_txn_doc["gtxn"]["stopDate"].as<char *>();
          lc_stop_metervalue = get_txn_doc["gtxn"]["stop_metervalue"].as<char *>();
          lc_units = get_txn_doc["gtxn"]["units"].as<char *>();
          lc_reason = get_txn_doc["gtxn"]["reason"].as<char *>();

          gc_offline_txn_startDate = lc_startDate;
          gc_offline_txn_start_metervalue = lc_start_metervalue;
          gc_offline_txn_stopDate = lc_stopDate;
          gc_offline_txn_stop_metervalue = lc_stop_metervalue;
          gc_offline_txn_reason = lc_reason;

          //   Send Stop Transaction Packet to CMS and LED indication as BLUE Indication
          //   offlineStopTxn(idTag, offlinestarttxnId, connectorId);
          // else Transaction id is valid or Server/CMS Sent Transaction id -> i.e. 0x5432111221 then consider as semi offline charging info
          //   then Send Stop Transaction Packet to CMS and LED indication as BLUE Indication
          gs32_offlinestartStop_connId = lc_conId;
          Serial.println("lc_txnid :" + String(lc_txnid));
          Serial.println("gs32_offlinestartStop_connId :" + String(gs32_offlinestartStop_connId));

          if (lc_txnid.equals("0xFFFF") == true)
          {

            gs32_offlinestartStoptxn_update_flag = 1;
            // int offlinestarttxnId = offlineStratTxn(lidtag, lc_conId);
            // gs32_offlineAuthorizeIdTag = 0;
            offlineAuthorizeTxn(lidtag);

            // if(gs32_offlineAuthorizeIdTag == 1)
            // {

            //   offlineStratTxn(lidtag, lc_conId);
            //   offlineStopTxn(lidtag, gs32_offlinestarttxnId, lc_conId);
            // }
            // else
            // {
            //   Serial.print("gs32_fetch_offline_txnlist_count 1 : " + String(gs32_fetch_offline_txnlist_count));
            //   gs32_fetch_offline_txnlist_count--; // Decrement the fetch offline transaction count
            //   Serial.print("gs32_fetch_offline_txnlist_count 2 : " + String(gs32_fetch_offline_txnlist_count));

            //   if (gs32_fetch_offline_txnlist_count > 0)
            //   {
            //     gu8_fetch_offline_txn_flag = 1;
            //     gu8_clear_offline_txn_flag = 0;
            //     gs32_offlineAuthorizeIdTag = 0;
            //     gs32_offlinestartStoptxn_update_guard_Interval = OFFLINE_START_STOP_TXN_GUARD_INTERVAL;
            //   }
            //   else if (gs32_fetch_offline_txnlist_count <= 0)
            //   {
            //     gs32_fetch_offline_txnlist_count = 0;
            //     gu8_fetch_offline_txn_flag = 0;
            //     gs32_offlineAuthorizeIdTag = 0;
            //     gu8_clear_offline_txn_flag = 1;
            //   }
            // }
            // offlineStopTxn(lidtag, gs32_offlinestarttxnId, lc_conId);
          }
          else
          {
            int loffline_txnid = lc_txnid.toInt();
            switch (lc_conId)
            {
            case 1:
              gs32_offlinestarttxnId_A = lc_txnid.toInt();
              break;

            case 2:
              gs32_offlinestarttxnId_B = lc_txnid.toInt();
              break;
            case 3:
              gs32_offlinestarttxnId_C = lc_txnid.toInt();
              break;
            default:
              break;
            }

            gs32_offlinestartStoptxn_update_flag = 2;
            // offlineStopTxn(lidtag, lc_txnid, lc_conId);
            offlineStopTxn(lidtag, loffline_txnid, lc_conId);
          }
        }

        if (gs32_offlineAuthorizeIdTag == 1)
        {
          gs32_offlinestarttxn_update_flag = 1;
          offlineStratTxn(lidtag, lc_conId);
          offlineStopTxn(lidtag, gs32_offlinestarttxnId, lc_conId);
        }
      }
      if (gu8_clear_offline_txn_flag == 1)
      {
        gu8_clear_offline_txn_flag = 0;
// requestLed(BLUE, START, 1);
// requestLed(BLUE, START, 2);
// requestLed(BLUE, START, 3);
// Clear the Transaction List in Slave offline Transaction Database i.e. Sqlite3 dB
#if OFFLINE_STORAGE_ENABLE
        requestClearTxnList();
#endif
        Serial.println("Offline Txn Update completed...!");
      }
#endif
    }
    else if (online_to_offline_flag == true)
    {
      if (online_session_ongoing_A == 1)
      {
        if (offline_charging_A)
        {
          EVSE_A_offline_Loop();
        }

#if OFFLINE_STORAGE_ENABLE
        if (session_ongoing_A_count == 1)
        {
          session_ongoing_A_count = 2;
#if OFFLINE_STORAGE_ENABLE
          int lget_txn_list_A = requestGetTxnList();
          get_txn_list_A = (++lget_txn_list_A);
          Serial.println("get_txn_list_A : " + String(get_txn_list_A));
          requestSendStartTxn(String(get_txn_list_A), String(1));
#endif
        }
        else if (session_ongoing_A_count == 2)
        {
          // String(float((online_charging_Enargy_A - globalmeterstartA) / 1000))
          // float lmeterStop_A = meteringService->currentEnergy_A();
          // float lmeterStop_A = float((online_charging_Enargy_A - globalmeterstartA) / 1000);
          // if(offline_charging_A)
          // {
          //   lmeterStop_A = float((offline_charging_Enargy_A - globalmeterstartA) / 1000);
          // }
          float lmeterStop_A = 0;
          gf32meterStop_A = 0;

          if (offline_charging_A)
          {
            // lmeterStop_A = float((offline_charging_Enargy_A - globalmeterstartA) / 1000);
            lmeterStop_A = offline_charging_Enargy_A;
            Serial.println("offline_charging_Enargy_A : " + String(offline_charging_Enargy_A));
          }
          else
          {
            // lmeterStop_A = float((online_charging_Enargy_A - globalmeterstartA) / 1000);
            lmeterStop_A = online_charging_Enargy_A;
            Serial.println("online_charging_Enargy_A : " + String(online_charging_Enargy_A));
          }
          gf32meterStop_A = lmeterStop_A;
          Serial.println("lmeterStop_A : " + String(lmeterStop_A));
          requestSendStopTxn(String(get_txn_list_A), String(1), String(lmeterStop_A));
          session_on_offline_txn_completed = true;
        }
#endif
      }

      if (online_session_ongoing_B == 1)
      {
        if (offline_charging_B)
        {
          EVSE_B_offline_Loop();
        }
#if OFFLINE_STORAGE_ENABLE
        if (session_ongoing_B_count == 1)
        {
          session_ongoing_B_count = 2;
          volatile int lget_txn_list_B = requestGetTxnList();
          get_txn_list_B = (++lget_txn_list_B);
          Serial.println("get_txn_list_B : " + String(get_txn_list_B));
          requestSendStartTxn(String(get_txn_list_B), String(2));
        }
        else if (session_ongoing_B_count == 2)
        {
          // float lmeterStop_B = meteringService->currentEnergy_B();
          // float lmeterStop_B = float((online_charging_Enargy_B - globalmeterstartB) / 1000);
          // if (offline_charging_B)
          // {
          //   lmeterStop_B = float((offline_charging_Enargy_B - globalmeterstartB) / 1000);
          // }

          float lmeterStop_B = 0;
          gf32meterStop_B = 0;
          if (offline_charging_B)
          {
            // lmeterStop_B = float((offline_charging_Enargy_B - globalmeterstartB) / 1000);
            lmeterStop_B = offline_charging_Enargy_B;
            Serial.println("offline_charging_Enargy_B : " + String(offline_charging_Enargy_B));
          }
          else
          {
            // lmeterStop_B = float((online_charging_Enargy_B - globalmeterstartB) / 1000);
            lmeterStop_B = online_charging_Enargy_B;
            Serial.println("online_charging_Enargy_B : " + String(online_charging_Enargy_B));
          }
          gf32meterStop_B = lmeterStop_B;
          Serial.println("lmeterStop_B : " + String(lmeterStop_B));
          requestSendStopTxn(String(get_txn_list_B), String(2), String(lmeterStop_B));
          session_on_offline_txn_completed = true;
        }

#endif
      }

      if (online_session_ongoing_C == 1)
      {
        if (offline_charging_C)
        {

          EVSE_C_offline_Loop();
        }
#if OFFLINE_STORAGE_ENABLE
        if (session_ongoing_C_count == 1)
        {
          session_ongoing_C_count = 2;

          volatile int lget_txn_list_C = requestGetTxnList();
          get_txn_list_C = (++lget_txn_list_C);
          Serial.println("get_txn_list_C : " + String(get_txn_list_C));
          requestSendStartTxn(String(get_txn_list_C), String(3));
        }
        else if (session_ongoing_C_count == 2)
        {
          // float lmeterStop_C = meteringService->currentEnergy_C();
          // float lmeterStop_C = float((online_charging_Enargy_C - globalmeterstartC) / 1000);
          float lmeterStop_C = 0;
          gf32meterStop_C = 0;

          if (offline_charging_C)
          {
            // lmeterStop_C = float((offline_charging_Enargy_C - globalmeterstartC) / 1000);
            lmeterStop_C = offline_charging_Enargy_C;
            Serial.println("offline_charging_Enargy_C : " + String(offline_charging_Enargy_C));
          }
          else
          {
            // lmeterStop_C = float((online_charging_Enargy_C - globalmeterstartC) / 1000);
            lmeterStop_C = online_charging_Enargy_C;
            Serial.println("online_charging_Enargy_C : " + String(online_charging_Enargy_C));
          }
          gf32meterStop_C = lmeterStop_C;
          Serial.println("lmeterStop_C : " + String(lmeterStop_C));

          requestSendStopTxn(String(get_txn_list_C), String(3), String(lmeterStop_C));

          session_on_offline_txn_completed = true;
        }
#endif
      }
      // }
    }
    webSocket.loop();
    cloudConnectivityLed_Loop();
  }
  // if there is atleast one connector charging in offline we do not connect websocket @Abhigna
  if ((offline_charging_A == 1) || (offline_charging_B == 1) || (offline_charging_C == 1))
  {
    Serial.println("Offline charging offline_charging_A:" + String(offline_charging_A));
    Serial.println("Offline charging offline_charging_B:" + String(offline_charging_B));
    Serial.println("Offline charging offline_charging_C:" + String(offline_charging_C));
    offline_charging = true;
  }
  else
  {
    offline_charging = false;
  }

#if 1
  // commenting for testing wifi_loop switch logic
  // if (session_ongoing_flag == false && webSocketConncted == 0)
  // if there is atleast one connector charging in offline we do not connect websocket @Abhigna
  if ((offline_charging == false) && (webSocketConncted == 0))
  {
    Serial.println("session_ongoing_flag Internet loop 1");
    if (gu8_websocket_begin_once == 1)
    {
      Serial.println("evse_ConnectWebsocket");
      gu8_websocket_begin_once = 0;
      Serial.println("host_m Name:" + String(host_m));
      Serial.println("port_m Name:" + String(port_m));
      Serial.println("path_m Name:" + String(path_m));
      Serial.println("protocol Name:" + String(protocol));
      // webSocket.begin(host_m, port_m, path_m, protocol);
      // connectToWebsocket();
      delay(700);
    }
    internetLoop();
    webSocket.loop();
  }
#endif

  // if (session_ongoing_flag == false)
  // {
  //   Serial.println("session_ongoing_flag Internet loop 1");
  //   internetLoop();
  //   webSocket.loop();
  // }

  // getChargePointStatusService_A()->loop();
  // getChargePointStatusService_B()->loop();
  // getChargePointStatusService_C()->loop();

  meteringService->loop();

#if LCD_DISPLAY_ENABLED
  stateTimer();
  // disp_lcd_meter();//commented by Abhigna - Implemeted another function for Display 20*4
#endif

#if DISPLAY_ENABLED
  stateTimer();
  disp_lcd_meter();
#endif

#if DWIN_ENABLED
  stateTimer();
  disp_dwin_meter();
#endif

#if CP_ACTIVE
  ControlP_loop();
#endif
  // Checks for Device Available for all 3 connectors and restarts when OTA Update is requested from server@Abhigna
  // if ((evse_ChargePointStatus_A == Available) && (evse_ChargePointStatus_B == Available) && (evse_ChargePointStatus_C == Available))
  if((getChargePointStatusService_A()->getChargePointstatus() == Available) &&
     (getChargePointStatusService_B()->getChargePointstatus() == Available) &&
     (getChargePointStatusService_C()->getChargePointstatus() == Available))
  {
    Serial.println("CHARGER available");
    if (fota_available == 1)
    {
      Serial.println("Fota available");
      fota_available = 0;
      ESP.restart();
    }
  }

  // if(getChargePointStatusService_A()->getChargePointstatus() != Unavailable)

  Serial.println("Websocket: " + String(webSocketConncted));
  Serial.printf("%4d - SIGNAL STRENGTH \n", WiFi.RSSI());
  Serial.println("FREE HEAP");
  Serial.println(ESP.getFreeHeap());
  Serial.println("\n*****E*****");
}

#if DWIN_ENABLED
#if 1
/***************************************EVSE_READINPUT BLOCK*********************************************************/
String readIdTag = "";
void EVSE_ReadInput(MFRC522 *mfrc522)
{ // this funtion should be called only if there is Internet
  readIdTag = "";
  bool result = false;
  unsigned long tout = millis();
  int8_t readConnectorVal = 0;
  readIdTag = readRfidTag(true, mfrc522);
  if (readIdTag.equals("") == false)
  {
    // EVSE_StopTxnRfid(readIdTag);
    // readConnectorVal = requestConnectorStatus();
#if DWIN_ENABLED
    change_page[9] = 3; // change to page 3 and wait for input
    uint8_t err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
    delay(10);
    err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
    delay(10);
    err = DWIN_SET(select_connector, sizeof(select_connector) / sizeof(select_connector[0]));
    delay(10);
    flush_dwin();
    requestLed(BLUE, START, 1);
    requestLed(BLUE, START, 2);
    requestLed(BLUE, START, 3);
    while (millis() - tout < 15000)
    {

      readConnectorVal = dwin_input();
      /*
        @brief: If we want to use both the physical switches/display
      */
      // readConnectorVal = requestConnectorStatus();
      if (readConnectorVal > 0)
      {
        // change_page[9] = 6; // change to page 3 and wait for input
        // err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
        /*switch (readConnectorVal)
        {
          case 1: err = DWIN_SET(cid1, sizeof(cid1) / sizeof(cid1[0]));
            break;
          case 2: err = DWIN_SET(cid2, sizeof(cid2) / sizeof(cid2[0]));
            break;
          case 3: err = DWIN_SET(cid3, sizeof(cid3) / sizeof(cid3[0]));
            break;
        }*/
        // bool result = assignEvseToConnector(readIdTag, readConnectorVal);

        if (offline_connect)
        {
          currentIdTag = readIdTag;
          Serial.println(F("******selecting via offline*****"));
          result = assignEvseToConnector_Offl(readIdTag, readConnectorVal);
        }
        else
        {
          uint8_t result_checker = 0;
          Serial.println(F("******selecting via online*****"));
          // result = assignEvseToConnector(readIdTag, readConnectorVal);
          /*
           * @brief ReserveNow for A,B,C condition checker.
           */

          if (reservation_start_flag_A)
          {
            if (readIdTag.equals(reserve_currentIdTag_A))
            {
              result = assignEvseToConnector(readIdTag, readConnectorVal);
              if (result)
              {
                result_checker++;
              }
            }
            else
            {
              result = false;
            }
          }

          if (reservation_start_flag_B)
          {
            if (readIdTag.equals(reserve_currentIdTag_B))
            {
              result = assignEvseToConnector(readIdTag, readConnectorVal);
              if (result)
              {
                result_checker++;
              }
            }
            else
            {
              result = false;
            }
          }
          if (reservation_start_flag_C)
          {
            if (readIdTag.equals(reserve_currentIdTag_C))
            {
              result = assignEvseToConnector(readIdTag, readConnectorVal);
              if (result)
              {
                result_checker++;
              }
            }
            else
            {
              result = false;
            }
          }

          if (result_checker == 0)
            result = assignEvseToConnector(readIdTag, readConnectorVal);
        }
        if (result == true)
        {
          Serial.println(F("Attached/Detached EVSE to the requested connector"));
          break;
        }
        else
        {
          Serial.println(F("Unable To attach/detach EVSE to the requested connector"));
          // err = DWIN_SET(CONN_UNAVAIL, sizeof(CONN_UNAVAIL) / sizeof(CONN_UNAVAIL[0]));
          // delay(10);

          // break;
        }
      }
      else
      {
        Serial.println(F("Invalid Connector Id Received"));
        /*err = DWIN_SET(CONN_UNAVAIL, sizeof(CONN_UNAVAIL) / sizeof(CONN_UNAVAIL[0]));
          delay(10);
          change_page[9] = 0; // change to page 3 and wait for input
          uint8_t err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
          delay(10);*/
        // break;
        // delay(2000);
      }
    }
    change_page[9] = 0; // Now it should come back to home page
    err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
    delay(10);
#endif
  }
  // delay(100);
  /*  uint8_t err = DWIN_SET(avail, sizeof(avail) / sizeof(avail[0]));
    delay(10);
    change_page[9] = 0; // change to page 0 and wait for input
    err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
    delay(10);

    change_page[9] = 0; // change to page 0 and wait for input
    err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
    delay(10);

    change_page[9] = 0; // change to page 0 and wait for input
    err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
    delay(10);*/
}
#endif

#if 0
/***************************************EVSE_READINPUT BLOCK*********************************************************/
String readIdTag = "";
void EVSE_ReadInput(MFRC522* mfrc522) {    // this funtion should be called only if there is Internet
  readIdTag = "";
  unsigned long tout = millis();
  int8_t readConnectorVal = 0;
  readIdTag = readRfidTag(true, mfrc522);
  if (readIdTag.equals("") == false) {
    //EVSE_StopTxnRfid(readIdTag);
    //readConnectorVal = requestConnectorStatus();
#if DWIN_ENABLED
    change_page[9] = 2; // change to page 3 and wait for input
    uint8_t err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
    delay(10);
    err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
    delay(10);
    err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
    delay(10);
    flush_dwin();
    
      //readConnectorVal = dwin_input();
      /*
        @brief: If we want to use both the physical switches/display
      */
      readConnectorVal = requestConnectorStatus();
      if (readConnectorVal > 0) {
        change_page[9] = 6; // change to page 3 and wait for input
        err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
        switch (readConnectorVal)
        {
          case 1: err = DWIN_SET(cid1, sizeof(cid1) / sizeof(cid1[0]));
            break;
          case 2: err = DWIN_SET(cid2, sizeof(cid2) / sizeof(cid2[0]));
            break;
          case 3: err = DWIN_SET(cid3, sizeof(cid3) / sizeof(cid3[0]));
            break;
        }
        bool result = assignEvseToConnector(readIdTag, readConnectorVal);
        if (result == true) {
          Serial.println(F("Attached/Detached EVSE to the requested connector"));
          //break;
        } else {
          Serial.println(F("Unable To attach/detach EVSE to the requested connector"));
          //err = DWIN_SET(CONN_UNAVAIL, sizeof(CONN_UNAVAIL) / sizeof(CONN_UNAVAIL[0]));
          //delay(10);
           change_page[9] = 0; // change to page 3 and wait for input
     err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));

          //break;
        }
      } else {
        Serial.println(F("Invalid Connector Id Received"));
          change_page[9] = 0; // change to page 3 and wait for input
     err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
        /*err = DWIN_SET(CONN_UNAVAIL, sizeof(CONN_UNAVAIL) / sizeof(CONN_UNAVAIL[0]));
          delay(10);
          change_page[9] = 0; // change to page 3 and wait for input
          uint8_t err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
          delay(10);*/
        //break;
        //delay(2000);
      }

#endif
  }
  //delay(100);
  /*  uint8_t err = DWIN_SET(avail, sizeof(avail) / sizeof(avail[0]));
    delay(10);
    change_page[9] = 0; // change to page 0 and wait for input
    err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
    delay(10);

    change_page[9] = 0; // change to page 0 and wait for input
    err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
    delay(10);

    change_page[9] = 0; // change to page 0 and wait for input
    err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
    delay(10);*/

}
#endif

#else
  bool flag_offline_stopA = false;
  bool flag_offline_stopB = false;
  bool flag_offline_stopC = false;

  /***************************************EVSE_READINPUT BLOCK*********************************************************/
  String readIdTag = "";
  void EVSE_ReadInput(MFRC522 * mfrc522)
  { // this funtion should be called only if there is Internet

    readIdTag = "";

    readIdTag = readRfidTag(true, mfrc522);
    if (readIdTag.equals("") == false)
    {
      // EVSE_StopTxnRfid(readIdTag);
#if LCD_DISPLAY_ENABLED
      lcd.clear();
      lcd.setCursor(4, 1);
      lcd.print("RFID TAPPED");
      lcd.setCursor(0, 2);
      lcd.print("PLEASE SELECT OUTPUT");
#endif
#if DISPLAY_ENABLED
      connAvail(1, "PUSH TO START/STOP");
      checkForResponse_Disp();
      connAvail(2, "PUSH TO START/STOP");
      checkForResponse_Disp();
      connAvail(3, "PUSH TO START/STOP");
      checkForResponse_Disp();
#endif
      readConnectorVal = requestConnectorStatus();
      // Set Bit for Display - @Abhigna
      if (readConnectorVal > 0)
      {
        switch (readConnectorVal)
        {
        case 1:
          start_session_A = 0;
          Display_Set(start_session_A);
          break;
        case 2:
          start_session_B = 1;
          Display_Set(start_session_B);
          break;
        case 3:
          start_session_C = 2;
          Display_Set(start_session_C);
          break;
        }
      }
      if (readConnectorVal > 0)
      {
        bool result = false;
// if (offline_connect || online_to_offline_flag)
#if 0
        if (webSocketConncted == 0)//Only Stop in Offline no start in offline - @Abhigna
        {
          switch (readConnectorVal)
          {
          case 1:
            if (flag_evRequestsCharge_A == true)
            {
              // idTagData_A = resumeTxn_A.getString("idTagData_A", "");
              // Serial.println("currentidtag_AAAAAAA"+String(idTagData_A));
              currentIdTag = readIdTag;
              Serial.println("Current id_tag:" + String(currentIdTag));
              Serial.println("selecting via offline");
              if (currentIdTag_A == currentIdTag)
              {
                Online_offline_stop_A = 1;
                Serial.println("sTOPPING IN OFFLINe");
                result = assignEvseToConnector_Offl(readIdTag, readConnectorVal);
              }
            }
            break;
          case 2:
            if (flag_evRequestsCharge_B == true)
            {
              currentIdTag = readIdTag;
              Serial.println("selecting via offline");
              if (currentIdTag_B == currentIdTag)
              {
                Online_offline_stop_B = 1;
                Serial.println("sTOPPING IN OFFLINe2");
                result = assignEvseToConnector_Offl(readIdTag, readConnectorVal);
              }
            }
            break;
          case 3:
            if (flag_evRequestsCharge_C == true)
            {
              currentIdTag = readIdTag;
              Serial.println("selecting via offline");
              if (currentIdTag_C == currentIdTag)
              {
                Online_offline_stop_C = 1;
                Serial.println("sTOPPING IN OFFLINe");
                result = assignEvseToConnector_Offl(readIdTag, readConnectorVal);
              }
            }
            break;
          }
        }
#endif
#if 1
        if (webSocketConncted == 0)
        {
          currentIdTag = readIdTag;
          Serial.println("******selecting via offline*****");
          result = assignEvseToConnector_Offl(readIdTag, readConnectorVal);
        }
#endif
        else
        {
          // gu8_online_trans_initiated = 1;
          Serial.println("selecting via online*****");
          result = assignEvseToConnector(readIdTag, readConnectorVal);
          switch (readConnectorVal)
          {
          case 1:
            Stored_Tag_ID_A = readIdTag;
            break;
          case 2:
            Stored_Tag_ID_B = readIdTag;
            break;
          case 3:
            Stored_Tag_ID_C = readIdTag;
            break;
          }
        }

        if (result == true)
        {
          Serial.println("Select output");
#if LCD_ENABLED
          lcd.clear();

          lcd.setCursor(0, 1);
          lcd.print("ATTACHED/DETACHED");
          lcd.setCursor(0, 2);
          lcd.print("PLEASE WAIT");
#endif

#if DISPLAY_ENABLED
          flag_freeze = true;
          switch (readConnectorVal)
          {
          case 1:
            connAvail(1, "SELECTED PLEASE WAIT");
            break;
          case 2:
            connAvail(2, "SELECTED PLEASE WAIT");
            break;
          case 3:
            connAvail(3, "SELECTED PLEASE WAIT");
            break;
          }
          checkForResponse_Disp();
#endif
        }
        else
        {
          Serial.println("Unable To attach/detach");
#if DISPLAY_ENABLED
          switch (readConnectorVal)
          {
          case 1:
            connAvail(1, "UNABLE TO SELECT");
            break;
          case 2:
            connAvail(2, "UNABLE TO SELECT");
            break;
          case 3:
            connAvail(3, "UNABLE TO SELECT");
            break;
          }
          checkForResponse_Disp();
#endif
#if LCD_DISPLAY_ENABLED
          lcd.clear();
          lcd.setCursor(0, 1);
          lcd.print("UNABLE TO");
          lcd.setCursor(0, 2);
          lcd.print("SELECT OUTPUT");
#endif
        }
      }
      else
      {
        Serial.println("Invalid Connector Id Received");
#if DISPLAY_ENABLED
        flag_freeze = false;
        flag_unfreeze = true;
        /*connAvail(1,"AVAILABLE");
          checkForResponse_Disp();
          connAvail(2,"AVAILABLE");
          checkForResponse_Disp();
          connAvail(3,"AVAILABLE");
          checkForResponse_Disp();*/
        /*switch(readConnectorVal)
          {
             case 1: connAvail(1,"INVALID CONNECTOR ID");
             break;
             case 2:connAvail(2,"INVALID CONNECTOR ID");
             break;
             case 3:connAvail(3,"INVALID CONNECTOR ID");
             break;
          }*/
        // checkForResponse_Disp();
#endif
#if LCD_DISPLAY_ENABLED
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("NO OUTPUT");
        lcd.setCursor(0, 2);
        lcd.print("RECEIVED");
#endif
        delay(2000);
      }
    }
    delay(100);
#if 0
  if (Online_offline_stop_A == 1)
    {
      Online_offline_stop_A = 0;
      getChargePointStatusService_A()->startTransaction(transactionId_A);
      reasonForStop_A = 3;
      EVSE_A_StopSession();
    }
    if (Online_offline_stop_B == 1)
    {
      Online_offline_stop_B = 0;
      getChargePointStatusService_B()->startTransaction(transactionId_B);
      reasonForStop_B = 3;
      EVSE_B_StopSession();
    }
    if (Online_offline_stop_C == 1)
    {
      Online_offline_stop_C = 0;
      getChargePointStatusService_C()->startTransaction(transactionId_C);
      reasonForStop_C = 3;
      EVSE_C_StopSession();
    }
#endif
  }

#endif

#if DWIN_ENABLED
extern bool disp_evse_A;
extern bool disp_evse_B;
extern bool disp_evse_C;
void display_avail()
{
  // Serial.println(F("[DWIN MAIN] Trying to update status"));
  // if (millis() - timer_dwin_avail > 5000)
  //{
  // timer_dwin_avail = millis();
  uint8_t faulty_count = 0;
  uint8_t err = 0;
  if (isInternetConnected)
  {
    if (notFaulty_A && !EMGCY_FaultOccured_A && !disp_evse_A && !evse_A_unavail && !flag_nopower)
    {
      avail[4] = 0X66;
      // avail[5] = 0X00;
      err = DWIN_SET(avail, sizeof(clear_avail) / sizeof(clear_avail[0]));
      err = DWIN_SET(avail, sizeof(avail) / sizeof(avail[0]));
      delay(10);
    }
    else
    {
      // Serial.println(F("****Display A faulty or charging****"));
      faulty_count++;
      // err = DWIN_SET(unavail, sizeof(unavail) / sizeof(unavail[0]));
      // delay(10);
    }

    if (notFaulty_C && !EMGCY_FaultOccured_C && !disp_evse_C && !evse_C_unavail && !flag_nopower)
    {

      avail[4] = 0X7B;
      avail[5] = 0X00;
      err = DWIN_SET(avail, sizeof(clear_avail) / sizeof(clear_avail[0]));
      err = DWIN_SET(avail, sizeof(avail) / sizeof(avail[0]));
    }
    else
    {
      // Serial.println(F("****Display C faulty or charging****"));
      faulty_count++;
    }
  }
  else

  {
    Serial.println(F("Internet not available : Hence not updating status."));
    /*
     * @brief: Here all 3 must go to unavailable! By G. Raja Sumant 02/09/2022
     */
#if DWIN_ENABLED
    if (!flag_nopower)
    {
      CONN_UNAVAIL[4] = 0X66;
      err = DWIN_SET(CONN_UNAVAIL, sizeof(CONN_UNAVAIL) / sizeof(CONN_UNAVAIL[0]));
      CONN_UNAVAIL[4] = 0X71;
      err = DWIN_SET(CONN_UNAVAIL, sizeof(CONN_UNAVAIL) / sizeof(CONN_UNAVAIL[0]));
      CONN_UNAVAIL[4] = 0X7B;
      err = DWIN_SET(CONN_UNAVAIL, sizeof(CONN_UNAVAIL) / sizeof(CONN_UNAVAIL[0]));
    }
    // When all 3 are not available!
    faulty_count = 3;
#endif
  }
  /*
   * @brief : This is kept outside as it has some issues inside
   */
  if (isInternetConnected)
  {
    if (notFaulty_B && !EMGCY_FaultOccured_B && !disp_evse_B && !evse_B_unavail && !flag_nopower)
    {
#if DWIN_ENABLED
      avail[4] = 0X71;
      // avail[5] = 0X00;
      err = DWIN_SET(avail, sizeof(clear_avail) / sizeof(clear_avail[0]));
      err = DWIN_SET(avail, sizeof(avail) / sizeof(avail[0]));
      delay(10);
#endif
    }
    else
    {
      // Serial.println(F("****Display A faulty or charging****"));
      faulty_count++;
      // err = DWIN_SET(unavail, sizeof(unavail) / sizeof(unavail[0]));
      // delay(10);
    }
  }
  if (faulty_count >= 3)
  {
// When all 3 are not available!
#if DWIN_ENABLED
    err = DWIN_SET(unavail, sizeof(unavail) / sizeof(unavail[0]));
    delay(10);
#endif
  }
  else
  {
#if DWIN_ENABLED
    err = DWIN_SET(clear_tap_rfid, sizeof(clear_tap_rfid) / sizeof(clear_tap_rfid[0]));
    err = DWIN_SET(tap_rfid, sizeof(tap_rfid) / sizeof(tap_rfid[0]));
#endif
    // Serial.println(F("[TAP RFID]"));
  }
  //}
}
#endif

bool assignEvseToConnector(String readIdTag, int readConnectorVal)
{
  bool status = false;
  unsigned long tout = millis();
  if (readConnectorVal == 1)
  {
    if (getChargePointStatusService_A()->getIdTag() == readIdTag && getChargePointStatusService_A()->getTransactionId() != -1)
    {
      // stop session
      gu8_online_trans_initiated = 0;
      Serial.println("[EVSE_A] Stoppingtxn-RFID TAP");
      EVSE_A_StopSession();
      session_ongoing_flag = false;
      session_on_offline_txn_completed = false;
      status = true;
    }
    else if (getChargePointStatusService_A()->inferenceStatus() == ChargePointStatus::Available)
    {
      if (DEBUG_OUT)
        Serial.print("ChargePointStatus::Available .\n");
      getChargePointStatusService_A()->authorize(readIdTag, readConnectorVal);
      gu8_online_trans_initiated = 1;
      status = true;
    }
    else if (getChargePointStatusService_A()->inferenceStatus() == ChargePointStatus::Reserved)
    {
      if (flag_evseReserveNow_A)
      {

        if (DEBUG_OUT)
          Serial.print("ReserveIdTag_A \n");

        getChargePointStatusService_A()->authorize(readIdTag, readConnectorVal); // authorizing twice needed to be improvise
        status = true;
      }
    }
#if 0 
    else
    {
      if (DEBUG_OUT)
        Serial.print(F("EVSE_setOnAuthentication_A : ChargePointStatus::Available .\n"));
      getChargePointStatusService_A()->authorize(readIdTag, readConnectorVal);
      status = true;
    }
#endif
  }
  else if (readConnectorVal == 2)
  {
    if (getChargePointStatusService_B()->getIdTag() == readIdTag && getChargePointStatusService_B()->getTransactionId() != -1)
    {
      gu8_online_trans_initiated = 0;
      Serial.println("[EVSE_B] Stoppingtxn-RFID TAP");
      EVSE_B_StopSession();
      session_ongoing_flag = false;
      session_on_offline_txn_completed = false;
      status = true;
    }
    else if (getChargePointStatusService_B()->inferenceStatus() == ChargePointStatus::Available)
    {
      getChargePointStatusService_B()->authorize(readIdTag, readConnectorVal); // authorizing twice needed to be improvise
      status = true;
    }
    else if (getChargePointStatusService_B()->inferenceStatus() == ChargePointStatus::Reserved)
    {
      if (flag_evseReserveNow_B)
      {
        if (DEBUG_OUT)
          Serial.print("ReserveIdTag_B \n");

        getChargePointStatusService_B()->authorize(readIdTag, readConnectorVal); // authorizing twice needed to be improvise
        status = true;
        gu8_online_trans_initiated = 1;
      }
    }
#if 0
    else
    {
      if (DEBUG_OUT)
        Serial.print(F("EVSE_setOnAuthentication_B : ChargePointStatus::Available .\n"));
      getChargePointStatusService_B()->authorize(readIdTag, readConnectorVal);
      status = true;
    }
#endif
  }
  else if (readConnectorVal == 3)
  {
    if (getChargePointStatusService_C()->getIdTag() == readIdTag && getChargePointStatusService_C()->getTransactionId() != -1)
    {
      gu8_online_trans_initiated = 0;
      Serial.println("[EVSE_C] Stopping Txn-RFID TAP");
      EVSE_C_StopSession();
      session_ongoing_flag = false;
      session_on_offline_txn_completed = false;
      status = true;
    }
    else if (getChargePointStatusService_C()->inferenceStatus() == ChargePointStatus::Available)
    {
      getChargePointStatusService_C()->authorize(readIdTag, readConnectorVal);
      status = true;
    }
    else if (getChargePointStatusService_C()->inferenceStatus() == ChargePointStatus::Reserved)
    {
      if (flag_evseReserveNow_C)
      {
        if (DEBUG_OUT)
          Serial.print("ReserveIdTag_C \n");
        getChargePointStatusService_C()->authorize(readIdTag, readConnectorVal);
        status = true;
        gu8_online_trans_initiated = 1;
      }
    }
  }
  else
  {
    Serial.println("Connector Unavailable");
    status = false;
  }

  return status;
}

bool assignEvseToConnector_Offl(String readIdTag, int readConnectorVal)
{
  unsigned long tout = millis();
  if (readConnectorVal == 1)
  {
    if (getChargePointStatusService_A()->getIdTag() == readIdTag && getChargePointStatusService_A()->getEvDrawsEnergy() == true)
    {
      // stop session
      gu8_online_trans_initiated = 0;
      Serial.println("[EVSE_A] Offline Stopping txn RFID TAP");
      session_ongoing_flag = false;
      // offline_charging_A = 0;
      session_on_offline_txn_completed = true;
            if(!offline_charging_A)
      {
              EVSE_A_StopSession();
      }
      else
      {
      offline_charging_A = 0;
      EVSE_A_stopOfflineTxn();
      }

      // getChargePointStatusService_A()->stopEvDrawsEnergy();
      // getChargePointStatusService_A()->unauthorize();
      // requestForRelay(STOP,1);
      // float lmeterStop_A = meteringService->currentEnergy_A();
      // EVSE_A_stopOfflineTxn();
      disp_evse_A = false;
      gu8_Remote_start_A = 0;
      Stop_Session_A = 0;
      Display_Clear(Stop_Session_A);

#if OFFLINE_STORAGE_ENABLE
      requestSendStopTxn(String(get_txn_list_A), String(1), String(lmeterStop_A));
#endif
      // To Do List
      // Store offline StopTxn from Master to Slave DB
      Serial.println("Stopping Offline Charging by rfid");

      return true;
    }

#if OFFLINE_FUNATIONALITY
    else if (getChargePointStatusService_A()->inferenceStatus() == ChargePointStatus::Available)
    {
      if (getChargePointStatusService_A()->getEvDrawsEnergy() == false)
      {
        bool lu8_start_txn_flag_A = false;

#if 1
        if (requestGetAuthCache())
        {
          if (DEBUG_OUT)
            Serial.print(F("[EVSE] Offline idTag detected & Avaiable in Authentication Cache....!\n"));

          lu8_start_txn_flag_A = true;
        }
        else if (requestGetidTag())
        {
          if (DEBUG_OUT)
            Serial.print(F("[EVSE] Offline idTag detected & Un-Avaiable in Authentication Cache....!\n"));
          lu8_start_txn_flag_A = true;
          Serial.println("[EVSE] Offline idTag detected & Avaiable in Local Authentication List");
        }
        else
        {
          Serial.println("[EVSE] Offline idTag dtected & Un-Avaiable in Local Authentication List");
          // lu8_start_txn_flag_A = false;  // Allow known RFID to Start Transaction and stop
          lu8_start_txn_flag_A = true; // Allow Unknown RFID to Start Transaction and stop
        }
#endif

        if (lu8_start_txn_flag_A)
        {
          getChargePointStatusService_A()->authorize(readIdTag, readConnectorVal);
          getChargePointStatusService_A()->startEvDrawsEnergy();
          EVSE_A_startOfflineTxn();
          // To Do List
          // Store offline StopTxn from Master to Slave DB

          Tag_Id_A = readIdTag;
          return true;
        }
      }
      else
      {
        return false;
      }
    }
#endif
  }
  else if (readConnectorVal == 2)
  {
    if (getChargePointStatusService_B()->getIdTag() == readIdTag && getChargePointStatusService_B()->getEvDrawsEnergy() == true)
    {
      // stop session
      gu8_online_trans_initiated = 0;
      Serial.println(F("[EVSE_B] Offline Stopping Txn -RFID TAP"));
      session_ongoing_flag = false;
      session_on_offline_txn_completed = true;
      if(!offline_charging_B)
      {
              EVSE_B_StopSession();
      }
      else
      {
      offline_charging_B = 0;
      EVSE_B_stopOfflineTxn();
      }
      // EVSE_B_StopSession();
      // offline_charging_B = 0;
      // getChargePointStatusService_B()->stopEvDrawsEnergy();
      // getChargePointStatusService_B()->unauthorize();
      // requestForRelay(STOP,2);
      // float lmeterStop_B = meteringService->currentEnergy_B();
      // EVSE_B_stopOfflineTxn();
      disp_evse_B = false;
      gu8_Remote_start_B = 0;
      Stop_Session_B = 1;
      Display_Clear(Stop_Session_B);

#if OFFLINE_STORAGE_ENABLE
      requestSendStopTxn(String(get_txn_list_B), String(2), String(lmeterStop_B));
#endif
      // To Do List
      // Store offline StopTxn from Master to Slave DB
      Serial.println("Stopping Offline Charging by rfid");

      return true;
    }

#if OFFLINE_FUNATIONALITY
    else if (getChargePointStatusService_B()->inferenceStatus() == ChargePointStatus::Available)
    {
      if (getChargePointStatusService_B()->getEvDrawsEnergy() == false)
      {
        bool lu8_start_txn_flag_B = false;
#if 1
        if (requestGetAuthCache())
        {
          if (DEBUG_OUT)
            Serial.print(F("[EVSE] Offline idTag detected & Avaiable in Authentication Cache....!\n"));

          lu8_start_txn_flag_B = true;
        }
        else if (requestGetidTag())
        {
          if (DEBUG_OUT)
            Serial.print(F("[EVSE] Offline idTag detected & Un-Avaiable in Authentication Cache....!\n"));
          lu8_start_txn_flag_B = true;
          Serial.println("[EVSE] Offline idTag detected & Avaiable in Local Authentication List");
        }
        else
        {
          Serial.println("[EVSE] Offline idTag dtected & Un-Avaiable in Local Authentication List");
          // lu8_start_txn_flag_B = false;
          lu8_start_txn_flag_B = true; // Allow Unknown RFID to Start Transaction and stop
        }
#endif
        if (lu8_start_txn_flag_B)
        {
          getChargePointStatusService_B()->startEvDrawsEnergy();
          getChargePointStatusService_B()->authorize(readIdTag, readConnectorVal);
          EVSE_B_startOfflineTxn();
          // To Do List
          // Store offline StartTxn from Master to Slave DB
          Tag_Id_B = readIdTag;
          return true;
        }
      }
      else
      {
        return false;
      }
    }
#endif
  }
  else if (readConnectorVal == 3)
  {
    if (getChargePointStatusService_C()->getIdTag() == readIdTag && getChargePointStatusService_C()->getEvDrawsEnergy() == true)
    {
      // stop session
      gu8_online_trans_initiated = 0;
      Serial.println("[EVSE_B] Offline Stopping Transaction with RFID TAP");
      session_ongoing_flag = false;
      // EVSE_C_StopSession();
      // offline_charging_C = 0;
      session_on_offline_txn_completed = true;
      if(!offline_charging_C)
      {
        EVSE_C_StopSession();
      }
      else
      {
      offline_charging_C = 0;
      EVSE_C_stopOfflineTxn();
      }
      // getChargePointStatusService_C()->stopEvDrawsEnergy();
      // getChargePointStatusService_C()->unauthorize();
      // requestForRelay(STOP,3);
      // float lmeterStop_C = meteringService->currentEnergy_C();
      // EVSE_C_stopOfflineTxn();
      disp_evse_C = false;
      gu8_Remote_start_C = 0;
      Stop_Session_C = 2;
      Display_Clear(Stop_Session_C);

#if OFFLINE_STORAGE_ENABLE
      requestSendStopTxn(String(get_txn_list_C), String(3), String(lmeterStop_C));
#endif
      // To Do List
      // Store offline StopTxn from Master to Slave DB
      Serial.println("Stopping Offline Charging by rfid");

      return true;
    }

#if OFFLINE_FUNATIONALITY
    else if (getChargePointStatusService_C()->inferenceStatus() == ChargePointStatus::Available)
    {
      if (getChargePointStatusService_C()->getEvDrawsEnergy() == false)
      {
        bool lu8_start_txn_flag_C = false;
#if 1
        if (requestGetAuthCache())
        {
          if (DEBUG_OUT)
            Serial.print(F("[EVSE] Offline idTag detected & Avaiable in Authentication Cache....!\n"));

          lu8_start_txn_flag_C = true;
        }
        else if (requestGetidTag())
        {
          if (DEBUG_OUT)
            Serial.print(F("[EVSE] Offline idTag detected & Un-Avaiable in Authentication Cache....!\n"));
          lu8_start_txn_flag_C = true;
          Serial.println("[EVSE] Offline idTag detected & Avaiable in Local Authentication List");
        }
        else
        {
          Serial.println("[EVSE] Offline idTag dtected & Un-Avaiable in Local Authentication List");
          // lu8_start_txn_flag_C = false;
          lu8_start_txn_flag_C = true; // Allow Unknown RFID to Start Transaction and stop
        }
#endif
        if (lu8_start_txn_flag_C)
        {

          getChargePointStatusService_C()->startEvDrawsEnergy();
          getChargePointStatusService_C()->authorize(readIdTag, readConnectorVal);
          EVSE_C_startOfflineTxn();
          // To Do List
          // Store offline StartTxn from Master to Slave DB
          Tag_Id_C = readIdTag;
          return true;
        }
      }
      else
      {
        return false;
      }
    }
#endif
  }
}

/*
   Called by Websocket library on incoming message on the internet link
*/
#if WIFI_ENABLED || ETHERNET_ENABLED
// extern OnSendHeartbeat onSendHeartbeat;
int wscDis_counter = 0;
int wscConn_counter = 0;
#if 0
void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
  case WStype_DISCONNECTED:
    webSocketConncted = false;
    Serial.println("Counter:" + String(wscDis_counter));
    if (DEBUG_OUT)
      // Serial.print(F("[WSc] Disconnected!!!\n"));
      Serial.print("[WSc] Disconnected!!!\n");
    if (wscDis_counter++ > 1)
    {
      delay(200);
      // Serial.println(F("Trying to reconnect to WSC endpoint"));
      Serial.println("Trying to reconnect to WSC endpoint");
      wscDis_counter = 0;
      gu8_online_flag = 0;
      online_to_offline_flag = true;
      Serial.println("URL:" + String(path_m));
      webSocket.begin(host_m, port_m, path_m, protocol);
      while (!webSocketConncted)
      { // how to take care if while loop fails
        // Serial.print(F("..**.."));
        Serial.print("..**..");
        delay(100);       // bit**
        // webSocket.loop(); // after certain time stop relays and set fault state
        if (wscConn_counter++ > 3)
        {
          wscConn_counter = 0;
          // webSocket.disconnect();
          if (ethernet_connect)
          {
            counter_ethconnect++;
          }
          Serial.println("[Wsc] Unable To Connect");
          // Serial.println(F("[Wsc] Unable To Connect"));
          break;
        }
      }
    }
    // have to add websocket.begin if websocket is unable to connect //Static variable
    break;
  case WStype_CONNECTED:
    // offline_connect = 0; // commented by shiva
    if (gu8_online_flag == 0)
    {
      Serial.println("WiFi gu8_offline_to_online_detect_flag done...!");
      gu8_offline_to_online_detect_flag = 1;
      delay(50);
      Serial.println("Not_Set..............1");
      getChargePointStatusService_A()->setChargePointstatus(NOT_SET);
      Serial.println("Not_Set..............2");
      getChargePointStatusService_B()->setChargePointstatus(NOT_SET);
      Serial.println("Not_Set..............3");
      getChargePointStatusService_C()->setChargePointstatus(NOT_SET);
    }
    gu8_online_flag = 1;
    webSocketConncted = true;
    online_to_offline_flag = false;

    if (ethernet_connect)
    {
      counter_ethconnect = 0;
    }
    if (DEBUG_OUT)
      Serial.printf("[WSc] Connected to url: %s\n", payload);
    break;
  case WStype_TEXT:
    if (DEBUG_OUT)
      if (DEBUG_OUT)
        Serial.printf("[WSc] get text: %s\n", payload);

    if (!processWebSocketEvent(payload, length))
    { // forward message to OcppEngine
      if (DEBUG_OUT)
        // Serial.print(F("[WSc] Processing WebSocket input event failed!\n"));
        Serial.print("[WSc] Processing WebSocket input event failed!\n");
    }
    break;
  case WStype_BIN:
    if (DEBUG_OUT)
      Serial.print("[WSc] Incoming binary data stream not supported");
    break;
  case WStype_PING:
    // pong will be send automatically
    if (DEBUG_OUT)
      Serial.print("[WSc] get ping\n");
    // Serial.print(F("[WSc] get ping\n"));
    break;
  case WStype_PONG:
    // answer to a ping we send
    if (DEBUG_OUT)
      Serial.print("[WSc] get pong\n");
    // Serial.print(F("[WSc] get pong\n"));
    break;
  }
}
#endif
void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
  case WStype_DISCONNECTED:
    webSocketConncted = false;
    gu8_online_flag = 0;
    online_to_offline_flag = true;
    Serial.println("Counter:" + String(wscDis_counter));
    if (DEBUG_OUT)
      Serial.print("[WSc] Disconnected!!!\n");
#if 1
    if (wscDis_counter++ > 1)
    {
      delay(200);
      Serial.println("Trying to reconnect to WSC endpoint");
      wscDis_counter = 0;
      gu8_online_flag = 0;
      online_to_offline_flag = true;
      Serial.println("URL:" + String(path_m));
      webSocket.begin(host_m, port_m, path_m, protocol);
      while (!webSocketConncted)
      { // how to take care if while loop fails
        Serial.print("..**..");
        delay(100);       // bit**
        webSocket.loop(); // after certain time stop relays and set fault state
        if (wscConn_counter++ > 3)
        {
          wscConn_counter = 0;
          // webSocket.disconnect();
          if (ethernet_connect)
          {
            counter_ethconnect++;
          }
          Serial.println("[Wsc] Unable To Connect");
          // Serial.println(F("[Wsc] Unable To Connect"));
          break;
        }
      }
    }
    #endif
    // have to add websocket.begin if websocket is unable to connect //Static variable
    break;
  case WStype_CONNECTED:
      if (gu8_online_flag == 0)
    {
      Serial.println("WiFi gu8_offline_to_online_detect_flag done...!");
      gu8_offline_to_online_detect_flag = 1;
      delay(50);
      Serial.println("Not_Set..............1");
      getChargePointStatusService_A()->setChargePointstatus(NOT_SET);
      Serial.println("Not_Set..............2");
      getChargePointStatusService_B()->setChargePointstatus(NOT_SET);
      Serial.println("Not_Set..............3");
      getChargePointStatusService_C()->setChargePointstatus(NOT_SET);
    }
    gu8_online_flag = 1;
    webSocketConncted = true;
    online_to_offline_flag = false;

    if (DEBUG_OUT)
      Serial.printf("[WSc] Connected to url: %s\n", payload);
    break;
  case WStype_TEXT:
    if (DEBUG_OUT)
      if (DEBUG_OUT)
        Serial.printf("[WSc] get text: %s\n", payload);

    if (!processWebSocketEvent(payload, length))
    { // forward message to OcppEngine
      if (DEBUG_OUT)
        // Serial.print(F("[WSc] Processing WebSocket input event failed!\n"));
        Serial.print("[WSc] Processing WebSocket input event failed!\n");
    }
    break;
  case WStype_BIN:
    if (DEBUG_OUT)
      Serial.print("[WSc] Incoming binary data stream not supported");
    break;
  case WStype_PING:
    // pong will be send automatically
    if (DEBUG_OUT)
      Serial.print("[WSc] get ping\n");
    break;
  case WStype_PONG:
    // answer to a ping we send
    if (DEBUG_OUT)
      Serial.print("[WSc] get pong\n");
    break;
  }
  
}
#endif
#if 1
/*
  @brief : Read the touch display
*/
#if DWIN_ENABLED
int8_t dwin_input()
{

  button = DWIN_read();
  Serial.printf("Button pressed : %d", button);
  // delay(50);
  return button;
}
#endif
#endif
// #if WIFI_ENABLED
int wifi_counter = 0;
#if DISPLAY_ENABLED
unsigned long cloud_refresh = 0;
#endif
#if 0
void wifi_Loop()
{
  Serial.println("[WiFi_Loop]");
  if (WiFi.status() != WL_CONNECTED)
  {
    // delay(200);
    if ((wifi_counter++ > 10) && (WiFi.status() != WL_CONNECTED))
    {
      // online_to_offline_flag = true;
      wifi_counter = 0;
      Serial.print(".");
      WiFi.disconnect(true);
      // WiFi.mode(WIFI_OFF);
      delay(1000);
      Serial.println("[WIFI] Trying to reconnect again");
      // WiFi.mode(WIFI_STA);
      WiFi.begin(ssid_m.c_str(), key_m.c_str());
      Serial.print("ssid_m.c_str() : ");
      Serial.println(ssid_m.c_str());
      Serial.print("key_m.c_str() : ");
      Serial.println(key_m.c_str());
      // wifi_connection_available = 1;
      wifi_connection_available = 0;
      delay(2000);
    }
  }
  else if(WiFi.status() == WL_CONNECTED)
  {
      wifi_connection_available = 1;
      online_to_offline_flag = false;
      if (wifi_connection_available == 1)
      {
        delay(100);
        connectToWebsocket();
        // connectToWebsocket();
      }
      // webSocket.loop();

  }
}
#endif
void wifi_Loop()
{
  Serial.println("[WiFi_Loop]");
  if ((wifi_counter++ > 10) && (WiFi.status() != WL_CONNECTED))
  {
    // delay(200);

      // online_to_offline_flag = true;
      wifi_counter = 0;
      Serial.print(".");
      WiFi.disconnect(true);
      // WiFi.mode(WIFI_OFF);
      // delay(1000);
      Serial.println("[WIFI] Trying to reconnect again");
      // WiFi.mode(WIFI_STA);
      WiFi.begin(ssid_m.c_str(), key_m.c_str());
      Serial.print("ssid_m.c_str() : ");
      Serial.println(ssid_m.c_str());
      Serial.print("key_m.c_str() : ");
      Serial.println(key_m.c_str());
      // wifi_connection_available = 1;
      wifi_connection_available = 0;
      delay(2000);
    }
  else if(WiFi.status() == WL_CONNECTED)
  {
      
      online_to_offline_flag = false;
      if (wifi_connection_available == 0)
      {
        wifi_connection_available = 1;
        delay(100);
        connectToWebsocket();
        webSocket.loop();
        // connectToWebsocket();
      }
      webSocket.loop();

  }
#if DISPLAY_ENABLED
    while (millis() - cloud_refresh > 5000)
    {
      // cloud offline
      cloud_refresh = millis();
      cloudConnect_Disp(3);
      checkForResponse_Disp();
    }
#endif
}
// #endif
short int counterPing = 0;
uint16_t gu16_wifi_ws_disconnect_count = 0;
void cloudConnectivityLed_Loop()
{
#if GSM_PING
  if (gsm_connect == true)
  {
    if (counterPing++ >= 3 && !flag_ping_sent)
    {
      gu8_online_flag = 1;
      // sending ping after every 30 sec [if internet is not there sending ping packet itself consumes 10sec]
      Serial.println("sending ping");
      // char c = 0x09;
      // client.write(0X09); // send a ping
      // client.write("[9,\"heartbeat\"]"); // send a ping
      /*String p = "9";
      sendPingGsmStr(p);*/
      String p = "rockybhai";
      sendPingGsmStr(p);
      // sendFrame(WSop_ping,"HB",size_t(2),true,true);
      // flag_ping_sent = true;
      counterPing = 0;
      // check for pong inside gsmOnEvent
    }
  }
#endif

#if DWIN_ENABLED
  uint8_t err = 0;
#endif
#if 0
  if (wifi_connect == true)
  {
    // if (counterPing++ >= 3)
    if (counterPing++ >= 2)
    {
      // sending ping after every 30 sec [if internet is not there sending ping packet itself consumes 10sec]
      isInternetConnected = webSocket.sendPing();
      Serial.println("*Sending Ping: " + String(isInternetConnected));
#if EVSECEREBRO_CMS_SERVER_PING_ENABLED
      if (isInternetConnected == 0)
      {
        // offline_connect = 1; // commented by shiva
        online_to_offline_flag = 1;

        if (webSocketConncted == true)
        {
          WiFi.disconnect(true);
          gu8_websocket_begin_once = 1;
          webSocketConncted = false;
          online_to_offline_flag = 1;
        }

        // gu16_wifi_ws_disconnect_count ++;
        // if (gu16_wifi_ws_disconnect_count > 5)
        // {
        //   gu16_wifi_ws_disconnect_count = 0;
        //   WiFi.disconnect(true);
        //   webSocket.disconnect();
        // }
        // WiFi.disconnect(true);
        // webSocket.disconnect();
        // Serial.println("online_to_offline_flag 1 : " + String(online_to_offline_flag));
      }
      else
      {
        online_to_offline_flag = 0;
        // gu8_online_flag = 1;
        // Serial.println("online_to_offline_flag 0 : " + String(online_to_offline_flag));
      }
#endif
      counterPing = 0;
    }
    if ((WiFi.status() != WL_CONNECTED || webSocketConncted == false || isInternetConnected == false) && getChargePointStatusService_A()->getEmergencyRelayClose() == false)
    { // priority is on fault
      if (millis() - timercloudconnect > 10000)
      { // updates in 5sec
#if DISPLAY_ENABLED
        // cloud offline
        cloudConnect_Disp(false);
        checkForResponse_Disp();
        connAvail(1, "UNAVAILABLE");
        checkForResponse_Disp();
        connAvail(2, "UNAVAILABLE");
        checkForResponse_Disp();
        connAvail(3, "UNAVAILABLE");
        checkForResponse_Disp();
        setHeader("RFID UNAVAILABLE");
        checkForResponse_Disp();
#endif
#if LCD_ENABLED
        lcd.clear();
        lcd.setCursor(0, 0); // Or setting the cursor in the desired position.
        lcd.print("A: UNAVAILABLE");
        lcd.setCursor(0, 1);
        lcd.print("B: UNAVAILABLE");
        lcd.setCursor(0, 2);
        lcd.print("C: UNAVAILABLE");
        lcd.setCursor(0, 3);
        lcd.print("OFFLINE. NO RFID!");
        // lcd.print("CLOUD: offline");
#endif
#if LED_ENABLED
        requestLed(BLINKYWHITE_ALL, START, 1);
#endif
#if DWIN_ENABLED
        err = DWIN_SET(clun, sizeof(clun) / sizeof(clun[0]));
#endif
        timercloudconnect = millis();
      }
    }
#if DWIN_ENABLED
    err = DWIN_SET(wi, sizeof(wi) / sizeof(wi[0]));
#endif
  }
  #endif
  else if (gsm_connect == true && client.connected() == false && getChargePointStatusService_A()->getEmergencyRelayClose() == false)
  {
    if (millis() - timercloudconnect > 10000)
    { // updates in 5sec
#if DISPLAY_ENABLED
      // cloud offline
      cloudConnect_Disp(false);
      checkForResponse_Disp();
#endif
#if LCD_ENABLED
      lcd.clear();
      lcd.setCursor(0, 0); // Or setting the cursor in the desired position.
      lcd.print("STATUS: UNAVAILABLE");
      // lcd.setCursor(0, 1);
      // lcd.print("TAP RFID/SCAN QR");
      lcd.setCursor(0, 2);
      lcd.print("CONNECTION");
      lcd.setCursor(0, 3);
      lcd.print("CLOUD: OFFLINE");
      // lcd.print("CLOUD: offline");

#endif
#if LED_ENABLED
      requestLed(BLINKYWHITE_ALL, START, 1);
#endif
#if DWIN_ENABLED
      err = DWIN_SET(clun, sizeof(clun) / sizeof(clun[0]));
#endif
      timercloudconnect = millis();
    }
  }
  if (gsm_connect == true && client.connected() == true)
  {
#if DWIN_ENABLED
    err = DWIN_SET(g, sizeof(g) / sizeof(g[0]));
#endif
  }
  /*
   * @brief: Logic for ethernet to be offline and to be shown on display.
   * Added by G. Raja Sumant 02/09/2022
   */
  if (ethernet_connect)
  {
    if (counterPing++ >= 3)
    { // sending ping after every 30 sec [if internet is not there sending ping packet itself consumes 10sec]
      isInternetConnected = webSocket.sendPing();
      Serial.println("*Sending Ping: " + String(isInternetConnected));
      counterPing = 0;
    }
    if ((Ethernet.linkStatus() != LinkON || webSocketConncted == false) && getChargePointStatusService_A()->getEmergencyRelayClose() == false)
    {
      if (millis() - timercloudconnect > 10000)
      { // updates in 5sec
#if LED_ENABLED
        requestLed(BLINKYWHITE_ALL, START, 1);
#endif
#if DWIN_ENABLED
        err = DWIN_SET(clun, sizeof(clun) / sizeof(clun[0]));
#endif
        timercloudconnect = millis();
      }
    }
    if (ethernet_connect == true && webSocketConncted == true)
    {
#if DWIN_ENABLED
      err = DWIN_SET(et, sizeof(et) / sizeof(et[0]));
#endif
    }
  }
  /*else
  {
    Serial.println(F("[CloudLed] Something wrong with ethernet connect!"));
  }*/
}

void connectToWebsocket()
{
  // url_m = String(ws_url_prefix_m);
  //  String cpSerial = String("");
  //  EVSE_A_getChargePointSerialNumber(cpSerial);
  //  url_m += cpSerial; //most OCPP-Server require URLs like this. Since we're testing with an echo server here, this is obsolete

  // #if WIFI_ENABLED || ETHERNET_ENABLED
  Serial.print("Connecting to: ");
  Serial.println(path_m);
  // webSocket.begin(host_m, port_m, url_m, protocol_m);

  Serial.println(String(host_m.c_str()) + "::" + String(port_m) + "::" + String(path_m.c_str()) + "::" + String(protocol));

  evse_cp_ws_connection = ws_connect_initiated;

  // 192.168.0.80::8080::ws://192.168.0.80:8080/steve/websocket/CentralSystemService/ac001rdtest::ocpp1.6
  webSocket.begin(host_m, port_m, path_m, protocol);
  // webSocket.begin("192.168.0.80", 8080, "ws://192.168.0.80/steve/websocket/CentralSystemService/ac001rdtest", "ocpp1.6");
  //  event handler

  webSocket.onEvent(webSocketEvent);
  // try ever 5000 again if connection has failed
  webSocket.setReconnectInterval(5000);
  webSocket.enableHeartbeat(20000, 3000, 2);

  evse_cp_ws_connection = ws_connecting;

  // #endif
}

void connectivity_Loop()
{

  if (wifi_connect == true)
  {

    if ((WiFi.status() != WL_CONNECTED || webSocketConncted == false || isInternetConnected == false) && getChargePointStatusService_A()->getEmergencyRelayClose() == false)
    {

      flag_internet = checkInternet();
      if (flag_internet == false)
      {
        Offline_Loop();
      }
    }
    else
    {
      flag_offline = false;
    }
  }
  else if (gsm_connect == true)
  {

    if (client.connected() == false && getChargePointStatusService_A()->getEmergencyRelayClose() == false)
    {

      flag_internet = checkInternet();
      if (flag_internet == false)
      {
        Offline_Loop();
      }
    }
    else
    {

      flag_offline = false;
    }
  }
}

void Offline_Loop()
{
  // EVSE_A_Offline_Loop();
  // EVSE_B_Offline_Loop();
  // EVSE_C_Offline_Loop();
}

#if 0

void setup_WIFI_OTA_get_1(void)
{
  HTTPClient http;

  uint8_t gu8_wifi_count = 100;
  WiFi.disconnect(true);
  delay(100);
  WiFi.mode(WIFI_STA); /*wifi station mode*/
  // WiFi.begin(ssid_, password);
  WiFi.begin(ssid_m.c_str(), key_m.c_str());
  Serial.println("\nConnecting");

  while ((WiFi.status() != WL_CONNECTED) && (gu8_wifi_count))
  {
    Serial.print(".");
    delay(100);
    gu8_wifi_count--;
    if (gu8_wifi_count <= 0)
    {
      gu8_wifi_count = 0;
      WiFi.begin(ssid_m.c_str(), key_m.c_str());
    }
    // vTaskDelay(10000 / portTICK_PERIOD_MS);
  }

  Serial.println("\nConnected to the WiFi network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());
  delay(50);
  // vTaskDelay(5000 / portTICK_PERIOD_MS);

  Serial.setDebugOutput(true);

  // WiFiMulti.addAP("EVRE", "Amplify5");

  // Client address
  Serial.print("WebSockets Client started @ IP address: ");
  Serial.println(WiFi.localIP());

  //  String URI = String("http://34.100.138.28/fota2.php");

#if TEST_OTA
  // String URI = String("http://34.100.138.28/evse_test_ota.php");
  String URI = String("http://34.100.138.28/evse_bm_ac001_sale_sr_ota.php");
  Serial.println("[OTA] Test OTA Begin...");
#else
  String URI = String("http://34.100.138.28/evse_ota.php");
  Serial.println("[OTA] OTA Begin...");
#endif

  Serial.println("[HTTP] begin...");

  Serial.print("The URL given is:");
  //   Serial.println(uri);
  Serial.println(URI);

#if 1
  int updateSize = 0;

  // configure server and url
  // String post_data = "{\"version\":\"CP001/hello.ino.esp32\", \"deviceId\":\"CP001\"}";
  // String post_data = "{\"version\":\"display_TestUART.ino.esp32\",\"deviceId\":\"CP001\"}";
  /*http.begin("https://us-central1-evre-iot-308216.cloudfunctions.net/otaUpdate");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Connection", "keep-alive");*/

  // http.begin("http://34.93.75.210/fota2.php");
  http.begin(URI);

  http.setUserAgent(F("ESP32-http-Update"));
  http.addHeader(F("x-ESP32-STA-MAC"), WiFi.macAddress());
  http.addHeader(F("x-ESP32-AP-MAC"), WiFi.softAPmacAddress());
  // http.addHeader(F("x-ESP32-sketch-md5"), String(ESP.getSketchMD5()));
  http.addHeader(F("x-ESP32-STA-MAC"), ESP.getSdkVersion());
  http.addHeader(F("x-ESP32-STA-MAC"), String(ESP.getFreeSketchSpace()));
  // http.addHeader(F("x-ESP32-sketch-size"), String(ESP.getSketchSize()));
  // http.addHeader(F("x-ESP32-device-id: "), DEVICE_ID);
  http.addHeader(F("x-ESP32-device-id"), CP_Id_m);
  http.addHeader(F("x-ESP32-firmware-version"), OTA_VERSION /* + "\r\n" */);
  http.addHeader(F("x-ESP32-sketch-md5"), String(ESP.getSketchMD5()) /*  + String("\r\n")) */);
  // http.addHeader(F("x-ESP32-sketch-md5"), 425b2e2a27e2308338f7c8ede108ee9f);

  // int httpCode = http.POST(post_data);
  // int httpCode = http.GET(post_data);
  int httpCode = http.GET();
  // int httpCode = http.POST();

  if (httpCode > 0)
  {
    // HTTP header has been send and Server response header has been handled
    Serial.println("Checking for new firmware updates...");

    // If file found at server
    if (httpCode == HTTP_CODE_OK)
    {
      // get lenght of document (is -1 when Server sends no Content-Length header)
      int len = http.getSize();
      updateSize = len;
      Serial.printf("[OTA] Update found, File size(bytes) : %d\n", len);
      String get_response = http.getString();
      Serial.printf("[OTA] http response : %s\n", get_response);
      Serial.println("[HTTP] connection closed or file end.\n");

      if (get_response.equals("true") == true)
      {
        Serial.print("OTA update available");
        gu8_OTA_update_flag = 2;
      }
      else if (get_response.equals("false") == false)
      {
        gu8_OTA_update_flag = 3;
        Serial.print("no OTA update");
      }
    }
    // If there is no file at server
    if (httpCode == HTTP_CODE_INTERNAL_SERVER_ERROR)
    {
      Serial.println("[HTTP] No Updates");
      Serial.println();
      // ESP.restart();
    }
    Serial.println("[HTTP] Other response code");
    Serial.println(httpCode);
    Serial.println();
  }
  http.end();

#endif
}

void setup_WIFI_OTA_1(void)
{
  uint8_t err_cnt = 0;
  HTTPClient http;

  uint8_t gu8_wifi_count = 100;
#if 0
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA); /*wifi station mode*/
  // WiFi.begin(ssid_, password);
  WiFi.begin(ssid_m.c_str(), key_m.c_str());
  Serial.println("\nConnecting");

  while ((WiFi.status() != WL_CONNECTED) && (gu8_wifi_count))
  {
    Serial.print(".");
    delay(100);
    gu8_wifi_count--;
    if (gu8_wifi_count <= 0)
    {
      gu8_wifi_count = 0;
    }
    // vTaskDelay(10000 / portTICK_PERIOD_MS);
  }
#endif
  Serial.println("\nConnected to the WiFi network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());

  delay(50);
  // vTaskDelay(5000 / portTICK_PERIOD_MS);

  Serial.setDebugOutput(true);

  // WiFiMulti.addAP("EVRE", "Amplify5");

  // Client address
  Serial.print("WebSockets Client started @ IP address: ");
  Serial.println(WiFi.localIP());

  //  String URI = String("http://34.100.138.28/fota2.php");

#if TEST_OTA
  String URI = String("http://34.100.138.28/evse_bm_ac001_sale_sr_ota.php");
  Serial.println("[OTA] Test OTA Begin...");
#else
  String URI = String("http://34.100.138.28/evse_ota.php");
  Serial.println("[OTA] OTA Begin...");
#endif

  Serial.println("[HTTP] begin...");

  Serial.print("The URL given is:");
  //   Serial.println(uri);
  Serial.println(URI);

#if 1
  int updateSize = 0;

  // configure server and url
  // String post_data = "{\"version\":\"CP001/hello.ino.esp32\", \"deviceId\":\"CP001\"}";
  // String post_data = "{\"version\":\"display_TestUART.ino.esp32\",\"deviceId\":\"CP001\"}";
  /*http.begin("https://us-central1-evre-iot-308216.cloudfunctions.net/otaUpdate");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Connection", "keep-alive");*/

  // http.begin("http://34.93.75.210/fota2.php");
  http.begin(URI);

  http.setUserAgent(F("ESP32-http-Update"));
  http.addHeader(F("x-ESP32-STA-MAC"), WiFi.macAddress());
  http.addHeader(F("x-ESP32-AP-MAC"), WiFi.softAPmacAddress());
  http.addHeader(F("x-ESP32-sketch-md5"), String(ESP.getSketchMD5()));
  http.addHeader(F("x-ESP32-STA-MAC"), ESP.getSdkVersion());
  http.addHeader(F("x-ESP32-STA-MAC"), String(ESP.getFreeSketchSpace()));
  // http.addHeader(F("x-ESP32-sketch-size"), String(ESP.getSketchSize()));
  http.addHeader(F("x-ESP32-device-id"), CP_Id_m);
  // http.addHeader(F("x-ESP32-device-test-id: "), DEVICE_ID);
  http.addHeader(F("x-ESP32-firmware-version"), OTA_VERSION /* + "\r\n" */);

  //   int httpCode = http.POST(post_data);
  // int httpCode = http.GET(post_data);
  // int httpCode = http.GET();
  int httpCode = http.POST(DEVICE_ID);

  if (httpCode > 0)
  {
    // HTTP header has been send and Server response header has been handled
    Serial.println("Checking for new firmware updates...");

    // If file found at server
    if (httpCode == HTTP_CODE_OK)
    {
      // get lenght of document (is -1 when Server sends no Content-Length header)
      int len = http.getSize();
      updateSize = len;
      Serial.printf("[OTA] Update found, File size(bytes) : %d\n", len);

#if 1
      // get tcp stream
      WiFiClient *client = http.getStreamPtr();
      // Serial.println();
      performUpdate_WiFi_1(*client, (size_t)updateSize);
      while (err_cnt < 2)
      {
        if (E_reason == 8)
        {
          performUpdate_WiFi_1(*client, (size_t)updateSize);
        }
        err_cnt++;
      }
      Serial.println("[HTTP] connection closed or file end.\n");
#endif
      Serial.println("[HTTP] connection closed or file end.\n");
    }
    // If there is no file at server
    if (httpCode == HTTP_CODE_INTERNAL_SERVER_ERROR)
    {
      Serial.println("[HTTP] No Updates");
      Serial.println();
      // ESP.restart();
    }
    Serial.println("[HTTP] Other response code");
    Serial.println(httpCode);
    Serial.println();
  }
  http.end();
#endif
}

#endif

#if 0

// perform the actual update from a given stream
void performUpdate_WiFi_1(WiFiClient &updateSource, size_t updateSize)
{
  if (Update.begin(updateSize))
  {
    Serial.println("...Downloading File...");
    Serial.println();

    // Writing Update
    size_t written = Update.writeStream(updateSource);

    printPercent_1(written, updateSize);

    if (written == updateSize)
    {
      Serial.println("Written : " + String(written) + "bytes successfully");
    }
    else
    {
      Serial.println("Written only : " + String(written) + "/" + String(updateSize) + ". Retry?");
      // ptr_leds->red_led();
      for (uint8_t idx = 0; idx < NUM_LEDS; idx++)
      {
        leds[idx] = CRGB::Red;
        FastLED.show(COLOR_BRIGHTNESS);
      }
    }
    if (Update.end())
    {
      Serial.println("OTA done!");
      if (Update.isFinished())
      {
        Serial.println("Update successfully completed. Rebooting...");
        Serial.println();
        ESP.restart();
      }
      else
      {
        Serial.println("Update not finished? Something went wrong!");
      }
    }
    else
    {
      E_reason = Update.getError();
      Serial.println("Error Occurred. Error #: " + String(Update.getError()));
    }
  }
  else
  {
    Serial.println("Not enough space to begin OTA");
  }
}

void printPercent_1(uint32_t readLength, uint32_t contentLength)
{
  // If we know the total length
  if (contentLength != (uint32_t)-1)
  {
    Serial.print("\r ");
    Serial.print((100.0 * readLength) / contentLength);
    Serial.print('%');
  }
  else
  {
    Serial.println(readLength);
  }
}

void setup_WIFI_OTA_getconfig_1(void)
{
  HTTPClient http;

  uint8_t gu8_wifi_count = 100;
#if 1
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA); /*wifi station mode*/
  // WiFi.begin(ssid_, password);
  WiFi.begin(ssid_m.c_str(), key_m.c_str());
  Serial.println("\nConnecting");

  while ((WiFi.status() != WL_CONNECTED) && (gu8_wifi_count))
  {
    Serial.print(".");
    delay(100);
    gu8_wifi_count--;
    if (gu8_wifi_count <= 0)
    {
      gu8_wifi_count = 0;
    }
    // vTaskDelay(10000 / portTICK_PERIOD_MS);
  }
#endif

  Serial.println("\nConnected to the WiFi network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());
  delay(50);
  // vTaskDelay(5000 / portTICK_PERIOD_MS);

  Serial.setDebugOutput(true);

  // WiFiMulti.addAP("EVRE", "Amplify5");

  // Client address
  Serial.print("WebSockets Client started @ IP address: ");
  Serial.println(WiFi.localIP());

  //  String URI = String("http://34.100.138.28/fota2.php");

#if TEST_OTA
  // String URI = String("http://34.100.138.28/evse_test_ota.php");
  String URI_GET_CONFIG = String("http://34.100.138.28/evse-config-update.php");

  Serial.println("[OTA]  Get config Begin...");
#else
  String URI = String("http://34.100.138.28/evse_ota.php");
  Serial.println("[OTA] OTA Begin...");
#endif

  Serial.println("[HTTP] begin...");

  Serial.print("The URL given is:");
  //   Serial.println(uri);
  Serial.println(URI_GET_CONFIG);

#if 1
  int updateSize = 0;

  // configure server and url
  // String post_data = "{\"version\":\"CP001/hello.ino.esp32\", \"deviceId\":\"CP001\"}";
  // String post_data = "{\"version\":\"display_TestUART.ino.esp32\",\"deviceId\":\"CP001\"}";
  /*http.begin("https://us-central1-evre-iot-308216.cloudfunctions.net/otaUpdate");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Connection", "keep-alive");*/

  // http.begin("http://34.93.75.210/fota2.php");
  http.begin(URI_GET_CONFIG);

  http.setUserAgent(F("ESP32-http-Update"));
  http.addHeader(F("x-ESP32-STA-MAC"), WiFi.macAddress());
  http.addHeader(F("x-ESP32-AP-MAC"), WiFi.softAPmacAddress());
  http.addHeader(F("x-ESP32-sketch-md5"), String(ESP.getSketchMD5()));
  http.addHeader(F("x-ESP32-STA-MAC: "), ESP.getSdkVersion());
  http.addHeader(F("x-ESP32-STA-MAC: "), String(ESP.getFreeSketchSpace()));
  // http.addHeader(F("x-ESP32-sketch-size"), String(ESP.getSketchSize()));
  http.addHeader(F("x-ESP32-device-id: "), CP_Id_m);
  // http.addHeader(F("x-ESP32-device-test-id: "), DEVICE_ID);
  http.addHeader(F("x-ESP32-firmware-version: "), OTA_VERSION /* + "\r\n" */);

  // int httpCode = http.POST(post_data);
  // int httpCode = http.GET(post_data);
  int httpCode = http.GET();
  // int httpCode = http.POST();

  if (httpCode > 0)
  {
    // HTTP header has been send and Server response header has been handled
    Serial.println("Checking for new configs...");

    // If file found at server
    if (httpCode == HTTP_CODE_OK)
    {
      // get lenght of document (is -1 when Server sends no Content-Length header)
      int len = http.getSize();
      // updateSize = len;
      Serial.printf("[OTA] get config ,  : %d\n", len);
      String get_response = http.getString();
      Serial.printf("[OTA] http response : %s\n", get_response);
      Serial.println("[HTTP] connection closed or file end.\n");

#if 0 
      DeserializationError error = deserializeJson(server_config, get_response);

      //{"wifi":"EVRE","port":"80","otaupdatetime":"86400"}

      if (error)
      {
        Serial.print(F("DeserializeJson() failed: "));
        Serial.println(error.f_str());
        // return connectedToWifi;
      }
      if (server_config.containsKey("wifi"))
      {
        wifi_server = server_config["wifi"];
      }
      if (server_config.containsKey("port"))
      {
        port_server = server_config["port"];
      }

      if (server_config.containsKey("otaupdatetime"))
      {
        ota_update_time = server_config["otaupdatetime"];
      }

        get_response="";
      // put_server_config(); 
      Serial.println("\r\nclient disconnected....");
#endif
    }
    // If there is no file at server
    if (httpCode == HTTP_CODE_INTERNAL_SERVER_ERROR)
    {
      Serial.println("[HTTP] No Updates");
      Serial.println();
      // ESP.restart();
    }
    Serial.println("[HTTP] Other response code");
    Serial.println(httpCode);
    Serial.println();
  }
  http.end();

#endif
}

#endif
