# EVSE_BM_AC001_ONLINE_OFFLINE

Firmware version------EVSE_AC001_OSR_V1.2.2               
Hardware version------Supports VL_1.4/VL_1.3
Supported Displays DWIN/LCD 20x4

Updated date: 23-02-2024
Updated by: Abhigna Valmiki

************ OTA URL - http://34.100.138.28/evse_test_ac001_ota.php ****************** - EVSE_AC001_OSR_V1.2.2

CHANGES - V1.2.2
-------
Changed complete wifi functionality(WiFi Driver).
Changed arduino websocket version from 2.3.6 - 2.3.7
Relay stop after stop transaction is accepted
Send stop transaction for session stopped in offline in case of powerloss(After power recycle)
When battery backup is present, Status notification,Stop transaction,Reason for stop as PowerLoss 

CHANGES - V1.2.1
-------
Chargepoint id in boot notification
Display changes
Removed finishing state
Added configuration based OTA - WIFI & 4G
Added Re initializing boot notification till the acceptance recieved
Updating metervalue to 0 in stop transaction if the meter stop value exceeds by 19900000
All relays starts after boot notification sent only - for session resume after power recycle
Removed energy consumption if session starts in offline

EVSE_AC001_OSR_V1.2.2 - Basic Functionality
-------------------------------------------
* WIFI_OTA - Enabled 
* 4G_OTA   - Enabled
* Session RESUME after power recycle
* OFFLINE & ONLINE - Starts in Offline & online 
* Doesnot store any data if the session started in offline


KNOWN ISSUES
-------------
*starting energy consumption from 0 when power recycle in display
.......................................................................................

-->Implemented 20*4 LCD Display Driver.
-->Implemented LED State machine

1.Fixed Websocket reconnect Issue.
2.Fixed Connector are showing Preparing State after sessions terminated Issue.
3.Fixed Invalid Meter values starting from zero,after session resume on Power recycle Issue.
4.Fixed Session is stop through Emergency Switch,  Meter values are not updating in offline, sending stop with online last updated Watt-Hour Issue.
5.Fixed Low current count/timeout is not working in Off-line Issue.
6.Fixed Multiple status notification.
7.Fixed Relays are starting in offline once emergency pressed and released Issue.
8.Fixed LED's Indicating RED randomly Issue.
9.Fixed relays are toggling while charging Issue.
10.Fixed Low current count is not working Issue.
11.Dynamic URL_Parser added.


12.Session Stop after power recycle/Session resume after power recycle - Should be Enabled in Code as per requirement
13.Only ONLINE / ONLINE & OFFLINE Functionality - Should be Enabled in Code as per requirement