
#ifndef _WIFI_OTA_H_
#define _WIFI_OTA_H_

#include <ArduinoJson.h>
#include "libraries/arduinoWebSockets-master/src/WebSocketsClient.h"
#include "OcppEngine.h"
#include "EVSE_A.h"
#include "Variants.h"
#include "internet.h"
#include "Master.h"
#include "CustomGsm.h"
#include "Preferences.h"
#include <HTTPClient.h>
#include <Update.h>
#include "FFat.h"
#include <TinyGsmClient.h>
#include <CRC32.h>
#include "FFat.h"
#include "FS.h"
#include "SPIFFS.h"
#include "esp32-hal-cpu.h"
#include "ESP32Time.h"
#include "Esp.h"
#include "core_version.h"
#include "evse_addr_led.h"

//Power Cycle
#include"OTA.h"
#include"internet.h"

#include "fota_handler.h"
#include "FirmwareStatus_OCPP.h"
#include <FastLED.h>

void setup_WIFI_OTA_get_1(void);
void setup_WIFI_OTA_1(void);
void performUpdate_WiFi_1(WiFiClient& updateSource, size_t updateSize);
void printPercent_1(uint32_t readLength, uint32_t contentLength);
void setup_WIFI_OTA_getconfig_1(void);

#endif
