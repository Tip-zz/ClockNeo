  Clock based on WS2812B Neopixel LEDs from Amazon and a DS3231 RTC from Adafruit with a Nano host.
  This is a stripped down version of my overly complicated WallClock.ino sketch.  It is still
  a bit complicated, but should provide an example of functional code.

  This version controls a display consisting of 13 LED strips with 8 LEDs each, for a total of 104 LEDs.
  These are arranged in groups of 3 for each of 4 digits, and a single strip for the colon.  As written
  this sketch expects strips connected in a serpentine manner with pixel 0 at the lower right. Vertical format
  is set in the shodig() function located in the ClockSubroutine module.  There you can edit for serpentine of linear
  order and vertical orientation.  Horizontal format is set in the writeCol() function located in the ClockSubroutine
  module. direction can be flipped by using either "(ledCount-1-pp, clr.rgb)" or "(pp, clr.rgb)" in two places.
