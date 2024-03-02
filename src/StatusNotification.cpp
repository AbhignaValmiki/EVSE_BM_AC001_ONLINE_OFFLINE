// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#include "Variants.h"

#include "StatusNotification.h"
#include "dwin.h"

#include <string.h>

extern int8_t fault_code_A;
extern int8_t fault_code_B;
extern int8_t fault_code_C;

extern bool flag_trigger_status_A;
extern bool flag_trigger_status_B;
extern bool flag_trigger_status_C;

#if DWIN_ENABLED
extern unsigned char fault_suspEV[28];
extern unsigned char fault_suspEVSE[28];
extern unsigned char charging[28];
extern unsigned char preparing[28];
extern unsigned char reserved[28];
extern unsigned char finishing[28];
extern unsigned char avail[28];
extern unsigned char CONN_UNAVAIL[28];
#endif

extern bool evse_A_unavail;
extern bool evse_B_unavail;
extern bool evse_C_unavail;

StatusNotification::StatusNotification(ChargePointStatus currentStatus, int connectorId)
	: currentStatus(currentStatus), connectorId(connectorId)
{

	if (!getJsonDateStringFromSystemTime(timestamp, JSONDATE_LENGTH))
	{
		Serial.print(F("[StatusNotification] Error reading time string. Expect format like 2020-02-01T20:53:32.486Z\n"));
	}

	if (DEBUG_OUT)
	{
		Serial.print(F("[StatusNotification] New StatusNotification with timestamp "));
		Serial.print(timestamp);
		Serial.print(F(". New Status: "));
	}
	// This fix is to get the current status and avoid NOT_SET

	//	EEPROM.begin(sizeof(EEPROM_Data));
	switch (currentStatus)
	{
	case (ChargePointStatus::Available):
		if (DEBUG_OUT)
			Serial.print(F("Available\n"));
		//	EEPROM.put(8, "Available");
		break;
	case (ChargePointStatus::Preparing):
		if (DEBUG_OUT)
			Serial.print(F("Preparing\n"));
		//	EEPROM.put(8, "Preparing");
		break;
	case (ChargePointStatus::Charging):
		if (DEBUG_OUT)
			Serial.print(F("Charging\n"));
		//	EEPROM.put(8, "Charging");
		break;
	case (ChargePointStatus::SuspendedEVSE):
		if (DEBUG_OUT)
			Serial.print(F("SuspendedEVSE\n"));
		//	EEPROM.put(8, "SuspendedEVSE");
		break;
	case (ChargePointStatus::SuspendedEV):
		if (DEBUG_OUT)
			Serial.print(F("SuspendedEV\n"));
		//	EEPROM.put(8, "SuspendedEV");
		break;
	case (ChargePointStatus::Finishing):
		if (DEBUG_OUT)
			Serial.print(F("Finishing\n"));
		//	EEPROM.put(8, "Finishing");
		break;
	case (ChargePointStatus::Reserved):
		if (DEBUG_OUT)
			Serial.print(F("Reserved\n"));
		//	EEPROM.put(8, "Reserved");
		break;
	case (ChargePointStatus::Unavailable):
		if (DEBUG_OUT)
			Serial.print(F("Unavailable\n"));
		//	EEPROM.put(8, "Unavailable");
		break;
	case (ChargePointStatus::Faulted):
		if (DEBUG_OUT)
			Serial.print(F("Faulted\n"));
		//	EEPROM.put(8, "Faulted");
		break;
	case (ChargePointStatus::NOT_SET):
		currentStatus = getChargePointStatusService_A()->inferenceStatus();
		Serial.print(F("*NOT_SET*\n"));
		break;
	default:
		Serial.print(F("[Error, invalid status code]\n"));
		break;
	}
	//	EEPROM.commit();
	//	EEPROM.end();*/
}

const char *StatusNotification::getOcppOperationType()
{
	return "StatusNotification";
}

// TODO if the status has changed again when sendReq() is called, abort the operation completely (note: if req is already sent, stick with listening to conf). The ChargePointStatusService will enqueue a new operation itself
DynamicJsonDocument *StatusNotification::createReq()
{
	DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(4) + (JSONDATE_LENGTH + 1));
	JsonObject payload = doc->to<JsonObject>();

	payload["connectorId"] = connectorId; // Hardcoded to be one because only one connector is supported
	payload["errorCode"] = "NoError";	  // No error diagnostics support
										  //	currentStatus = getChargePointStatusService()->inferenceStatus();
										  // Part of Offline Implementation
#if DWIN_ENABLED
	uint8_t err = 0;
	switch (connectorId)
	{
	case 1:

		switch (currentStatus)
		{
		case (ChargePointStatus::Available):
			evse_A_unavail = false;
			avail[4] = 0X66;
			err = DWIN_SET(avail, sizeof(avail) / sizeof(avail[0]));
			break;
		case (ChargePointStatus::Preparing):
			preparing[4] = 0X66;
			err = DWIN_SET(preparing, sizeof(preparing) / sizeof(preparing[0]));
			break;
		case (ChargePointStatus::Charging):
			charging[4] = 0X66;
			err = DWIN_SET(charging, sizeof(charging) / sizeof(charging[0]));
			break;
		case (ChargePointStatus::SuspendedEVSE):
			fault_suspEVSE[4] = 0X66;
			err = DWIN_SET(fault_suspEVSE, sizeof(fault_suspEVSE) / sizeof(fault_suspEVSE[0]));
			break;
		case (ChargePointStatus::SuspendedEV):
			fault_suspEV[4] = 0X66;
			err = DWIN_SET(fault_suspEV, sizeof(fault_suspEV) / sizeof(fault_suspEV[0]));
			break;
		case (ChargePointStatus::Finishing):
			finishing[4] = 0X66;
			err = DWIN_SET(finishing, sizeof(finishing) / sizeof(finishing[0]));
			break;
		case (ChargePointStatus::Reserved):
			reserved[4] = 0X66;
			err = DWIN_SET(reserved, sizeof(reserved) / sizeof(reserved[0]));
			break;
		case (ChargePointStatus::Unavailable):
			evse_A_unavail = true;
			CONN_UNAVAIL[4] = 0X66;
			err = DWIN_SET(CONN_UNAVAIL, sizeof(CONN_UNAVAIL) / sizeof(CONN_UNAVAIL[0]));
			break;
		}
		break;
	case 2:
		switch (currentStatus)
		{
		case (ChargePointStatus::Available):
			evse_B_unavail = false;
			avail[4] = 0X71;
			err = DWIN_SET(avail, sizeof(avail) / sizeof(avail[0]));
			break;
		case (ChargePointStatus::Preparing):
			preparing[4] = 0X71;
			err = DWIN_SET(preparing, sizeof(preparing) / sizeof(preparing[0]));
			break;
		case (ChargePointStatus::Charging):
			charging[4] = 0X71;
			err = DWIN_SET(charging, sizeof(charging) / sizeof(charging[0]));
			break;
		case (ChargePointStatus::SuspendedEVSE):
			fault_suspEVSE[4] = 0X71;
			err = DWIN_SET(fault_suspEVSE, sizeof(fault_suspEVSE) / sizeof(fault_suspEVSE[0]));
			break;
		case (ChargePointStatus::SuspendedEV):
			fault_suspEV[4] = 0X71;
			err = DWIN_SET(fault_suspEV, sizeof(fault_suspEV) / sizeof(fault_suspEV[0]));
			break;
		case (ChargePointStatus::Finishing):
			finishing[4] = 0X71;
			err = DWIN_SET(finishing, sizeof(finishing) / sizeof(finishing[0]));
			break;
		case (ChargePointStatus::Reserved):
			reserved[4] = 0X71;
			err = DWIN_SET(reserved, sizeof(reserved) / sizeof(reserved[0]));
			break;
		case (ChargePointStatus::Unavailable):
			evse_B_unavail = true;
			CONN_UNAVAIL[4] = 0X71;
			err = DWIN_SET(CONN_UNAVAIL, sizeof(CONN_UNAVAIL) / sizeof(CONN_UNAVAIL[0]));
			break;
		}
		break;
	case 3:
		switch (currentStatus)
		{
		case (ChargePointStatus::Available):
			evse_C_unavail = false;
			avail[4] = 0X7B;
			err = DWIN_SET(avail, sizeof(avail) / sizeof(avail[0]));
			break;
		case (ChargePointStatus::Preparing):
			preparing[4] = 0X7B;
			err = DWIN_SET(preparing, sizeof(preparing) / sizeof(preparing[0]));
			break;
		case (ChargePointStatus::Charging):
			charging[4] = 0X7B;
			err = DWIN_SET(charging, sizeof(charging) / sizeof(charging[0]));
			break;
		case (ChargePointStatus::SuspendedEVSE):
			fault_suspEVSE[4] = 0X7B;
			err = DWIN_SET(fault_suspEVSE, sizeof(fault_suspEVSE) / sizeof(fault_suspEVSE[0]));
			break;
		case (ChargePointStatus::SuspendedEV):
			fault_suspEV[4] = 0X7B;
			err = DWIN_SET(fault_suspEV, sizeof(fault_suspEV) / sizeof(fault_suspEV[0]));
			break;
		case (ChargePointStatus::Finishing):
			finishing[4] = 0X7B;
			err = DWIN_SET(finishing, sizeof(finishing) / sizeof(finishing[0]));
			break;
		case (ChargePointStatus::Reserved):
			reserved[4] = 0X7B;
			err = DWIN_SET(reserved, sizeof(reserved) / sizeof(reserved[0]));
			break;
		case (ChargePointStatus::Unavailable):
			evse_C_unavail = true;
			CONN_UNAVAIL[4] = 0X7B;
			err = DWIN_SET(CONN_UNAVAIL, sizeof(CONN_UNAVAIL) / sizeof(CONN_UNAVAIL[0]));
			break;
		}
		break;
	}
#endif

#if 1
	if (flag_trigger_status_A)
	{
		currentStatus = getChargePointStatusService_A()->inferenceStatus();
		payload["connectorId"] = 1;
		// connectorId = 1; // Do not  mess with this connectorId variable.
		//flag_trigger_status_A = false;
	}
	else if (flag_trigger_status_B)
	{
		currentStatus = getChargePointStatusService_B()->inferenceStatus();
		payload["connectorId"] = 2;
		// connectorId = 2;  // Do not  mess with this connectorId variable.
		//flag_trigger_status_B = false;
	}
	else if (flag_trigger_status_C)
	{
		currentStatus = getChargePointStatusService_C()->inferenceStatus();
		payload["connectorId"] = 3;
		// connectorId = 3;  // Do not  mess with this connectorId variable.
		//flag_trigger_status_C = false;
	}
#endif
	// EEPROM.begin(sizeof(EEPROM_Data));
	switch (currentStatus)
	{
	case (ChargePointStatus::Available):
		payload["status"] = "Available";
		//	EEPROM.put(8, "Available");
		break;
	case (ChargePointStatus::Preparing):
		payload["status"] = "Preparing";
		//	EEPROM.put(8, "Preparing");
		break;
	case (ChargePointStatus::Charging):
		payload["status"] = "Charging";
		//	EEPROM.put(8, "Charging");
		break;
	case (ChargePointStatus::SuspendedEVSE):
		payload["status"] = "SuspendedEVSE";
		//	EEPROM.put(8, "SuspendedEVSE");
		break;
	case (ChargePointStatus::SuspendedEV):
		payload["status"] = "SuspendedEV";
		//	EEPROM.put(8, "SuspendedEV");
		break;
	case (ChargePointStatus::Finishing):
		payload["status"] = "Finishing";
		//	EEPROM.put(8, "Finishing");
		break;
	case (ChargePointStatus::Reserved):
		payload["status"] = "Reserved";
		//	EEPROM.put(8, "Reserved");
		break;
	case (ChargePointStatus::Unavailable):
		payload["status"] = "Unavailable";
		//	EEPROM.put(8, "Unavailable");
		break;
	case (ChargePointStatus::Faulted):
		payload["status"] = "Faulted";
		switch (connectorId)
		{
		case 1:
			switch (fault_code_A)
			{
			case 0:
				payload["errorCode"] = "OverVoltage";
				break;
			case 1:
				payload["errorCode"] = "UnderVoltage";
				break;
			case 2:
				payload["errorCode"] = "OverCurrentFailure";
				break;
			case 3:
				payload["errorCode"] = "UnderCurrent";
				break;
			case 4:
				payload["errorCode"] = "HighTemperature";
				break;
			case 5:
				payload["errorCode"] = "UnderTemperature";
				break;
			case 6:
				payload["errorCode"] = "GroundFailure";
				break;
			case 7:
				payload["errorCode"] = "OtherError";
				payload["vendorId"] = "EVRE";
				payload["vendorErrorCode"] = "EARTH DISCONNECT";
				break;
			case 8:
				payload["errorCode"] = "OtherError";
				payload["vendorId"] = "EVRE";
				payload["vendorErrorCode"] = "EmergencyStop";
				break;
			case 9:
				payload["errorCode"] = "OtherError";
				payload["vendorId"] = "EVRE";
				payload["vendorErrorCode"] = "PowerLoss";
			default:
				Serial.printf("[StatusNotification] Error code of A is %d",fault_code_A);
				payload["errorCode"] = "OtherError"; // No error diagnostics support
				break;
			}

			fault_code_A = -1; // clearing the fault code so that the next status notification will not be effected.
			break;

		case 2:
			switch (fault_code_B)
			{
			case 0:
				payload["errorCode"] = "OverVoltage";
				break;
			case 1:
				payload["errorCode"] = "UnderVoltage";
				break;
			case 2:
				payload["errorCode"] = "OverCurrentFailure";
				break;
			case 3:
				payload["errorCode"] = "UnderCurrent";
				break;
			case 4:
				payload["errorCode"] = "HighTemperature";
				break;
			case 5:
				payload["errorCode"] = "UnderTemperature";
				break;
			case 6:
				payload["errorCode"] = "GroundFailure";
				break;
			case 7:
				payload["errorCode"] = "OtherError";
				payload["vendorId"] = "EVRE";
				payload["vendorErrorCode"] = "EARTH DISCONNECT";
				break;
			case 8:
				payload["errorCode"] = "OtherError";
				payload["vendorId"] = "EVRE";
				payload["vendorErrorCode"] = "EmergencyStop";
				break;
			case 9:
				payload["errorCode"] = "OtherError";
				payload["vendorId"] = "EVRE";
				payload["vendorErrorCode"] = "PowerLoss";
			default:
				Serial.printf("[StatusNotification] Error code is %d",fault_code_B);
				payload["errorCode"] = "OtherError"; // No error diagnostics support
				break;
			}

			fault_code_B = -1; // clearing the fault code so that the next status notification will not be effected.

			break;
		case 3:
			switch (fault_code_C)
			{
			case 0:
				payload["errorCode"] = "OverVoltage";
				break;
			case 1:
				payload["errorCode"] = "UnderVoltage";
				break;
			case 2:
				payload["errorCode"] = "OverCurrentFailure";
				break;
			case 3:
				payload["errorCode"] = "UnderCurrent";
				break;
			case 4:
				payload["errorCode"] = "HighTemperature";
				break;
			case 5:
				payload["errorCode"] = "UnderTemperature";
				break;
			case 6:
				payload["errorCode"] = "GroundFailure";
				break;
			case 7:
				payload["errorCode"] = "OtherError";
				payload["vendorId"] = "EVRE";
				payload["vendorErrorCode"] = "EARTH DISCONNECT";
				break;
			case 8:
				payload["errorCode"] = "OtherError";
				payload["vendorId"] = "EVRE";
				payload["vendorErrorCode"] = "EmergencyStop";
				break;
			case 9:
				payload["errorCode"] = "OtherError";
				payload["vendorId"] = "EVRE";
				payload["vendorErrorCode"] = "PowerLoss";
			default:
			Serial.printf("[StatusNotification] Error code of A is %d",fault_code_C);
				payload["errorCode"] = "OtherError"; // No error diagnostics support
				break;
			}

			fault_code_C = -1; // clearing the fault code so that the next status notification will not be effected.

			break;
		}
		//	EEPROM.put(8, "Faulted");
		break;
	default:
		// set and then send.
		if (connectorId == 1)
		{
			currentStatus = getChargePointStatusService_A()->inferenceStatus();
		}
		else if (connectorId == 2)
		{
			currentStatus = getChargePointStatusService_B()->inferenceStatus();
		}
		else if (connectorId == 3)
		{
			currentStatus = getChargePointStatusService_C()->inferenceStatus();
		}
		switch (currentStatus)
		{
		case (ChargePointStatus::Available):
			payload["status"] = "Available";
			//	EEPROM.put(8, "Available");
			break;
		case (ChargePointStatus::Preparing):
			payload["status"] = "Preparing";
			//	EEPROM.put(8, "Preparing");
			break;
		case (ChargePointStatus::Charging):
			payload["status"] = "Charging";
			//	EEPROM.put(8, "Charging");
			break;
		case (ChargePointStatus::SuspendedEVSE):
			payload["status"] = "SuspendedEVSE";
			//	EEPROM.put(8, "SuspendedEVSE");
			break;
		case (ChargePointStatus::SuspendedEV):
			payload["status"] = "SuspendedEV";
			//	EEPROM.put(8, "SuspendedEV");
			break;
		case (ChargePointStatus::Finishing):
			payload["status"] = "Finishing";
			//	EEPROM.put(8, "Finishing");
			break;
		case (ChargePointStatus::Reserved):
			payload["status"] = "Reserved";
			//	EEPROM.put(8, "Reserved");
			break;
		case (ChargePointStatus::Unavailable):
			payload["status"] = "Unavailable";
			//	EEPROM.put(8, "Unavailable");
			break;
		case (ChargePointStatus::Faulted):
			payload["status"] = "Faulted";
			//	EEPROM.put(8, "Faulted");
			Serial.println(F("[StatusNotification] Faulted but reached here!"));
			break;

		default:
			payload["status"] = "Unavailable";
			Serial.print(F("[StatusNotification] Error: Sending status is still NOT_SET!\n"));
			break;
		}
	}

#if 1
	/*
	 * @brief : To address the trigger message status of faulted!
	 */
	if (flag_trigger_status_A || flag_trigger_status_B || flag_trigger_status_C)
	{
		int8_t fc = -1;
		if (flag_trigger_status_A)
		{
			flag_trigger_status_A = false;
			fc = fault_code_A;
		}
		else if (flag_trigger_status_B)
		{
			flag_trigger_status_B = false;
			fc = fault_code_B;
		}
		else if (flag_trigger_status_C)
		{
			flag_trigger_status_C = false;
			fc = fault_code_C;
		}

		switch (currentStatus)
		{
		case (ChargePointStatus::Faulted):
			payload["status"] = "Faulted";
			switch (fc)
			{
			case 0:
				payload["errorCode"] = "OverVoltage";
				break;
			case 1:
				payload["errorCode"] = "UnderVoltage";
				break;
			case 2:
				payload["errorCode"] = "OverCurrentFailure";
				break;
			case 3:
				payload["errorCode"] = "UnderCurrent";
				break;
			case 4:
				payload["errorCode"] = "HighTemperature";
				break;
			case 5:
				payload["errorCode"] = "UnderTemperature";
				break;
			case 6:
				payload["errorCode"] = "GroundFailure";
				break;
			case 7:
				payload["errorCode"] = "OtherError";
				payload["vendorId"] = "EVRE";
				payload["vendorErrorCode"] = "EARTH DISCONNECT";
				break;
			case 8:
				payload["errorCode"] = "OtherError";
				payload["vendorId"] = "EVRE";
				payload["vendorErrorCode"] = "EmergencyStop";
				break;
			case 9:
				payload["errorCode"] = "OtherError";
				payload["vendorId"] = "EVRE";
				payload["vendorErrorCode"] = "PowerLoss";
			default:
				payload["errorCode"] = "NoError"; // No error diagnostics support
				break;
			}

			//fault_code_A = -1; // clearing the fault code so that the next status notification will not be effected.
			break;
		}
	}
	#endif
	//	EEPROM.commit();
	//	EEPROM.end();
	if (!getJsonDateStringFromSystemTime(timestamp, JSONDATE_LENGTH))
	{
		Serial.print(F("[StatusNotification] Error reading time string. Expect format like 2020-02-01T20:53:32.486Z\n"));
	}
	payload["timestamp"] = timestamp;

	return doc;
}

void StatusNotification::processConf(JsonObject payload)
{
	/*
	 * Empty payload
	 */
}

/*
 * For debugging only
 */
StatusNotification::StatusNotification()
{
}

/*
 * For debugging only
 */
void StatusNotification::processReq(JsonObject payload)
{
}

/*
 * For debugging only
 */
DynamicJsonDocument *StatusNotification::createConf()
{
	DynamicJsonDocument *doc = new DynamicJsonDocument(0);
	JsonObject payload = doc->to<JsonObject>();
	return doc;
}
