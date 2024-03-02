// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#include "Variants.h"

#include "CancelReservation.h"
#include "OcppEngine.h"
#include "TimeHelper.h"

extern bool flag_evseReserveNow_A; // added by @mkrishna
extern bool flag_evseReserveNow_B; // added by @mkrishna
extern bool flag_evseReserveNow_C; // added by @mkrishna

extern bool flag_evseCancelReservation;	  // added by @mkrishna
extern bool flag_evseCancelReservation_A; // added by @mkrishna
extern bool flag_evseCancelReservation_B; // added by @mkrishna
extern bool flag_evseCancelReservation_C; // added by @mkrishna

extern String currentIdTag;
extern String expiryDate;

extern int reservationId;

extern int reservationId_A;
extern int reservationId_B;
extern int reservationId_C;

bool send_cancelReserve_flag = false;

CancelReservation::CancelReservation()
{
}

const char *CancelReservation::getOcppOperationType()
{
	return "CancelReservation";
}

void CancelReservation::processReq(JsonObject payload)
{

	send_cancelReserve_flag = false;
	reservationId = payload["reservationId"].as<int>();
	if (reservationId == reservationId_A)
	{
		flag_evseCancelReservation_A = true;
		send_cancelReserve_flag = true;
	}
	else if (reservationId == reservationId_B)
	{
		flag_evseCancelReservation_B = true;
		send_cancelReserve_flag = true;
	}
	else if (reservationId == reservationId_C)
	{
		flag_evseCancelReservation_C = true;
		send_cancelReserve_flag = true;
	}

	Serial.println("\r\n***********************************[CancelReservation] ************************************\r\n ");
}

DynamicJsonDocument *CancelReservation::createConf()
{
	DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1));
	JsonObject payload = doc->to<JsonObject>();

	// bool accepted = true;
	// if (flag_evseReserveNow) // Todo
	if (send_cancelReserve_flag)
	{
		payload["status"] = "Accepted";
		/*
		 * @brief : Change made by G. Raja Sumant for removing reservation.
		 */
		// getChargePointStatusService()->setReserved(false);

		// flag_evseCancelReservation_A = true;
	}
	else
	{

		payload["status"] = "Rejected";
	}
	// accepted = false; // reset the flag
	Serial.println("\r\n***********************************[CancelReservation] ************************************\r\n ");
	Serial.println("[CancelReservation] \r\n ");

	return doc;
}

DynamicJsonDocument *CancelReservation::createReq()
{
	DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1));
	JsonObject payload = doc->to<JsonObject>();

	payload["idTag"] = "fefed1d19876";
	payload["connectorId"] = 1;

	return doc;
}

void CancelReservation::processConf(JsonObject payload)
{
	String status = payload["status"] | "Invalid";

	if (status.equals("Accepted"))
	{
		if (DEBUG_OUT)
			Serial.println("[CancelReservation] Request has been accepted!\n");
	}
	else
	{
		Serial.println("[CancelReservation] Request has been denied!");
	}
}

CancelReservationstatus CancelReservation::getCancelReservation()
{
	return currentStatus;
}

void CancelReservation::setCancelReservation(CancelReservationstatus cancelreservationStatus)
{
	this->currentStatus = cancelreservationStatus;
}
