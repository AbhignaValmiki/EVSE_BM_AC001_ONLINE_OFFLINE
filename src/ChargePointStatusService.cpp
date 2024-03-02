// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#include "Variants.h"

#include "ChargePointStatusService.h"
#include "StatusNotification.h"
#include "OcppEngine.h"
#include "Master.h"
#include "SimpleOcppOperationFactory.h"
#include "LCD_I2C.h"
#include "EVSE_A.h"
#include "EVSE_B.h"
#include "EVSE_C.h"

#if DISPLAY_ENABLED
#include "display.h"
#endif
extern bool disp_evse_A;
extern bool disp_evse_B;
extern bool disp_evse_C;
extern bool notFaulty_A;
extern bool notFaulty_B;
extern bool notFaulty_C;
extern bool EMGCY_FaultOccured_A;
extern bool EMGCY_FaultOccured_B;
extern bool EMGCY_FaultOccured_C;

extern bool flag_evRequestsCharge_A;
extern bool flag_evseStopTransaction_A;
extern bool flag_evRequestsCharge_B;
extern bool flag_evseStopTransaction_B;
extern bool flag_evRequestsCharge_C;
extern bool flag_evseStopTransaction_C;

extern bool flag_trigger_status_A;
extern bool flag_trigger_status_B;
extern bool flag_trigger_status_C;
extern uint8_t gu8_online_flag;

extern uint8_t count_trigger;

#if DWIN_ENABLED
#include "dwin.h"
extern unsigned char charging[28];
extern unsigned char change_page[10];
extern unsigned char avail[28];
extern unsigned char not_avail[28];
extern unsigned char fault_emgy[28];
extern unsigned char fault_noearth[28];
extern unsigned char fault_overVolt[28];
extern unsigned char fault_underVolt[28];
extern unsigned char fault_overTemp[28];
extern unsigned char fault_overCurr[28];
extern unsigned char fault_underCurr[28];
extern unsigned char fault_suspEV[28];
extern unsigned char fault_suspEVSE[28];
#endif
EvseDevStatuse EvseDevStatus_connector_1;

// extern ATM90E36 eic_earth;

#include <string.h>

extern bool webSocketConncted;
extern bool wifi_connect;
extern bool gsm_connect;
extern bool isInternetConnected;
extern bool flag_ed_A;

extern volatile bool session_ongoing_flag;

extern LCD_I2C lcd;

extern uint8_t fault_occured_A;
extern uint8_t fault_occured_B;
extern uint8_t fault_occured_C;

ChargePointStatus evse_ChargePointStatus_A;
ChargePointStatus evse_ChargePointStatus_B;
ChargePointStatus evse_ChargePointStatus_C;

ChargePointStatusService::ChargePointStatusService() {} // dummy constructor

ChargePointStatusService::ChargePointStatusService(WebSocketsClient *webSocket)
	: webSocket(webSocket)
{
	setChargePointStatusService(this);
}

ChargePointStatus ChargePointStatusService::inferenceStatus()
{
#if 0
	if (reserved)
	{
		// reserved = false;
		return ChargePointStatus::Reserved;
	}

	/*if (ongoingTxn_m)
	{
		return ChargePointStatus::Charging;
	}*/

	/*
	 * @brief : Feature added by G. Raja Sumant
	 * 09/07/2022 as part of change availability
	 */
	if (unavailable)
	{
		return ChargePointStatus::Unavailable;
	}

	if (!authorized)
	{
		if (emergencyRelayClose)
		{
			return ChargePointStatus::Faulted;
		}
		/*else if (finished)
	{
		return ChargePointStatus::Finishing;
	}*/
		else
		{
			return ChargePointStatus::Available;
		}
	}
	else if (!transactionRunning)
	{
		 if (finished)
	{
		return ChargePointStatus::Finishing;
	}
	else
		return ChargePointStatus::Preparing;
	}
	else
	{
		// Transaction is currently running
		if (emergencyRelayClose)
		{
			return ChargePointStatus::Faulted;
		}
		
		else
		{

			if (!evDrawsEnergy)
			{
				//return ChargePointStatus::SuspendedEV;
				return ChargePointStatus::Preparing;
			}
			if (!evseOffersEnergy)
			{
				return ChargePointStatus::SuspendedEVSE;
			}
			return ChargePointStatus::Charging;
		}
	}
#endif
	return evse_ChargePointStatus;
}
#if DISPLAY_ENABLED
unsigned long refreshInterval = 0;
#endif
void ChargePointStatusService::loop()
{

ChargePointStatus inferencedStatus = inferenceStatus();

// if(flag_trigger_status_A && connectorId == 1 && count_trigger >= 1)
#if 1
if(flag_trigger_status_A && connectorId == 1 ) 
{
	count_trigger--;
	Serial.println("[ChargePointStatusService] Trigger status A");
	OcppOperation *statusNotification = makeOcppOperation(webSocket,
															  new StatusNotification(inferencedStatus, 1));
		initiateOcppOperation(statusNotification);
		return;
}

// else if(flag_trigger_status_B  && connectorId == 2 && count_trigger >= 1) 
else if(flag_trigger_status_B  && connectorId == 2) 
{
	count_trigger--;
	Serial.println("[ChargePointStatusService] Trigger status B");
	OcppOperation *statusNotification = makeOcppOperation(webSocket,
															  new StatusNotification(inferencedStatus, 2));
		initiateOcppOperation(statusNotification);
		return;
}

// else if(flag_trigger_status_C && connectorId == 3 && count_trigger >= 1) 
else if(flag_trigger_status_C && connectorId == 3 ) 
{
	count_trigger--;
	Serial.println("[ChargePointStatusService] Trigger status C");
	OcppOperation *statusNotification = makeOcppOperation(webSocket,
															  new StatusNotification(inferencedStatus, 3));
		initiateOcppOperation(statusNotification);
		return;
}
#endif

	if (inferencedStatus != currentStatus)
	{
		currentStatus = inferencedStatus;


		Serial.print("CP LOOP:");
		switch (currentStatus)
		{
		case Available:
			Serial.println("Available");
			break;
		case Preparing:
			Serial.print("Preparing");
			break;
		case Charging:
			Serial.print("Charging");
			break;
		case SuspendedEVSE:
			Serial.print("SuspendedEVSE");
			break;
		case SuspendedEV:
			Serial.print("SuspendedEV");
			break;
		case Finishing: // not supported by this client
			Serial.print("Finishing");
			break;
		case Reserved: // Implemented reserve now
			Serial.print("Reserved");
			break;
		case Unavailable: // Implemented Change Availability
			Serial.print("Unavailable");
			break;
		case Faulted: // Implemented Faulted.
			Serial.print("Faulted");
			break;
		default:
			// evse_ChargePointStatus = Available;
			// Serial.print("Available");
			break;
		}

		if (DEBUG_OUT)
		Serial.println("[ChargePointStatusService] Status changed\n");
		Serial.println("connectorid:" + String(connectorId));
#if 1
		// if(currentStatus != Unavailable)
		// {

		// if (EvseDevStatus_connector_1 != flag_EVSE_is_Booted )
		// if ((EvseDevStatus_connector_1 != flag_EVSE_is_Booted))
	    if ((EvseDevStatus_connector_1 != flag_EVSE_is_Booted) && (currentStatus != NOT_SET) && (gu8_online_flag == 1))
		{
			// fire StatusNotification
			// TODO check for online condition: Only inform CS about status change if CP is online
			// TODO check for too short duration condition: Only inform CS about status change if it lasted for longer than MinimumStatusDuration
			OcppOperation *statusNotification = makeOcppOperation(webSocket,
																  new StatusNotification(currentStatus, connectorId));
			initiateOcppOperation(statusNotification);
		}

		if ((EvseDevStatus_connector_1 != flag_EVSE_is_Booted) && (currentStatus == NOT_SET) && (gu8_online_flag == 1))
		{
			if (getChargePointStatusService_A()->getChargePointstatus() == NOT_SET)
			{
				fault_occured_A = 1;
			}
			if (getChargePointStatusService_B()->getChargePointstatus() == NOT_SET)
			{
				fault_occured_B = 1;
			}
			if (getChargePointStatusService_C()->getChargePointstatus() == NOT_SET)
			{
				fault_occured_C = 1;
			}
		}
#endif
	}
}

void ChargePointStatusService::setChargePointstatus(ChargePointStatus lenum_evse_ChargePointStatus)
{
	this->evse_ChargePointStatus = lenum_evse_ChargePointStatus;
}

ChargePointStatus ChargePointStatusService::getChargePointstatus()
{
	return this->evse_ChargePointStatus;
}

void ChargePointStatusService::authorize(String &idTag, int connectorId)
{
	this->idTag = String(idTag);
	this->connectorId = connectorId;
	authorize();
}

void ChargePointStatusService::authorize(String &idTag)
{
	this->idTag = String(idTag);
	authorize();
}

void ChargePointStatusService::authorize()
{
	if (authorized == true)
	{
		if (DEBUG_OUT)
			Serial.println("[ChargePointStatusService] Warning: authorized twice or didn't unauthorize before\n");
		return;
	}
	Serial.println("[ChargePointStatusService] Auth accepted");
	authorized = true;
}

int &ChargePointStatusService::getConnectorId()
{
	return connectorId;
}

void ChargePointStatusService::setConnectorId(int connectorId)
{
	this->connectorId = connectorId;
}
String &ChargePointStatusService::getIdTag()
{
	return this->idTag;
}

bool ChargePointStatusService::getOverVoltage()
{
	return overVoltage;
}

void ChargePointStatusService::setOverVoltage(bool ov)
{
	this->overVoltage = ov;
}

bool ChargePointStatusService::getUnderVoltage()
{
	return underVoltage;
}

void ChargePointStatusService::setUnderVoltage(bool uv)
{
	this->underVoltage = uv;
}

/*
* @brief: Earth fault detection using octocoupler
* HCPL3700 - Logic
* INPUT OUTPUT
  HIGH  LOW (EARTH PRESENCE)
  LOW   HIGH(EARTH ABSENCE)
*/

void ChargePointStatusService::setEarthDisconnect(bool ed)
{
	this->EarthDisconnect = ed;
}

bool ChargePointStatusService::getEarthDisconnect()
{
	return EarthDisconnect;
}

bool ChargePointStatusService::getOverTemperature()
{
	return overTemperature;
}

void ChargePointStatusService::setOverTemperature(bool ot)
{
	this->overTemperature = ot;
}

bool ChargePointStatusService::getUnderTemperature()
{
	return underTemperature;
}

void ChargePointStatusService::setUnderTemperature(bool ut)
{
	this->underTemperature = ut;
}

bool ChargePointStatusService::getOverCurrent()
{
	return overCurrent;
}

void ChargePointStatusService::setOverCurrent(bool oc)
{
	this->overCurrent = oc;
}

bool ChargePointStatusService::getUnderCurrent()
{
	return underCurrent;
}

void ChargePointStatusService::setUnderCurrent(bool uc)
{
	this->underCurrent = uc;
}

bool ChargePointStatusService::getEmergencyRelayClose()
{
	return emergencyRelayClose;
}
void ChargePointStatusService::setEmergencyRelayClose(bool erc)
{
	this->emergencyRelayClose = erc;
}

void ChargePointStatusService::unauthorize()
{
	if (authorized == false)
	{
		if (DEBUG_OUT)
			Serial.println("[ChargePointStatusService] Warning: unauthorized twice or didn't authorize before\n");
		return;
	}
	if (DEBUG_OUT)
		Serial.println("[ChargePointStatusService] idTag value will be cleared.\n");
	this->idTag.clear();
	authorized = false;
}
/*
 * @brief : Changing the variable transactionId from call be value to reference using 'this' pointer
 * G. Raja Sumant - 13/09/2022 For avoiding multiple stop transactions.
 */
void ChargePointStatusService::startTransaction(int transId)
{
	if (transactionRunning == true)
	{
		if (DEBUG_OUT)
			Serial.println("[ChargePointStatusService] Warning: started transaction twice or didn't stop transaction before\n");
	}
	// transactionId = transId;
	// transactionRunning = true;
	this->transactionId = transId;
	this->transactionRunning = true;
}

void ChargePointStatusService::boot()
{
	// TODO Review: Is it necessary to check in inferenceStatus(), if the CP is booted at all? Problably not ...
}

void ChargePointStatusService::stopTransaction()
{
	if (transactionRunning == false)
	{
		if (DEBUG_OUT)
			Serial.println("[ChargePointStatusService] Warning: stopped transaction twice or didn't start transaction before\n");
	}
	// transactionRunning = false;
	// transactionId = -1;

	this->transactionRunning = false;
	this->transactionId = -1;
	// EEPROM.begin(sizeof(EEPROM_Data));
	// EEPROM.put(68,transactionId);
	// EEPROM.commit();
	// EEPROM.end();
}

int ChargePointStatusService::getTransactionId()
{
	if (this->transactionId < 0)
	{
		if (DEBUG_OUT)
			Serial.println("[ChargePointStatusService] Warning: getTransactionId() returns invalid transactionId. Have you called it after stopTransaction()? (can only be called before) Have you called it before startTransaction?\n");
	}
	if (this->transactionRunning == false)
	{
		if (DEBUG_OUT)
			Serial.println("[ChargePointStatusService] Warning: getTransactionId() called, but there is no transaction running. Have you called it after stopTransaction()? (can only be called before) Have you called it before startTransaction?\n");
	}
	return this->transactionId;
}

void ChargePointStatusService::startEvDrawsEnergy()
{
	if (evDrawsEnergy == true)
	{
		if (DEBUG_OUT)
			Serial.println("[ChargePointStatusService] Warning: startEvDrawsEnergy called twice or didn't call stopEvDrawsEnergy before\n");
	}
	evDrawsEnergy = true;
	// this->evse_ChargePointStatus = Preparing;
}

void ChargePointStatusService::stopEvDrawsEnergy()
{
	if (evDrawsEnergy == false)
	{
		if (DEBUG_OUT)
			Serial.println("[ChargePointStatusService] Warning: stopEvDrawsEnergy called twice or didn't call startEvDrawsEnergy before\n");
	}
	evDrawsEnergy = false;
}
void ChargePointStatusService::startEnergyOffer()
{
	if (evseOffersEnergy == true)
	{
		if (DEBUG_OUT)
			Serial.println("[ChargePointStatusService] Warning: startEnergyOffer called twice or didn't call stopEnergyOffer before\n");
	}
	evseOffersEnergy = true;
}

void ChargePointStatusService::stopEnergyOffer()
{
	if (evseOffersEnergy == false)
	{
		if (DEBUG_OUT)
			Serial.println("[ChargePointStatusService] Warning: stopEnergyOffer called twice or didn't call startEnergyOffer before\n");
	}
	evseOffersEnergy = false;
}

/*****Added new Definition @wamique***********/
bool ChargePointStatusService::getEvDrawsEnergy()
{
	return evDrawsEnergy;
}

/*********************************************/

/*
 * @brief : Feature added by G. Raja Sumant
 * 09/07/2022 For Change availability
 */
void ChargePointStatusService::setUnavailable(bool su)
{
	this->unavailable = su;
	// this->evse_ChargePointStatus = Unavailable;
}

bool ChargePointStatusService::getUnavailable()
{
	return unavailable;
}

/*
 * @brief : Feature added by G. Raja Sumant
 * 19/07/2022 For ReserveNow
 */
void ChargePointStatusService::setReserved(bool re)
{
	this->reserved = re;
	// this->evse_ChargePointStatus = Reserved;
}

bool ChargePointStatusService::getReserved()
{
	return reserved;
}

/*
 * @brief : Feature added by G. Raja Sumant
 * 19/07/2022 For Finishing state
 */
void ChargePointStatusService::setFinishing(bool fi)
{
	this->finished = fi;
	// this->evse_ChargePointStatus = Finishing;
}

bool ChargePointStatusService::getFinishing()
{
	return finished;
}