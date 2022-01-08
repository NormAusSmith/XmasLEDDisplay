# XmasLEDDisplay

## Personal project to drive a set of LED Christmas lights 🎄

The main reason for uploading this to Github was as a learning experience.

For this project I used the following hardware:
```
Heltec_Wifi_Kit_32 and
BTF-LIGHTING WS2812B RGB 5050SMD from Amazon
5m long (60 leds/metre) = 300 leds
```
This code has the following effects:
```
Comets      - a number of comets in Xmas colours
TwinkleFox  - From FastLED examples
Sparkles    - looks a bit like random strobing lights
Icicles     - random dripping icicles
Firewindow  - flames licking up either side of the window
```

In my case I have a 263 led strip, 100 up one side of my window, 63 across
the top and the final 100 down the other side of the window, so g_LEDs[0] is
the bottom of one side and g_LEDs[262] is the bottom of the other.

Most of these effects were scavenged from the web and modified 
to suit my installation and preferences.  I'm not claiming them as my own.

The following were very good resource for some of these effects
```
https://github.com/Aircoookie/AirCoookie
FastLED examples
https://www.youtube.com/c/DavesGarage
```





