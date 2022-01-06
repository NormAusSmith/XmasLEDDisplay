
#include <Arduino.h>
#define FASTLED_INTERNAL
#include <fastLED.h>

#include "ledgfx.h"

/*
Draws 2 climbing flames up either side of the window.
LEDs are run up one side, across the top, then down the other side
so logic for the second flame is reversed.

In my case I have a 263 led string, 100 up one side, 63 across
the top and the final 100 down the other side, so g_LEDs[0] is
the bottom of one side and g_LEDs[262] is the bottom of the other.

This function takes 4 parameters.

1) CoolingFactor: 		How fast a flame cools down. More cooling means shorter flames.
2) SparkingPossibility: The chance (out of 255) that a spark will ignite in the sparking area
3) SparkingArea: 		Where in the bottom area sparks will propogate.
4) SpeedDelay: 			Slow down the fire activity … a higher value makes the flame appear slower.
*/

//  defs

int g_LEDs_on_Side = 99;

//  forward declarations

void FireWindow(int CoolingFactor, int SparkingPossibility, int SparkingArea, int SpeedDelay);

void DrawFireWindow()
{
    //  allow for logic to be called once, the first time this pattern is called.

    if (g_First_Time)
    {
        //  put one time logic here
        g_First_Time = false;
    }

    FireWindow(150, 120, 7, 3);

    //  Parm 1 - CoolingFactor          150 / 255   - larger number cools down quicker for a shorter flame			
    //  Parm 2 - SparkingPossibility    120 / 255   - how likely is a new spark.
    //  Parm 3 - SparkingArea           7           - dependent on string length, between 4 and 7 should work
    //  Parm 3 - SpeedDelay             3           - how fast the flame climbs. smaller value = faster
}

void FireWindow(int CoolingFactor, int SparkingPossibility, int SparkingArea, int SpeedDelay)
{
    static byte heat[NUM_LEDS];
    int cooldown;

    // Step 1.  Cool down every cell a little

    for (int i = 0; i < NUM_LEDS; i++)
    {
        cooldown = random(0, ((CoolingFactor * 10) / NUM_LEDS) + 2);

        if (cooldown > heat[i])
        {
            heat[i] = 0;
        }
        else
        {
            heat[i] = heat[i] - cooldown;
        }
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little

    //  Working down from the middle toward the base of each side,
    //  the heat of each pixel is an average of the pixels below.
    //  This has the effect of making the flames grow upwards as they cool and diminish.

    for (int k = g_LEDs_on_Side; k >= 2; k--)
    {
        int heat_of_neighbour_1_below = heat[k - 1];
        int heat_of_neighbour_2_below = heat[k - 2];

        heat[k] = (heat_of_neighbour_1_below + (2 * heat_of_neighbour_2_below)) / 3;
    }

    for (int k = (NUM_LEDS - g_LEDs_on_Side); k <= (NUM_LEDS - 3); k++)
    {
        //  same thing for the far side

        int heat_of_neighbour_1_above = heat[k + 1];
        int heat_of_neighbour_2_above = heat[k + 2];

        heat[k] = (heat_of_neighbour_1_above + (2 * heat_of_neighbour_2_above)) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' in the left and right SparkingArea
  
    if (random(255) < SparkingPossibility)
    {
        int y = random(SparkingArea);
        heat[y] = heat[y] + random(160, 255);
    }

    if (random(255) < SparkingPossibility)
    {
        int y = random(SparkingArea);
        y = NUM_LEDS - y;
        heat[y] = heat[y] + random(160, 255);
    }

    // Step 4.  Convert heat to LED colours

    for (int j = 0; j < NUM_LEDS; j++)
    {
        g_LEDs[j] = HeatColor(heat[j]);
    }

    FastLED.show();
    FastLED.delay(SpeedDelay);
}
