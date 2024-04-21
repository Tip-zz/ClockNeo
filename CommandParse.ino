/*
File:     SerialParse.ino
Origin:   01-Oct-2022
Author:   Tip Partridge

Description:
  Part of WallClock.ino.  This file parses commands from USB-Serial

Revisions:
  09-Oct-2022 (TEP) Split from WallClock.ino.
*/

void CommandParse()
{
// Don't declare variables inside a switch or bad things happen!!!
int iTemp;
float fTemp;
int ier;
int ev, od, i, j;
int yy, nn, dd, hh, mm, ss;   // temp time variable for setting time.
char c;                       // Serial command character
c = Serial.read();

switch (c)
  {
  case 'A':                     // toggle between am/pm and military time
  case 'a': if (p.hourStyle == ampm) p.hourStyle = military;
            else p.hourStyle = ampm;
            AmPmMil(p.hourStyle); newln();
            break;
  case 'B':                     // Brightness
  case 'b': prt(F("Enter brightness (1..155)")); Default(p.pnlBrite);
            ier = getInt(& iTemp, 5000);
            newln();
            if (ier > 0 && iTemp > 0 && iTemp < 256)
              {
              p.pnlBrite = iTemp;
              }
            prt(F("Brightness = "));
            prtln(p.pnlBrite);
            break;
  case 'C':                     // Center color
  case 'c': prt(F("Enter center color (mid=180)")); Default(p.center);
            ier = getInt(& iTemp, 5000);
            newln();
            if (ier > 0)
              {
              p.center = iTemp;
              }
            prt(F("Center = "));
            prtln(int(p.center));
            calcTBD();
            break;
  case 'D':                      // dir mode
  case 'd':
            prt(F("Enter shift Direction (0=fwd,1=rev) [")); Direction(p.dirMode); prt(F("]: "));
            ier = getInt(& iTemp, 5000);
            if (ier>0 && iTemp>=0 && iTemp<=1)
              {
              if (iTemp == 0) p.dirMode = fwd;
              else p.dirMode = rev;
              g_dirMode = p.dirMode;
              }
            prt(F("Dir mode = ")); Direction(p.dirMode); newln();
            break;
  case 'F':                     // toggle font
  case 'f': p.fontN += 1;
            if (p.fontN > 1 || p.fontN < 0) p.fontN = 0;
            loadFont();
            break;
  case 'H': h += 1;             // hour up
            if (h > 23) h = 0;
            updateRTC = true;
            prtln(h);
            break;
  case 'h': h -= 1;             // hour down
            if (h < 0) h = 23;
            updateRTC = true;
            prtln(h);
            break;
  case 'M': m += 1;             // minute up
            if (m > 59) m = 0;
            updateRTC = true;
            prtln(m);
            break;
  case 'm': m -= 1;             // minute down
            if (m < 0) m = 59;
            updateRTC = true;
            prtln(m);
            break;
  case 'P':                     // display runtime parameters
  case 'p': showGPs();
            newln();
            break;
  case 'R':                     // Repeat
  case 'r': prt(F("Enter Repeat count")); Default(p.repeat);
            ier = getFloat(& fTemp, 5000);
            newln();
            if (ier > 0)
              {
              if (fTemp < 0.0) fTemp = 0.0;
              if (fTemp > 360.0) fTemp = 360.0;
              p.repeat = fTemp;
              }
            prt(F("Repeat count = "));
            prtln((p.repeat));
            break;
  case 'S':                     // Color Span
  case 's': prt(F("Enter color span (full=360)")); Default(p.span);
            ier = getInt(& iTemp, 5000);
            newln();
            if (ier > 0 && iTemp > 0)
              {
              p.span = iTemp;
              }
            prt(F("Span = "));
            prtln(p.span);
            break;
  case 'T':                      // display date, time
  case 't':
            showTime();
            break;
  case 'U':                     // Set color update period
  case 'u': prt(F("Enter Update period (s)")); Default(p.upPeriod);
            ier = getFloat(& fTemp, 5000);
            newln();
            if (ier > 0)
              {
              if (fTemp < 0.001) fTemp = 0.001;
              p.upPeriod = fTemp;
              }
            prt(F("Loop time = "));
            prtln((p.upPeriod));
            break;
  case 'V':                     // display global variables
  case 'v': shoGlob();
            newln();
            break;
  case 'W':                      // wrap mode
  case 'w':
            prt(F("Enter Wrap mode (0=wrap,1=bounce) [")); Wrap(p.wrapMode); prt(F("]: "));
            ier = getInt(& iTemp, 5000);
            if (ier>0 && iTemp>=0 && iTemp<=1)
              {
              if (iTemp == 0) {p.wrapMode = wrap; g_dirMode=p.dirMode;}
              else p.wrapMode = bounce;
              }
            prt(F("Wrap mode = ")); Wrap(p.wrapMode); newln();
            break;
  case 'Z':                      // Dump RGB frame buffer as csv
  case 'z':
            ev = 1;
            od = 15;
            prtln(F("Red"));
            for (i=0; i<8; i++)
              {
              for (j=0; j<32; j++)
                {
                if (j == 0) iTemp = i;
                else if (j & 0x0001) iTemp = iTemp + od;
                else iTemp = iTemp + ev;
                prt((pixels.getPixelColor(iTemp)&0x00ff0000)>>16);
                if (j!=31) prt(F(","));
                }
              newln();
              ev += 2;
              od -= 2;
              }
            ev = 1;
            od = 15;
            prtln(F("Green"));
            for (i=0; i<8; i++)
              {
              for (j=0; j<32; j++)
                {
                if (j == 0) iTemp = i;
                else if (j & 0x0001) iTemp = iTemp + od;
                else iTemp = iTemp + ev;
                prt((pixels.getPixelColor(iTemp)&0x0000ff00)>>8);
                if (j!=31) prt(F(","));
                }
              newln();
              ev += 2;
              od -= 2;
              }
            ev = 1;
            od = 15;
            prtln(F("Blue"));
            for (i=0; i<8; i++)
              {
              for (j=0; j<32; j++)
                {
                if (j == 0) iTemp = i;
                else if (j & 0x0001) iTemp = iTemp + od;
                else iTemp = iTemp + ev;
                prt(pixels.getPixelColor(iTemp)&0x000000ff);
                if (j!=31) prt(F(","));
                }
              newln();
              ev += 2;
              od -= 2;
              }
            break;
  case '0':                     // zero seconds
  case ')': s = 0;
            if (h < 0) h = 23;
            updateRTC = true;
            break;
  case 4:   // ^D                   // Load global parameters with defaults
            prt(F("Loading default global parameters..."));
            setG2D( & p);
            prt(F("done."));
            showGPs();
            break;
  case 7  : // ^G               // display default parameters
            showDOs();
            newln();
            break;
  case 10:  // Ignore <LF> and <CR> = 10, 13 = ^J, ^M
  case 13:  break;
  case 16:  // ^P                   // select palette
            p.palette = ++p.palette % 2;
            prt(F("palette = "));
            prtln((p.palette));
            break;
  case 20:  // ^T                // set date, time
            yy=y; nn=n; dd=d; hh=h; mm=m; ss=s;
            newln();
            prtln(F("Set Time:"));
// Year
            prt(F("Year")); Default(yy);
            ier = getInt(& iTemp, 10000);
            if (ier > 0 && iTemp >= 2023 && iTemp <= 2099)
              {
              yy = iTemp;
              updateRTC = true;
              }
// Month
            prt(F("Month")); Default(nn);
            ier = getInt(& iTemp, 10000);
            if (ier > 0 && iTemp >= 1 && iTemp <= 12)
              {
              nn = iTemp;
              updateRTC = true;
              }
// Day
            prt(F("Day")); Default(dd);
            ier = getInt(& iTemp, 10000);
            if (ier > 0 && iTemp >= 1 && iTemp <= 31)
              {
              dd = iTemp;
              updateRTC = true;
              }
// Hour
            prt(F("Hour")); Default(hh);
            ier = getInt(& iTemp, 10000);
            if (ier > 0 && iTemp >= 0 && iTemp <= 23)
              {
              hh = iTemp;
              updateRTC = true;
              }
// Min
            prt(F("Minute")); Default(mm);
            ier = getInt(& iTemp, 10000);
            if (ier > 0 && iTemp >= 0 && iTemp <= 59)
              {
              mm = iTemp;
              updateRTC = true;
              }
// Sec
            prt(F("Second")); Default(ss);
            ier = getInt(& iTemp, 10000);
            if (ier > 0 && iTemp >= 0 && iTemp <= 59)
              {
              ss = iTemp;
              updateRTC = true;
              }
//
            if (updateRTC)
              {
              prt(F("Set this time?   "));
              prt(yy);prt(F("/"));prt(nn);prt(F("/"));prt(dd);prt(F(" "));
              prt(hh);prt(F(":"));prt(mm);prt(F(":"));prt(ss);
              prt(F("  [Y]/N "));
              ier = getYN(10000);
              if (ier<= 0) updateRTC = false;
              }
            if (updateRTC)
              {
              y=yy; n=nn; d=dd; h=hh; m=mm; s=ss;
              prt(F("Time updated.\r\n"));
              }
            else
               prt(F("Time not changed.\r\n"));
            break;
  case 24:  // ^X                   // load with time from last compile
            rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
            prtln(F("Time updated to compile time"));
            break;
  case 25:  // ^Y                // toggle loop time display
            tglTim = !tglTim;
            if (tglTim) prtln(F("On")); else prtln(F("Off"));
            break;
  case '?':                     // display Menu  (76 char wide)
  case '/': newln();
            prtln(F("===TIME====================================================================="));
            prtln(F("H/h  increment Hour      A   AM/PM/Military"));
            prtln(F("M/m  increment Minute   ^T   set date & Time"));
            prtln(F(" 0   zero seconds        T   display date & Time"));
            prtln(F("===COLORS==================================================================="));
            prtln(F(" S   set Span (0..360)                 W   set Wrap mode"));
            prtln(F(" C   set Center (0..360)               D   set Direction mode"));
            prtln(F(" R   set Repeat                        U   set color Update period (s)"));
            prtln(F("^P   select color Palette              B   set display brightness"));
            prtln(F("===PARAMETERS==============================================================="));
            prtln(F(" P   display runtime Parameters"));
            prtln(F("^D   load runtime params with Defaults"));
            prtln(F("^G   display default parameters        F   toggle font"));
            prtln(F("===DEBUG===================================================================="));
            prtln(F(" Z   dump RGB Frame buffer            ^Y   toggle loop time display"));
            prtln(F("^X   set to compile time               V   display global Variables"));
            break;
  default:  prt(int(c)); prtln(F(" ?"));
            break;
  } // end switch
gotIO = false;
}  // CommandParse
