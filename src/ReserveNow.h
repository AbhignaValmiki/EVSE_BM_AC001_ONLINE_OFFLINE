// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#ifndef RESERVENOW_H
#define RESERVENOW_H

#include "Variants.h"

#include "OcppMessage.h"
#include "ChargePointStatusService.h"

#include <LinkedList.h>

// Let's define a new class
class reserveNow_slot {

	public:
		String idTag;
		bool isAccepted;
		int connectorId;
		int reservationId_A;
		int reservationId_B;
		int reservationId_C;
		time_t Slot_time;
		time_t start_time;
		time_t expiry_time; 
};


enum class reserveNowstatus {
	Accepted,
	Faulted,
	Occupied,
	Rejected,
	Unavailable,  
	NOT_SET 	  //not part of OCPP 1.6
};

class ReserveNow : public OcppMessage {
private:
	String idTag;
	int connectorId = -1; 
	bool accepted = false;
	//bool ReserveStatus = false;
	reserveNowstatus currentStatus_A = reserveNowstatus::NOT_SET;
	reserveNowstatus currentStatus_B = reserveNowstatus::NOT_SET;
	reserveNowstatus currentStatus_C = reserveNowstatus::NOT_SET;

	LinkedList<reserveNow_slot*> ReserveNowSlotList = LinkedList<reserveNow_slot*>();
	
public:
	ReserveNow();

	const char* getOcppOperationType();

	DynamicJsonDocument* createReq();

	void processConf(JsonObject payload);

	void processReq(JsonObject payload);

	DynamicJsonDocument* createConf();

	reserveNowstatus ReserveStatus(int connector_id);

	reserveNowstatus getReserveStatus_A();
	reserveNowstatus getReserveStatus_B();
	reserveNowstatus getReserveStatus_C();

	void setReserveStatus_A(reserveNowstatus ReserveStatus);
	void setReserveStatus_B(reserveNowstatus ReserveStatus);
	void setReserveStatus_C(reserveNowstatus ReserveStatus);

	int compare(time_t a, time_t b);
};



#endif
