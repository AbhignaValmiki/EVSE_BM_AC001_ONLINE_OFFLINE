/************************************************************4G_OTA***************************************************************/
#include "WiFi_OTA.h"

//WIFI_OTA_GET
extern String key_m;
extern String ssid_m;
extern String path_m;
extern String CP_Id_m;
extern uint8_t gu8_OTA_update_flag;

extern size_t E_reason;

#if 1

void setup_WIFI_OTA_get_1(void)
{
  HTTPClient http;

  uint8_t gu8_wifi_count = 50;
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
  String URI = String("http://34.100.138.28/evse_test_ac001_ota.php");
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
  HTTPClient http;
  uint8_t err_cnt = 0;

  uint8_t gu8_wifi_count = 50;
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
  String URI = String("http://34.100.138.28/evse_test_ac001_ota.php");
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
  // http.addHeader(F("x-ESP32-device-id"), DEVICE_ID);
  http.addHeader(F("x-ESP32-device-id"), CP_Id_m);
  // http.addHeader(F("x-ESP32-device-test-id: "), DEVICE_ID);
  http.addHeader(F("x-ESP32-firmware-version"), OTA_VERSION /* + "\r\n" */);

  //   int httpCode = http.POST(post_data);
  // int httpCode = http.GET(post_data);
  // int httpCode = http.GET();
  // int httpCode = http.POST(DEVICE_ID);
  int httpCode = http.POST(CP_Id_m);

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
      WiFiClient* client = http.getStreamPtr();
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

#if 1

// perform the actual update from a given stream
void performUpdate_WiFi_1(WiFiClient& updateSource, size_t updateSize)
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
      // for (uint8_t idx = 0; idx < NUM_LEDS; idx++)
      // {
      //   leds[idx] = CRGB::Red;
      //   FastLED.show(COLOR_BRIGHTNESS);
      // }
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

  uint8_t gu8_wifi_count = 50;
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
  // http.addHeader(F("x-ESP32-device-id: "), DEVICE_ID);
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

      get_response = "";
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


