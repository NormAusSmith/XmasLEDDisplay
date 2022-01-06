

#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>
#include "ledgfx.h"

/*
Simple routine that randomly turns 3 white LEDs on and off very quickly
leading to a sparkling effect.
*/

extern CRGB g_LEDs[];

void DrawSparklers()
{
    //  allow for logic to be called once, the first time this pattern is called.

    if (g_First_Time)
    {
        //  put one time logic here
        g_First_Time = false;
    }

    //  turn 3 pixels on and off

    int x = random(NUM_LEDS);
    int y = random(NUM_LEDS);
    int z = random(NUM_LEDS);

	//	on
	
    g_LEDs[x] = CRGB::White;
    g_LEDs[y] = CRGB::White;
    g_LEDs[z] = CRGB::White;

    FastLED.show();
    FastLED.delay(0);

	//	off
	
    g_LEDs[x] = CRGB::Black;
    g_LEDs[y] = CRGB::Black;
    g_LEDs[z] = CRGB::Black;
}