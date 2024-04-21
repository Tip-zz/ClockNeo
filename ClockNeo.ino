/*
File:     ClockNeo.ino
Origin:   20-Apr-2024
Author:   Tip Partridge

Description:
  Clock based on WS2812B Neopixel LEDs from Amazon and a DS3231 RTC from Adafruit with a Nano host.
  This is a stripped down version of my overly complicated WallClock.ino sketch.  It is still
  a bit complicated, but should provide an example of functional code.

  This version controls a display consisting of 13 LED strips with 8 LEDs each, for a total of 104 LEDs.
  These are arranged in groups of 3 for each of 4 digits, and a single strip for the colon.  As written
  this sketch expects strips connected in a serpentine manner with pixel 0 at the lower right. Vertical format
  is set in the shodig() function located in the ClockSubroutine module.  There you can edit for serpentine of linear
  order and vertical orientation.  Horizontal format is set in the writeCol() function located in the ClockSubroutine
  module. direction can be flipped by using either "(ledCount-1-pp, clr.rgb)" or "(pp, clr.rgb)" in two places.

Revisions:
  20-Apr-2024 (TEP) v0.74 Create as example from WallclockNeoWS2812B v0.74
*/

#define verstr "ClockNeo v0.74"
#include <LibPrintf.h>  // need this for sprintf %f. Be sure to use this one because the other Printf library is too big.
#include <Wire.h>       // I2C for RTC

// RTC
#include "RTClib.h"
RTC_DS3231 rtc;
DateTime now;
bool gotRTC = false;    // true if RTC found

// ******* Display Panel
#include <Adafruit_NeoPixel.h>
const uint8_t dataPin = 11;
const uint16_t ledCount = 104;          // Set the number of LEDs to control.
const float g_npixels[2] = {104, 80};   // number of pixels used
Adafruit_NeoPixel pixels(ledCount, dataPin, NEO_GRB + NEO_KHZ800);
union WRGB32
  {
  uint32_t rgb;
  uint8_t color[4];   // 0=B 1=G 2=R 3=W
  };
const uint8_t _w = 3;
const uint8_t _r = 2;
const uint8_t _g = 1;
const uint8_t _b = 0;

// *******

// display font: 0..9, :, A, P, M, and ' '
#define Colon 10
#define Aye 11
#define Bee 12
#define Emm 13
#define Space 14
// See Font3x8.xlsx.
const uint8_t fontP[] PROGMEM = {
// font 0
  0xFF,   0x81,   0xFF, 
  0x00,   0xFF,   0x00, 
  0xF9,   0x89,   0x8F, 
  0x89,   0x89,   0xFF, 
  0x0F,   0x08,   0xFF, 
  0x8F,   0x89,   0xF9, 
  0xFF,   0x89,   0xF9, 
  0x01,   0x01,   0xFF, 
  0xFF,   0x89,   0xFF, 
  0x0F,   0x09,   0xFF, 
  0x24,   0x00,   0x00, 
  0xFC,   0x24,   0xFC, 
  0xFC,   0x24,   0x3C, 
  0xFC,   0x18,   0xFC, 
  0x00,   0x00,   0x00,
// font 1
  0x7E,   0x81,   0x7E, 
  0x02,   0xFF,   0x00, 
  0xF1,   0x89,   0x86, 
  0x89,   0x89,   0x76, 
  0x0F,   0x08,   0xFF, 
  0x8F,   0x89,   0x71, 
  0x7E,   0x89,   0x71, 
  0x01,   0x01,   0xFF, 
  0x7E,   0x89,   0x7E, 
  0x06,   0x09,   0xFF, 
  0x24,   0x00,   0x00, 
  0xF8,   0x24,   0xF8, 
  0xFC,   0x24,   0x18, 
  0xFC,   0x18,   0xFC, 
  0x00,   0x00,   0x00, 
  };
uint8_t font[15][3];

// Enums
enum hour_style {ampm, military};
enum wrap_mode {wrap,bounce};
enum dir_mode {fwd, rev};

// Default parameters
const int d_span = 360;
const int d_center = 180;
const int d_palette = 0;
const int d_pnlBrite = 100;
const float d_repeat = 0.75;
const float d_upPeriod = 1.0;
const hour_style d_hourStyle = ampm;
const wrap_mode d_wrapMode = wrap;
const dir_mode d_dirMode = fwd;
const int d_fontN = 0;
#define d_Name "Wall Clock"
#define d_Location "Wall"

// Global parameters
struct Parameters
  {
  int span;
  int center;
  int palette;
  uint8_t pnlBrite;     // panel brightness (1..255)
  float repeat;
  float upPeriod;
  hour_style hourStyle;
  wrap_mode wrapMode;
  dir_mode dirMode;
  int fontN;
  };
Parameters p;
// Miscellaneous global variables and such
int didxidx;
int y, n, d, h, m, s;   // Current time
bool updateRTC;         // flag to update the RTC with y, n, d, h, m, s in main loop.
float g_color = 0.0;    // pixel color. hue = 0..360
float g_color0 = 0.0;   // base color, updated each second
float g_dcolor;         // color increment per loop
float g_colorBot;       // lowest color value
float g_colorTop;       // highest color value
float g_ADC;            // display brightness
dir_mode g_dirMode;     // current direction
unsigned long tMilli;   // color update timer (toggles for bounce)
bool tglTim;            // toggle loop time display flag, for debug benchmark
unsigned long bMilli;   // brightness update timer
const unsigned long bMilVal = 500;   // update each second

// IO
boolean gotIO = false;

// I2C Stuff
const uint8_t ClientID = 1;       // Client I2C address
char i2Str[33];                   // I2C transmit buffer
uint8_t i2Len;                    // number of bytes in I2C transmit buffer
#define i2RcvMax 8                // Size of I2C receive buffer.  Probably only need 7 bytes
volatile char I2C_rcv[i2RcvMax];  // For i2C receive ISR, received character
volatile int I2_pnt;              // index used in I2C receive ISR to put characters into I2C_str
volatile bool GotI2C = false;     // For i2C receive ISR, data available flag
volatile bool I2CReq = false;     // For I2C request USR, data requested
volatile bool isLen;              // Flag to requestEvent ISR to send length

// ***** TIMER ***************************************************
// system clock = 16 MHz
// timer prescale = 64;
#define TicksPer100ms 25000       // clock ticks per 100 ms
volatile bool g_timeout = false;  // global falg set when 100ms timer timeout.

// ***************   SSSSS   EEEEEE   TTTTTTTT   UU   UU   PPPPP    *********
// ***************  SS       EE          TT      UU   UU   PP   PP  *********
// Setup *********    SS     EEEEE       TT      UU   UU   PPPPP    *********
// ***************      SS   EE          TT      UU   UU   PP       *********
// ***************  SSSSS    EEEEEE      TT        UUU     PP       *********

void setup()
{
  char cstr[17];
// Say Hi!
  Serial.begin(115200);
  Serial.println(F(verstr));
  Serial.println(F("  ? for menu"));

// Load default parameters
  setG2D( & p);

// ***** Start RTC ***********************************************
  if (! rtc.begin())
    {
    Serial.println(F("No RTC!!"));   // Oh no!!!
    gotRTC = false;
    delay(500);    // wait a moment 'til we maybe crash. Don't seem to...
    }
  else gotRTC = true;

// ***** Set up global variables *********************************
  calcTBD();        // Calculate g_colorTop, g_colorBot, and g_dcolor
  g_color0 = g_colorBot;    // Initial color
  g_dirMode = p.dirMode;

// set up and clear display ************************************************
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.clear(); // Set all pixel colors to 'off'
  pixels.setBrightness(255); // Set brightness
  delay(100);

  tMilli = millis();  // start timing
  bMilli = tMilli;    // start timing

// Load font ************************************************
  loadFont();

// ***** Get timee ************************************************
  now = rtc.now();
  y = now.year();
  n = now.month();
  d = now.day();
  h = now.hour();
  m = now.minute();
  s = now.second();

} // end setup()

//*******************  LL       OOOOO     OOOOO    PPPPPP   **************************
//*******************  LL      00   00   00   00   PP   PP  **************************
// Loop *************  LL      00   O0   00   00   PPPPPP   **************************
//*******************  LL      00   OO   00   00   PP       **************************
//*******************  LLLLLL   00O00     OOOOO    PP       **************************

void loop()
{
// Shift color pallet by g_dcolor every upPeriod seconds.  Fastest update is about 0.150s.
  if ((millis()-tMilli) >= ((unsigned long)(p.upPeriod * 1000.)))
    {
    if (tglTim) prtln(millis()-tMilli);
    tMilli = millis();  //reset timer
    nextColor();
    for(uint16_t i = 0; i < ledCount; i++) pixels.setPixelColor(255-i, pixels.Color(0, 0, 0));  // clear frame buffer
    }

// Get current time
    now = rtc.now();
    y = now.year();
    n = now.month();
    d = now.day();
    h = now.hour();
    m = now.minute();
    s = now.second();

// Check for commands and parse any
// Check for USB
  if (Serial.available())
    {
    gotIO = true;
    }
// Parse
  if (gotIO) CommandParse();
    
//
// Change RTC time?
//
  if (updateRTC)
    {
    rtc.adjust(DateTime( y, n, d, h, m, s));
    updateRTC = false;
    }

// restart color
  g_color = g_color0;

//
// Update display frame buffer
//
  int bb, uu;   // bottom and upper digits
  bb = now.hour();
// adjust for am or pm
  if (p.hourStyle != military && bb>=12) bb=bb-12;
  if (p.hourStyle == ampm && bb == 0) bb = 12;
// hour
  if (bb>9)
    {
    if (didxidx==1)
      {
      didxidx = 0;
      calcTBD();  // recalcutate globals
      }
    uu=bb/10;
    shodig( uu, 0, 3);
    bb=bb-(10*uu);
    }
  else
    {
    if (didxidx==0)
      {
      didxidx = 1;
      calcTBD();  // recalcutate globals
      }
    shodig( Space, 0, 3);
    }
  shodig( bb, 3, 3);
// colon
  shodig( Colon, 6, 1);
// minute
  bb = now.minute();
  bb=bb%60;
  if (bb>9)
    {
    uu=bb/10;
    shodig( uu, 7, 3);
    bb=bb-(10*uu);
    }
  else 
    {
    shodig( 0, 7, 3);
    }
  shodig( bb, 10, 3);
//
// Write to display
   pixels.show();
}
// end loop()

//**********************************************************
