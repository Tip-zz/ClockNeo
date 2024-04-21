//**************************************************************************
// calcTBD - Calculate global runtime variables
//**************************************************************************
void calcTBD()
// Calculate g_colorTop, g_colorBot, and g_dcolor
  {
  g_dcolor = (p.span-1) / g_npixels[didxidx] * p.repeat; //216 or 176 active pixels
  for(uint16_t i = 0; i < ledCount; i++) pixels.setPixelColor(255-i, pixels.Color(0, 0, 0));  // clear frame buffer
  g_colorBot = float(p.center) - (p.span)/2.0;
  g_colorTop = float(p.center) + (p.span-2)/2.0;
  g_color0 = g_colorTop;
  }
//**************************************************************************
// shodig - Write character to display
//**************************************************************************
void shodig( int dig, int posn, int columns)
// Write character to display buffer
// dig is index into font. 0 = '0'
// posn is display column to start. First (left) column is 0.
// columns is number of colums to write.
// Use color from global "g_color"
// Increment color by global "g_dcolor" for each pixel
  {
  int cc, c, pp, bb;
  c = posn;
  pp = c*8;

  for (cc=0; cc<columns; cc++)  // write columns to display buffer
    {
     if (c%2)      // odd columns go top to bottom (?)
      {
      for (bb=7; bb>=0; bb--)
        {
        writeCol(dig, bb, cc, pp);
        pp+=1;
        }
      }
    else          // even columns go bottom to top
      {
      for (bb=0; bb<8; bb++)
        {
        writeCol(dig, bb, cc, pp);
        pp+=1;
        }
      }
    c+=1;

    }
  }

inline void writeCol(int dig, int bb, int cc, int pp)
// Write one pixel into frame buffer
// dig = digit 0..' '
// bb = vert 0..7
// cc = col 0..5
// pp = pointer into frame buffer
  {
  int bits = 1;
  WRGB32 clr;
  g_color = g_color + g_dcolor;
  if (g_color >= g_colorTop) g_color = g_colorBot;
  switch (p.palette)
    {
    case 0:   clr = nnnToRgb(int(g_color), byte(p.pnlBrite));
              break;
    case 1:   clr = hsvToRgb(int(g_color), byte(p.pnlBrite));
              break;
    }
  if (font[dig][cc] & bits<<bb) {pixels.setPixelColor(ledCount-1-pp, clr.rgb);}
  else {pixels.setPixelColor(ledCount-1-pp, pixels.Color( 0, 0, 0));}
  }

//**************************************************************************
// nextColor - Advance color
//**************************************************************************
inline void nextColor()
  {
// advance
  if (g_dirMode == fwd) 
    {
    g_color0 = g_color0 - g_dcolor;
    }
  else 
    {
    g_color0 = g_color0 + g_dcolor;
    }
// check for lower bound
  if (g_color0 < g_colorBot)
    {
    if (p.wrapMode == wrap)         // wrap
      g_color0 = g_colorTop;
    else                            // bounce
      {
      g_color0 = g_colorBot;
      g_dirMode = rev;
      }
    }
  else if (g_color0 > g_colorTop)   // wrap
    {
    if (p.wrapMode == wrap)
      g_color0 = g_colorBot;
    else                            // bounce
      {
      g_color0 = g_colorTop;
      g_dirMode = fwd;
      }
    }
  }

//**************************************************************************
// Print strings
//**************************************************************************
void Direction(dir_mode mode)
  {
  if (mode==fwd) prt(F("Fwd"));
  else prt(F("Rev"));
  }

void Wrap(wrap_mode mode)
  {
  if (mode==wrap) prt(F("Wrap"));
  else prt(F("Bounce"));
  }

void AmPmMil(hour_style hourStyle)
  {
  if (hourStyle==ampm) prt(F("AM/PM"));
  else prt(F("Military"));
  }
 
//**************************************************************************
// Print formatted default value
//**************************************************************************
void Default( int dflt)
  {
  prt(F(" ["));
  prt(dflt);
  prt(F("]: "));
  }
void Default( float dflt)
  {
  prt(F(" ["));
  prt(dflt);
  prt(F("]: "));
  }
void Default( uint8_t dflt)
  {
  prt(F(" ["));
  prt(dflt);
  prt(F("]: "));
  }
void Default( char * dflt)
  {
  prt(F(" ["));
  prt(dflt);
  prt(F("]: "));
  }

//**************************************************************************
// Load selected font from Flash to RAM
//**************************************************************************
void loadFont()
  {
  int rr, cc, ii;
  ii = p.fontN * 45;
  for (rr = 0; rr < 15; rr++)
    for (cc = 0; cc < 3; cc++)
      font[rr][cc] = pgm_read_byte_near(fontP + ii++);
  }

//**************************************************************************
// Measure free memory
//**************************************************************************
#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

int freeMemory() 
  {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
  }
