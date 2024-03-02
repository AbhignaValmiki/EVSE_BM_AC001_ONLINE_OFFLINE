/*
 * fota_handler.cpp
 *
 * Copyright 2022 raja <raja@raja-IdeaPad-Gaming-3-15IMH05>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */
#include "fota_handler.h"

extern String uri;

void FOTA_WiFi()
{
  Serial.println("[HTTP] begin...");

  Serial.print("The URL given is:");
  Serial.println(uri);

  int updateSize = 0;
  HTTPClient http;

  // configure server and url
  // String post_data = "{\"version\":\"CP001/hello.ino.esp32\", \"deviceId\":\"CP001\"}";
  String post_data = "{\"version\":\"display_TestUART.ino.esp32\",\"deviceId\":\"CP001\"}";
  /*http.begin("https://us-central1-evre-iot-308216.cloudfunctions.net/otaUpdate");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Connection", "keep-alive");*/

  // http.begin("http://34.93.75.210/fota2.php");
  http.begin(uri);
  // http.begin("http://10.0.0.8/fota2.php");
  http.setUserAgent(F("ESP32-http-Update"));
  http.addHeader(F("x-ESP32-STA-MAC"), WiFi.macAddress());
  http.addHeader(F("x-ESP32-AP-MAC"), WiFi.softAPmacAddress());
  http.addHeader(F("x-ESP32-sketch-md5"), String(ESP.getSketchMD5()));
  http.addHeader(F("x-ESP32-sdk-version"), ESP.getSdkVersion());
  http.addHeader(F("x-ESP32-free-space"), String(ESP.getFreeSketchSpace()));
  // http.addHeader(F("x-ESP32-sketch-size"), String(ESP.getSketchSize()));
  // http.addHeader(F("x-ESP32-chip-size"), String(ESP.getFlashChipRealSize()));

  int httpCode = http.POST(post_data);

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

      // String response = http.getString();

      // get tcp stream
      WiFiClient *client = http.getStreamPtr();
      /* String res = "";
       File file = SPIFFS.open("/update.bin", FILE_WRITE);
       while (client->available())
       {
         res = client->readStringUntil('\n');
         res = res + '\n';
         //Serial.print(res + "\n"); // works fine

         //Now I am writing it to FATFS

         if (!file.print(res))
         {
           Serial.println("Appending file");
         }

       }*/

      // Serial.println();
      performUpdate_WiFi(*client, (size_t)updateSize);
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
}

// perform the actual update from a given stream
void performUpdate_WiFi(WiFiClient &updateSource, size_t updateSize)
{
  if (Update.begin(updateSize))
  {
    Serial.println("...Downloading File...");
    Serial.println();

    // Writing Update
    size_t written = Update.writeStream(updateSource);
    if (written == updateSize)
    {
      Serial.println("Written : " + String(written) + "bytes successfully");
    }
    else
    {
      Serial.println("Written only : " + String(written) + "/" + String(updateSize) + ". Retry?");
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
      Serial.println("Error Occurred. Error #: " + String(Update.getError()));
    }
  }
  else
  {
    Serial.println("Not enough space to begin OTA");
  }
}
