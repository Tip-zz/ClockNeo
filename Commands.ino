/*
File:     Commands.ino
Origin:   01-Oct-2022
Author:   Tip Partridge

Description:
  Part of WallClock.ino.  This file contains USB Serial command routines

Revisions:
  09-Oct-2022 (TEP) Split from WallClock.ino.
*/
//**************************************************************************
// shoGlob - Display global runtime variables
//**************************************************************************
void shoGlob()
  {
  newln();
  prtln(F("Global Variables:"));
  prt(F("g_dirMode  = "));
  Direction(g_dirMode); newln();
  prt(F("g_dcolor   = "));
  prtln(g_dcolor);
  prt(F("g_colorBot = "));
  prtln(g_colorBot);
  prt(F("g_colorTop = "));
  prtln(g_colorTop);
  prt(F("g_color0   = "));
  prtln(g_color0);
  prt(F("g_npixels  = "));
  prtln(g_npixels[didxidx]);
  prt(F("tglTim     = "));
  prtln(tglTim);
  prt(F("Free memory = "));
  prtln(freeMemory());
  }

//**************************************************************************
// setG2D - Set runtime Parameters to default values
//**************************************************************************
void setG2D( Parameters * gg)
  {
  gg->span = d_span;
  gg->center = d_center;
  gg->palette = d_palette;
  gg->pnlBrite = d_pnlBrite;
  gg->repeat = d_repeat;
  gg->upPeriod = d_upPeriod;
  gg->hourStyle = d_hourStyle;
  gg->wrapMode = d_wrapMode;
  gg->dirMode = d_dirMode;
  g_dirMode = gg->dirMode;      // also set working variable
  gg->fontN = d_fontN;
  }

//**************************************************************************
// showGPs - Display runtime Parameters
void showGPs()
  {
  newln();
  prtln(F("Runtime Parameters:"));
  printParams( &p, "p");
  }

//**************************************************************************
// showDOs - Display default Parameters
void showDOs()
  {
  Parameters gg;
  setG2D( & gg);
  newln();
  prtln(F("Default Parameters:"));
  printParams( &gg, "d");
  }

//**************************************************************************
// printParams - Display parameters
//**************************************************************************

void printParams(Parameters *gg, const char pp[])
  {
  prt(pp); prt(F(".span      S = ")); prtln( gg->span);
  prt(pp); prt(F(".center    C = ")); prtln( gg->center);
  prt(pp); prt(F(".palette  ^P = ")); prtln( gg->palette);
  prt(pp); prt(F(".pnlBrite  B = ")); prtln( gg->pnlBrite);
  prt(pp); prt(F(".repeat    R = ")); prtln( gg->repeat);
  prt(pp); prt(F(".upPeriod  U = ")); prtln( gg->upPeriod);
  prt(pp); prt(F(".hourStyle A = ")); AmPmMil( gg->hourStyle); newln();
  prt(pp); prt(F(".wrapMode  W = ")); Wrap( gg->wrapMode); newln();
  prt(pp); prt(F(".dirMode   D = ")); Direction( gg->dirMode); newln();
  prt(pp); prt(F(".fontN     F = ")); prtln( gg->fontN);
  }

//**********************************************************
// showTime - Display current date and time
//**********************************************************

void showTime()
  {
  prt(y);prt(F("/"));prt(n);prt(F("/"));prt(d);prt(F(" "));
  prt(h);prt(F(":"));prt(m);prt(F(":"));prtln(s);
  }


  
