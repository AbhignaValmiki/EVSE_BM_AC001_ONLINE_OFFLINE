
#include "evse_addr_led.h"
// #include <Arduino.h>
// #include <FastLED.h>

volatile int gu8_led_count = 0;
volatile int gu8_arr_index = 0;

// array declaration for time axis.(lookup table)
// static int arr[10]={1,0,1,0,1,0,1,0,1,0};

ADDR_LEDs led_rgb_obj;
ADDR_LEDs *led_rgb;

CRGB leds[NUM_LEDS];

void ADDR_LEDs::ADDR_LEDs_Init(void)
{
    FastLED.addLeds<WS2813, DATA_PIN, GRB>(leds, NUM_LEDS);
    // FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);
}

void ADDR_LEDs::red_led(void)
{
    ADDR_LEDs_main(Red);
}

void ADDR_LEDs::Greeen_led(void)
{
    ADDR_LEDs_main(Green);
}

void ADDR_LEDs::blue_led(void)
{
    ADDR_LEDs_main(Blue);
}

void ADDR_LEDs::Yellow_led(void)
{
    ADDR_LEDs_main(Yellow);
}

void ADDR_LEDs::Purple_led(void)
{
    ADDR_LEDs_main(Purple);
}

void ADDR_LEDs::White_led(void)
{
    ADDR_LEDs_main(White);
}

void ADDR_LEDs::Black_led(void)
{
    ADDR_LEDs_main(Black);
}

void ADDR_LEDs::blink_red_led(void)
{
    ADDR_LEDs_main(Blinky_Red);
}

void ADDR_LEDs::blink_green_led(void)
{
    ADDR_LEDs_main(Blinky_Green);
}

void ADDR_LEDs::blink_blue_led(void)
{
    ADDR_LEDs_main(Blinky_Blue);
}

void ADDR_LEDs::blink_yellow_led(void)
{
    ADDR_LEDs_main(Blinky_Yellow);
}

void ADDR_LEDs::blink_Purple_led(void)
{
    ADDR_LEDs_main(Blinky_Purple);
}

void ADDR_LEDs::blink_black_led(void)
{
    ADDR_LEDs_main(Blinky_Black);
}

void ADDR_LEDs::blink_white_led(void)
{
    ADDR_LEDs_main(Blinky_White);
}

void ADDR_LEDs::ADDR_LEDs_main(int led_status)
{
    switch (led_status)
    {
    case Green:
        Serial.println("green led");

#if 0
         leds[NUM_LEDS] = CRGB::Green;
         FastLED.show(COLOR_BRIGHTNESS);
#else
        for (uint8_t idx = 0; idx < NUM_LEDS; idx++)
        {
            leds[idx] = CRGB::Green;
            FastLED.show(COLOR_BRIGHTNESS);
        }
#endif

        break;

    case Blue:

#if 0
        leds[NUM_LEDS] = CRGB::Blue;
        FastLED.show(COLOR_BRIGHTNESS);
#else
        for (uint8_t idx = 0; idx < NUM_LEDS; idx++)
        {
            leds[idx] = CRGB::Blue;
            FastLED.show(COLOR_BRIGHTNESS);
        }
#endif
        break;

    case Red:

#if 0
        leds[NUM_LEDS] = CRGB::Red;
        FastLED.show(COLOR_BRIGHTNESS);
#else
        for (uint8_t idx = 0; idx < NUM_LEDS; idx++)
        {
            leds[idx] = CRGB::Red;
            FastLED.show(COLOR_BRIGHTNESS);
        }
#endif
        break;

    case Black:

#if 0
        leds[NUM_LEDS] = CRGB::Black;
        FastLED.show(COLOR_BRIGHTNESS);
#else
        for (uint8_t idx = 0; idx < NUM_LEDS; idx++)
        {
            leds[idx] = CRGB::Black;
            FastLED.show(COLOR_BRIGHTNESS);
        }
#endif
        break;

    case White:
        for (uint8_t idx = 0; idx < NUM_LEDS; idx++)
        {
            leds[idx] = CRGB::White;
            FastLED.show(WHITE_BRIGHTNESS);
        }

        break;

    case Yellow:
        for (uint8_t idx = 0; idx < NUM_LEDS; idx++)
        {
            leds[idx] = CRGB::Yellow;
            FastLED.show(COLOR_BRIGHTNESS);
        }
        break;

#if 1
    case Blinky_Green:
        //   Serial.Print("BLINKY GREEN BLINK");
        led_rgb->led_on_off(Blinky_Green);
        break;

    case Blinky_Blue:
        // Serial.Print("BLINKY BLUE BLINK");
        led_rgb->led_on_off(Blinky_Blue);
        break;

    case Blinky_Red:
        //  Serial.Print("BLINKY RED BLINK");
        led_rgb->led_on_off(Blinky_Red);
        break;

    case Blinky_Yellow:
        //  Serial.Print("BLINKY YELLOW BLINK");
        led_rgb->led_on_off(Blinky_Yellow);
        break;

    case Blinky_Black:
        // Serial.Print("BLINKY BLACK BLINK");
        led_rgb->led_on_off(Blinky_Black);
        break;

    case Blinky_White:
        // Serial.Print("BLINKY WHITE BLINK");

        led_rgb->led_on_off(Blinky_White);
        break;

    case Blinky_Purple:
        // Serial.Print("BLINKY PURPLE BLINK");
        led_rgb->led_on_off(Blinky_Purple);
        break;
#endif
    }
}

/*....................*/
// function to blink leds(200ms_on, 200ms_off)
void ADDR_LEDs::led_on_off(int led_status)
{
    // give led_count till reaches it the max count so that delay becomes 200ms (10*20=200ms).
    gu8_led_count++;
    // ESP_LOGI("LOG", "gu8_led_count %d \r\n ",gu8_led_count);
    if (gu8_led_count >= MAX_COUNT)
    {
        gu8_led_count = 0;
        switch (led_status)
        {
        case Blinky_Green:
            // Serial.println("green led");

            for (uint8_t idx = 0; idx < NUM_LEDS; idx++)
            {
                leds[idx] = CRGB::Green;
                FastLED.show(COLOR_BRIGHTNESS);
            }
            delay(500);
            for (uint8_t idx = 0; idx < NUM_LEDS; idx++)
            {

                leds[idx] = CRGB::Black;
                FastLED.show(COLOR_BRIGHTNESS);
            }
            delay(50);

            break;

        case Blinky_Blue:

            for (uint8_t idx = 0; idx < NUM_LEDS; idx++)
            {
                leds[idx] = CRGB::Blue;
                FastLED.show(COLOR_BRIGHTNESS);
            }
            delay(500);
            for (uint8_t idx = 0; idx < NUM_LEDS; idx++)
            {

                leds[idx] = CRGB::Black;
                FastLED.show(COLOR_BRIGHTNESS);
            }
            delay(50);

            break;

        case Blinky_Red:

            for (uint8_t idx = 0; idx < NUM_LEDS; idx++)
            {
                leds[idx] = CRGB::Red;
                FastLED.show(COLOR_BRIGHTNESS);
                // delay(100);
            }
            delay(500);
            for (uint8_t idx = 0; idx < NUM_LEDS; idx++)
            {

                leds[idx] = CRGB::Black;
                FastLED.show(COLOR_BRIGHTNESS);
                // delay(100);
            }
            delay(50);

            break;

        case Blinky_Yellow:

            break;

        case Blinky_Black:

            break;

        case Blinky_White:

            for (uint8_t idx = 0; idx < NUM_LEDS; idx++)
            {
                leds[idx] = CRGB::White;
                FastLED.show(COLOR_BRIGHTNESS);
            }
            delay(500);
            for (uint8_t idx = 0; idx < NUM_LEDS; idx++)
            {

                leds[idx] = CRGB::Black;
                FastLED.show(COLOR_BRIGHTNESS);
            }
            delay(50);

            break;

            break;
        }
        gu8_arr_index++;
    }

// whenever the array reaches the max index position set the index value to 0 again.
#if 1
    if (gu8_arr_index > ARR_MAX_COUNT)
    {
        gu8_arr_index = 0;
    }
#endif
}
