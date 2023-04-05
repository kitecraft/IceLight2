#include "Colour_Utilities.h"


void nscale8( CRGB* leds, uint16_t num_leds, uint8_t scale)
{
    for( uint16_t i = 0; i < num_leds; i++) {
        leds[i].nscale8( scale);
    }
}

//Uses nscale8 to reduce the brightness
void FadeToBlackBy( CRGB* leds, uint16_t num_leds, uint8_t fadeBy)
{
    nscale8( leds, num_leds, 255 - fadeBy);
}

void nscale8_video( CRGB* leds, uint16_t num_leds, uint8_t scale)
{
    for( uint16_t i = 0; i < num_leds; i++) {
        leds[i].nscale8_video( scale);
    }
}

//Uses nscale8_video to reduce the brightness
void FadeLightBy(CRGB* leds, uint16_t num_leds, uint8_t fadeBy)
{
    nscale8_video( leds, num_leds, 255 - fadeBy);
}

void FadeWithColor( CRGB* leds, uint16_t numLeds, const CRGB_SMALL& colormask)
{
    uint8_t fr, fg, fb;
    fr = colormask.r;
    fg = colormask.g;
    fb = colormask.b;

    for( uint16_t i = 0; i < numLeds; i++) {
        *leds[i].r = scale8( *leds[i].r, fr);
        *leds[i].g = scale8( *leds[i].g, fg);
        *leds[i].b = scale8( *leds[i].b, fb);
    }
}


void Blend( CRGB& existing, const CRGB_SMALL& overlay, uint8_t amountOfOverlay )
{
    if( amountOfOverlay == 0) {
        return;
    }

    if( amountOfOverlay == 255) {
        existing = overlay;
        return;
    }
    
    *existing.r   = blend8( *existing.r,   overlay.r,   amountOfOverlay);
    *existing.g = blend8( *existing.g, overlay.g, amountOfOverlay);
    *existing.b  = blend8( *existing.b,  overlay.b,  amountOfOverlay);
}

void Blend( CRGB& existing, const CRGB& overlay, fract8 amountOfOverlay )
{
    Blend(existing, CRGB_SMALL(*overlay.r, *overlay.g, *overlay.b), amountOfOverlay);
}

/* not sure if this works
void Blend( CHSV& existing, const CHSV& overlay, fract8 amountOfOverlay, GradientDirectionCode_t directionCode)
{
    if( amountOfOverlay == 0) {
        return;
    }

    if( amountOfOverlay == 255) {
        existing = overlay;
        return;
    }

    fract8 amountOfKeep = 255 - amountOfOverlay;

    uint8_t huedelta8 = overlay.hue - existing.hue;

    if( directionCode == SHORTEST_HUES ) {
        directionCode = FORWARD_HUES;
        if( huedelta8 > 127) {
            directionCode = BACKWARD_HUES;
        }
    }

    if( directionCode == LONGEST_HUES ) {
        directionCode = FORWARD_HUES;
        if( huedelta8 < 128) {
            directionCode = BACKWARD_HUES;
        }
    }

    if( directionCode == FORWARD_HUES) {
        existing.hue = existing.hue + scale8( huedelta8, amountOfOverlay);
    }
    else /* directionCode == BACKWARD_HUES */
    {
        huedelta8 = -huedelta8;
        existing.hue = existing.hue - scale8( huedelta8, amountOfOverlay);
    }

    existing.sat   = scale8( existing.sat,   amountOfKeep)
    + scale8( overlay.sat,    amountOfOverlay);
    existing.val = scale8( existing.val, amountOfKeep)
    + scale8( overlay.val,  amountOfOverlay);
}
*/


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


void FillGradientRGB( CRGB* leds, uint16_t numLeds, const CRGB_SMALL& c1, const CRGB_SMALL& c2)
{
    uint16_t last = numLeds - 1;
    FillGradientRGB( leds, 0, c1, last, c2);
}


void FillGradientRGB( CRGB* leds, uint16_t numLeds, const CRGB_SMALL& c1, const CRGB_SMALL& c2, const CRGB_SMALL& c3)
{
    uint16_t half = (numLeds / 2);
    uint16_t last = numLeds - 1;
    FillGradientRGB( leds,    0, c1, half, c2);
    FillGradientRGB( leds, half, c2, last, c3);
}

void FillGradientRGB( CRGB* leds, uint16_t numLeds, const CRGB_SMALL& c1, const CRGB_SMALL& c2, const CRGB_SMALL& c3, const CRGB_SMALL& c4)
{
    uint16_t onethird = (numLeds / 3);
    uint16_t twothirds = ((numLeds * 2) / 3);
    uint16_t last = numLeds - 1;
    FillGradientRGB( leds,         0, c1,  onethird, c2);
    FillGradientRGB( leds,  onethird, c2, twothirds, c3);
    FillGradientRGB( leds, twothirds, c3,      last, c4);
}