// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#ifndef CHARGEPOINTSTATUSSERVICE_H
#define CHARGEPOINTSTATUSSERVICE_H

#include "libraries/arduinoWebSockets-master/src/WebSocketsClient.h"
#if 0
enum class ChargePointStatus {
	Available,
	Preparing,
	Charging,
	SuspendedEVSE,
	SuspendedEV,
	Finishing,    //not supported by this client
	Reserved,     //not supported by this client
	Unavailable,  //not supported by this client
	Faulted,      //not supported by this client
	NOT_SET 	  //not part of OCPP 1.6
};
#endif

enum ChargePointStatus {
	Available,
	Preparing,
	Charging,
	SuspendedEVSE,
	SuspendedEV,
	Finishing,    //not supported by this client
	Reserved,     //Implemented reserve now
	Unavailable,  //Implemented Change Availability
	Faulted,      //Implemented Faulted.
	NOT_SET 	  //not part of OCPP 1.6
};

extern ChargePointStatus evse_ChargePointStatus_A;
extern ChargePointStatus evse_ChargePointStatus_B;
extern ChargePointStatus evse_ChargePointStatus_C;


class ChargePointStatusService {
private:
	bool authorized = false;
	String idTag = String('\0');
	int connectorId = 1;             //not in use
	bool transactionRunning = false;
	int transactionId = -1;
	bool evDrawsEnergy = false;
	bool evseOffersEnergy = false;
	bool overVoltage = false;
	bool underVoltage = false;
	bool EarthDisconnect = false;
	bool overTemperature = false;
	bool underTemperature = false;
	bool overCurrent = false;
	bool underCurrent = false;
	bool emergencyRelayClose = false;
	//Feature added by G. Raja Sumant for charge point status service.
	bool unavailable = false;
	//Feature added by G. Raja Sumant for reservation status service.
	bool reserved = false;
	//Feature added by G. Raja Sumant for Finishing.
	bool finished = false;
	ChargePointStatus currentStatus = ChargePointStatus::NOT_SET;
	WebSocketsClient *webSocket;
	ChargePointStatus evse_ChargePointStatus;

public:
	ChargePointStatusService(WebSocketsClient *webSocket);
	ChargePointStatusService(); //dummy constructor
	void authorize(String &idTag, int connectorId);
	void authorize(String &idTag);
	void authorize();
	void unauthorize();
	String &getIdTag();
	int &getConnectorId();
	bool getOverVoltage();
	void setOverVoltage(bool ov);
	bool getUnderVoltage();
	void setUnderVoltage(bool uv);
	bool getOverCurrent();
	void setOverCurrent(bool oc);
	void setEarthDisconnect(bool ed);
	bool getEarthDisconnect();
	bool getUnderCurrent();
	void setUnderCurrent(bool uc);
	bool getOverTemperature();
	void setOverTemperature(bool ot);
	bool getUnderTemperature();
	void setUnderTemperature(bool ut);
	bool getEmergencyRelayClose();
	void setEmergencyRelayClose(bool erc);
	void startTransaction(int transactionId);
	void stopTransaction();
	int getTransactionId();
	void boot();
	void startEvDrawsEnergy();
	void stopEvDrawsEnergy();
	void startEnergyOffer();
	void stopEnergyOffer();
	//Feature added by G. Raja Sumant for Change availabilty
	void setUnavailable(bool su);
	bool getUnavailable();
	//Feature added by G. Raja Sumant for reservation
	void setReserved(bool);
	bool getReserved();
	//Feature added by G. Raja Sumant for Finishing
	void setFinishing(bool);
	bool getFinishing();
	void loop();

	ChargePointStatus inferenceStatus();
    void setChargePointstatus(ChargePointStatus lenum_evse_ChargePointStatus);
	ChargePointStatus getChargePointstatus();


	/***Adding new funtion @wamique**/
	void setConnectorId(int connectorId);
	bool getEvDrawsEnergy();
	/*******************************/
};

#endif
