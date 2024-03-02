// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#include "Variants.h"

#include "ReserveNow.h"
#include "OcppEngine.h"
#include "TimeHelper.h"

#define RESERVE_DURATION (900) // 15 Minutes 15 * 1 * 60 = 900 Seconds

extern bool flag_evseReserveNow_A; // added by @mkrishna
extern bool flag_evseReserveNow_B; // added by @mkrishna
extern bool flag_evseReserveNow_C; // added by @mkrishna
extern bool reservation_start_flag_A;
extern bool reservation_start_flag_B;
extern bool reservation_start_flag_C;

extern String currentIdTag;
String reserve_currentIdTag_A;
String reserve_currentIdTag_B;
String reserve_currentIdTag_C;

String expiryDate_A;
int reservationId;
int reserve_connectorId;
int reservationId_A;
int reserve_connectorId_A;
time_t reservation_start_time_A = 0;
time_t reserveDate_A = 0;
time_t reservedDuration_A = RESERVE_DURATION; // 15 minutes slot Duration 15 * (1 * 60)
bool reserve_state_A = false;

int reservationId_B;
int reserve_connectorId_B;
time_t reservation_start_time_B = 0;
time_t reserveDate_B = 0;
time_t reservedDuration_B = RESERVE_DURATION; // 15 minutes slot Duration 15 * (1 * 60)
bool reserve_state_B = false;

int reservationId_C;
int reserve_connectorId_C;
time_t reservation_start_time_C = 0;
time_t reserveDate_C = 0;
time_t reservedDuration_C = RESERVE_DURATION; // 15 minutes slot Duration 15 * (1 * 60)
bool reserve_state_C = false;

ReserveNow::ReserveNow()
{

	ReserveNowSlotList = LinkedList<reserveNow_slot *>();
}

const char *ReserveNow::getOcppOperationType()
{
	return "ReserveNow";
}

void ReserveNow::processReq(JsonObject payload)
{
	ReserveNowSlotList = LinkedList<reserveNow_slot *>();
	idTag = String(payload["idTag"].as<String>());
	connectorId = payload["connectorId"].as<int>();
	reservationId = payload["reservationId"].as<int>();
	const char *expiryTime = payload["expiryDate"] | "Invalid";
	reserve_connectorId = connectorId;
	Serial.print("\r\n[ReserveNow]\r\n ");
    
	switch (connectorId)
	{
	case  1: 
		/* code */
        if(flag_evseReserveNow_A)
		{
			accepted = false;
			return;
		}
		break;
	case  2: 
		/* code */
		 if(flag_evseReserveNow_B)
		{
			accepted = false;
			return;
		}
		break;
	case  3: 
		/* code */
		 if(flag_evseReserveNow_C)
		{
			accepted = false;
			return;
		}
		break;
	
	default:
		break;
	}

	if (strcmp(expiryTime, "Invalid"))
	{
		switch (connectorId)
		{
		case 1:
		{

			reserve_currentIdTag_A = idTag;
			reserve_connectorId_A = connectorId;
			reservationId_A = reservationId;

			bool success = getTimeFromJsonDateString(expiryTime, &reserveDate_A);
			if (success)
			{
				/*
				 * @brief : If you are using steve, uncomment the below
				 */
				//reserveDate_A = reserveDate_A - 19800; //  19800 seconds are substrating from Asia/Kolkata (IST) to GMT
				Serial.print(F("[ReserveNow] expiryDate_A : "));
				Serial.println(reserveDate_A);

				//reservation_start_time_A = reserveDate_A - reservedDuration_A;
                reservedDuration_A = reserveDate_A - now();

				Serial.print(F("[ReserveNow] currentIdTag : "));
				Serial.println(reserve_currentIdTag_A);

				Serial.print(F("[ReserveNow] connectorId : "));
				Serial.println(connectorId);

				reserveNowstatus Reservestatus = ReserveNow::ReserveStatus(connectorId);
				if (!flag_evseReserveNow_A)
				{
					Reservestatus = reserveNowstatus::Accepted;
					ReserveNow::setReserveStatus_A(Reservestatus);
					accepted = true;
				}

				if (Reservestatus == reserveNowstatus::Faulted ||
					Reservestatus == reserveNowstatus::Occupied ||
					Reservestatus == reserveNowstatus::Rejected ||
					Reservestatus == reserveNowstatus::Unavailable)
				{
					if (DEBUG_OUT)
						Serial.println("Current Status::Charging/Preparing/SuspendedEV");
					flag_evseReserveNow_A = false;
				}
				else if (Reservestatus == reserveNowstatus::Accepted)
				{

					if (!flag_evseReserveNow_A)
					{
						flag_evseReserveNow_A = true;
						reservation_start_flag_A = false;
						
#if 0
						reserveNow_slot *current_reserveslot_A = new reserveNow_slot();

						current_reserveslot_A->idTag = currentIdTag;
						current_reserveslot_A->isAccepted = true;
						current_reserveslot_A->connectorId = connectorId;
						current_reserveslot_A->reservationId_A = reservationId_A;
						current_reserveslot_A->Slot_time = RESERVE_DURATION;
						current_reserveslot_A->start_time = (reserveDate_A - RESERVE_DURATION);
						current_reserveslot_A->expiry_time = reserveDate_A;
#endif

						if (DEBUG_OUT)
							Serial.println("Current::Available");

						// https://www.epochconverter.com/timezones?q=1657527293
						// time_t current_time = now() + 19800; // Added the 19800 seconds for Asia/Kolkata (IST)
						time_t current_time = now();
						Serial.print(F("[ReserveNow] current time : "));
						Serial.println(current_time);
						time_t delta = reserveDate_A - now();
						Serial.print(F("[ReserveNow] expiryDate_A Delta time : "));
						Serial.println(delta);
						// ReserveNowSlotList.add(current_reserveslot_A);
					}
				}
			}
			else
			{
				Serial.print(F("[ReserveNow] Error reading time string. Expect format like 2020-02-01T20:53:32.486Z\n"));
			}
		}
		break;

		case 2:
		{

			reserve_currentIdTag_B = idTag;
			reserve_connectorId_B = connectorId;
			reservationId_B = reservationId;

			bool success = getTimeFromJsonDateString(expiryTime, &reserveDate_B);
			if (success)
			{
				/*
				 * @brief : If you are using steve, uncomment the below
				 */
				//reserveDate_B = reserveDate_B - 19800; //  19800 seconds are substrating from Asia/Kolkata (IST) to GMT
				Serial.print(F("[ReserveNow] expiryDate_B : "));
				Serial.println(reserveDate_B);

				//reservation_start_time_B = reserveDate_B - reservedDuration_B;
				reservedDuration_B = reserveDate_B - now();

				Serial.print(F("[ReserveNow] currentIdTag_B : "));
				Serial.println(reserve_currentIdTag_B);

				Serial.print(F("[ReserveNow] connectorId : "));
				Serial.println(connectorId);

				reserveNowstatus Reservestatus = ReserveNow::ReserveStatus(connectorId);
				if (!flag_evseReserveNow_B)
				{
					Reservestatus = reserveNowstatus::Accepted;
					ReserveNow::setReserveStatus_B(Reservestatus);
					accepted = true;
				}

				if (Reservestatus == reserveNowstatus::Faulted ||
					Reservestatus == reserveNowstatus::Occupied ||
					Reservestatus == reserveNowstatus::Rejected ||
					Reservestatus == reserveNowstatus::Unavailable)
				{
					if (DEBUG_OUT)
						Serial.println("Current Status::Charging/Preparing/SuspendedEV");
					flag_evseReserveNow_B = false;
				}
				else if (Reservestatus == reserveNowstatus::Accepted)
				{

					if (!flag_evseReserveNow_B)
					{
						flag_evseReserveNow_B = true;
						reservation_start_flag_B = false;

#if 0
						reserveNow_slot *current_reserveslot_B = new reserveNow_slot();

						current_reserveslot_B->idTag = currentIdTag;
						current_reserveslot_B->isAccepted = true;
						current_reserveslot_B->connectorId = connectorId;
						current_reserveslot_B->reservationId_A = reservationId_B;
						current_reserveslot_B->Slot_time = RESERVE_DURATION;
						current_reserveslot_B->start_time = (reserveDate_B - RESERVE_DURATION);
						current_reserveslot_B->expiry_time = reserveDate_B;
#endif

						if (DEBUG_OUT)
							Serial.println("Current::Available");

						// https://www.epochconverter.com/timezones?q=1657527293
						// time_t current_time = now() + 19800; // Added the 19800 seconds for Asia/Kolkata (IST)
						time_t current_time = now();
						Serial.print(F("[ReserveNow] current time : "));
						Serial.println(current_time);
						time_t delta = reserveDate_B - now();
						Serial.print(F("[ReserveNow] expiryDate_A Delta time : "));
						Serial.println(delta);
						// ReserveNowSlotList.add(current_reserveslot_A);
					}
				}
			}
			else
			{
				Serial.print(F("[ReserveNow] Error reading time string. Expect format like 2020-02-01T20:53:32.486Z\n"));
			}
		}
		break;

		case 3:
		{

			reserve_currentIdTag_C = idTag;
			reserve_connectorId_C = connectorId;
			reservationId_C = reservationId;

			bool success = getTimeFromJsonDateString(expiryTime, &reserveDate_C);
			if (success)
			{
				/*
				 * @brief : If you are using steve, uncomment the below
				 */
				//reserveDate_C = reserveDate_C - 19800; //  19800 seconds are substrating from Asia/Kolkata (IST) to GMT
				Serial.print(F("[ReserveNow] expiryDate_C : "));
				Serial.println(reserveDate_C);

				//reservation_start_time_C = reserveDate_C - reservedDuration_C;
				reservedDuration_C = reserveDate_C - now();

				Serial.print(F("[ReserveNow] currentIdTag_C : "));
				Serial.println(reserve_currentIdTag_C);

				Serial.print(F("[ReserveNow] connectorId : "));
				Serial.println(connectorId);

				reserveNowstatus Reservestatus = ReserveNow::ReserveStatus(connectorId);
				if (!flag_evseReserveNow_C)
				{
					Reservestatus = reserveNowstatus::Accepted;
					ReserveNow::setReserveStatus_C(Reservestatus);
					accepted = true;
				}

				if (Reservestatus == reserveNowstatus::Faulted ||
					Reservestatus == reserveNowstatus::Occupied ||
					Reservestatus == reserveNowstatus::Rejected ||
					Reservestatus == reserveNowstatus::Unavailable)
				{
					if (DEBUG_OUT)
						Serial.println(F("Current Status::Charging/Preparing/SuspendedEV"));
					flag_evseReserveNow_C = false;
				}
				else if (Reservestatus == reserveNowstatus::Accepted)
				{

					if (!flag_evseReserveNow_C)
					{
						flag_evseReserveNow_C = true;
						reservation_start_flag_C = false;

#if 0
						reserveNow_slot *current_reserveslot_C = new reserveNow_slot();

						current_reserveslot_C->idTag = currentIdTag;
						current_reserveslot_C->isAccepted = true;
						current_reserveslot_C->connectorId = connectorId;
						current_reserveslot_C->reservationId_A = reservationId_C;
						current_reserveslot_C->Slot_time = RESERVE_DURATION;
						current_reserveslot_C->start_time = (reserveDate_C - RESERVE_DURATION);
						current_reserveslot_C->expiry_time = reserveDate_C;
#endif

						if (DEBUG_OUT)
							Serial.println(F("Current::Available"));

						// https://www.epochconverter.com/timezones?q=1657527293
						// time_t current_time = now() + 19800; // Added the 19800 seconds for Asia/Kolkata (IST)
						time_t current_time = now();
						Serial.print(F("[ReserveNow] current time : "));
						Serial.println(current_time);
						time_t delta = reserveDate_C - now();
						Serial.print(F("[ReserveNow] expiryDate_C Delta time : "));
						Serial.println(delta);
						// ReserveNowSlotList.add(current_reserveslot_C);
					}
				}
			}
			else
			{
				Serial.print(F("[ReserveNow] Error reading time string. Expect format like 2020-02-01T20:53:32.486Z\n"));
			}
		}
		break;
		}
	}
	else
	{
		Serial.print(F("[ReserveNow] Error reading time string. Expect format like 2020-02-01T20:53:32.486Z\n"));
	}
}

DynamicJsonDocument *ReserveNow::createConf()
{
	DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1));
	JsonObject payload = doc->to<JsonObject>();

    reserveNowstatus Reservestatus;
    switch(reserve_connectorId)
	{
		case 1:
			 Reservestatus = ReserveNow::getReserveStatus_A();
			break;
		case 2:
			 Reservestatus = ReserveNow::getReserveStatus_B();
			break;
		case 3:
			 Reservestatus = ReserveNow::getReserveStatus_C();
			break;

		default:
			break;
	}
	

	switch (Reservestatus)
	{
	case reserveNowstatus::Faulted:
		payload["status"] = "Faulted";
		break;
	case reserveNowstatus::Occupied:
		payload["status"] = "Occupied";
		break;
	case reserveNowstatus::Accepted:
		payload["status"] = "Accepted";
		break;
	case reserveNowstatus::Unavailable:
		payload["status"] = "Unavailable";
		break;
	// case reserveNowstatus::Rejected: // To Do
	// 		payload["status"] = "Rejected";
	// 		break;
	default:
		payload["status"] = "Faulted";
	}
	if(!accepted)
	{
		payload["status"] = "Occupied";
	}

	Serial.print("\r\n[ReserveNow]\r\n ");

	return doc;
}

DynamicJsonDocument *ReserveNow::createReq()
{
	DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1));
	JsonObject payload = doc->to<JsonObject>();

	payload["idTag"] = "fefed1d19876";
	payload["connectorId"] = 1;

	return doc;
}

void ReserveNow::processConf(JsonObject payload)
{
	String status = payload["status"] | "Invalid";

	if (status.equals("Accepted"))
	{
		if (DEBUG_OUT)
			Serial.print(F("[ReserveNow] Request has been accepted!\n"));
	}
	else
	{
		Serial.print(F("[ReserveNow] Request has been denied!"));
	}
}

reserveNowstatus ReserveNow::ReserveStatus(int connector_id)
{

	reserveNowstatus reserve_status;
	ChargePointStatus inferencedStatus;

	switch (connector_id)
	{
	case 1:
		 inferencedStatus = getChargePointStatusService_A()->inferenceStatus();
		break;
	case 2:
		 inferencedStatus = getChargePointStatusService_B()->inferenceStatus();
		break;
	case 3:
		 inferencedStatus = getChargePointStatusService_C()->inferenceStatus();
		break;
	default:
		break;
	}
	switch (inferencedStatus)
	{
	case ChargePointStatus::Preparing:
	case ChargePointStatus::SuspendedEVSE:
	case ChargePointStatus::SuspendedEV:
		reserve_status = reserveNowstatus::Faulted;
		break;
	case ChargePointStatus::Charging:
		reserve_status = reserveNowstatus::Occupied;
		break;
	case ChargePointStatus::Available:
		reserve_status = reserveNowstatus::Accepted;
		break;
	case ChargePointStatus::Unavailable:
		reserve_status = reserveNowstatus::Unavailable;
		break;
	// case ChargePointStatus::Unavailable: // To Do
	// 		reserve_status = reserveNowstatus::Rejected;
	// 		break;
	default:
		reserve_status = reserveNowstatus::Faulted;
	}

	switch (connector_id)
	{
	case 1:
		ReserveNow::setReserveStatus_A(reserve_status);
		break;
	case 2:
		ReserveNow::setReserveStatus_B(reserve_status);
		break;
	case 3:
		ReserveNow::setReserveStatus_C(reserve_status);
		break;
	default:
		break;
	}

	return reserve_status;
}

reserveNowstatus ReserveNow::getReserveStatus_A()
{
	return currentStatus_A;
}

reserveNowstatus ReserveNow::getReserveStatus_B()
{
	return currentStatus_B;
}

reserveNowstatus ReserveNow::getReserveStatus_C()
{
	return currentStatus_C;
}

void ReserveNow::setReserveStatus_A(reserveNowstatus ReserveStatus)
{
	this->currentStatus_A = ReserveStatus;
}

void ReserveNow::setReserveStatus_B(reserveNowstatus ReserveStatus)
{
	this->currentStatus_B = ReserveStatus;
}

void ReserveNow::setReserveStatus_C(reserveNowstatus ReserveStatus)
{
	this->currentStatus_C = ReserveStatus;
}

int ReserveNow::compare(time_t a, time_t b)
{

	int ret = 0;

	Serial.print("a :");
	Serial.println(a);
	Serial.print("b :");
	Serial.println(b);
	if (a == b)
	{
		Serial.print("equal\n");
		ret = 0;
	}
	if (a > b)
	{
		Serial.print("second date is later\n");
		ret = 1;
	}

	if (a < b)
	{
		printf("seocnd date is earlier\n");
		ret = 2;
	}

	return ret;
}