#include "LED.h"
#include "ChargePointStatusService.h"
#include "Master.h"
#include "OcppEngine.h"
#include "display_meterValues.h"
extern bool webSocketConncted;
extern bool notFaulty_A;
extern bool notFaulty_B;
extern bool notFaulty_C;

extern int8_t fault_code_A;
extern int8_t fault_code_B;
extern int8_t fault_code_C;

extern bool EMGCY_FaultOccured_A;
extern bool EMGCY_FaultOccured_B;
extern bool EMGCY_FaultOccured_C;

// uint8_t led_A = 0;
// uint8_t led_B = 0;
// uint8_t led_C = 0;
uint8_t flag_offled = 0;
ChargePointStatus inferencedStatus_led;

void EVSE_Led_loop(void)
{
    if (webSocketConncted)
    {
        inferencedStatus_led = getChargePointStatusService_A()->inferenceStatus();
        switch (inferencedStatus_led)
        {
        case Available:
            requestLed(GREEN, START, 1);
            break;
        case Preparing:
            requestLed(BLINKYBLUE, START, 1);
            break;
        case Charging:
            requestLed(BLINKYGREEN, START, 1);
            break;
        case Finishing:
            requestLed(BLINKYBLUE, START, 1);
            break;
        case Reserved:
            requestLed(WHITE, START, 1);
            break;
        case Unavailable:
            requestLed(BLINKYWHITE, START, 1);
            break;
        case Faulted:
            if (EMGCY_FaultOccured_A)
            {
                requestLed(BLINKYRED, START, 1);
            }
            else
            {
                requestLed(RED, START, 1);
            }
            break;
        default:
            break;
        }
        inferencedStatus_led = getChargePointStatusService_B()->inferenceStatus();

        switch (inferencedStatus_led)
        {
        case Available:
            requestLed(GREEN, START, 2);
            break;
        case Preparing:
            requestLed(BLINKYBLUE, START, 2);
            break;
        case Charging:
            requestLed(BLINKYGREEN, START, 2);
            break;
        case Finishing:
            requestLed(BLINKYBLUE, START, 2);
            break;
        case Reserved:
            requestLed(WHITE, START, 2);
            break;
        case Unavailable:
            requestLed(BLINKYWHITE, START, 2);
            break;
        case Faulted:
            if (EMGCY_FaultOccured_B)
            {
                requestLed(BLINKYRED, START, 2);
            }
            else
            {
                requestLed(RED, START, 2);
            }
            break;
        default:
            break;
        }

        inferencedStatus_led = getChargePointStatusService_C()->inferenceStatus();
        switch (inferencedStatus_led)
        {
        case Available:
            requestLed(GREEN, START, 3);
            break;
        case Preparing:
            requestLed(BLINKYBLUE, START, 3);
            break;
        case Charging:
            requestLed(BLINKYGREEN, START, 3);
            break;
        case Finishing:
            requestLed(BLINKYBLUE, START, 3);
            break;
        case Reserved:
            requestLed(WHITE, START, 3);
            break;
        case Unavailable:
            requestLed(BLINKYWHITE, START, 3);
            break;
        case Faulted:
            if (EMGCY_FaultOccured_C)
            {
                requestLed(BLINKYRED, START, 3);
            }
            else
            {
                requestLed(RED, START, 3);
            }
            break;
        default:
        
            break;
        }
    }
    else if (webSocketConncted == 0)
    {
        flag_offled++;
        if(flag_offled > 4)
        {
            flag_offled = 0;
            requestLed(BLINKYWHITE_ALL,START,1);
            delay(1000);
        }
        inferencedStatus_led = getChargePointStatusService_A()->inferenceStatus();
        switch (inferencedStatus_led)
        {
        case Available:
            requestLed(GREEN, START, 1);
            break;
        case Preparing: 
            requestLed(BLINKYBLUE, START, 1);
            break;
        case Charging:
        {
            // led_A++;
            // if (led_A >= 4)
            // {
            //     led_A = 0;
            //     requestLed(BLINKYWHITE, START, 1);
            // }
            // else
            // {
                requestLed(BLINKYGREEN, START, 1);
            // }
        }
        break;
        case Finishing:
            requestLed(BLINKYBLUE, START, 1);
            break;
        case Reserved:
            requestLed(WHITE, START, 1);
            break;
        case Unavailable:
            requestLed(BLINKYWHITE, START, 1);
            break;
        case Faulted:
            if (EMGCY_FaultOccured_A)
            {
                requestLed(BLINKYRED, START, 1);
            }
            else
            {
                requestLed(RED, START, 1);
            }
            break;
        default:
        requestLed(GREEN, START, 1);
            break;
        }
        inferencedStatus_led = getChargePointStatusService_B()->inferenceStatus();

        switch (inferencedStatus_led)
        {
        case Available:
            // led_B++;
             requestLed(GREEN, START, 2);
            break;
        case Preparing:
            requestLed(BLINKYBLUE, START, 2);
            break;
        case Charging:
            // led_B++;
            // if (led_B >= 4)
            // {
            //     led_B = 0;
            //     requestLed(BLINKYWHITE, START, 2);
            // }
            // else
            // {
                requestLed(BLINKYGREEN, START, 2);
            // }
            break;
        case Finishing:
            requestLed(BLINKYBLUE, START, 2);
            break;
        case Reserved:
            requestLed(WHITE, START, 2);
            break;
        case Unavailable:
            requestLed(BLINKYWHITE, START, 2);
            break;
        case Faulted:
            if (EMGCY_FaultOccured_B)
            {
                requestLed(BLINKYRED, START, 2);
            }
            else
            {
                requestLed(RED, START, 2);
            }
            break;
        default:
              requestLed(GREEN, START, 2);
            break;
        }

        inferencedStatus_led = getChargePointStatusService_C()->inferenceStatus();
        switch (inferencedStatus_led)
        {
        case Available:
        requestLed(GREEN, START, 3);
            break;
        case Preparing:
            requestLed(BLINKYBLUE, START, 3);
            break;
        case Charging:
            // led_C++;
            // if (led_C >= 4)
            // {
            //     led_C = 0;
            //     requestLed(BLINKYWHITE, START, 3);
            // }
            // else
            // {
                requestLed(BLINKYGREEN, START, 3);
            // }
            break;
        case Finishing:
            requestLed(BLINKYBLUE, START, 3);
            break;
        case Reserved:
            requestLed(WHITE, START, 3);
            break;
        case Unavailable:
            requestLed(BLINKYWHITE, START, 3);
            break;
        case Faulted:
            if (EMGCY_FaultOccured_C)
            {
                requestLed(BLINKYRED, START, 3);
            }
            else
            {
                requestLed(RED, START, 3);
            }
            break;
        default:
        requestLed(GREEN, START, 3);
            break;
        }
    }
}
