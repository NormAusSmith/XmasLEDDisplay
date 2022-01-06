#include <Arduino.h>
#define FASTLED_INTERNAL
#include <fastLED.h>
#include "ledgfx.h"

/*
Transition pattern.  Rethinking this as it doesn't look that good !
*/

extern CRGB g_LEDs[];
extern int g_Brightness;

void ChristmasTransition()
{
#define Xmas_Green 0x00580C
#define Xmas_Red 0xB00402

#define RAMP_UP_MSECS 2000
#define WIPE_MSECS 1000

    //  .25 second delay at start and end

    //  1) initially turn off all leds
    //  2) set brightness very low and randomly populate with red and green
    //  3) increase brightness to g_Brightness over n seconds
    //  4) wipe all cells from middle to bottom over n seconds

    delay(250);

    //  2) randomly populate with red/green/black

    for (int i = 0; i < NUM_LEDS; i++)
    {
        CRGB rand_colour[] = {Xmas_Green, Xmas_Red, CRGB::Black, Xmas_Green, Xmas_Red};
        g_LEDs[i].maximizeBrightness(5); //  if you set the brighness below 2, it erases the value
        g_LEDs[i] = rand_colour[random(256) % 5];
    }

    delay(250);

    //  3) Ramp up brightness over 2 seconds in 45 steps using a sine wave curve

    for (int i = 0; i < 45; i++)
    {
        double deg_to_rad = 0.017453;
        double r = i * deg_to_rad;
        double s = sin(r);
        double b = s * g_Brightness;

        if (b < 2)
            b = 2;

        for (int j = 0; j < NUM_LEDS; j++)
            g_LEDs[j].maximizeBrightness(b);

        FastLED.show();
        FastLED.delay(RAMP_UP_MSECS / 45);
    }

    //  4) curtain wipe from centre outwards over 2 seconds

    int middle = (NUM_LEDS / 2);

    for (int i = 1; i < middle; i++)
    {
        int l = middle - i;
        int r = middle + i;

        for (int j = l; j < r; j++)
            g_LEDs[j].fadeToBlackBy(64);

        FastLED.show();
        FastLED.delay(WIPE_MSECS / middle);
    }

    FastLED.clear(true);
    delay(250);
}
