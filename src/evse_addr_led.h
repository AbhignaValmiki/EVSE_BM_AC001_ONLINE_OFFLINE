#ifndef EVSE_ADD_LED_H
#define EVSE_ADD_LED_H_

#include <Arduino.h>
#include <FastLED.h>

#define DATA_PIN          (19)

#define NUM_LEDS          (10) 


#define MAX_COUNT         (10)
#define ARR_MAX_COUNT      (9)
#define BLINK_COUNT       (50)

#define WHITE_BRIGHTNESS   (50)
#define COLOR_BRIGHTNESS   (200)

// declaring enum for configuration of led colours.
enum led_col_config
{
    Red,
    Green,
    Blue,
    Black,
    Yellow,
    Purple,
    White,
    Blinky_Red,
    Blinky_Green,
    Blinky_Blue,
    Blinky_Black,
    Blinky_Yellow,
    Blinky_Purple,
    Blinky_White
};


class ADDR_LEDs
{
public:
void red_led(void);
void Greeen_led(void);
void blue_led(void);
void Yellow_led(void);
void Purple_led(void);
void White_led(void);
void Black_led(void);
void blink_red_led(void);
void blink_green_led(void);
void blink_blue_led(void);
void blink_yellow_led(void);
void blink_Purple_led(void);
void blink_black_led(void);
void blink_white_led(void);


void ADDR_LEDs_main(int led_status);
void ADDR_LEDs_Init(void);
void led_on_off(int led_status);
};

extern ADDR_LEDs  led_rgb_obj;

extern enum led_col_config led_status; // enum variable.

#endif



