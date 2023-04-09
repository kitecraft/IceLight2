#include "Colour_Utilities.h"


void FillSolid( CHSV* leds, int numToFill, const CHSV color)
{
    for( int i = 0; i < numToFill; i++) {
        leds[i] = color;
    }
}



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

void FadeWithColor( CRGB* leds, uint16_t numLeds, const CRGBSmall& colormask)
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


void Blend( CRGB& existing, const CRGBSmall& overlay, uint8_t amountOfOverlay )
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
    Blend(existing, CRGBSmall(*overlay.r, *overlay.g, *overlay.b), amountOfOverlay);
}

#if 0
/* not sure if this works*/
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
#endif

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
                   uint16_t startpos, CRGBSmall startcolor,
                   uint16_t endpos,   CRGBSmall endcolor )
{
    // if the points are in the wrong order, straighten them
    if( endpos < startpos ) {
        uint16_t t = endpos;
        CRGBSmall tc = endcolor;
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
        leds[i] = CRGBSmall( r88 >> 8, g88 >> 8, b88 >> 8);
        r88 += rdelta87;
        g88 += gdelta87;
        b88 += bdelta87;
    }
}

void FillGradientRGB( CRGB* leds, uint16_t numLeds, const CRGBSmall& c1, const CRGBSmall& c2)
{
    uint16_t last = numLeds - 1;
    FillGradientRGB( leds, 0, c1, last, c2);
}

void FillGradientRGB( CRGB* leds, uint16_t numLeds, const CRGBSmall& c1, const CRGBSmall& c2, const CRGBSmall& c3)
{
    uint16_t half = (numLeds / 2);
    uint16_t last = numLeds - 1;
    FillGradientRGB( leds,    0, c1, half, c2);
    FillGradientRGB( leds, half, c2, last, c3);
}

void FillGradientRGB( CRGB* leds, uint16_t numLeds, const CRGBSmall& c1, const CRGBSmall& c2, const CRGBSmall& c3, const CRGBSmall& c4)
{
    uint16_t onethird = (numLeds / 3);
    uint16_t twothirds = ((numLeds * 2) / 3);
    uint16_t last = numLeds - 1;
    FillGradientRGB( leds, 0, c1,  onethird, c2);
    FillGradientRGB( leds, onethird, c2, twothirds, c3);
    FillGradientRGB( leds, twothirds, c3,      last, c4);
}

void FillGradientRGB( CRGBSmall* leds,
                   uint16_t startpos, CRGBSmall startcolor,
                   uint16_t endpos,   CRGBSmall endcolor )
{
    // if the points are in the wrong order, straighten them
    if( endpos < startpos ) {
        uint16_t t = endpos;
        CRGBSmall tc = endcolor;
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
        leds[i] = CRGBSmall( r88 >> 8, g88 >> 8, b88 >> 8);
        r88 += rdelta87;
        g88 += gdelta87;
        b88 += bdelta87;
    }
}

void FillGradientRGB( CRGBSmall* leds, uint16_t numLeds, const CRGBSmall& c1, const CRGBSmall& c2)
{
    uint16_t last = numLeds - 1;
    FillGradientRGB( leds, 0, c1, last, c2);
}

void FillGradientRGB( CRGBSmall* leds, uint16_t numLeds, const CRGBSmall& c1, const CRGBSmall& c2, const CRGBSmall& c3)
{
    uint16_t half = (numLeds / 2);
    uint16_t last = numLeds - 1;
    FillGradientRGB( leds,    0, c1, half, c2);
    FillGradientRGB( leds, half, c2, last, c3);
}

void FillGradientRGB( CRGBSmall* leds, uint16_t numLeds, const CRGBSmall& c1, const CRGBSmall& c2, const CRGBSmall& c3, const CRGBSmall& c4)
{
    uint16_t onethird = (numLeds / 3);
    uint16_t twothirds = ((numLeds * 2) / 3);
    uint16_t last = numLeds - 1;
    FillGradientRGB( leds,         0, c1,  onethird, c2);
    FillGradientRGB( leds,  onethird, c2, twothirds, c3);
    FillGradientRGB( leds, twothirds, c3,      last, c4);
}

// lsrX4: helper function to divide a number by 16, aka four LSR's.
// On avr-gcc, "u8 >> 4" generates a loop, which is big, and slow.
// merely forcing it to be four /=2's causes avr-gcc to emit
// a SWAP instruction followed by an AND 0x0F, which is faster, and smaller.
inline uint8_t lsrX4( uint8_t dividend) __attribute__((always_inline));
inline uint8_t lsrX4( uint8_t dividend)
{
    dividend >>= 4;
    return dividend;
}


CRGBSmall ColorFromPalette( const CRGBPalette16& pal, uint8_t index, uint8_t brightness, TBlendType blendType)
{
    //      hi4 = index >> 4;
    uint8_t hi4 = lsrX4(index);
    uint8_t lo4 = index & 0x0F;
    
    // const CRGB* entry = &(pal[0]) + hi4;
    // since hi4 is always 0..15, hi4 * sizeof(CRGB) can be a single-byte value,
    // instead of the two byte 'int' that avr-gcc defaults to.
    // So, we multiply hi4 X sizeof(CRGB), giving hi4XsizeofCRGB;
    uint8_t hi4XsizeofCRGB = hi4 * sizeof(CRGBSmall);
    // We then add that to a base array pointer.
    const CRGBSmall* entry = (CRGBSmall*)( (uint8_t*)(&(pal[0])) + hi4XsizeofCRGB);
    
    uint8_t blend = lo4 && (blendType != NOBLEND);
    
    uint8_t red1   = entry->r;
    uint8_t green1 = entry->g;
    uint8_t blue1  = entry->b;
    
    
    if( blend ) {
        
        if( hi4 == 15 ) {
            entry = &(pal[0]);
        } else {
            entry++;
        }
        
        uint8_t f2 = lo4 << 4;
        uint8_t f1 = 255 - f2;
        
        //    rgb1.nscale8(f1);
        uint8_t red2   = entry->r;
        red1   = scale8( red1,   f1);
        red2   = scale8( red2,   f2);
        red1   += red2;

        uint8_t green2 = entry->g;
        green1 = scale8( green1, f1);
        green2 = scale8( green2, f2);
        green1 += green2;

        uint8_t blue2  = entry->b;
        blue1  = scale8( blue1,  f1);
        blue2  = scale8( blue2,  f2);
        blue1  += blue2;
        
        
    }
    
    if( brightness != 255) {
        if( brightness ) {
            brightness++; // adjust for rounding
            // Now, since brightness is nonzero, we don't need the full scale8_video logic;
            // we can just to scale8 and then add one (unless scale8 fixed) to all nonzero inputs.
            if( red1 )   {
                red1 = scale8( red1, brightness);
                red1++;
            }
            if( green1 ) {
                green1 = scale8( green1, brightness);
                green1++;
            }
            if( blue1 )  {
                blue1 = scale8( blue1, brightness);
                blue1++;
            }
            
        } else {
            red1 = 0;
            green1 = 0;
            blue1 = 0;
        }
    }
    
    return CRGBSmall( red1, green1, blue1);
}

CRGBSmall ColorFromPalette( const RGBPalette16& pal, uint8_t index, uint8_t brightness, TBlendType blendType)
{
    //      hi4 = index >> 4;
    uint8_t hi4 = lsrX4(index);
    uint8_t lo4 = index & 0x0F;

    CRGBSmall entry   =  FL_PGM_READ_DWORD_NEAR( &(pal[0]) + hi4 );
    

    uint8_t red1   = entry.r;
    uint8_t green1 = entry.g;
    uint8_t blue1  = entry.b;

    uint8_t blend = lo4 && (blendType != NOBLEND);

    if( blend ) {

        if( hi4 == 15 ) {
            entry =   FL_PGM_READ_DWORD_NEAR( &(pal[0]) );
        } else {
            entry =   FL_PGM_READ_DWORD_NEAR( &(pal[1]) + hi4 );
        }

        uint8_t f2 = lo4 << 4;
        uint8_t f1 = 255 - f2;

        uint8_t red2   = entry.r;
        red1   = scale8( red1,   f1);
        red2   = scale8( red2,   f2);
        red1   += red2;

        uint8_t green2 = entry.g;
        green1 = scale8( green1, f1);
        green2 = scale8( green2, f2);
        green1 += green2;

        uint8_t blue2  = entry.b;
        blue1  = scale8( blue1,  f1);
        blue2  = scale8( blue2,  f2);
        blue1  += blue2;
    }

    if( brightness != 255) {
        if( brightness ) {
            brightness++; // adjust for rounding
            // Now, since brightness is nonzero, we don't need the full scale8_video logic;
            // we can just to scale8 and then add one (unless scale8 fixed) to all nonzero inputs.
            if( red1 )   {
                red1 = scale8( red1, brightness);
                red1++;
            }
            if( green1 ) {
                green1 = scale8( green1, brightness);
                green1++;
            }
            if( blue1 )  {
                blue1 = scale8( blue1, brightness);
                blue1++;
            }
            
        } else {
            red1 = 0;
            green1 = 0;
            blue1 = 0;
        }
    }

    return CRGBSmall( red1, green1, blue1);
}


CRGBSmall ColorFromPalette( const CRGBPalette32& pal, uint8_t index, uint8_t brightness, TBlendType blendType)
{
    uint8_t hi5 = index;
    hi5 >>= 3;
    uint8_t lo3 = index & 0x07;
    
    // const CRGB* entry = &(pal[0]) + hi5;
    // since hi5 is always 0..31, hi4 * sizeof(CRGB) can be a single-byte value,
    // instead of the two byte 'int' that avr-gcc defaults to.
    // So, we multiply hi5 X sizeof(CRGB), giving hi5XsizeofCRGB;
    uint8_t hi5XsizeofCRGB = hi5 * sizeof(CRGBSmall);
    // We then add that to a base array pointer.
    const CRGBSmall* entry = (CRGBSmall*)( (uint8_t*)(&(pal[0])) + hi5XsizeofCRGB);
    
    uint8_t red1   = entry->r;
    uint8_t green1 = entry->g;
    uint8_t blue1  = entry->b;
    
    uint8_t blend = lo3 && (blendType != NOBLEND);
    
    if( blend ) {
        
        if( hi5 == 31 ) {
            entry = &(pal[0]);
        } else {
            entry++;
        }
        
        uint8_t f2 = lo3 << 5;
        uint8_t f1 = 255 - f2;
        
        uint8_t red2   = entry->r;
        red1   = scale8( red1,   f1);
        red2   = scale8( red2,   f2);
        red1   += red2;
        
        uint8_t green2 = entry->g;
        green1 = scale8( green1, f1);
        green2 = scale8( green2, f2);
        green1 += green2;
        
        uint8_t blue2  = entry->b;
        blue1  = scale8( blue1,  f1);
        blue2  = scale8( blue2,  f2);
        blue1  += blue2;
    }
    
    if( brightness != 255) {
        if( brightness ) {
            brightness++; // adjust for rounding
            // Now, since brightness is nonzero, we don't need the full scale8_video logic;
            // we can just to scale8 and then add one (unless scale8 fixed) to all nonzero inputs.
            if( red1 )   {
                red1 = scale8( red1, brightness);
                red1++;
            }
            if( green1 ) {
                green1 = scale8( green1, brightness);
                green1++;
            }
            if( blue1 )  {
                blue1 = scale8( blue1, brightness);
                blue1++;
            }
            
        } else {
            red1 = 0;
            green1 = 0;
            blue1 = 0;
        }
    }
    
    return CRGBSmall( red1, green1, blue1);
}


CRGBSmall ColorFromPalette( const RGBPalette32& pal, uint8_t index, uint8_t brightness, TBlendType blendType)
{
    uint8_t hi5 = index;
    hi5 >>= 3;
    uint8_t lo3 = index & 0x07;
    
    CRGBSmall entry = FL_PGM_READ_DWORD_NEAR( &(pal[0]) + hi5);
    
    uint8_t red1   = entry.r;
    uint8_t green1 = entry.g;
    uint8_t blue1  = entry.b;
    
    uint8_t blend = lo3 && (blendType != NOBLEND);
    
    if( blend ) {
        
        if( hi5 == 31 ) {
            entry =   FL_PGM_READ_DWORD_NEAR( &(pal[0]) );
        } else {
            entry =   FL_PGM_READ_DWORD_NEAR( &(pal[1]) + hi5 );
        }
        
        uint8_t f2 = lo3 << 5;
        uint8_t f1 = 255 - f2;
        
        uint8_t red2   = entry.r;
        red1   = scale8( red1,   f1);
        red2   = scale8( red2,   f2);
        red1   += red2;
        
        uint8_t green2 = entry.g;
        green1 = scale8( green1, f1);
        green2 = scale8( green2, f2);
        green1 += green2;
        
        uint8_t blue2  = entry.b;
        blue1  = scale8( blue1,  f1);
        blue2  = scale8( blue2,  f2);
        blue1  += blue2;
    }
    
    if( brightness != 255) {
        if( brightness ) {
            brightness++; // adjust for rounding
            // Now, since brightness is nonzero, we don't need the full scale8_video logic;
            // we can just to scale8 and then add one (unless scale8 fixed) to all nonzero inputs.
            if( red1 )   {
                red1 = scale8( red1, brightness);
                red1++;
            }
            if( green1 ) {
                green1 = scale8( green1, brightness);
                green1++;
            }
            if( blue1 )  {
                blue1 = scale8( blue1, brightness);
                blue1++;
            }
            
        } else {
            red1 = 0;
            green1 = 0;
            blue1 = 0;
        }
    }
    
    return CRGBSmall( red1, green1, blue1);
}



CRGBSmall ColorFromPalette( const CRGBPalette256& pal, uint8_t index, uint8_t brightness, TBlendType)
{
    const CRGBSmall* entry = &(pal[0]) + index;

    uint8_t red   = entry->r;
    uint8_t green = entry->g;
    uint8_t blue  = entry->b;

    if( brightness != 255) {
        brightness++; // adjust for rounding
        red   = scale8_video( red,   brightness);
        green = scale8_video( green, brightness);
        blue  = scale8_video( blue,  brightness);
    }

    return CRGBSmall( red, green, blue);
}


CHSV ColorFromPalette( const struct CHSVPalette16& pal, uint8_t index, uint8_t brightness, TBlendType blendType)
{
    //      hi4 = index >> 4;
    uint8_t hi4 = lsrX4(index);
    uint8_t lo4 = index & 0x0F;

    //  CRGB rgb1 = pal[ hi4];
    const CHSV* entry = &(pal[0]) + hi4;

    uint8_t hue1   = entry->hue;
    uint8_t sat1   = entry->sat;
    uint8_t val1   = entry->val;

    uint8_t blend = lo4 && (blendType != NOBLEND);

    if( blend ) {

        if( hi4 == 15 ) {
            entry = &(pal[0]);
        } else {
            entry++;
        }

        uint8_t f2 = lo4 << 4;
        uint8_t f1 = 255 - f2;

        uint8_t hue2  = entry->hue;
        uint8_t sat2  = entry->sat;
        uint8_t val2  = entry->val;

        // Now some special casing for blending to or from
        // either black or white.  Black and white don't have
        // proper 'hue' of their own, so when ramping from
        // something else to/from black/white, we set the 'hue'
        // of the black/white color to be the same as the hue
        // of the other color, so that you get the expected
        // brightness or saturation ramp, with hue staying
        // constant:

        // If we are starting from white (sat=0)
        // or black (val=0), adopt the target hue.
        if( sat1 == 0 || val1 == 0) {
            hue1 = hue2;
        }

        // If we are ending at white (sat=0)
        // or black (val=0), adopt the starting hue.
        if( sat2 == 0 || val2 == 0) {
            hue2 = hue1;
        }


        sat1  = scale8( sat1, f1);
        val1  = scale8( val1, f1);

        sat2  = scale8( sat2, f2);
        val2  = scale8( val2, f2);

        //    

        // These sums can't overflow, so no qadd8 needed.
        sat1  += sat2;
        val1  += val2;

        uint8_t deltaHue = (uint8_t)(hue2 - hue1);
        if( deltaHue & 0x80 ) {
          // go backwards
          hue1 -= scale8( 256 - deltaHue, f2);
        } else {
          // go forwards
          hue1 += scale8( deltaHue, f2);
        }

        
    }

    if( brightness != 255) {
        val1 = scale8_video( val1, brightness);
    }

    return CHSV( hue1, sat1, val1);
}


CHSV ColorFromPalette( const struct CHSVPalette32& pal, uint8_t index, uint8_t brightness, TBlendType blendType)
{
    uint8_t hi5 = index;
    hi5 >>= 3;
    uint8_t lo3 = index & 0x07;
    
    uint8_t hi5XsizeofCHSV = hi5 * sizeof(CHSV);
    const CHSV* entry = (CHSV*)( (uint8_t*)(&(pal[0])) + hi5XsizeofCHSV);
    
    uint8_t hue1   = entry->hue;
    uint8_t sat1   = entry->sat;
    uint8_t val1   = entry->val;
    
    uint8_t blend = lo3 && (blendType != NOBLEND);
    
    if( blend ) {
        
        if( hi5 == 31 ) {
            entry = &(pal[0]);
        } else {
            entry++;
        }
        
        uint8_t f2 = lo3 << 5;
        uint8_t f1 = 255 - f2;
        
        uint8_t hue2  = entry->hue;
        uint8_t sat2  = entry->sat;
        uint8_t val2  = entry->val;
        
        // Now some special casing for blending to or from
        // either black or white.  Black and white don't have
        // proper 'hue' of their own, so when ramping from
        // something else to/from black/white, we set the 'hue'
        // of the black/white color to be the same as the hue
        // of the other color, so that you get the expected
        // brightness or saturation ramp, with hue staying
        // constant:
        
        // If we are starting from white (sat=0)
        // or black (val=0), adopt the target hue.
        if( sat1 == 0 || val1 == 0) {
            hue1 = hue2;
        }
        
        // If we are ending at white (sat=0)
        // or black (val=0), adopt the starting hue.
        if( sat2 == 0 || val2 == 0) {
            hue2 = hue1;
        }
        
        
        sat1  = scale8( sat1, f1);
        val1  = scale8( val1, f1);
        
        sat2  = scale8( sat2, f2);
        val2  = scale8( val2, f2);
        
        //    
        
        // These sums can't overflow, so no qadd8 needed.
        sat1  += sat2;
        val1  += val2;
        
        uint8_t deltaHue = (uint8_t)(hue2 - hue1);
        if( deltaHue & 0x80 ) {
            // go backwards
            hue1 -= scale8( 256 - deltaHue, f2);
        } else {
            // go forwards
            hue1 += scale8( deltaHue, f2);
        }
        
        
    }
    
    if( brightness != 255) {
        val1 = scale8_video( val1, brightness);
    }
    
    return CHSV( hue1, sat1, val1);
}

CHSV ColorFromPalette( const struct CHSVPalette256& pal, uint8_t index, uint8_t brightness, TBlendType)
{
    CHSV hsv = *( &(pal[0]) + index );

    if( brightness != 255) {
        hsv.value = scale8_video( hsv.value, brightness);
    }

    return hsv;
}


void UpscalePalette(const struct CRGBPalette16& srcpal16, struct CRGBPalette256& destpal256)
{
    for( int i = 0; i < 256; i++) {
        destpal256[(uint8_t)(i)] = ColorFromPalette( srcpal16, i);
    }
}

void UpscalePalette(const struct CHSVPalette16& srcpal16, struct CHSVPalette256& destpal256)
{
    for( int i = 0; i < 256; i++) {
        destpal256[(uint8_t)(i)] = ColorFromPalette( srcpal16, i);
    }
}


void UpscalePalette(const struct CRGBPalette16& srcpal16, struct CRGBPalette32& destpal32)
{
    for( uint8_t i = 0; i < 16; i++) {
        uint8_t j = i * 2;
        destpal32[j+0] = srcpal16[i];
        destpal32[j+1] = srcpal16[i];
    }
}

void UpscalePalette(const struct CHSVPalette16& srcpal16, struct CHSVPalette32& destpal32)
{
    for( uint8_t i = 0; i < 16; i++) {
        uint8_t j = i * 2;
        destpal32[j+0] = srcpal16[i];
        destpal32[j+1] = srcpal16[i];
    }
}

void UpscalePalette(const struct CRGBPalette32& srcpal32, struct CRGBPalette256& destpal256)
{
    for( int i = 0; i < 256; i++) {
        destpal256[(uint8_t)(i)] = ColorFromPalette( srcpal32, i);
    }
}

void UpscalePalette(const struct CHSVPalette32& srcpal32, struct CHSVPalette256& destpal256)
{
    for( int i = 0; i < 256; i++) {
        destpal256[(uint8_t)(i)] = ColorFromPalette( srcpal32, i);
    }
}
