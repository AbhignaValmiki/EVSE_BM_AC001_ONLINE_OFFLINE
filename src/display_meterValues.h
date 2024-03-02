#ifndef DISPLAY_METERVALUES_H
#define DISPLAY_METERVALUES_H

#include "Master.h"
#include "OcppMessage.h"
#include "ChargePointStatusService.h"
#include "OcppEngine.h"


enum Connector_Charging_Status
{
NotCharging,
Connector_A,
Connector_B,
Connector_A_B,
Connector_C,
Connector_A_C,
Connector_B_C,
Connector_A_B_C,

};
 
extern Connector_Charging_Status Connector_Charging_Status_Disp;

void stateTimer();
void disp_dwin_meter();
void disp_lcd_meter();
uint8_t Display_Set(uint8_t lu8_Status);
uint8_t Display_Clear(uint8_t lu8_Status);
uint8_t EVSE_Display_Read_status(void);
void charging_disp_A(void);
void charging_disp_B(void);
void charging_disp_C(void);
void disp_fault_code_A(void);
void disp_fault_code_B(void);
void disp_fault_code_C(void);



#endif