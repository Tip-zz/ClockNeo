/*
File:     Wall_IO.ino
Origin:   04-Feb-2023
Author:   Tip Partridge

Description:
  Part of WallClock.ino.  This file contains USB / I2C output routines

Revisions:
  
*/

// ********************************************************************

void newln()
  {
  Serial.println();
  }

// ********************************************************************

void prtc( char chr)
  {
  Serial.write( chr);
  }

// ********************************************************************

void prt( char * str)
  {
  Serial.print( str);
  }

void prtln( char * str)
  {
  Serial.print( str);
  newln();
  }

// ********************************************************************

void prt( const char * str)
  {
  Serial.print( str);
  }

void prtln( const char * str)
  {
  Serial.print( str);
  newln();
  }

// ********************************************************************

void prt( const __FlashStringHelper* str)
  {
  char astr[90];
  PGM_P p = reinterpret_cast<PGM_P>(str);
  size_t n = 0;
  while (1) 
    {
    unsigned char c = pgm_read_byte(p++);
    if (c == 0) break;
    astr[n++] = c;
    }
  astr[n]=0;
  Serial.print( astr);
  }

void prtln( const __FlashStringHelper* str)
  {
  char astr[80];
  PGM_P p = reinterpret_cast<PGM_P>(str);
  size_t n = 0;
  while (1) 
    {
    unsigned char c = pgm_read_byte(p++);
    if (c == 0) break;
    astr[n++] = c;
    }
  astr[n]=0;
  Serial.print( astr);
  newln();
  }

// ********************************************************************

void prt( float flt)
  {
  char str[10];
  sprintf( str, "%1.3f", flt);
  prt( str);
  }

void prt1( float flt)
  {
  char str[10];
  sprintf( str, "%1.1f", flt);
  prt( str);
  }

void prtln( float flt)
  {
  char str[10];
  sprintf( str, "%1.3f", flt);
  prt( str);
  newln();
  }

// ********************************************************************

void prt( int num)
  {
  char str[10];
  sprintf( str, "%d", num);
  prt( str);
  }

void prtln( int num)
  {
  char str[10];
  sprintf( str, "%d", num);
  prt( str);
  newln();
  }

// ********************************************************************

void prt( unsigned long num)
  {
  char str[10];
  sprintf( str, "%lu", num);
  prt( str);
  }

// ********************************************************************

void prtln( unsigned long num)
  {
  char str[10];
  sprintf( str, "%lu", num);
  prt( str);
  newln();
  }

// ********************************************************************
