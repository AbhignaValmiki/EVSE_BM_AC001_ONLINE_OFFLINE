#include"CustomGsm.h"
#include "CustomEthernet.h"
#include "libraries/arduinoWebSockets-master/src/WebSocketsClient.h"
#include <WiFi.h>

void internetLoop();
void wifi_gsm_connect();
void cloudConnectivityLed_Loop();
bool checkInternet();
void eth_4g_connect();
void eth_connect();
