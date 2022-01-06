
#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>

/*
Draw twinkling christmas lights that ooze on and off randomly
*/

//  forward declarations

void DrawTwinkleFox();
void drawTwinkles();
void chooseNextColorPalette(CRGBPalette16 &pal);
CRGB computeOneTwinkle(uint32_t ms, uint8_t salt);
uint8_t attackDecayWave8(uint8_t i);

//	defs

#define TWINKLE_SPEED 4             //  0 (VERY slow) to 8 (VERY fast). 4 - 6 are recommended, default is 4.
#define TWINKLE_DENSITY 2           //  Twinkle density.  Default is 5
#define SECONDS_PER_PALETTE 60      //  how often to change the palette

// Toward the bottom of the file is an array
// called "ActivePaletteList" which controls which color
// palettes are used; you can add or remove color palettes
// from there freely.

CRGBPalette16 gCurrentPalette;
CRGBPalette16 gTargetPalette;

extern CRGB g_LEDs[];
extern char g_Msg[];

void DrawTwinkleFox()
{
    //  allow for logic to be called once, the first time this pattern is called.

    if (g_First_Time)
    {
        //  put one time logic here
        chooseNextColorPalette(gTargetPalette);
        g_First_Time = false;
    }


    EVERY_N_SECONDS(SECONDS_PER_PALETTE)
    {
        chooseNextColorPalette(gTargetPalette);
    }

    EVERY_N_MILLISECONDS(10)
    {
        nblendPaletteTowardPalette(gCurrentPalette, gTargetPalette, 12);
    }

    drawTwinkles();

    FastLED.show();
}

//  This function loops over each pixel, calculates the
//  adjusted 'clock' that this pixel should use, and calls
//  "CalculateOneTwinkle" on each pixel.  It then displays
//  either the twinkle color of the background color,
//  whichever is brighter.

void drawTwinkles()
{
    // "PRNG16" is the pseudorandom number generator
    // It MUST be reset to the same starting value each time
    // this function is called, so that the sequence of 'random'
    // numbers that it generates is (paradoxically) stable.

    uint16_t PRNG16 = 11337;
    uint32_t clock32 = millis();
    CRGB bg = CRGB::Black;

    uint8_t backgroundBrightness = bg.getAverageLight();

    for (int i = 0; i < NUM_LEDS; i++)
    {
        PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384; // next 'random' number
        uint16_t myclockoffset16 = PRNG16;         // use that number as clock offset
        PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384; // next 'random' number
        // use that number as clock speed adjustment factor (in 8ths, from 8/8ths to 23/8ths)
        uint8_t myspeedmultiplierQ5_3 = ((((PRNG16 & 0xFF) >> 4) + (PRNG16 & 0x0F)) & 0x0F) + 0x08;
        uint32_t myclock30 = (uint32_t)((clock32 * myspeedmultiplierQ5_3) >> 3) + myclockoffset16;
        uint8_t myunique8 = PRNG16 >> 8; // get 'salt' value for this pixel

        // We now have the adjusted 'clock' for this pixel, now we call
        // the function that computes what color the pixel should be based
        // on the "brightness = f( time )" idea.

        CRGB c = computeOneTwinkle(myclock30, myunique8);

        uint8_t cbright = c.getAverageLight();
        int16_t deltabright = cbright - backgroundBrightness;
        
        if (deltabright >= 32 || (!bg))
        {
            // If the new pixel is significantly brighter than the background color,
            // use the new color.

            g_LEDs[i] = c;
        }
        else if (deltabright > 0)
        {
            // If the new pixel is just slightly brighter than the background color,
            // mix a blend of the new color and the background color

            g_LEDs[i] = blend(bg, c, deltabright * 8);
        }
        else
        {
            // if the new pixel is not at all brighter than the background color,
            // just use the background color.

            g_LEDs[i] = bg;
        }
    }
}

//  This function takes a time in pseudo-milliseconds,
//  figures out brightness = f( time ), and also hue = f( time )
//  The 'low digits' of the millisecond time are used as
//  input to the brightness wave function.
//  The 'high digits' are used to select a color, so that the color
//  does not change over the course of the fade-in, fade-out
//  of one cycle of the brightness wave function.
//  The 'high digits' are also used to determine whether this pixel
//  should light at all during this cycle, based on the TWINKLE_DENSITY.

CRGB computeOneTwinkle(uint32_t ms, uint8_t salt)
{
    uint16_t ticks = ms >> (8 - TWINKLE_SPEED);
    uint8_t fastcycle8 = ticks;
    uint16_t slowcycle16 = (ticks >> 8) + salt;
    slowcycle16 += sin8(slowcycle16);
    slowcycle16 = (slowcycle16 * 2053) + 1384;
    uint8_t slowcycle8 = (slowcycle16 & 0xFF) + (slowcycle16 >> 8);

    uint8_t bright = 0;
    if (((slowcycle8 & 0x0E) / 2) < TWINKLE_DENSITY)
    {
        bright = attackDecayWave8(fastcycle8);
    }

    uint8_t hue = slowcycle8 - salt;

    CRGB c;
    if (bright > 0)
    {
        c = ColorFromPalette(gCurrentPalette, hue, bright, NOBLEND);
    }
    else
    {
        c = CRGB::Black;
    }
    return c;
}

/*
This function is like 'triwave8', which produces a
symmetrical up-and-down triangle sawtooth waveform, except that this
function produces a triangle wave with a faster attack and a slower decay:
    / \
   /     \
  /         \
 /             \

*/

uint8_t attackDecayWave8(uint8_t i)
{
    if (i < 86)
    {
        return i * 3;
    }
    else
    {
        i -= 86;
        return 255 - (i + (i / 2));
    }
}


/*
 *   -------------------------------------------.
 *    Colour palettes; each with 16 colours     !
 *   -------------------------------------------'
 */
// A mostly red palette with green accents and white trim.
// "CRGB::Gray" is used as white to keep the brightness more uniform.

const TProgmemRGBPalette16 RedGreenWhite_p FL_PROGMEM =
    {CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red,
     CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red,
     CRGB::Red, CRGB::Red, CRGB::Gray, CRGB::Gray,
     CRGB::Green, CRGB::Green, CRGB::Green, CRGB::Green};

// A mostly (dark) green palette with red berries.
#define Holly_Green 0x00580C
#define Holly_Red 0xB00402
const TProgmemRGBPalette16 Holly_p FL_PROGMEM =
    {Holly_Green, Holly_Green, Holly_Green, Holly_Green,
     Holly_Green, Holly_Green, Holly_Green, Holly_Red,
     Holly_Green, Holly_Green, Holly_Green, Holly_Green,
     Holly_Green, Holly_Green, Holly_Green, Holly_Red};

// A red and white striped palette
// "CRGB::Gray" is used as white to keep the brightness more uniform.
const TProgmemRGBPalette16 RedWhite_p FL_PROGMEM =
    {CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red,
     CRGB::Gray, CRGB::Gray, CRGB::Gray, CRGB::Gray,
     CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red,
     CRGB::Gray, CRGB::Gray, CRGB::Gray, CRGB::Gray};

// A mostly blue palette with white accents.
// "CRGB::Gray" is used as white to keep the brightness more uniform.
const TProgmemRGBPalette16 BlueWhite_p FL_PROGMEM =
    {CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue,
     CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue,
     CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue,
     CRGB::Blue, CRGB::Gray, CRGB::Gray, CRGB::Gray};

// A pure "fairy light" palette with some brightness variations
#define HALFFAIRY ((CRGB::FairyLight & 0xFEFEFE) / 2)
#define QUARTERFAIRY ((CRGB::FairyLight & 0xFCFCFC) / 4)

const TProgmemRGBPalette16 FairyLight_p FL_PROGMEM =
    {CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight,
     HALFFAIRY, HALFFAIRY, CRGB::FairyLight, CRGB::FairyLight,
     QUARTERFAIRY, QUARTERFAIRY, CRGB::FairyLight, CRGB::FairyLight,
     CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight};

// A palette of soft snowflakes with the occasional bright one
#define SnowFlake_Normal 0x304048
#define SnowFlake_Bright 0xE0F0FF

const TProgmemRGBPalette16 Snow_p FL_PROGMEM =
    {SnowFlake_Normal, SnowFlake_Normal, SnowFlake_Normal, SnowFlake_Normal,
     SnowFlake_Normal, SnowFlake_Normal, SnowFlake_Normal, SnowFlake_Normal,
     SnowFlake_Normal, SnowFlake_Normal, SnowFlake_Normal, SnowFlake_Normal,
     SnowFlake_Normal, SnowFlake_Normal, SnowFlake_Normal, SnowFlake_Bright};

// A palette reminiscent of large 'old-school' C9-size tree lights
// in the five classic colors: red, orange, green, blue, and white.
#define C9_Red 0xB80400
#define C9_Orange 0x902C02
#define C9_Green 0x046002
#define C9_Blue 0x070758
#define C9_White 0x606820
const TProgmemRGBPalette16 RetroC9_p FL_PROGMEM =
    {C9_Red, C9_Orange, C9_Red, C9_Orange,
     C9_Orange, C9_Red, C9_Orange, C9_Red,
     C9_Green, C9_Green, C9_Green, C9_Green,
     C9_Blue, C9_Blue, C9_Blue, C9_White};

// A cold, icy pale blue palette
#define Ice_Blue1 0x0C1040
#define Ice_Blue2 0x182080
#define Ice_Blue3 0x5080C0
const TProgmemRGBPalette16 Ice_p FL_PROGMEM =
    {
        Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
        Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
        Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
        Ice_Blue2, Ice_Blue2, Ice_Blue2, Ice_Blue3};

// Add or remove palette names from this list to control which color
// palettes are used, and in what order.

const TProgmemRGBPalette16 *ActivePaletteList[] = {
    &RetroC9_p,
    &BlueWhite_p,
    &RainbowColors_p,
    &FairyLight_p,
    &RedGreenWhite_p,
    &PartyColors_p,
    &RedWhite_p,
    &Snow_p,
    &Holly_p,
    &Ice_p};

const String PaletteNames[] = {
    "Retro C9",
    "BlueWhite",
    "Rainbow",
    "FairyLights",
    "RedGreenWhite",
    "Party",
    "RedWhite",
    "Snow",
    "Holly",
    "Ice"};

// Advance to the next color palette in the list (above).

void chooseNextColorPalette(CRGBPalette16 &pal)
{
    const uint8_t numberOfPalettes = sizeof(ActivePaletteList) / sizeof(ActivePaletteList[0]);
    static uint8_t whichPalette = -1;
    whichPalette = addmod8(whichPalette, 1, numberOfPalettes);

    pal = *(ActivePaletteList[whichPalette]);

    sprintf(g_Msg, "TwinkleFox: Palette# %i - %s", whichPalette, PaletteNames[whichPalette].c_str());
    Serial.println(g_Msg);
}
