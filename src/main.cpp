
#include <Arduino.h>
#define FASTLED_INTERNAL
#include <fastLED.h>

//	LED light strip
//	BTF-LIGHTING WS2812B RGB 5050SMD from Amazon
//	5m long (60 leds/metre) = 300 leds

//  globals

int g_Brightness = 255;  //  max = 255    Actual brightness value
int g_PowerLimit = 4500; //  max = 5000   Power limit in milliwatts
bool g_First_Time = true;

char g_Msg[256]; //  for debugging

//  handy macros

#define ARRAYSIZE(x) (sizeof(x) / sizeof(x[0]))

//  fastLED settings

#define LED_PIN 17
#define NUM_LEDS 263
#define PATTERN_COUNT 5

#define SECS_PER_PATTERN (60 * 15)
//#define SECS_PER_PATTERN (10)

CRGB g_LEDs[NUM_LEDS] = {0};

//  include headers for each pattern

#include "ledgfx.h"

#include "donothing.h"

#include "transition.h"
#include "comet_walker.h"
#include "twinklefox.h"
#include "firewindow.h"
#include "sparklers.h"
#include "Icicle.h"

void setup()
{
  //  init serial port

  Serial.begin(9600);
  while (!Serial)
  {
  }

  Serial.println("ESP32 Startup...");

  //  FastLED Init

  pinMode(LED_PIN, OUTPUT);

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(g_LEDs, NUM_LEDS);
  FastLED.clear(false);

  pinMode(LED_BUILTIN, OUTPUT);
  set_max_power_indicator_LED(LED_BUILTIN);

  FastLED.setMaxPowerInMilliWatts(g_PowerLimit);
  FastLED.setBrightness(g_Brightness);
}

void loop()
{
  static int loop_counter = 0;

  while (true)
  {
    EVERY_N_SECONDS(SECS_PER_PATTERN)
    {
      g_First_Time = true;
      loop_counter++;
      FastLED.clear();

      // only show the transition at the beginning of the set

      if (loop_counter % PATTERN_COUNT == 0)
      {
        ChristmasTransition();
      }

      FastLED.clear();
    }

    switch (loop_counter % PATTERN_COUNT)
    {
    case 0:
      if (g_First_Time)
        Serial.println("TwinkleFox");
      DrawTwinkleFox();
      break;

    case 1:
      if (g_First_Time)
        Serial.println("CometWalkers");
      DrawCometWalkers();
      break;

    case 2:
      if (g_First_Time)
        Serial.println("Icicles");
      DrawIcicles();
      break;

    case 3:
      if (g_First_Time)
        Serial.println("FireWindow");
      DrawFireWindow();
      break;

    case 4:
      if (g_First_Time)
        Serial.println("Sparklers");
      DrawSparklers();
      break;

    default:
      if (g_First_Time)
        Serial.println("Should not happen");

      break;
    }

    FastLED.setBrightness(g_Brightness);
    FastLED.delay(1);
  }
}