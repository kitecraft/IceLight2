#include "Colour_Utilities.h"

void FillSolid( CRGB * leds, int numToFill, const CRGB& color)
{
    for( int i = 0; i < numToFill; i++) {
        leds[i] = color;
    }
}

void FillSolid( CRGB * leds, int numToFill, const CRGB_SMALL& color)
{
    for( int i = 0; i < numToFill; i++) {
        leds[i] = color;
    }
}

void FillSolid( CRGB * leds, int numToFill, const uint32_t color)
{
    for( int i = 0; i < numToFill; i++) {
        leds[i] = color;
    }
}

/*
void FillSolid( struct CHSV * targetArray, int numToFill, const struct CHSV& hsvColor)
{
    for( int i = 0; i < numToFill; i++) {
        targetArray[i] = hsvColor;
    }
}
*/

void FillRainbow( struct CRGB * pFirstLED, int numToFill,
                  uint8_t initialhue,
                  uint8_t deltahue )
{
    CHSV hsv;
    hsv.hue = initialhue;
    hsv.val = 255;
    hsv.sat = 240;
    for( int i = 0; i < numToFill; i++) {
        pFirstLED[i] = hsv;
        hsv.hue += deltahue;
    }
}
/*
void FillRainbow( struct CHSV * targetArray, int numToFill,
                  uint8_t initialhue,
                  uint8_t deltahue )
{
    CHSV hsv;
    hsv.hue = initialhue;
    hsv.val = 255;
    hsv.sat = 240;
    for( int i = 0; i < numToFill; i++) {
        targetArray[i] = hsv;
        hsv.hue += deltahue;
    }
}
*/


void FillGradientRGB( CRGB* leds,
                   uint16_t startpos, CRGB_SMALL startcolor,
                   uint16_t endpos,   CRGB_SMALL endcolor )
{
    // if the points are in the wrong order, straighten them
    if( endpos < startpos ) {
        uint16_t t = endpos;
        CRGB_SMALL tc = endcolor;
        endcolor = startcolor;
        endpos = startpos;
        startpos = t;
        startcolor = tc;
    }

    int16_t rdistance87;
    int16_t gdistance87;
    int16_t bdistance87;

    rdistance87 = (endcolor.r - startcolor.r) << 7;
    gdistance87 = (endcolor.g - startcolor.g) << 7;
    bdistance87 = (endcolor.b - startcolor.b) << 7;

    uint16_t pixeldistance = endpos - startpos;
    int16_t divisor = pixeldistance ? pixeldistance : 1;

    int16_t rdelta87 = rdistance87 / divisor;
    int16_t gdelta87 = gdistance87 / divisor;
    int16_t bdelta87 = bdistance87 / divisor;

    rdelta87 *= 2;
    gdelta87 *= 2;
    bdelta87 *= 2;

    accum88 r88 = startcolor.r << 8;
    accum88 g88 = startcolor.g << 8;
    accum88 b88 = startcolor.b << 8;
    for( uint16_t i = startpos; i <= endpos; i++) {
        leds[i] = CRGB_SMALL( r88 >> 8, g88 >> 8, b88 >> 8);
        r88 += rdelta87;
        g88 += gdelta87;
        b88 += bdelta87;
    }
}
