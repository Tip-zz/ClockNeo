// This function from Adafruit APA102 library Rainbow.ino example.
inline WRGB32 hsvToRgb(uint16_t h, uint8_t v)
/* Converts a color from HSV to RGB.
   h is hue, as a number between 0 and 360.
   v is the value, as a number between 0 and 255.
*/
{
  WRGB32 rgbTemp;
  h = (h % 360);
  uint8_t f = (h % 60) * 255 / 60;
  uint8_t q = (255 - f) * (uint16_t)v / 255;
  uint8_t t = f * (uint16_t)v / 255;

  uint8_t r = 0, g = 0, b = 0;
  switch((h / 60) % 6){
      case 0: r = v; g = t; b = 0; break;
      case 1: r = q; g = v; b = 0; break;
      case 2: r = 0; g = v; b = t; break;
      case 3: r = 0; g = q; b = v; break;
      case 4: r = t; g = 0; b = v; break;
      case 5: r = v; g = 0; b = q; break;
  }
#ifdef testfont
  if (h==1)
    {
    rgbTemp.rgb=0;
    return rgbTemp;
    }
#endif

  rgbTemp.color[_w] = 0;
  rgbTemp.color[_r] = r;
  rgbTemp.color[_g] = g;
  rgbTemp.color[_b] = b;

  return rgbTemp;
}


inline WRGB32 nnnToRgb(uint16_t hh, uint8_t v)
// Return RGB color.
// h is hue, as a number between 0 and 360.
// v is the value, as a number between 0 and 255.
  {
  WRGB32 rgbTemp;
  uint8_t r = 0, g = 0, b = 0;
  hh = (hh % 360);
  float h = hh * 764.0 / 360.0;
  if (h >= 510)
    {
    r = h - 510;
    g = 0;
    b = 765 - h;
    }
  else if (h >= 255)
    {
    r = 0;
    g = 510 - h;
    b = h - 255;
    }
  else
    {
    r = 255 - h;
    g = h;
    b = 0;
    }
  r = r * v / 255;
  g = g * v / 255;
  b = b * v / 255;
  rgbTemp.color[_w] = 0;
  rgbTemp.color[_r] = r;
  rgbTemp.color[_g] = g;
  rgbTemp.color[_b] = b;

  return rgbTemp;
  }
