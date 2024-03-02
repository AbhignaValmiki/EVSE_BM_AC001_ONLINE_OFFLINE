// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#ifndef VARIANTS_H
#define VARIANTS_H

/*****************/
/*
In order to Enable Ethernet/WiFi, 
* Edit this "virtual void begin(uint16_t port=0) =0;" for WIFI :C:\Users\Mr Cool\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.6\cores\esp32\Server.h
* Set proper flag in OCPP_ETH_WIFI file kept in arduinoWebsockets/src
*/

//3.1 Concurrent fix
#define VERSION                      String("EVSE_AC001_OSR_1.2.1")
#define OTA_VERSION                  String("121")
#define CHARGE_POINT_VENDOR          String("EVRE")
#define CHARGE_POINT_MODEL           String("AC001")
#define CHARGE_POINT_VENDOR_SIZE     strlen("EVRE")
#define CHARGE_POINT_MODEL_SIZE      strlen("AC001")

#define GSM_ENABLED   		 0
#define WIFI_ENABLED 		 1
#define ETHERNET_ENABLED 	 1

#define BLE_ENABLE  		 1

#define CP_ACTIVE 			 0

#define DISPLAY_ENABLED		 0

#define MASTERPIN_ENABLED    0

#define LED_ENABLED 		 0

#define LCD_ENABLED		     0
#define LCD_DISPLAY_ENABLED		     1


#define LCD_ENABLED_FAULT	 0

#define EVSE_A_ENABLED       1

#define EVSE_B_ENABLED       1

#define EVSE_C_ENABLED       1

#define EARTH_DISCONNECT     1


#define DWIN_ENABLED		 0

#define EVSECEREBRO 		 1
#define STEVE                0

#define V_charge_lite1_4     1

#define SEEMA_CONNECT       (0) // '1' session stop '0' session resume after power recycle

#define EVSE_FOTA_ENABLE_WIFI    (1)
#define EVSE_FOTA_ENABLE_4G    (1)

#define TEST_OTA             1

#define EVSECEREBRO_CMS_SERVER_PING_ENABLED (1)

#define OFFLINE_TXN_TEST_CASE  (0)

#define OFFLINE_START_STOP_TXN_GUARD_INTERVAL    (10*2)

#define OFFLINE_STORAGE_ENABLE (0)

#define OFFLINE_FUNATIONALITY  (1)

#define GSM_PING                1

/*****************/
#define DEBUG_OUT true
#define DEBUG_EXTRA false
#define DEBUG_OUT_M true

//#define OCPP_SERVER //comment out if this should be compiled as server <--- needs to be implemented again

enum  EvseDevStatuse {

  flag_EVSE_is_Booted,
  flag_EVSE_Read_Id_Tag,
  falg_EVSE_Authentication,
  flag_EVSE_Start_Transaction,
  flag_EVSE_Request_Charge,
  flag_EVSE_Stop_Transaction,
  flag_EVSE_UnAutharized,
  flag_EVSE_Reboot_Request,
  flag_EVSE_Reserve_Now,
  flag_EVSE_Cancle_Reservation,
  flag_EVSE_Local_Authantication

};

  extern EvseDevStatuse EvseDevStatus_connector_1;

typedef enum  evse_boot_stat_t
{
    EVSE_BOOT_INITIATED = 0,
    EVSE_BOOT_SENT,
    EVSE_BOOT_ACCEPTED,
    EVSE_BOOT_REJECTED,
    EVSE_BOOT_DEFAULT,
};
extern evse_boot_stat_t evse_boot_state;


#endif
