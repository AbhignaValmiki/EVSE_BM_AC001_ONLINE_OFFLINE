#include "ATM90E36.h"
#include "OcppEngine.h"
#include "Master.h"
#include "EVSE_A.h"
#include "EVSE_B.h"
#include "EVSE_C.h"
#if DISPLAY_ENABLED
#include "display.h"
#endif

#if DWIN_ENABLED
#include "dwin.h"
extern unsigned char avail[28];
extern unsigned char fault_emgy[28];
extern unsigned char fault_noearth[28];
extern unsigned char fault_overVolt[28];
extern unsigned char fault_underVolt[28];
extern unsigned char fault_overTemp[28];
extern unsigned char fault_overCurr[28];
extern unsigned char fault_underCurr[28];
extern unsigned char fault_suspEV[28];
extern unsigned char fault_suspEVSE[28];
extern unsigned char charging[28];
extern unsigned char fault_nopower[28];
#endif

#if LCD_DISPLAY_ENABLED
#include "LCD_I2C.h"
extern LCD_I2C lcd;
#endif

#if V_charge_lite1_4
#include "Master.h"
#endif

/*
 * @brief: Feature added by Raja
 * This feature will avoid hardcoding of messages.
 */

// uint8_t reasonForStop = 3; //Local is the default value

uint8_t reasonForStop_A = 3; // Local is the default value
uint8_t reasonForStop_B = 3; // Local is the default value
uint8_t reasonForStop_C = 3; // Local is the default value

extern bool disp_evse_A;
extern bool disp_evse_B;
extern bool disp_evse_C;
extern float drawing_current_A;
extern float drawing_current_B;
extern float drawing_current_C;
extern float minCurr;
extern short int counter_drawingCurrent_B;
extern short int counter_drawingCurrent_A;
extern short int counter_drawingCurrent_C;
extern uint8_t currentCounterThreshold_A;
 extern uint8_t currentCounterThreshold_B;
 extern uint8_t currentCounterThreshold_C;
extern bool notFaulty_A;
extern bool notFaulty_B;
extern bool notFaulty_C;
extern uint8_t fault_occured_A;
extern uint8_t fault_occured_B;
extern uint8_t fault_occured_C;

extern uint8_t offline_charging_A;
extern uint8_t offline_charging_B;
extern uint8_t offline_charging_C;

extern bool ongoingTxnoff_A;
extern bool ongoingTxnoff_B;
extern bool ongoingTxnoff_C;
extern Preferences resumeTxn_A;
extern Preferences resumeTxn_B;
extern Preferences resumeTxn_C;

typedef enum resonofstop
{
  EmergencyStop,
  EVDisconnected,
  HardReset,
  Local,
  Other,
  PowerLoss,
  Reboot,
  Remote,
  Softreset,
  UnlockCommand,
  DeAuthorized
};

static const char *resonofstop_str[] = {"EmergencyStop", "EVDisconnected", "HardReset", "Local", "Other", "PowerLoss", "Reboot", "Remote", "SoftReset", "UnlockCommand", "DeAuthorized"};

/*
 * @brief: Feature added by Raja
 * This feature will avoid hardcoding of messages.
 */
typedef enum faultCode
{
  OverVoltage,
  UnderVoltage,
  OverCurrent,
  UnderCurrent,
  OverTemperature,
  UnderTemperature,
  GFCI,
  EarthDisconnect,
  OtherError,
  Power_Loss
};

int8_t fault_code_A = -1;
int8_t fault_code_B = -1;
int8_t fault_code_C = -1;

bool flag_ed_A = false;
bool flag_nopower = false;

ATM90E36::ATM90E36(int pin) // Object
{
  // energy_IRQ = 2; 	// (In development...)
  _energy_CS = pin; // SS PIN
  // energy_WO = 8; 		// (In development...)
}

/* CommEnergyIC - Communication Establishment */
/*
- Defines Register Mask
- Treats the Register and SPI Comms
- Outputs the required value in the register
*/
unsigned short ATM90E36::CommEnergyIC(unsigned char RW, unsigned short address, unsigned short val)
{
  unsigned char *data = (unsigned char *)&val;
  unsigned char *adata = (unsigned char *)&address;
  unsigned short output;
  unsigned short address1;

  // Slows the SPI interface to communicate
#if !defined(ENERGIA) && !defined(ESP8266) && !defined(ARDUINO_ARCH_SAMD)
  SPISettings settings(200000, MSBFIRST, SPI_MODE0);
#endif

#if defined(ESP8266)
  SPISettings settings(200000, MSBFIRST, SPI_MODE2);
#endif

#if defined(ARDUINO_ARCH_SAMD)
  SPISettings settings(200000, MSBFIRST, SPI_MODE3);
#endif

  // Switch MSB and LSB of value
  output = (val >> 8) | (val << 8);
  val = output;

  // Set R/W flag
  address |= RW << 15;

  // Swap byte address
  address1 = (address >> 8) | (address << 8);
  address = address1;

  // Transmit & Receive Data
#if !defined(ENERGIA)
  // SPI.beginTransaction(settings);
  hspi->beginTransaction(settings);
#endif

  // Chip enable and wait for SPI activation
  digitalWrite(_energy_CS, LOW);
  delayMicroseconds(10);

  // Write address byte by byte
  for (byte i = 0; i < 2; i++)
  {
    hspi->transfer(*adata);
    adata++;
  }

  // SPI.transfer16(address);
  /* Must wait 4 us for data to become valid */
  delayMicroseconds(4);

  // READ Data
  // Do for each byte in transfer
  if (RW)
  {
    for (byte i = 0; i < 2; i++)
    {
      *data = hspi->transfer(0x00);
      data++;
    }
    // val = SPI.transfer16(0x00);
  }
  else
  {
    for (byte i = 0; i < 2; i++)
    {
      hspi->transfer(*data);
      data++;
    }
    // SPI.transfer16(val);
  }

  // Chip enable and wait for transaction to end
  digitalWrite(_energy_CS, HIGH);
  delayMicroseconds(10);
#if !defined(ENERGIA)
  hspi->endTransaction();
#endif

  output = (val >> 8) | (val << 8); // reverse MSB and LSB
  return output;

  // Use with transfer16
  // return val;
}

/* Parameters Functions*/
/*
- Gets main electrical parameters,
such as: Voltage, Current, Power, Energy,
and Frequency
- Also gets the temperature
*/
// VOLTAGE
double ATM90E36::GetLineVoltageA()
{

  unsigned short voltage = CommEnergyIC(READ, UrmsA, 0xFFFF);
  double volt = (double)voltage / 100;
  unsigned short voltage_B = CommEnergyIC(READ, UrmsB, 0xFFFF);
  double volt_B = (double)voltage_B / 100;
  unsigned short voltage_C = CommEnergyIC(READ, UrmsC, 0xFFFF);
  double volt_C = (double)voltage_C / 100;
  if ((volt < 20) && (volt_B < 20) && (volt_C < 20))
  {
    flag_nopower = true;
  }
  else
  {
    flag_nopower = false;
  }
  uint8_t err = 0;

  // bool EMGCY_status = requestEmgyStatus();

#if EARTH_DISCONNECT
  if (flag_nopower)
  {
    if (flag_nopower)
    {
    Serial.println("***Power fail****");
    reasonForStop_A = PowerLoss;
    reasonForStop_B = PowerLoss;
    reasonForStop_C = PowerLoss;
    fault_code_A = Power_Loss;
    fault_code_B = Power_Loss;
    fault_code_C = Power_Loss;
    notFaulty_A = false;
    notFaulty_B = false;
    notFaulty_C = false;
    fault_occured_A = 1;
		fault_occured_B = 1;
		fault_occured_C = 1;

    getChargePointStatusService_A()->setChargePointstatus(Faulted);
    getChargePointStatusService_B()->setChargePointstatus(Faulted);
    getChargePointStatusService_C()->setChargePointstatus(Faulted);
    if (getChargePointStatusService_A()->getEvDrawsEnergy() == true)
    {
      EVSE_A_StopSession();
    }
    if (getChargePointStatusService_B()->getEvDrawsEnergy() == true)
    {
      EVSE_B_StopSession();
    }
    if (getChargePointStatusService_C()->getEvDrawsEnergy() == true)
    {
      EVSE_C_StopSession();
    }
#if LCD_ENABLED_FAULT

    // lcd.clear();
#if 0
      lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      lcd.print("                    "); // Clear the line
      lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      lcd.print("STATUS: FAULTED");
#endif
    lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
    lcd.print("                    "); // Clear the line
    lcd.setCursor(0, 1);               // Or setting the cursor in the desired position.
    lcd.print("                    "); // Clear the line
    lcd.setCursor(0, 2);               // Or setting the cursor in the desired position.
    lcd.print("                    "); // Clear the line
    lcd.setCursor(0, 3);               // Or setting the cursor in the desired position.
    lcd.print("***POWER FAILURE***");

#endif
#if DWIN_ENABLED
    fault_nopower[4] = 0X66; // In the fourth page.
    fault_nopower[5] = 0X00; // In the fourth page.
    err = DWIN_SET(fault_nopower, sizeof(fault_nopower) / sizeof(fault_nopower[0]));
    fault_nopower[4] = 0X71; // In the fifth page.
    fault_nopower[5] = 0X00; // In the fourth page.
    err = DWIN_SET(fault_nopower, sizeof(fault_nopower) / sizeof(fault_nopower[0]));
    fault_nopower[4] = 0X71; // In the fifth page.
    fault_nopower[5] = 0X50; // In the fourth page.
    err = DWIN_SET(fault_nopower, sizeof(fault_nopower) / sizeof(fault_nopower[0]));
    fault_nopower[4] = 0X7B; // In the sixth page.
    fault_nopower[5] = 0X00; // In the fourth page.
    err = DWIN_SET(fault_nopower, sizeof(fault_nopower) / sizeof(fault_nopower[0]));
    fault_nopower[4] = 0X7B; // In the sixth page.
    fault_nopower[5] = 0X50; // In the fourth page.
    err = DWIN_SET(fault_nopower, sizeof(fault_nopower) / sizeof(fault_nopower[0]));
#endif
    }
    else
    {
      notFaulty_A = true;
      notFaulty_B = true;
      notFaulty_C = true;
      fault_occured_A = 0;
			fault_occured_B = 0;
			fault_occured_C = 0;
    }
  }
  else if (flag_nopower == false)
  {
#if V_charge_lite1_4
    bool stat = requestEarthDiscStatus();
#else
    bool stat = digitalRead(earth_disconnect_pin);
#endif
    if (stat)
    {
      Serial.println("earth disconnect");
#if DISPLAY_ENABLED
      connAvail(1, "EARTH DISCONNECT");
      checkForResponse_Disp();
      connAvail(2, "EARTH DISCONNECT");
      checkForResponse_Disp();
      connAvail(3, "EARTH DISCONNECT");
      checkForResponse_Disp();
      setHeader("RFID UNAVAILABLE");
      checkForResponse_Disp();
#endif
      getChargePointStatusService_A()->setEarthDisconnect(true);
      fault_code_A = EarthDisconnect;
      fault_code_B = EarthDisconnect;
      fault_code_C = EarthDisconnect;
      flag_ed_A = true;
      reasonForStop_A = Other;
      reasonForStop_B = Other;
      reasonForStop_C = Other;
#if DWIN_ENABLED
      fault_noearth[4] = 0X66; // In the fourth page.
      fault_noearth[5] = 0X00; // In the fourth page.
      err = DWIN_SET(fault_noearth, sizeof(fault_noearth) / sizeof(fault_noearth[0]));
      // fault_noearth[4] = 0X51; // In the fourth page.
      // fault_noearth[5] = 0X00; // In the fourth page.
      // err = DWIN_SET(fault_noearth,sizeof(fault_noearth)/sizeof(fault_noearth[0]));
      fault_noearth[4] = 0X71; // In the fifth page.
      fault_noearth[5] = 0X00; // In the fourth page.
      err = DWIN_SET(fault_noearth, sizeof(fault_noearth) / sizeof(fault_noearth[0]));
      fault_noearth[4] = 0X71; // In the fifth page.
      fault_noearth[5] = 0X50; // In the fourth page.
      err = DWIN_SET(fault_noearth, sizeof(fault_noearth) / sizeof(fault_noearth[0]));
      fault_noearth[4] = 0X7B; // In the sixth page.
      fault_noearth[5] = 0X00; // In the fourth page.
      err = DWIN_SET(fault_noearth, sizeof(fault_noearth) / sizeof(fault_noearth[0]));
      fault_noearth[4] = 0X7B; // In the sixth page.
      fault_noearth[5] = 0X50; // In the fourth page.
      err = DWIN_SET(fault_noearth, sizeof(fault_noearth) / sizeof(fault_noearth[0]));
#endif
#if LCD_ENABLED_FAULT

      // lcd.clear();
#if 0
      lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      lcd.print("                    "); // Clear the line
      lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      lcd.print("STATUS: FAULTED");
#endif
      lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      lcd.print("                    "); // Clear the line
      lcd.setCursor(0, 3);               // Or setting the cursor in the desired position.
      if (volt > 20)
      {
        lcd.print("EARTH DISCONNECT");
        lcd.setCursor(0, 1);               // Or setting the cursor in the desired position.
        lcd.print("                    "); // Clear the line
        lcd.setCursor(0, 3);               // Or setting the cursor in the desired position.
        lcd.print("                    "); // Clear the line
        lcd.setCursor(0, 2);               // Or setting the cursor in the desired position.
        lcd.print("                    "); // Clear the line
        lcd.setCursor(0, 3);               // Or setting the cursor in the desired position.
        lcd.print("                    "); // Clear the line
      }
      else
        lcd.print("NO POWER");

#endif
    }
    else
    {
      getChargePointStatusService_A()->setEarthDisconnect(false);
      // lcd.setCursor(0, 1); // Or setting the cursor in the desired position.
      // lcd.print("                    ");//Clear the line
      flag_ed_A = false;
    }
  }
#endif
  if (volt < 200)
  {
    // if (volt<170){ // For LB nagar hubs
    if (!flag_ed_A && !flag_nopower)
    {
      getChargePointStatusService_A()->setUnderVoltage(true);
      getChargePointStatusService_A()->setOverVoltage(false);
      fault_code_A = UnderVoltage;
      reasonForStop_A = Other;
#if DISPLAY_ENABLED
      if (volt > 20)
      {
        connAvail(1, "UNDER VOLTAGE");
        checkForResponse_Disp();
      }
      else
      {
        connAvail(1, "NO POWER");
        checkForResponse_Disp();
      }
#endif

#if LCD_ENABLED_FAULT
      if (!disp_evse_C && !disp_evse_B)
      {
#if 0
      lcd.clear();
      lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      lcd.print("                    "); // Clear the line
      lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      lcd.print("STATUS: FAULTED");
#endif
        lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
        lcd.print("                    "); // Clear the line
        lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
        if (volt > 20)
          lcd.print("A: UNDER VOLTAGE");
        else
          lcd.print("A: NO POWER");
      }
/*else
{
  lcd.clear();
  lcd.setCursor(0, 0);
  if (volt > 20)
        lcd.print("A: UNDER VOLTAGE");
      else
        lcd.print("A: NO POWER");
}*/
#endif
#if DWIN_ENABLED
      if (volt > 20)
      {
        fault_underVolt[4] = 0X66; // In the fourth page.
        fault_underVolt[5] = 0X00; // In the fourth page.
        err = DWIN_SET(fault_underVolt, sizeof(fault_underVolt) / sizeof(fault_underVolt[0]));
      }
      else
      {
        fault_nopower[4] = 0X66; // In the fourth page.
        fault_nopower[5] = 0X00; // In the fourth page.
        err = DWIN_SET(fault_nopower, sizeof(fault_nopower) / sizeof(fault_nopower[0]));
      }
// fault_underVolt[4] = 0X51; // In the fourth page.
// fault_underVolt[5] = 0X00; // In the fourth page.
// err = DWIN_SET(fault_underVolt,sizeof(fault_underVolt)/sizeof(fault_underVolt[0]));
#endif
      if (DEBUG_OUT)
        Serial.println("[EVSE_A] Under Voltage");
      return custom_round(volt);
    }
    else
    {
      return custom_round(volt);
    }
  }
  else if (volt > 275)
  {
    if (!flag_ed_A && !flag_nopower)
    {
      getChargePointStatusService_A()->setUnderVoltage(false);
      getChargePointStatusService_A()->setOverVoltage(true);
      fault_code_A = OverVoltage;
      reasonForStop_A = Other;
#if DISPLAY_ENABLED
      connAvail(1, "OVER VOLTAGE");
      checkForResponse_Disp();
#endif
#if LCD_ENABLED_FAULT
      if (!disp_evse_C && !disp_evse_B)
      {
// lcd.clear();
#if 0
      lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      lcd.print("                    "); // Clear the line
      lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      lcd.print("STATUS: FAULTED");
#endif
        lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
        lcd.print("                    "); // Clear the line
        lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
        lcd.print("A: OVER VOLTAGE");
      }
/*else
{
 lcd.clear();
 lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
 lcd.print("A: OVER VOLTAGE");
}*/
#endif
#if DWIN_ENABLED
      fault_overVolt[4] = 0X66;
      fault_overVolt[5] = 0X00;
      err = DWIN_SET(fault_overVolt, sizeof(fault_overVolt) / sizeof(fault_overVolt[0]));
// fault_overVolt[4] = 0X51;
// fault_overVolt[5] = 0X00;
// err = DWIN_SET(fault_overVolt,sizeof(fault_overVolt)/sizeof(fault_overVolt[0]));
#endif
      if (DEBUG_OUT)
        Serial.println("[EVSE_A] Over Voltage");
      return custom_round(volt);
    }
    else
    {
      return custom_round(volt);
    }
  }
  else
  {

    getChargePointStatusService_A()->setUnderVoltage(false);
    getChargePointStatusService_A()->setOverVoltage(false);
// if(DEBUG_OUT) Serial.println(F("[EVSE_A] Normal Voltage Range."));
#if DWIN_ENABLED
// avail[4] = 0X51;
// err = DWIN_SET(avail,sizeof(avail)/sizeof(avail[0]));
// charging[4] = 0X66;
// err = DWIN_SET(charging,sizeof(charging)/sizeof(charging[0]));
#endif
    return custom_round(volt);
  }
}

double ATM90E36::GetLineVoltageB()
{
  unsigned short voltage = CommEnergyIC(READ, UrmsB, 0xFFFF);
  double volt = (double)voltage / 100;
  unsigned short voltage_A = CommEnergyIC(READ, UrmsA, 0xFFFF);
  double volt_A = (double)voltage_A / 100;
  unsigned short voltage_C = CommEnergyIC(READ, UrmsC, 0xFFFF);
  double volt_C = (double)voltage_C / 100;
  if ((volt < 20) && (volt_A < 20) && (volt_C < 20))
  {
    flag_nopower = true;
  }
  else
  {
    flag_nopower = false;
  }
  uint8_t err = 0;
  // bool EMGCY_status = requestEmgyStatus();
#if EARTH_DISCONNECT
  if (flag_nopower)
  {
    Serial.println("Power fail");
    reasonForStop_A = PowerLoss;
    reasonForStop_B = PowerLoss;
    reasonForStop_C = PowerLoss;
    fault_code_A = Power_Loss;
    fault_code_B = Power_Loss;
    fault_code_C = Power_Loss;
  }
#endif

  // unsigned short voltage = CommEnergyIC(READ, UrmsB, 0xFFFF);
  // double volt = (double)voltage / 100;
  // uint8_t err = 0;
  if (volt < 200)
  {
    // if (volt<170){ // For LB nagar hubs
    if (!flag_ed_A && !flag_nopower)
    {
      getChargePointStatusService_B()->setUnderVoltage(true);
      getChargePointStatusService_B()->setOverVoltage(false);
      fault_code_B = UnderVoltage;
      reasonForStop_B = Other;
#if DISPLAY_ENABLED
      if (volt > 20)
      {
        connAvail(2, "UNDER VOLTAGE");
        checkForResponse_Disp();
      }
      else
      {
        connAvail(2, "NO POWER");
        checkForResponse_Disp();
      }
#endif
#if LCD_ENABLED_FAULT
      if (!disp_evse_A && !disp_evse_C)
      {
// lcd.clear();
#if 0
      lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      lcd.print("                    "); // Clear the line
      lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      lcd.print("STATUS: FAULTED");
#endif
        lcd.setCursor(0, 1);               // Or setting the cursor in the desired position.
        lcd.print("                    "); // Clear the line
        lcd.setCursor(0, 1);               // Or setting the cursor in the desired position.
        if (volt > 20)
          lcd.print("B: UNDER VOLTAGE");
        else
          lcd.print("B: NO POWER");
      }
/*else
{
  //This means any one is charging. Try clearing the entire screen.
   lcd.clear();
   lcd.setCursor(0, 1);
      if (volt > 20)
        lcd.print("B: UNDER VOLTAGE");
      else
        lcd.print("B: NO POWER");
}*/
#endif
#if DWIN_ENABLED
      if (volt > 20)
      {
        fault_underVolt[4] = 0X71;
        fault_underVolt[5] = 0X00;
        err = DWIN_SET(fault_underVolt, sizeof(fault_underVolt) / sizeof(fault_underVolt[0]));
      }
      else
      {
        fault_nopower[4] = 0X71; // In the fifth page.
        fault_nopower[5] = 0X00; // In the fourth page.
        err = DWIN_SET(fault_nopower, sizeof(fault_nopower) / sizeof(fault_nopower[0]));
      }

// fault_underVolt[4] = 0X51;
// fault_underVolt[5] = 0X00;
// err = DWIN_SET(fault_underVolt,sizeof(fault_underVolt)/sizeof(fault_underVolt[0]));
#endif
      if (DEBUG_OUT)
        Serial.println("[EVSE_B] Under Voltage");
      return custom_round(volt);
    }
    else
    {
      return custom_round(volt);
    }
  }
  else if (volt > 275)
  {
    if (!flag_ed_A && !flag_nopower)
    {
      getChargePointStatusService_B()->setUnderVoltage(false);
      getChargePointStatusService_B()->setOverVoltage(true);
      fault_code_B = OverVoltage;
      reasonForStop_B = Other;
#if DISPLAY_ENABLED
      connAvail(2, "OVER VOLTAGE");
      checkForResponse_Disp();
#endif
#if LCD_ENABLED_FAULT
      if (!disp_evse_A && !disp_evse_C)
      {
// lcd.clear();
#if 0
      lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      lcd.print("                    "); // Clear the line
      lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      lcd.print("STATUS: FAULTED");
#endif
        lcd.setCursor(0, 1);               // Or setting the cursor in the desired position.
        lcd.print("                    "); // Clear the line
        lcd.setCursor(0, 1);               // Or setting the cursor in the desired position.
        lcd.print("B: OVER VOLTAGE");
      }
/*else
{
  lcd.clear();
  lcd.setCursor(0, 1);
    lcd.setCursor(0, 1);               // Or setting the cursor in the desired position.
      lcd.print("B: OVER VOLTAGE");
}*/
#endif
#if DWIN_ENABLED
      fault_overVolt[4] = 0X71;
      fault_overVolt[5] = 0X00;
      err = DWIN_SET(fault_overVolt, sizeof(fault_overVolt) / sizeof(fault_overVolt[0]));
// fault_overVolt[4] = 0X51;
// fault_overVolt[5] = 0X00;
// err = DWIN_SET(fault_overVolt,sizeof(fault_overVolt)/sizeof(fault_overVolt[0]));
#endif
      if (DEBUG_OUT)
        Serial.println("[EVSE_B] Over Voltage");
      return custom_round(volt);
    }
    else
    {
      return custom_round(volt);
    }
  }
  else
  {
    getChargePointStatusService_B()->setUnderVoltage(false);
    getChargePointStatusService_B()->setOverVoltage(false);
// if(DEBUG_OUT) Serial.println(F("[EVSE_B] Normal Voltage Range."));
#if DWIN_ENABLED
// avail[4] = 0X51;
// err = DWIN_SET(avail,sizeof(avail)/sizeof(avail[0]));
// charging[4] = 0X71;
// err = DWIN_SET(charging,sizeof(charging)/sizeof(charging[0]));
#endif
    return custom_round(volt);
  }
}

double ATM90E36::GetLineVoltageC()
{
  unsigned short voltage = CommEnergyIC(READ, UrmsC, 0xFFFF);
  double volt = (double)voltage / 100;
  unsigned short voltage_B = CommEnergyIC(READ, UrmsB, 0xFFFF);
  double volt_B = (double)voltage_B / 100;
  unsigned short voltage_A = CommEnergyIC(READ, UrmsA, 0xFFFF);
  double volt_A = (double)voltage_A / 100;
  if ((volt < 20) && (volt_B < 20) && (volt_A < 20))
  {
    flag_nopower = true;
  }
  else
  {
    flag_nopower = false;
  }
  uint8_t err = 0;

  // bool EMGCY_status = requestEmgyStatus();

#if EARTH_DISCONNECT
  if (flag_nopower)
  {
    Serial.println("Power fail");
    reasonForStop_A = PowerLoss;
    reasonForStop_B = PowerLoss;
    reasonForStop_C = PowerLoss;
    fault_code_A = Power_Loss;
    fault_code_B = Power_Loss;
    fault_code_C = Power_Loss;
  }
#endif
  // unsigned short voltage = CommEnergyIC(READ, UrmsC, 0xFFFF);
  // double volt = (double)voltage / 100;
  // uint8_t err = 0;
  if (volt < 200)
  {
    // if (volt<170){ // For LB nagar hubs
    if (!flag_ed_A && !flag_nopower)
    {
      getChargePointStatusService_C()->setUnderVoltage(true);
      getChargePointStatusService_C()->setOverVoltage(false);
      if (DEBUG_OUT)
        Serial.println("[EVSE_C] Under Voltage");
      fault_code_C = UnderVoltage;
      reasonForStop_C = Other;
#if DISPLAY_ENABLED
      if (volt > 20)
      {
        connAvail(3, "UNDER VOLTAGE");
        checkForResponse_Disp();
      }
      else
      {
        connAvail(3, "NO POWER");
        checkForResponse_Disp();
      }
#endif
#if LCD_ENABLED_FAULT
      if (!disp_evse_A && !disp_evse_B)
      {
// lcd.clear();
#if 0
      lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      lcd.print("                    "); // Clear the line
      lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      lcd.print("STATUS: FAULTED");
#endif
        lcd.setCursor(0, 2);               // Or setting the cursor in the desired position.
        lcd.print("                    "); // Clear the line
        lcd.setCursor(0, 2);               // Or setting the cursor in the desired position.
        if (volt > 20)
          lcd.print("C: UNDER VOLTAGE");
        else
          lcd.print("C: NO POWER");
      }
/*else
{
  lcd.clear();
   lcd.setCursor(0, 2);               // Or setting the cursor in the desired position.
      if (volt > 20)
        lcd.print("C: UNDER VOLTAGE");
      else
        lcd.print("C: NO POWER");
}*/
#endif
#if DWIN_ENABLED
      if (volt > 20)
      {
        fault_underVolt[4] = 0X7B;
        fault_underVolt[5] = 0X00;
        err = DWIN_SET(fault_underVolt, sizeof(fault_underVolt) / sizeof(fault_underVolt[0]));
      }
      else
      {
        fault_nopower[4] = 0X7B;
        fault_nopower[5] = 0X00;
        err = DWIN_SET(fault_nopower, sizeof(fault_nopower) / sizeof(fault_nopower[0]));
      }
#endif
      return custom_round(volt);
    }
    else
    {
      return custom_round(volt);
    }
  }
  else if (volt > 275)
  {
    if (!flag_ed_A && !flag_nopower)
    {
      getChargePointStatusService_C()->setUnderVoltage(false);
      getChargePointStatusService_C()->setOverVoltage(true);
      if (DEBUG_OUT)
        Serial.println("[EVSE_C] Over Voltage");
      fault_code_C = OverVoltage;
      reasonForStop_C = Other;
#if DISPLAY_ENABLED
      connAvail(3, "OVER VOLTAGE");
      checkForResponse_Disp();
#endif
#if LCD_ENABLED_FAULT
      if (!disp_evse_A && !disp_evse_B)
      {
// lcd.clear();
#if 0
      lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      lcd.print("                    "); // Clear the line
      lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      lcd.print("STATUS: FAULTED");
#endif
        lcd.setCursor(0, 2);               // Or setting the cursor in the desired position.
        lcd.print("                    "); // Clear the line
        lcd.setCursor(0, 2);               // Or setting the cursor in the desired position.
        lcd.print("C: OVER VOLTAGE");
      }
/*else
{
  lcd.clear();
   lcd.setCursor(0, 2);               // Or setting the cursor in the desired position.
      lcd.print("C: OVER VOLTAGE");
}*/
#endif
#if DWIN_ENABLED
      fault_overVolt[4] = 0X7B;
      fault_overVolt[5] = 0X00;
      err = DWIN_SET(fault_overVolt, sizeof(fault_overVolt) / sizeof(fault_overVolt[0]));
// fault_underVolt[4] = 0X51;
// fault_underVolt[5] = 0X00;
// err = DWIN_SET(fault_underVolt,sizeof(fault_underVolt)/sizeof(fault_underVolt[0]));
#endif
      return custom_round(volt);
    }
    else
    {
      return custom_round(volt);
    }
  }
  else
  {
    getChargePointStatusService_C()->setUnderVoltage(false);
    getChargePointStatusService_C()->setOverVoltage(false);
    // if(DEBUG_OUT) Serial.println(F("[EVSE_C] Normal Voltage Range."));
#if DWIN_ENABLED
    // avail[4] = 0X51;
// err = DWIN_SET(avail,sizeof(avail)/sizeof(avail[0]));
// charging[4] = 0X7B;
// err = DWIN_SET(charging,sizeof(charging)/sizeof(charging[0]));
#endif
    return custom_round(volt);
  }
}

// CURRENT
double ATM90E36::GetLineCurrentA()
{
  unsigned short current = CommEnergyIC(READ, IrmsA, 0xFFFF);
  double currentA = (double)current / 1000;
  uint8_t err = 0;

#if 0
  double curr;
  if (currentA > 8.5)
  {
    curr = currentA * 2.21;
  }
  else if ((currentA > 0) && (currentA <= 8.5))
  {
    curr = currentA * 2.202;
  }
#endif

  double curr; //= currentA * 2.202-.2;;//=2.202*currentA;
#if 1
  if (currentA > 8.5)
  {
    curr = currentA * 2.21;
    curr = (curr * 0.98042) - 0.02957;
    // curr = (currentA * 0.98042)-0.02957;
    if (curr < 0)
    {
      curr = 0;
    }
  }

  else if ((currentA > 0) && (currentA <= 8.5))
  {
    curr = currentA * 2.202;
    // curr = (currentA * 0.98042)-0.02957;
    curr = (curr * 0.98042) - 0.02957;
    if (curr < 0)
    {
      curr = 0;
    }
  }
#endif

  // if (curr > 35){
  if (curr > 16)
  {
    getChargePointStatusService_A()->setOverCurrent(true);
    // getChargePointStatusService_A()->setEmergencyRelayClose(true);
    // getChargePointStatusService_A()->stopEvDrawsEnergy();
    Serial.println("[EVSE_A] Over Current");
    fault_code_A = OverCurrent;
    reasonForStop_A = Other;
#if DISPLAY_ENABLED
    connAvail(1, "OVER CURRENT");
    checkForResponse_Disp();
#endif
#if LCD_ENABLED_FAULT
// lcd.clear();
#if 0
    lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
    lcd.print("                    "); // Clear the line
    lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
    lcd.print("STATUS: FAULTED");
#endif
    lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
    lcd.print("                    "); // Clear the line
    lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
    lcd.print("A: OVER CURRENT");
#endif
#if DWIN_ENABLED
    fault_overCurr[4] = 0X66;
    fault_overCurr[5] = 0X00;
    err = DWIN_SET(fault_overCurr, sizeof(fault_overCurr) / sizeof(fault_overCurr[0]));
    fault_overCurr[4] = 0X51;
    fault_overCurr[5] = 0X00;
    err = DWIN_SET(fault_overCurr, sizeof(fault_overCurr) / sizeof(fault_overCurr[0]));
#endif
    return custom_round(curr);
  }
  else
  {
    getChargePointStatusService_A()->setOverCurrent(false);
#if DWIN_ENABLED
    // avail[4] = 0X51;
    // err = DWIN_SET(avail,sizeof(avail)/sizeof(avail[0]));
    // charging[4] = 0X66;
    // err = DWIN_SET(charging,sizeof(charging)/sizeof(charging[0]));
#endif
    return custom_round(curr);
  }
}
double ATM90E36::GetLineCurrentB()
{
  unsigned short current = CommEnergyIC(READ, IrmsB, 0xFFFF);
  double currentB = (double)current / 1000;
  uint8_t err = 0;

#if 0
  double curr;
  
  if (currentB > 8.5)
  {
    curr = currentB * 2.21;
  }
  else if ((currentB > 0) && (currentB <= 8.5))
  {
    curr = currentB * 2.202;
  }
#endif

  double curr; //= currentA * 2.202-.2;;//=2.202*currentA;
#if 1
  if (currentB > 8.5)
  {
    curr = currentB * 2.21;
    curr = (curr * 0.98042) - 0.02957;
    // curr = (currentB * 0.98042)-0.02957;
    if (curr < 0)
    {
      curr = 0;
    }
  }

  else if ((currentB > 0) && (currentB <= 8.5))
  {
    curr = currentB * 2.202;
    // curr = (currentB * 0.98042)-0.02957;
    curr = (curr * 0.98042) - 0.02957;
    if (curr < 0)
    {
      curr = 0;
    }
  }
#endif

  // if (curr > 35){
  if (curr > 16)
  {
    getChargePointStatusService_B()->setOverCurrent(true);
    // getChargePointStatusService_B()->setEmergencyRelayClose(true);
    // getChargePointStatusService_B()->stopEvDrawsEnergy();
    Serial.println("[EVSE_B] Over Current");
#if DISPLAY_ENABLED
    connAvail(2, "OVER CURRENT");
    checkForResponse_Disp();
#endif
    fault_code_B = OverCurrent;
    reasonForStop_B = Other;
#if LCD_ENABLED_FAULT
// lcd.clear();
#if 0
    lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
    lcd.print("                    "); // Clear the line
    lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
    lcd.print("STATUS: FAULTED");
#endif
    lcd.setCursor(0, 1);               // Or setting the cursor in the desired position.
    lcd.print("                    "); // Clear the line
    lcd.setCursor(0, 1);               // Or setting the cursor in the desired position.
    lcd.print("B: OVER CURRENT");
#endif
#if DWIN_ENABLED
    fault_overCurr[4] = 0X71;
    fault_overCurr[5] = 0X00;
    err = DWIN_SET(fault_overCurr, sizeof(fault_overCurr) / sizeof(fault_overCurr[0]));
    fault_overCurr[4] = 0X51;
    fault_overCurr[5] = 0X00;
    err = DWIN_SET(fault_overCurr, sizeof(fault_overCurr) / sizeof(fault_overCurr[0]));
#endif
    return custom_round(curr);
  }
  else
  {
    getChargePointStatusService_B()->setOverCurrent(false);
#if DWIN_ENABLED
// avail[4] = 0X51;
// err = DWIN_SET(avail,sizeof(avail)/sizeof(avail[0]));
// charging[4] = 0X71;
// err = DWIN_SET(charging,sizeof(charging)/sizeof(charging[0]));
#endif
    return custom_round(curr);
  }
}

double ATM90E36::GetLineCurrentC()
{
  unsigned short current = CommEnergyIC(READ, IrmsC, 0xFFFF);
  double currentC = (double)current / 1000;
  uint8_t err = 0;

#if 0
  double curr;
  if (currentC > 8.5)
  {
    curr = currentC * 2.21;
  }
  else if ((currentC > 0) && (currentC <= 8.5))
  {
    curr = currentC * 2.202;
  }
#endif

  double curr; //= currentC * 2.202-.2;;//=2.202*currentC;
#if 1
  if (currentC > 8.5)
  {
    curr = currentC * 2.21;
    curr = (curr * 0.98042) - 0.02957;
    // curr = (currentC * 0.98042)-0.02957;
    if (curr < 0)
    {
      curr = 0;
    }
  }

  else if ((currentC > 0) && (currentC <= 8.5))
  {
    curr = currentC * 2.202;
    // curr = (currentC * 0.98042)-0.02957;
    curr = (curr * 0.98042) - 0.02957;
    if (curr < 0)
    {
      curr = 0;
    }
  }
#endif

  // if (curr > 35){
  if (curr > 16)
  {
    getChargePointStatusService_C()->setOverCurrent(true);
    // getChargePointStatusService_C()->setEmergencyRelayClose(true);
    //  getChargePointStatusService_C()->stopEvDrawsEnergy();
    Serial.println("[EVSE_A] Over Current");
#if DISPLAY_ENABLED
    connAvail(3, "OVER CURRENT");
    checkForResponse_Disp();
#endif
    fault_code_C = OverCurrent;
    reasonForStop_C = Other;
#if LCD_ENABLED_FAULT
// lcd.clear();
#if 0
    lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
    lcd.print("                    "); // Clear the line
    lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
    lcd.print("STATUS: FAULTED");
#endif
    lcd.setCursor(0, 2);               // Or setting the cursor in the desired position.
    lcd.print("                    "); // Clear the line
    lcd.setCursor(0, 2);               // Or setting the cursor in the desired position.
    lcd.print("C: OVER CURRENT");
#endif
#if DWIN_ENABLED
    fault_overCurr[4] = 0X7B;
    fault_overCurr[5] = 0X00;
    err = DWIN_SET(fault_overCurr, sizeof(fault_overCurr) / sizeof(fault_overCurr[0]));
    // fault_overCurr[4] = 0X51;
    // fault_overCurr[5] = 0X00;
    err = DWIN_SET(fault_overCurr, sizeof(fault_overCurr) / sizeof(fault_overCurr[0]));
#endif
    return custom_round(curr);
  }
  else
  {
    getChargePointStatusService_C()->setOverCurrent(false);
#if DWIN_ENABLED
    // avail[4] = 0X51;
// err = DWIN_SET(avail,sizeof(avail)/sizeof(avail[0]));
// charging[4] = 0X7B;
// err = DWIN_SET(charging,sizeof(charging)/sizeof(charging[0]));
#endif
    return custom_round(curr);
  }

  // return (double)current / 1000;
}
double ATM90E36::GetLineCurrentN()
{
  unsigned short current = CommEnergyIC(READ, IrmsN0, 0xFFFF);
  return (double)current / 1000;
}

// ACTIVE POWER
double ATM90E36::GetActivePowerA()
{
  signed short apower = (signed short)CommEnergyIC(READ, PmeanA, 0xFFFF);
  if (apower & 0x8000)
  {
    apower = (apower & 0x7FFF) * -1;
  }
  return (double)apower / 1000;
}
double ATM90E36::GetActivePowerB()
{
  signed short apower = (signed short)CommEnergyIC(READ, PmeanB, 0xFFFF);
  if (apower & 0x8000)
  {
    apower = (apower & 0x7FFF) * -1;
  }
  return (double)apower / 1000;
}
double ATM90E36::GetActivePowerC()
{
  signed short apower = (signed short)CommEnergyIC(READ, PmeanC, 0xFFFF);
  if (apower & 0x8000)
  {
    apower = (apower & 0x7FFF) * -1;
  }
  return (double)apower / 1000;
}
double ATM90E36::GetTotalActivePower()
{
  signed short apower = (signed short)CommEnergyIC(READ, PmeanT, 0xFFFF);
  if (apower & 0x8000)
  {
    apower = (apower & 0x7FFF) * -1;
  }
  return (double)apower / 250;
}

// REACTIVE POWER
double ATM90E36::GetReactivePowerA()
{
  signed short apower = (signed short)CommEnergyIC(READ, QmeanA, 0xFFFF);
  if (apower & 0x8000)
  {
    apower = (apower & 0x7FFF) * -1;
  }
  return (double)apower / 1000;
}
double ATM90E36::GetReactivePowerB()
{
  signed short apower = (signed short)CommEnergyIC(READ, QmeanB, 0xFFFF);
  if (apower & 0x8000)
  {
    apower = (apower & 0x7FFF) * -1;
  }
  return (double)apower / 1000;
}
double ATM90E36::GetReactivePowerC()
{
  signed short apower = (signed short)CommEnergyIC(READ, QmeanC, 0xFFFF);
  if (apower & 0x8000)
  {
    apower = (apower & 0x7FFF) * -1;
  }
  return (double)apower / 1000;
}
double ATM90E36::GetTotalReactivePower()
{
  signed short apower = (signed short)CommEnergyIC(READ, QmeanT, 0xFFFF);
  if (apower & 0x8000)
  {
    apower = (apower & 0x7FFF) * -1;
  }
  return (double)apower / 250;
}

// APPARENT POWER
double ATM90E36::GetApparentPowerA()
{
  signed short apower = (signed short)CommEnergyIC(READ, SmeanA, 0xFFFF);
  if (apower & 0x8000)
  {
    apower = (apower & 0x7FFF) * -1;
  }
  return (double)apower / 1000;
}
double ATM90E36::GetApparentPowerB()
{
  signed short apower = (signed short)CommEnergyIC(READ, SmeanB, 0xFFFF);
  if (apower & 0x8000)
  {
    apower = (apower & 0x7FFF) * -1;
  }
  return (double)apower / 1000;
}
double ATM90E36::GetApparentPowerC()
{
  signed short apower = (signed short)CommEnergyIC(READ, SmeanC, 0xFFFF);
  if (apower & 0x8000)
  {
    apower = (apower & 0x7FFF) * -1;
  }
  return (double)apower / 1000;
}
double ATM90E36::GetTotalApparentPower()
{
  signed short apower = (signed short)CommEnergyIC(READ, SmeanT, 0xFFFF);
  if (apower & 0x8000)
  {
    apower = (apower & 0x7FFF) * -1;
  }
  return (double)apower / 250;
}

// FREQUENCY
double ATM90E36::GetFrequency()
{
  unsigned short freq = CommEnergyIC(READ, Freq, 0xFFFF);
  return (double)freq / 100;
}

// POWER FACTOR
double ATM90E36::GetPowerFactorA()
{
  short pf = (short)CommEnergyIC(READ, PFmeanA, 0xFFFF);
  // if negative
  if (pf & 0x8000)
  {
    pf = (pf & 0x7FFF) * -1;
  }
  return (double)pf / 1000;
}
double ATM90E36::GetPowerFactorB()
{
  short pf = (short)CommEnergyIC(READ, PFmeanB, 0xFFFF);
  if (pf & 0x8000)
  {
    pf = (pf & 0x7FFF) * -1;
  }
  return (double)pf / 1000;
}
double ATM90E36::GetPowerFactorC()
{
  short pf = (short)CommEnergyIC(READ, PFmeanC, 0xFFFF);
  // if negative
  if (pf & 0x8000)
  {
    pf = (pf & 0x7FFF) * -1;
  }
  return (double)pf / 1000;
}
double ATM90E36::GetTotalPowerFactor()
{
  short pf = (short)CommEnergyIC(READ, PFmeanT, 0xFFFF);
  // if negative
  if (pf & 0x8000)
  {
    pf = (pf & 0x7FFF) * -1;
  }
  return (double)pf / 1000;
}

// PHASE ANGLE
double ATM90E36::GetPhaseA()
{
  signed short apower = (signed short)CommEnergyIC(READ, PAngleA, 0xFFFF);
  return (double)apower / 10;
}
double ATM90E36::GetPhaseB()
{
  signed short apower = (signed short)CommEnergyIC(READ, PAngleB, 0xFFFF);
  return (double)apower / 10;
}
double ATM90E36::GetPhaseC()
{
  signed short apower = (signed short)CommEnergyIC(READ, PAngleC, 0xFFFF);
  return (double)apower / 10;
}

// TEMPERATURE
double ATM90E36::GetTemperature()
{
  short int temp = (short int)CommEnergyIC(READ, Temp, 0xFFFF);
  // return (double)temp;
  uint8_t err = 0;
  if (temp < -25)
  {
    getChargePointStatusService_A()->setUnderTemperature(true);
    getChargePointStatusService_A()->setOverTemperature(false);
    if (DEBUG_OUT)
      Serial.println("Under Temperature" + String(temp));
    reasonForStop_A = Other;
    reasonForStop_B = Other;
    reasonForStop_C = Other;
#if DISPLAY_ENABLED
    connAvail(1, "UNDER TEMPERATURE");
    checkForResponse_Disp();
#endif
#if LCD_ENABLED_FAULT
    // lcd.clear();
    lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
    lcd.print("                    "); // Clear the line
    lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
    lcd.print("STATUS: FAULTED");
    lcd.setCursor(0, 1);               // Or setting the cursor in the desired position.
    lcd.print("                    "); // Clear the line
    lcd.setCursor(0, 1);               // Or setting the cursor in the desired position.
    lcd.print("A: UNDER TEMP");
#endif
#if DWIN_ENABLED
    fault_overTemp[4] = 0X66;
    fault_overTemp[5] = 0X00;
    err = DWIN_SET(fault_overTemp, sizeof(fault_overTemp) / sizeof(fault_overTemp[0]));
    // fault_overTemp[4] = 0X51;
    // fault_overTemp[5] = 0X00;
    err = DWIN_SET(fault_overTemp, sizeof(fault_overTemp) / sizeof(fault_overTemp[0]));
#endif
    return temp;
  }
  else if (temp > 80)
  {
    getChargePointStatusService_A()->setUnderTemperature(false);
    getChargePointStatusService_A()->setOverTemperature(true);
    if (DEBUG_OUT)
      Serial.println("Over Temperature" + String(temp));
    reasonForStop_A = Other;
    reasonForStop_B = Other;
    reasonForStop_C = Other;
    fault_code_A = 4;
    fault_code_B = 4;
    fault_code_C = 4;
#if DISPLAY_ENABLED
    connAvail(1, "OVER TEMPERATURE");
    checkForResponse_Disp();
#endif
#if LCD_ENABLED_FAULT
    // lcd.clear();
    lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
    lcd.print("                    "); // Clear the line
    lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
    lcd.print("STATUS: FAULTED");
    lcd.setCursor(0, 1);               // Or setting the cursor in the desired position.
    lcd.print("                    "); // Clear the line
    lcd.setCursor(0, 1);               // Or setting the cursor in the desired position.
    lcd.print("A: OVER TEMP");
#endif
#if DWIN_ENABLED
    fault_overTemp[4] = 0X66;
    fault_overTemp[4] = 0X50;
    err = DWIN_SET(fault_overTemp, sizeof(fault_overTemp) / sizeof(fault_overTemp[0]));
    // fault_overTemp[4] = 0X51;
    // fault_overTemp[4] = 0X00;
    err = DWIN_SET(fault_overTemp, sizeof(fault_overTemp) / sizeof(fault_overTemp[0]));
#endif
    return temp;
  }
  else
  {
    getChargePointStatusService_A()->setUnderTemperature(false);
    getChargePointStatusService_A()->setOverTemperature(false);
    if (DEBUG_OUT)
      Serial.println("Normal Temperature: " + String(temp));
#if DWIN_ENABLED
// avail[4] = 0X51;
// err = DWIN_SET(avail,sizeof(avail)/sizeof(avail[0]));
// charging[4] = 0X66;
// err = DWIN_SET(charging,sizeof(charging)/sizeof(charging[0]));
#endif
    return temp;
  }
}

/* Gets the Register Value if Desired */
// REGISTER
unsigned short ATM90E36::GetValueRegister(unsigned short registerRead)
{
  return (CommEnergyIC(READ, registerRead, 0xFFFF)); // returns value register
}

// ENERGY MEASUREMENT
double ATM90E36::GetImportEnergy()
{
  unsigned short ienergyT = CommEnergyIC(READ, APenergyT, 0xFFFF);
  // unsigned short ienergyA = CommEnergyIC(READ, APenergyA, 0xFFFF);
  // unsigned short ienergyB = CommEnergyIC(READ, APenergyB, 0xFFFF);
  // unsigned short ienergyC = CommEnergyIC(READ, APenergyC, 0xFFFF);

  // unsigned short renergyT = CommEnergyIC(READ, RPenergyT, 0xFFFF);
  // unsigned short renergyA = CommEnergyIC(READ, RPenergyA, 0xFFFF);
  // unsigned short renergyB = CommEnergyIC(READ, RPenergyB, 0xFFFF);
  // unsigned short renergyC = CommEnergyIC(READ, RPenergyC, 0xFFFF);

  // unsigned short senergyT = CommEnergyIC(READ, SAenergyT, 0xFFFF);
  // unsigned short senergyA = CommEnergyIC(READ, SenergyA, 0xFFFF);
  // unsigned short senergyB = CommEnergyIC(READ, SenergyB, 0xFFFF);
  // unsigned short senergyC = CommEnergyIC(READ, SenergyC, 0xFFFF);

  return (double)ienergyT / 100 / 3200; // returns kWh
}

double ATM90E36::GetExportEnergy()
{

  unsigned short eenergyT = CommEnergyIC(READ, ANenergyT, 0xFFFF);
  // unsigned short eenergyA = CommEnergyIC(READ, ANenergyA, 0xFFFF);
  // unsigned short eenergyB = CommEnergyIC(READ, ANenergyB, 0xFFFF);
  // unsigned short eenergyC = CommEnergyIC(READ, ANenergyC, 0xFFFF);

  // unsigned short reenergyT = CommEnergyIC(READ, RNenergyT, 0xFFFF);
  // unsigned short reenergyA = CommEnergyIC(READ, RNenergyA, 0xFFFF);
  // unsigned short reenergyB = CommEnergyIC(READ, RNenergyB, 0xFFFF);
  // unsigned short reenergyC = CommEnergyIC(READ, RNenergyC, 0xFFFF);

  return (double)eenergyT / 100 / 3200; // returns kWh
}

/* System Status Registers */
unsigned short ATM90E36::GetSysStatus0()
{
  return CommEnergyIC(READ, SysStatus0, 0xFFFF);
}
unsigned short ATM90E36::GetSysStatus1()
{
  return CommEnergyIC(READ, SysStatus1, 0xFFFF);
}
unsigned short ATM90E36::GetMeterStatus0()
{
  return CommEnergyIC(READ, EnStatus0, 0xFFFF);
}
unsigned short ATM90E36::GetMeterStatus1()
{
  return CommEnergyIC(READ, EnStatus1, 0xFFFF);
}

/* Checksum Error Function */
bool ATM90E36::calibrationError()
{
  bool CS0, CS1, CS2, CS3;
  unsigned short systemstatus0 = GetSysStatus0();

  if (systemstatus0 & 0x4000)
  {
    CS0 = true;
  }
  else
  {
    CS0 = false;
  }

  if (systemstatus0 & 0x0100)
  {
    CS1 = true;
  }
  else
  {
    CS1 = false;
  }
  if (systemstatus0 & 0x0400)
  {
    CS2 = true;
  }
  else
  {
    CS2 = false;
  }
  if (systemstatus0 & 0x0100)
  {
    CS3 = true;
  }
  else
  {
    CS3 = false;
  }

#ifdef DEBUG_SERIAL
  Serial.print("Checksum 0: ");
  Serial.println(CS0);
  Serial.print("Checksum 1: ");
  Serial.println(CS1);
  Serial.print("Checksum 2: ");
  Serial.println(CS2);
  Serial.print("Checksum 3: ");
  Serial.println(CS3);
#endif

  if (CS0 || CS1 || CS2 || CS3)
    return (true);
  else
    return (false);
}

/* BEGIN FUNCTION */
/*
- Define the pin to be used as Chip Select
- Set serialFlag to true for serial debugging
- Use SPI MODE 0 for the ATM90E36
*/
void ATM90E36::begin()
{
  // pinMode(energy_IRQ, INPUT); // (In development...)
  pinMode(_energy_CS, OUTPUT);
  // pinMode(energy_WO, INPUT);  // (In development...)

  /* Enable SPI */
  hspi->begin();
#if defined(ENERGIA)
  hspi->setBitOrder(MSBFIRST);
  hspi->setDataMode(SPI_MODE0);
  hspi->setClockDivider(SPI_CLOCK_DIV16);
#endif

  CommEnergyIC(WRITE, SoftReset, 0x789A); // Perform soft reset
  CommEnergyIC(WRITE, FuncEn0, 0x0000);   // Voltage sag
  CommEnergyIC(WRITE, FuncEn1, 0x0000);   // Voltage sag
  CommEnergyIC(WRITE, SagTh, 0x0001);     // Voltage sag threshold

  /* SagTh = Vth * 100 * sqrt(2) / (2 * Ugain / 32768) */

  // Set metering config values (CONFIG)
  CommEnergyIC(WRITE, ConfigStart, 0x5678); // Metering calibration startup
  CommEnergyIC(WRITE, PLconstH, 0x0861);    // PL Constant MSB (default)
  CommEnergyIC(WRITE, PLconstL, 0xC468);    // PL Constant LSB (default)
  CommEnergyIC(WRITE, MMode0, 0x1087);      // Mode Config (60 Hz, 3P4W)
  CommEnergyIC(WRITE, MMode1, 0x1500);      // 0x5555 (x2) // 0x0000 (1x)
  CommEnergyIC(WRITE, PStartTh, 0x0000);    // Active Startup Power Threshold
  CommEnergyIC(WRITE, QStartTh, 0x0000);    // Reactive Startup Power Threshold
  CommEnergyIC(WRITE, SStartTh, 0x0000);    // Apparent Startup Power Threshold
  CommEnergyIC(WRITE, PPhaseTh, 0x0000);    // Active Phase Threshold
  CommEnergyIC(WRITE, QPhaseTh, 0x0000);    // Reactive Phase Threshold
  CommEnergyIC(WRITE, SPhaseTh, 0x0000);    // Apparent  Phase Threshold
  CommEnergyIC(WRITE, CSZero, 0x4741);      // Checksum 0

  // Set metering calibration values (CALIBRATION)
  CommEnergyIC(WRITE, CalStart, 0x5678); // Metering calibration startup
  CommEnergyIC(WRITE, GainA, 0x0000);    // Line calibration gain
  CommEnergyIC(WRITE, PhiA, 0x0000);     // Line calibration angle
  CommEnergyIC(WRITE, GainB, 0x0000);    // Line calibration gain
  CommEnergyIC(WRITE, PhiB, 0x0000);     // Line calibration angle
  CommEnergyIC(WRITE, GainC, 0x0000);    // Line calibration gain
  CommEnergyIC(WRITE, PhiC, 0x0000);     // Line calibration angle
  CommEnergyIC(WRITE, PoffsetA, 0x0000); // A line active power offset
  CommEnergyIC(WRITE, QoffsetA, 0x0000); // A line reactive power offset
  CommEnergyIC(WRITE, PoffsetB, 0x0000); // B line active power offset
  CommEnergyIC(WRITE, QoffsetB, 0x0000); // B line reactive power offset
  CommEnergyIC(WRITE, PoffsetC, 0x0000); // C line active power offset
  CommEnergyIC(WRITE, QoffsetC, 0x0000); // C line reactive power offset
  CommEnergyIC(WRITE, CSOne, 0x0000);    // Checksum 1

  // Set metering calibration values (HARMONIC)
  CommEnergyIC(WRITE, HarmStart, 0x5678); // Metering calibration startup
  CommEnergyIC(WRITE, POffsetAF, 0x0000); // A Fund. active power offset
  CommEnergyIC(WRITE, POffsetBF, 0x0000); // B Fund. active power offset
  CommEnergyIC(WRITE, POffsetCF, 0x0000); // C Fund. active power offset
  CommEnergyIC(WRITE, PGainAF, 0x0000);   // A Fund. active power gain
  CommEnergyIC(WRITE, PGainBF, 0x0000);   // B Fund. active power gain
  CommEnergyIC(WRITE, PGainCF, 0x0000);   // C Fund. active power gain
  CommEnergyIC(WRITE, CSTwo, 0x0000);     // Checksum 2

  // Set measurement calibration values (ADJUST)
  CommEnergyIC(WRITE, AdjStart, 0x5678); // Measurement calibration
  CommEnergyIC(WRITE, UgainA, 0x0002);   // A SVoltage rms gain
  CommEnergyIC(WRITE, IgainA, 0xFD7F);   // A line current gain
  CommEnergyIC(WRITE, UoffsetA, 0x0000); // A Voltage offset
  CommEnergyIC(WRITE, IoffsetA, 0x0000); // A line current offset
  CommEnergyIC(WRITE, UgainB, 0x0002);   // B Voltage rms gain
  CommEnergyIC(WRITE, IgainB, 0xFD7F);   // B line current gain
  CommEnergyIC(WRITE, UoffsetB, 0x0000); // B Voltage offset
  CommEnergyIC(WRITE, IoffsetB, 0x0000); // B line current offset
  CommEnergyIC(WRITE, UgainC, 0x0002);   // C Voltage rms gain
  CommEnergyIC(WRITE, IgainC, 0xFD7F);   // C line current gain
  CommEnergyIC(WRITE, UoffsetC, 0x0000); // C Voltage offset
  CommEnergyIC(WRITE, IoffsetC, 0x0000); // C line current offset
  CommEnergyIC(WRITE, IgainN, 0xFD7F);   // C line current gain
  CommEnergyIC(WRITE, CSThree, 0x02F6);  // Checksum 3

  // Done with the configuration
  CommEnergyIC(WRITE, ConfigStart, 0x5678);
  CommEnergyIC(WRITE, CalStart, 0x5678);  // 0x6886 //0x5678 //8765);
  CommEnergyIC(WRITE, HarmStart, 0x5678); // 0x6886 //0x5678 //8765);
  CommEnergyIC(WRITE, AdjStart, 0x5678);  // 0x6886 //0x5678 //8765);

  CommEnergyIC(WRITE, SoftReset, 0x789A); // Perform soft reset
}

//****************************Explicit function by @wamique******************************//
double custom_round(double var)
{
  // 37.66666 * 100 =3766.66
  // 3766.66 + .5 =3767.16    for rounding off value
  // then type cast to int so value is 3767
  // then divided by 100 so the value converted into 37.67
  double value = (int)(var * 100 + .5);
  return (double)value / 100;
}
void lowcurrent_loop_A_B_C(void)
{
      if(!offline_charging_A)
      {
      if (getChargePointStatusService_A()->getEvDrawsEnergy() == true)
      {
        // Current check
				drawing_current_A = eic.GetLineCurrentA();
				Serial.println("Current A: " + String(drawing_current_A));
				if (drawing_current_A <= minCurr)
				{
					Serial.println("Min current_A");
					if (notFaulty_A)
					{
						counter_drawingCurrent_A++;
						 Serial.println("counter_drawingCurrent_A online: " + (String)counter_drawingCurrent_A);
					}
					// if(counter_drawingCurrent_A > 120){
					if (counter_drawingCurrent_A > currentCounterThreshold_A)
					{
						counter_drawingCurrent_A = 0;
						if (reasonForStop_A != 3 || reasonForStop_A != 4)
							reasonForStop_A = 1; // EV disconnected
						Serial.println("Stopping session due to No current");
						EVSE_A_StopSession();
					}
				}
				else
				{
					counter_drawingCurrent_A = 0;
					if (notFaulty_A)
					{
						// currentCounterThreshold_A = 2;
						// reasonForStop_A = 1;
					}
					// currentCounterThreshold_A = 60; //ARAI expects 2
					Serial.println("counter_drawing Current Reset");
				}
      }
      }
      if(!offline_charging_B)
      {
      if (getChargePointStatusService_B()->getEvDrawsEnergy() == true)
      {
        drawing_current_B = eic.GetLineCurrentB();
				Serial.println("Current B: " + String(drawing_current_B));
				if (drawing_current_B <= minCurr)
				{
					Serial.println("Min current_B");

					if (notFaulty_B)
					{
						counter_drawingCurrent_B++;
						Serial.println("counter_drawingCurrent_B Online: " + (String)counter_drawingCurrent_B);
					}
					// if(counter_drawingCurrent_B > 120){
					if (counter_drawingCurrent_B > currentCounterThreshold_B)
					{
						counter_drawingCurrent_B = 0;
						// Check for the case where reasonForStop is not Local , Other
						if (reasonForStop_B != 3 || reasonForStop_B != 4)
							reasonForStop_B = 1; // EV disconnected
						Serial.println("Stopping session due to No current");
						EVSE_B_StopSession();
					}
				}
				else
				{
					counter_drawingCurrent_B = 0;
					if (notFaulty_B)
					{
						// currentCounterThreshold_B = 2;
					}
					Serial.println("counter_drawing Current Reset");
				}
      }
      }
      if(!offline_charging_C)
      {
      if (getChargePointStatusService_C()->getEvDrawsEnergy() == true)
      {
        drawing_current_C = eic.GetLineCurrentC();
				Serial.println("Current C: " + String(drawing_current_C));
				if (drawing_current_C <= minCurr)
				{
				Serial.println("Min current_C");

					if (notFaulty_C)
					{
						counter_drawingCurrent_C++;
						Serial.println("counter_drawingCurrent_C online: " + (String)counter_drawingCurrent_C);
					}
					// if(counter_drawingCurrent_C > 120){
					if (counter_drawingCurrent_C > currentCounterThreshold_C)
					{
						// Check for the case where reasonForStop is not Local , Other
						if (reasonForStop_C != 3 || reasonForStop_C != 4)
						reasonForStop_C = 1; // EV disconnected
						Serial.println("Stopping session due to No current");
						counter_drawingCurrent_C = 0;
						EVSE_C_StopSession();
					}
				}
				else
				{
					counter_drawingCurrent_C = 0;
					if (notFaulty_C)
					{
						// currentCounterThreshold_C = 2; // 2 ideally.
					}
					Serial.println("counter_drawingCurrent Reset");
				}
      }
      }
}