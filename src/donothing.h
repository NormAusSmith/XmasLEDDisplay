
#include <Arduino.h>
#define FASTLED_INTERNAL
#include <fastLED.h>

/*
Do nothing

Illuminate 1 pixel so we know this function is running (pilot light)
*/

extern CRGB g_LEDs[];


void DrawNothing()
{
    //  allow for logic to be called once, the first time this pattern is called.

    if (g_First_Time)
    {
        //  put one time logic here

		FastLED.clear();
        g_First_Time = false;
    }


    EVERY_N_MILLISECONDS(1000)
    {
        //  place holder
    }
		
	g_LEDs[1] = CRGB::Blue;
}
