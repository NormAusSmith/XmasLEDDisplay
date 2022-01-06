
#include <sys/time.h>
#include <queue>
using namespace std;

#include <Arduino.h>
#define FASTLED_INTERNAL
#include <fastLED.h>

#include <ledgfx.h>

extern CRGB g_LEDs[];
extern char g_Msg[];

//  Low end of the led string (starting at 0), goes up one side of the window and the icicles descend.
//  The LEDs are strung across the top of the window and down the other side.
//  The high end of the led string (ending in 300), go down the other side of the window so the
//  leds are reversed so they also descend on the far side.

class Icicle
{
public:
    int m_start;            //  start position of the icicle on the string
    int m_icicle_length;    //  length of the icicle
    int m_aperture_length;  //  length of the aperture in which the icicle descends
                            //  (may or may not be longer than the icicle)

    bool m_reversed;        //  icicles on the far side of the window (higher numbered LEDS) are reversed
    bool m_complete;        //  flag to indicate this icicle has been completely drawn

    int m_delay;            //  random delay between each step of the icicle.
    double m_last_update;   //  time of last step

    int m_step;             //  keep track of the steps

public:
    //  parameterised constructor

    Icicle(int startPos, int icicleLength, int apertureLength, bool drawReversed)
    {
        Set(startPos, icicleLength, apertureLength, drawReversed);
    }

    // default constructor

    Icicle()
    {
        Set(0, 1, 1, false);
    }

    ~Icicle(){};

    void Set(int startPos, int icicleLength, int apertureLength, bool drawReversed)
    {
        m_start = startPos;
        m_icicle_length = icicleLength;
        m_aperture_length = apertureLength;
        m_reversed = drawReversed;

        if (apertureLength < icicleLength)
            apertureLength = icicleLength;

        m_step = 0;
        m_complete = false;

        m_delay = random(30, 100);
        m_last_update = millis();
    }

    bool IsComplete()
    {
        return m_complete;
    }

    void Render()
    {
        if (m_complete)
            return;

        if (m_last_update + m_delay > millis())
            return;

        if (m_reversed)
        {
            if (m_step < m_icicle_length) //  fill
            {
                for (int i = m_start; i < m_start + m_icicle_length; i++)
                {
                    if (i > m_start + m_step)
                        SetLedPosAndColour(i, CRGB::Black);
                    else
                        SetLedPosAndColour(i, CRGB::White);
                }
            }
            else
            {
                //  reverse drain

                for (int i = m_start; i < m_start + m_aperture_length; i++)
                {
                    int top = m_start + (m_step - m_icicle_length);
                    int bot = (top + m_icicle_length + 1);

                    if (i < top || i > bot)
                        SetLedPosAndColour(i, CRGB::Black);
                    else
                        SetLedPosAndColour(i, CRGB::White);
                }
            }
        }
        else //  forward (not reversed)
        {
            if (m_step < m_icicle_length) //  fill
            {
                for (int i = m_start; i > m_start - m_icicle_length; i--)
                {
                    if (i > m_start - m_step)
                        SetLedPosAndColour(i, CRGB::White);
                    else
                        SetLedPosAndColour(i, CRGB::Black);
                }
            }
            else // drain
            {
                for (int i = m_start; i > m_start - m_aperture_length; i--)
                {
                    int top = m_start - (m_step - m_icicle_length);
                    int bot = (top - m_icicle_length + 1);

                    if (i > top || i < bot)
                        SetLedPosAndColour(i, CRGB::Black);
                    else
                        SetLedPosAndColour(i, CRGB::White);
                }
            }
        }

        FastLED.show();
        m_last_update = millis();

        m_step++;

        if (m_step > (m_icicle_length + m_aperture_length))
        {
            m_complete = true;
            return;
        }

    } // end Render

    void SetLedPosAndColour(int ledPosition, CRGB ledColour)
    {
        //  dont draw outside our aperture

        if (m_reversed)
        {
            if (ledPosition < m_start || ledPosition > (m_start + m_aperture_length))
                return;
        }
        else
        {
            if (ledPosition > m_start || ledPosition < (m_start - m_aperture_length))
                return;
        }

        //  ensure we don't exceed the limits of the LED strip

        if (ledPosition < 0 || ledPosition > NUM_LEDS - 1)
            return;

        g_LEDs[ledPosition] = ledColour;
        //FastLED.leds()[ledPosition] = ledColour;
    }

}; //  end Class Icicle

int ConstrainInts(int testValue, int minValue, int maxValue)
{
    //  simple worker function that ensures integers stay within 2 values

    int iReturn = testValue;

    if (minValue > maxValue)
        return testValue;

    if (testValue < minValue)
        iReturn = minValue;

    if (testValue > maxValue)
        iReturn = maxValue;

    return (iReturn);
}

#define NUM_ICICLES 6
#define ZONE_SIZE 33

Icicle g_Icicle[NUM_ICICLES];

void DrawIcicles()
{
    //  allow for logic to be called once, the first time this pattern is called.

    if (g_First_Time)
    {
        //  put one time logic here
        g_First_Time = false;
    }

    static int loop = 0;
    loop++;

    //  Hardcoded to my installation.
    //  I have 263 leds in total.  100 up one side of window, 63 across top, 100 down other side
    //  There are 3 icicles on each side, each restricted to a 33 pixel zone.
    //  Within each zone, the icicle has a random starting posistion, length, aperture and speed.

    int zones[] = {33, 66, 99, 164, 197, 230};

    int index = loop % NUM_ICICLES;

    if (g_Icicle[index].IsComplete())
    {
        // This icicle is complete, so set up new parameters

        int len = random(3, 10);                          //  length of an icicle
        int aperture = len + random(10);                  //  size of the aperture for the icicle
        bool reversed = (index > 2);                      //  if reversed (far side of window)?
        int zone_offset = (random(ZONE_SIZE) - aperture); //  start position of the icicle witin its zone

        zone_offset = ConstrainInts(zone_offset, 0, (ZONE_SIZE - aperture));

        int pos;

        if (reversed)
            pos = zones[index] + zone_offset;
        else
            pos = zones[index] - zone_offset;

        g_Icicle[index].Set(pos, len, aperture, reversed);
    }

    g_Icicle[index].Render();
}
