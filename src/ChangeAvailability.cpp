/*
 * ChangeAvailability.cpp
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

#include "Variants.h"

#include "ChangeAvailability.h"
#include "OcppEngine.h"

/*
 * @breif: Instantiate an object ChangeAvailability
 */

ChangeAvailability::ChangeAvailability()
{
}

/*
 * @breif: Method - getOcppOperationType => This method gives the type of Ocpp operation
 */

const char *ChangeAvailability::getOcppOperationType()
{
    return "ChangeAvailability";
}

void ChangeAvailability::processReq(JsonObject payload)
{

    // Required field
    connectorId = payload["connectorId"].as<int>();

    if (connectorId == 1 || connectorId == 2 || connectorId == 3 || connectorId == 0)
    {
        accepted = true;
    }
    else
    {
        accepted = false;
    }
    // Required field
    const char *type = payload["type"] | "Invalid";
    switch (connectorId)
    {
    case 0:
        if (!strcmp(type, "Inoperative"))
        {
            // requestLed(BLINKYWHITE, START, 1);
            // requestLed(BLINKYWHITE, START, 2);
            // requestLed(BLINKYWHITE, START, 3);
            // First check the current status.
            if ((getChargePointStatusService_A()->inferenceStatus() == ChargePointStatus::Available || getChargePointStatusService_A()->inferenceStatus() == ChargePointStatus::Faulted))
            {
                // set the status to unavailable. Status notification will take care of the rest.
                // getChargePointStatusService_A()->setUnavailable(true);
                getChargePointStatusService_A()->setChargePointstatus(Unavailable);

                accepted = true;
            }
            else
            {
                accepted = false;
            }

            if ((getChargePointStatusService_B()->inferenceStatus() == ChargePointStatus::Available || getChargePointStatusService_B()->inferenceStatus() == ChargePointStatus::Faulted))
            {
                // set the status to unavailable. Status notification will take care of the rest.
                getChargePointStatusService_B()->setUnavailable(true);
                accepted = true;
            }
            else
            {
                accepted = false;
            }

            if ((getChargePointStatusService_C()->inferenceStatus() == ChargePointStatus::Available || getChargePointStatusService_C()->inferenceStatus() == ChargePointStatus::Faulted))
            {
                // set the status to unavailable. Status notification will take care of the rest.
                getChargePointStatusService_C()->setUnavailable(true);
                accepted = true;
            }
            else
            {
                accepted = false;
            }
        }
        else if (!strcmp(type, "Operative"))
        {
            // change the status if it is unavailable only.
            if ((getChargePointStatusService_A()->inferenceStatus() == ChargePointStatus::Unavailable))
            {
                // It now becomes available.
                // getChargePointStatusService_A()->setUnavailable(false);
                getChargePointStatusService_A()->setChargePointstatus(Available);

                accepted = true;
            }
            else
            {
                accepted = false;
            }

            // change the status if it is unavailable only.
            if ((getChargePointStatusService_B()->inferenceStatus() == ChargePointStatus::Unavailable))
            {
                // It now becomes available.
                getChargePointStatusService_B()->setUnavailable(false);
                accepted = true;
            }
            else
            {
                accepted = false;
            }
            // change the status if it is unavailable only.
            if ((getChargePointStatusService_C()->inferenceStatus() == ChargePointStatus::Unavailable))
            {
                // It now becomes available.
                getChargePointStatusService_C()->setUnavailable(false);
                accepted = true;
            }
            else
            {
                accepted = false;
            }
        }

        break;
    case 1:
        if (!strcmp(type, "Inoperative"))
        {
            // requestLed(BLINKYWHITE, START, 1);
            // First check the current status.
            if ((getChargePointStatusService_A()->inferenceStatus() == ChargePointStatus::Available || getChargePointStatusService_A()->inferenceStatus() == ChargePointStatus::Faulted))
            {
                // set the status to unavailable. Status notification will take care of the rest.
                getChargePointStatusService_A()->setUnavailable(true);
                accepted = true;
            }
            else
            {
                accepted = false;
            }
        }
        else if (!strcmp(type, "Operative"))
        {
            // change the status if it is unavailable only.
            if ((getChargePointStatusService_A()->inferenceStatus() == ChargePointStatus::Unavailable))
            {
                // It now becomes available.
                getChargePointStatusService_A()->setUnavailable(false);
                accepted = true;
            }
            else
            {
                accepted = false;
            }
        }
        break;

    case 2:
        if (!strcmp(type, "Inoperative"))
        {
            // requestLed(BLINKYWHITE, START, 2);
            // First check the current status.
            if ((getChargePointStatusService_B()->inferenceStatus() == ChargePointStatus::Available || getChargePointStatusService_B()->inferenceStatus() == ChargePointStatus::Faulted))
            {
                // set the status to unavailable. Status notification will take care of the rest.
                getChargePointStatusService_B()->setUnavailable(true);
                accepted = true;
            }
            else
            {
                accepted = false;
            }
        }
        else if (!strcmp(type, "Operative"))
        {
            // change the status if it is unavailable only.
            if ((getChargePointStatusService_B()->inferenceStatus() == ChargePointStatus::Unavailable))
            {
                // It now becomes available.
                getChargePointStatusService_B()->setUnavailable(false);
                accepted = true;
            }
            else
            {
                accepted = false;
            }
        }
        break;

    case 3:
        if (!strcmp(type, "Inoperative"))
        {
            // requestLed(BLINKYWHITE, START, 3);
            // First check the current status.
            if ((getChargePointStatusService_C()->inferenceStatus() == ChargePointStatus::Available || getChargePointStatusService_C()->inferenceStatus() == ChargePointStatus::Faulted))
            {
                // set the status to unavailable. Status notification will take care of the rest.
                getChargePointStatusService_C()->setUnavailable(true);
                accepted = true;
            }
            else
            {
                accepted = false;
            }
        }
        else if (!strcmp(type, "Operative"))
        {
            // change the status if it is unavailable only.
            if ((getChargePointStatusService_C()->inferenceStatus() == ChargePointStatus::Unavailable))
            {
                // It now becomes available.
                getChargePointStatusService_C()->setUnavailable(false);
                accepted = true;
            }
            else
            {
                accepted = false;
            }
        }
        break;
    }

    Serial.println(F("[ChangeAvailability] got the request to change availability"));
}

void ChangeAvailability::processConf(JsonObject payload)
{
    /*
     * Change availability update should be processed.
     */
}

DynamicJsonDocument *ChangeAvailability::createReq(JsonObject payload)
{
    /*
     * Ignore Contents of this Req-message, because this is for debug purposes only
     */
}

/*
 * @breif: Added by G. Raja Sumant on the lines of ChangeConfiguration which has
 * 1 field defined as Enum - Accepted/Rejected/Scheduled.
 */
DynamicJsonDocument *ChangeAvailability::createConf()
{
    // As per OCPP 1.6 it is being given similar to ChangeConfiguration
    DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1));
    JsonObject payload = doc->to<JsonObject>();
    if (accepted)
    {
        payload["status"] = "Accepted";
    }
    else
    {

        payload["status"] = "Rejected";
    }
    accepted = false; // reset the flag
    return doc;
}
