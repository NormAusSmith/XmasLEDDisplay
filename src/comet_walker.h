
#include <Arduino.h>
#define FASTLED_INTERNAL
#include <fastLED.h>

#include "ledgfx.h"

/*
Draw sets of smooth scrolling coloured comets.
Each successive comet progresses a little bit slower.    
*/

extern CRGB g_LEDs[];

static const CRGB CometColours[] =
    {
        CRGB::Orange,
        CRGB::Blue,
        CRGB::Purple,
        CRGB::Red,
        CRGB::Green,
        CRGB::Orange,
        CRGB::Blue,
        CRGB::Purple,
        CRGB::Red,
        CRGB::Green};

static const int nArray = ARRAYSIZE(CometColours);

void DrawCometWalkers()
{
    //  allow for logic to be called once, the first time this pattern is called.

    if (g_First_Time)
    {
        //  put one time logic here
        g_First_Time = false;
    }

    EVERY_N_MILLISECONDS(10)
    {
        const float cometSize = 2.5f;
        const int fadeAmt = 64;
        float fPos = 1.0f;

        for (int i = 0; i < NUM_LEDS; i++)
            g_LEDs[i] = g_LEDs[i].fadeToBlackBy(fadeAmt);

        //  beatsin16   produces a sine wave changing 16 bit number
        //              In this case 1 per min for first comet, 2 for next and so on.
        //  Each comet start 1.5 secs offset so they don't all merge periodically

        int offset = 1500;

        for (int i = 0; i < nArray; i++)
        {
            int iPos = beatsin16(i + 1, 1, 100, i * offset);

            fPos = ((float)iPos * (NUM_LEDS - cometSize) / 100.0f);

            DrawPixels(fPos, cometSize, CometColours[i]);
        }
    }
}

