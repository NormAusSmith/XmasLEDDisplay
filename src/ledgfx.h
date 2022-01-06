

#ifndef LEDGFX_H
#define LEDGFX_H

#include <Arduino.h>
#define FASTLED_INTERNAL
#include <fastLED.h>


// FractionalColour
//
// Returns a fraction of a color; 
// Example: if we need to display 0.25, we fade by 0.75

CRGB ColourFraction(CRGB colourIn, float fraction)
{
    fraction = min(1.0f, fraction);
    CRGB fadedColour = (colourIn).fadeToBlackBy(255 * (1.0f - fraction));

    return (fadedColour);
}



void DrawPixels(float fPos, float count, CRGB colour)
{
    //  Example: fPos = 6.75   count = 3.75
    //
    //  Draw 0.25 of first pixel(6), 3 whole pixels, 0.5 of last pixel(10)
    //
    //  availFirstPixel = 1.0 - ( 6.75 - 6 )        = 0.25
    //  amtFirstPixel = min ( 0.25, 3.75)           = 0.25      //  in case count < availFirstPixel
    //  remaining = min ( 3.75, NUM_LEDS - 1.75)    = 3.75      //  in case we're close to the end
    //  iPos = 1

    // Calculate how much the first pixel will hold

    float availFirstPixel = 1.0f - (fPos - (long)(fPos));
    float amtFirstPixel = min(availFirstPixel, count);     
    float remaining = min(count, FastLED.size() - fPos);   
    int iPos = fPos;

    //  Blend (add) in the color of the first partial pixel
    //  This accomodates 2 overlapping drawings

    if (remaining > 0.0f)
    {
        FastLED.leds()[iPos] += ColourFraction(colour, amtFirstPixel);
        
        iPos++;
        remaining -= amtFirstPixel;
    }

    // Now draw any full pixels in the middle

    while (remaining > 1.0f)
    {
        
        FastLED.leds()[iPos] += colour;
        iPos++;
        remaining--;
    }

    // Draw tail pixel, up to a single full pixel

    if (remaining > 0.0f)
    {
        FastLED.leds()[iPos] += ColourFraction(colour, remaining);
    }
}

#endif
