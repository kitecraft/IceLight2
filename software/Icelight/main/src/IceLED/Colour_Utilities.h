#pragma once
#include <stdint.h>
#include <math.h>
#include "Pixel_Types.h"

#define constrain(amt,low,high) \
  ((amt) < (low) ? (low) : ( (amt)>(high) ? (high) : (amt) ) )

#define FL_PGM_READ_DWORD_NEAR(x) (*((const uint32_t*)(x)))

typedef enum {
    FORWARD_HUES,
    BACKWARD_HUES,
    SHORTEST_HUES,
    LONGEST_HUES
}TGradientDirectionCode;


void nscale8( CRGB* leds, uint16_t num_leds, uint8_t scale);
void FadeToBlackBy( CRGB* leds, uint16_t num_leds, uint8_t fadeBy);
void nscale8_video( CRGB* leds, uint16_t num_leds, uint8_t scale);
void FadeLightBy(CRGB* leds, uint16_t num_leds, uint8_t fadeBy);
void FadeWithColor( CRGB* leds, uint16_t numLeds, const CRGBSmall& colormask);

void Blend( CRGB& existing, const CRGBSmall& overlay, uint8_t amountOfOverlay );
void Blend( CRGB& existing, const CRGB& overlay, uint8_t amountOfOverlay );
//void Blend( CHSV& existing, const CHSV& overlay, fract8 amountOfOverlay, GradientDirectionCode_t directionCode);


template<class T>
void FillSolid( T * leds, int numToFill, const CRGB& color)
{
    for( int i = 0; i < numToFill; i++) {
        leds[i] = color;
    }
}

template<class T>
void FillSolid( T * leds, int numToFill, const CRGBSmall& color)
{
    for( int i = 0; i < numToFill; i++) {
        leds[i] = color;
    }
}

template<class T>
void FillSolid( T * leds, int numToFill, const uint32_t color)
{
    for( int i = 0; i < numToFill; i++) {
        leds[i] = color;
    }
}

template<class T>
void FillSolid( T * leds, int numToFill, const CHSV& color)
{
    for( int i = 0; i < numToFill; i++) {
        leds[i] = color;
    }
}


template void FillSolid<CRGB>( CRGB*, int, const CRGB&);
template void FillSolid<CRGB>( CRGB*, int, const CRGBSmall&);
template void FillSolid<CRGB>( CRGB*, int, const uint32_t);
template void FillSolid<CRGB>( CRGB*, int, const CHSV&);

//template void FillSolid<CRGBSmall>( CRGBSmall*, int, const CRGB&);
template void FillSolid<CRGBSmall>( CRGBSmall*, int, const CRGBSmall&);
template void FillSolid<CRGBSmall>( CRGBSmall*, int, const uint32_t);
template void FillSolid<CRGBSmall>( CRGBSmall*, int, const CHSV&);

void FillSolid( CHSV* leds, int numToFill, const CHSV color);

void FillRainbow( struct CRGB * pFirstLED, int numToFill, uint8_t initialhue, uint8_t deltahue);
//void FillRainbow( struct CHSV * targetArray, int numToFill, uint8_t initialhue, uint8_t deltahue);

void FillGradientRGB( CRGB* leds, uint16_t startpos, CRGBSmall startcolor, uint16_t endpos, CRGBSmall endcolor);
void FillGradientRGB( CRGB* leds, uint16_t numLeds, const CRGBSmall& c1, const CRGBSmall& c2);
void FillGradientRGB( CRGB* leds, uint16_t numLeds, const CRGBSmall& c1, const CRGBSmall& c2, const CRGBSmall& c3);
void FillGradientRGB( CRGB* leds, uint16_t numLeds, const CRGBSmall& c1, const CRGBSmall& c2, const CRGBSmall& c3, const CRGBSmall& c4);

void FillGradientRGB( CRGBSmall* leds, uint16_t startpos, CRGBSmall startcolor, uint16_t endpos, CRGBSmall endcolor);
void FillGradientRGB( CRGBSmall* leds, uint16_t numLeds, const CRGBSmall& c1, const CRGBSmall& c2);
void FillGradientRGB( CRGBSmall* leds, uint16_t numLeds, const CRGBSmall& c1, const CRGBSmall& c2, const CRGBSmall& c3);
void FillGradientRGB( CRGBSmall* leds, uint16_t numLeds, const CRGBSmall& c1, const CRGBSmall& c2, const CRGBSmall& c3, const CRGBSmall& c4);





#define saccum87 int16_t
/// fill_gradient - fill an array of colors with a smooth HSV gradient
/// between two specified HSV colors.
/// Since 'hue' is a value around a color wheel,
/// there are always two ways to sweep from one hue
/// to another.
/// This function lets you specify which way you want
/// the hue gradient to sweep around the color wheel:
///
///     FORWARD_HUES: hue always goes clockwise
///     BACKWARD_HUES: hue always goes counter-clockwise
///     SHORTEST_HUES: hue goes whichever way is shortest
///     LONGEST_HUES: hue goes whichever way is longest
///
/// The default is SHORTEST_HUES, as this is nearly
/// always what is wanted.
///
/// fill_gradient can write the gradient colors EITHER
///     (1) into an array of CRGBs (e.g., into leds[] array, or an RGB Palette)
///   OR
///     (2) into an array of CHSVs (e.g. an HSV Palette).
///
///   In the case of writing into a CRGB array, the gradient is
///   computed in HSV space, and then HSV values are converted to RGB
///   as they're written into the RGB array.
template <typename T>
void FillGradient( T* targetArray,
                    uint16_t startpos, CHSV startcolor,
                    uint16_t endpos,   CHSV endcolor,
                    TGradientDirectionCode directionCode  = SHORTEST_HUES )
{
    // if the points are in the wrong order, straighten them
    if( endpos < startpos ) {
        uint16_t t = endpos;
        CHSV tc = endcolor;
        endcolor = startcolor;
        endpos = startpos;
        startpos = t;
        startcolor = tc;
    }

    // If we're fading toward black (val=0) or white (sat=0),
    // then set the endhue to the starthue.
    // This lets us ramp smoothly to black or white, regardless
    // of what 'hue' was set in the endcolor (since it doesn't matter)
    if( endcolor.value == 0 || endcolor.saturation == 0) {
        endcolor.hue = startcolor.hue;
    }

    // Similarly, if we're fading in from black (val=0) or white (sat=0)
    // then set the starthue to the endhue.
    // This lets us ramp smoothly up from black or white, regardless
    // of what 'hue' was set in the startcolor (since it doesn't matter)
    if( startcolor.value == 0 || startcolor.saturation == 0) {
        startcolor.hue = endcolor.hue;
    }

    saccum87 huedistance87;
    saccum87 satdistance87;
    saccum87 valdistance87;

    satdistance87 = (endcolor.sat - startcolor.sat) << 7;
    valdistance87 = (endcolor.val - startcolor.val) << 7;

    uint8_t huedelta8 = endcolor.hue - startcolor.hue;

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
        huedistance87 = huedelta8 << 7;
    }
    else /* directionCode == BACKWARD_HUES */
    {
        huedistance87 = (uint8_t)(256 - huedelta8) << 7;
        huedistance87 = -huedistance87;
    }

    uint16_t pixeldistance = endpos - startpos;
    int16_t divisor = pixeldistance ? pixeldistance : 1;

    saccum87 huedelta87 = huedistance87 / divisor;
    saccum87 satdelta87 = satdistance87 / divisor;
    saccum87 valdelta87 = valdistance87 / divisor;

    huedelta87 *= 2;
    satdelta87 *= 2;
    valdelta87 *= 2;

    accum88 hue88 = startcolor.hue << 8;
    accum88 sat88 = startcolor.sat << 8;
    accum88 val88 = startcolor.val << 8;
    for( uint16_t i = startpos; i <= endpos; i++) {
        targetArray[i] = CHSV( hue88 >> 8, sat88 >> 8, val88 >> 8);
        hue88 += huedelta87;
        sat88 += satdelta87;
        val88 += valdelta87;
    }
}


// Convenience functions to fill an array of colors with a
// two-color, three-color, or four-color gradient
template <typename T>
void FillGradient( T* targetArray, uint16_t numLeds, const CHSV& c1, const CHSV& c2,
					TGradientDirectionCode directionCode = SHORTEST_HUES )
{
    uint16_t last = numLeds - 1;
    FillGradient( targetArray, 0, c1, last, c2, directionCode);
}

template <typename T>
void FillGradient( T* targetArray, uint16_t numLeds,
					const CHSV& c1, const CHSV& c2, const CHSV& c3,
					TGradientDirectionCode directionCode = SHORTEST_HUES )
{
    uint16_t half = (numLeds / 2);
    uint16_t last = numLeds - 1;
    FillGradient( targetArray,    0, c1, half, c2, directionCode);
    FillGradient( targetArray, half, c2, last, c3, directionCode);
}

template <typename T>
void FillGradient( T* targetArray, uint16_t numLeds,
					const CHSV& c1, const CHSV& c2, const CHSV& c3, const CHSV& c4,
					TGradientDirectionCode directionCode = SHORTEST_HUES )
{
    uint16_t onethird = (numLeds / 3);
    uint16_t twothirds = ((numLeds * 2) / 3);
    uint16_t last = numLeds - 1;
    FillGradient( targetArray,         0, c1,  onethird, c2, directionCode);
    FillGradient( targetArray,  onethird, c2, twothirds, c3, directionCode);
    FillGradient( targetArray, twothirds, c3,      last, c4, directionCode);
}

// convenience synonym
#define FillGradientHSV FillGradient


// fill_gradient_RGB - fill a range of LEDs with a smooth RGB gradient
//                     between two specified RGB colors.
//                     Unlike HSV, there is no 'color wheel' in RGB space,
//                     and therefore there's only one 'direction' for the
//                     gradient to go, and no 'direction code' is needed.
void FillGradientRGB( CRGB* leds,
                       uint16_t startpos, CRGB startcolor,
                       uint16_t endpos,   CRGB endcolor );
void FillGradientRGB( CRGB* leds, uint16_t numLeds, const CRGB& c1, const CRGB& c2);
void FillGradientRGB( CRGB* leds, uint16_t numLeds, const CRGB& c1, const CRGB& c2, const CRGB& c3);
void FillGradientRGB( CRGB* leds, uint16_t numLeds, const CRGB& c1, const CRGB& c2, const CRGB& c3, const CRGB& c4);


class CRGBPalette16;
class CRGBPalette32;
class CRGBPalette256;
class CHSVPalette16;
class CHSVPalette32;
class CHSVPalette256;
typedef uint32_t RGBPalette16[16];
typedef uint32_t HSVPalette16[16];
#define Palette16 RGBPalette16
typedef uint32_t RGBPalette32[32];
typedef uint32_t HSVPalette32[32];
#define Palette32 RGBPalette32

typedef const uint8_t RGBGradientPalette_byte ;
typedef const RGBGradientPalette_byte *RGBGradientPalette_bytes;
typedef RGBGradientPalette_bytes RGBGradientPalettePtr;
typedef union {
    struct {
        uint8_t index;
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };
    uint32_t dword;
    uint8_t  bytes[4];
} TRGBGradientPaletteEntryUnion;

typedef uint8_t TDynamicRGBGradientPalette_byte ;
typedef const TDynamicRGBGradientPalette_byte *TDynamicRGBGradientPalette_bytes;
typedef TDynamicRGBGradientPalette_bytes TDynamicRGBGradientPalettePtr;

// Convert a 16-entry palette to a 256-entry palette
void UpscalePalette(const struct CRGBPalette16& srcpal16, struct CRGBPalette256& destpal256);
void UpscalePalette(const struct CHSVPalette16& srcpal16, struct CHSVPalette256& destpal256);

// Convert a 16-entry palette to a 32-entry palette
void UpscalePalette(const struct CRGBPalette16& srcpal16, struct CRGBPalette32& destpal32);
void UpscalePalette(const struct CHSVPalette16& srcpal16, struct CHSVPalette32& destpal32);

// Convert a 32-entry palette to a 256-entry palette
void UpscalePalette(const struct CRGBPalette32& srcpal32, struct CRGBPalette256& destpal256);
void UpscalePalette(const struct CHSVPalette32& srcpal32, struct CHSVPalette256& destpal256);


class CHSVPalette16 {
public:
    CHSV entries[16];
    CHSVPalette16() {};
    CHSVPalette16( const CHSV& c00,const CHSV& c01,const CHSV& c02,const CHSV& c03,
                    const CHSV& c04,const CHSV& c05,const CHSV& c06,const CHSV& c07,
                    const CHSV& c08,const CHSV& c09,const CHSV& c10,const CHSV& c11,
                    const CHSV& c12,const CHSV& c13,const CHSV& c14,const CHSV& c15 )
    {
        entries[0]=c00; entries[1]=c01; entries[2]=c02; entries[3]=c03;
        entries[4]=c04; entries[5]=c05; entries[6]=c06; entries[7]=c07;
        entries[8]=c08; entries[9]=c09; entries[10]=c10; entries[11]=c11;
        entries[12]=c12; entries[13]=c13; entries[14]=c14; entries[15]=c15;
    };

    CHSVPalette16( const CHSVPalette16& rhs)
    {
        for (int i=0;i<16;i++) {
            entries[i] = rhs.entries[i];
        }
        // GCC8+ complains
        //memmove8( &(entries[0]), &(rhs.entries[0]), sizeof( entries));
    }
    CHSVPalette16& operator=( const CHSVPalette16& rhs)
    {
        // BB
        for (int i=0;i<16;i++) {
            entries[i] = rhs.entries[i];
        }
        // GCC8+ complains
        //memmove8( &(entries[0]), &(rhs.entries[0]), sizeof( entries));
        return *this;
    }

    CHSVPalette16( const HSVPalette16& rhs)
    {
        for( uint8_t i = 0; i < 16; i++) {
            CRGBSmall xyz   =  FL_PGM_READ_DWORD_NEAR( rhs + i);
            entries[i].hue = xyz.r;
            entries[i].sat = xyz.g;
            entries[i].val = xyz.b;
        }
    }
    CHSVPalette16& operator=( const HSVPalette16& rhs)
    {
        for( uint8_t i = 0; i < 16; i++) {
            CRGBSmall xyz   =  FL_PGM_READ_DWORD_NEAR( rhs + i);
            entries[i].hue = xyz.r;
            entries[i].sat = xyz.g;
            entries[i].val = xyz.b;
        }
        return *this;
    }

    inline CHSV& operator[] (uint8_t x) __attribute__((always_inline))
    {
        return entries[x];
    }
    inline const CHSV& operator[] (uint8_t x) const __attribute__((always_inline))
    {
        return entries[x];
    }

    inline CHSV& operator[] (int x) __attribute__((always_inline))
    {
        return entries[(uint8_t)x];
    }
    inline const CHSV& operator[] (int x) const __attribute__((always_inline))
    {
        return entries[(uint8_t)x];
    }

    operator CHSV*()
    {
        return &(entries[0]);
    }

    bool operator==( const CHSVPalette16 rhs)
    {
        const uint8_t* p = (const uint8_t*)(&(this->entries[0]));
        const uint8_t* q = (const uint8_t*)(&(rhs.entries[0]));
        if( p == q) return true;
        for( uint8_t i = 0; i < (sizeof( entries)); i++) {
            if( *p != *q) return false;
            p++;
            q++;
        }
        return true;
    }
    bool operator!=( const CHSVPalette16 rhs)
    {
        return !( *this == rhs);
    }
    
    CHSVPalette16( const CHSV& c1)
    {
        FillSolid( &(entries[0]), 16, c1);
    }
    CHSVPalette16( const CHSV& c1, const CHSV& c2)
    {
        FillGradient( &(entries[0]), 16, c1, c2);
    }
    CHSVPalette16( const CHSV& c1, const CHSV& c2, const CHSV& c3)
    {
        FillGradient( &(entries[0]), 16, c1, c2, c3);
    }
    CHSVPalette16( const CHSV& c1, const CHSV& c2, const CHSV& c3, const CHSV& c4)
    {
        FillGradient( &(entries[0]), 16, c1, c2, c3, c4);
    }

};

class CHSVPalette256 {
public:
    CHSV entries[256];
    CHSVPalette256() {};
    CHSVPalette256( const CHSV& c00,const CHSV& c01,const CHSV& c02,const CHSV& c03,
                  const CHSV& c04,const CHSV& c05,const CHSV& c06,const CHSV& c07,
                  const CHSV& c08,const CHSV& c09,const CHSV& c10,const CHSV& c11,
                  const CHSV& c12,const CHSV& c13,const CHSV& c14,const CHSV& c15 )
    {
        CHSVPalette16 p16(c00,c01,c02,c03,c04,c05,c06,c07,
                          c08,c09,c10,c11,c12,c13,c14,c15);
        *this = p16;
    };

    CHSVPalette256( const CHSVPalette256& rhs)
    {
        for (int i=0;i<256;i++) {
            entries[i] = rhs.entries[i];
        }
        // GCC8 complains
        //memmove8( &(entries[0]), &(rhs.entries[0]), sizeof( entries));
    }
    CHSVPalette256& operator=( const CHSVPalette256& rhs)
    {
        for (int i=0;i<256;i++) {
            entries[i] = rhs.entries[i];
        }
        // GCC complains
        //memmove8( &(entries[0]), &(rhs.entries[0]), sizeof( entries));
        return *this;
    }

    CHSVPalette256( const CHSVPalette16& rhs16)
    {
        UpscalePalette( rhs16, *this);
    }
    CHSVPalette256& operator=( const CHSVPalette16& rhs16)
    {
        UpscalePalette( rhs16, *this);
        return *this;
    }

    CHSVPalette256( const RGBPalette16& rhs)
    {
        CHSVPalette16 p16(rhs);
        *this = p16;
    }
    CHSVPalette256& operator=( const RGBPalette16& rhs)
    {
        CHSVPalette16 p16(rhs);
        *this = p16;
        return *this;
    }

    inline CHSV& operator[] (uint8_t x) __attribute__((always_inline))
    {
        return entries[x];
    }
    inline const CHSV& operator[] (uint8_t x) const __attribute__((always_inline))
    {
        return entries[x];
    }

    inline CHSV& operator[] (int x) __attribute__((always_inline))
    {
        return entries[(uint8_t)x];
    }
    inline const CHSV& operator[] (int x) const __attribute__((always_inline))
    {
        return entries[(uint8_t)x];
    }

    operator CHSV*()
    {
        return &(entries[0]);
    }

    bool operator==( const CHSVPalette256 rhs)
    {
        const uint8_t* p = (const uint8_t*)(&(this->entries[0]));
        const uint8_t* q = (const uint8_t*)(&(rhs.entries[0]));
        if( p == q) return true;
        for( uint16_t i = 0; i < (sizeof( entries)); i++) {
            if( *p != *q) return false;
            p++;
            q++;
        }
        return true;
    }
    bool operator!=( const CHSVPalette256 rhs)
    {
        return !( *this == rhs);
    }
    
    CHSVPalette256( const CHSV& c1)
    {
      FillSolid( &(entries[0]), 256, c1);
    }
    CHSVPalette256( const CHSV& c1, const CHSV& c2)
    {
        FillGradient( &(entries[0]), 256, c1, c2);
    }
    CHSVPalette256( const CHSV& c1, const CHSV& c2, const CHSV& c3)
    {
        FillGradient( &(entries[0]), 256, c1, c2, c3);
    }
    CHSVPalette256( const CHSV& c1, const CHSV& c2, const CHSV& c3, const CHSV& c4)
    {
        FillGradient( &(entries[0]), 256, c1, c2, c3, c4);
    }
};

class CRGBPalette16 {
public:
    CRGBSmall entries[16];
    CRGBPalette16() {};
    CRGBPalette16( const CRGBSmall& c00,const CRGBSmall& c01,const CRGBSmall& c02,const CRGBSmall& c03,
                    const CRGBSmall& c04,const CRGBSmall& c05,const CRGBSmall& c06,const CRGBSmall& c07,
                    const CRGBSmall& c08,const CRGBSmall& c09,const CRGBSmall& c10,const CRGBSmall& c11,
                    const CRGBSmall& c12,const CRGBSmall& c13,const CRGBSmall& c14,const CRGBSmall& c15 )
    {
        entries[0]=c00; entries[1]=c01; entries[2]=c02; entries[3]=c03;
        entries[4]=c04; entries[5]=c05; entries[6]=c06; entries[7]=c07;
        entries[8]=c08; entries[9]=c09; entries[10]=c10; entries[11]=c11;
        entries[12]=c12; entries[13]=c13; entries[14]=c14; entries[15]=c15;
    };

    CRGBPalette16( const CRGBPalette16& rhs)
    {
        for (int i=0;i<16;i++) {
            entries[i] = rhs.entries[i];
        }
        // GCC complains class-memaccess
        //memmove8( &(entries[0]), &(rhs.entries[0]), sizeof( entries));
    }
    CRGBPalette16( const CRGBSmall rhs[16])
    {
        for (int i=0;i<16;i++) {
            entries[i] = rhs[i];
        }
        //memmove8( &(entries[0]), &(rhs[0]), sizeof( entries));
    }
    CRGBPalette16& operator=( const CRGBPalette16& rhs)
    {
        for (int i=0;i<16;i++) {
            entries[i] = rhs.entries[i];
        }
        //memmove8( &(entries[0]), &(rhs.entries[0]), sizeof( entries));
        return *this;
    }
    CRGBPalette16& operator=( const CRGBSmall rhs[16])
    {
        for (int i=0;i<16;i++) {
            entries[i] = rhs[i];
        }
        // memmove8( &(entries[0]), &(rhs[0]), sizeof( entries));
        return *this;
    }

    CRGBPalette16( const CHSVPalette16& rhs)
    {
        for( uint8_t i = 0; i < 16; i++) {
    		entries[i] = rhs.entries[i]; // implicit HSV-to-RGB conversion
        }
    }
    CRGBPalette16( const CHSV rhs[16])
    {
        for( uint8_t i = 0; i < 16; i++) {
            entries[i] = rhs[i]; // implicit HSV-to-RGB conversion
        }
    }
    CRGBPalette16& operator=( const CHSVPalette16& rhs)
    {
        for( uint8_t i = 0; i < 16; i++) {
    		entries[i] = rhs.entries[i]; // implicit HSV-to-RGB conversion
        }
        return *this;
    }
    CRGBPalette16& operator=( const CHSV rhs[16])
    {
        for( uint8_t i = 0; i < 16; i++) {
            entries[i] = rhs[i]; // implicit HSV-to-RGB conversion
        }
        return *this;
    }

    CRGBPalette16( const RGBPalette16& rhs)
    {
        for( uint8_t i = 0; i < 16; i++) {
            entries[i] =  FL_PGM_READ_DWORD_NEAR( rhs + i);
        }
    }
    CRGBPalette16& operator=( const RGBPalette16& rhs)
    {
        for( uint8_t i = 0; i < 16; i++) {
            entries[i] =  FL_PGM_READ_DWORD_NEAR( rhs + i);
        }
        return *this;
    }

    bool operator==( const CRGBPalette16 rhs)
    {
        const uint8_t* p = (const uint8_t*)(&(this->entries[0]));
        const uint8_t* q = (const uint8_t*)(&(rhs.entries[0]));
        if( p == q) return true;
        for( uint8_t i = 0; i < (sizeof( entries)); i++) {
            if( *p != *q) return false;
            p++;
            q++;
        }
        return true;
    }
    bool operator!=( const CRGBPalette16 rhs)
    {
        return !( *this == rhs);
    }
    
    inline CRGBSmall& operator[] (uint8_t x) __attribute__((always_inline))
    {
        return entries[x];
    }
    inline const CRGBSmall& operator[] (uint8_t x) const __attribute__((always_inline))
    {
        return entries[x];
    }

    inline CRGBSmall& operator[] (int x) __attribute__((always_inline))
    {
        return entries[(uint8_t)x];
    }
    inline const CRGBSmall& operator[] (int x) const __attribute__((always_inline))
    {
        return entries[(uint8_t)x];
    }

    operator CRGBSmall*()
    {
        return &(entries[0]);
    }

    CRGBPalette16( const CHSV& c1)
    {
        FillSolid( &(entries[0]), 16, c1);
    }
    CRGBPalette16( const CHSV& c1, const CHSV& c2)
    {
        FillGradient( &(entries[0]), 16, c1, c2);
    }
    CRGBPalette16( const CHSV& c1, const CHSV& c2, const CHSV& c3)
    {
        FillGradient( &(entries[0]), 16, c1, c2, c3);
    }
    CRGBPalette16( const CHSV& c1, const CHSV& c2, const CHSV& c3, const CHSV& c4)
    {
        FillGradient( &(entries[0]), 16, c1, c2, c3, c4);
    }

    CRGBPalette16( const CRGBSmall& c1)
    {
        FillSolid( &(entries[0]), 16, c1);
    }
    CRGBPalette16( const CRGBSmall& c1, const CRGBSmall& c2)
    {
        FillGradientRGB( &(entries[0]), 16, c1, c2);
    }
    CRGBPalette16( const CRGBSmall& c1, const CRGBSmall& c2, const CRGBSmall& c3)
    {
        FillGradientRGB( &(entries[0]), 16, c1, c2, c3);
    }
    CRGBPalette16( const CRGBSmall& c1, const CRGBSmall& c2, const CRGBSmall& c3, const CRGBSmall& c4)
    {
        FillGradientRGB( &(entries[0]), 16, c1, c2, c3, c4);
    }


    // Gradient palettes are loaded into CRGB16Palettes in such a way
    // that, if possible, every color represented in the gradient palette
    // is also represented in the CRGBPalette16.
    // For example, consider a gradient palette that is all black except
    // for a single, one-element-wide (1/256th!) spike of red in the middle:
    //     0,   0,0,0
    //   124,   0,0,0
    //   125, 255,0,0  // one 1/256th-palette-wide red stripe
    //   126,   0,0,0
    //   255,   0,0,0
    // A naive conversion of this 256-element palette to a 16-element palette
    // might accidentally completely eliminate the red spike, rendering the
    // palette completely black.
    // However, the conversions provided here would attempt to include a
    // the red stripe in the output, more-or-less as faithfully as possible.
    // So in this case, the resulting CRGBPalette16 palette would have a red
    // stripe in the middle which was 1/16th of a palette wide -- the
    // narrowest possible in a CRGBPalette16.
    // This means that the relative width of stripes in a CRGBPalette16
    // will be, by definition, different from the widths in the gradient
    // palette.  This code attempts to preserve "all the colors", rather than
    // the exact stripe widths at the expense of dropping some colors.
    CRGBPalette16( RGBGradientPalette_bytes progpal )
    {
        *this = progpal;
    }
    CRGBPalette16& operator=( RGBGradientPalette_bytes progpal )
    {
        TRGBGradientPaletteEntryUnion* progent = (TRGBGradientPaletteEntryUnion*)(progpal);
        TRGBGradientPaletteEntryUnion u;

        // Count entries
        uint16_t count = 0;
        do {
            u.dword = FL_PGM_READ_DWORD_NEAR(progent + count);
            count++;;
        } while ( u.index != 255);

        int8_t lastSlotUsed = -1;

        u.dword = FL_PGM_READ_DWORD_NEAR( progent);
        CRGBSmall rgbstart( u.r, u.g, u.b);

        int indexstart = 0;
        uint8_t istart8 = 0;
        uint8_t iend8 = 0;
        while( indexstart < 255) {
            progent++;
            u.dword = FL_PGM_READ_DWORD_NEAR( progent);
            int indexend  = u.index;
            CRGBSmall rgbend( u.r, u.g, u.b);
            istart8 = indexstart / 16;
            iend8   = indexend   / 16;
            if( count < 16) {
                if( (istart8 <= lastSlotUsed) && (lastSlotUsed < 15)) {
                    istart8 = lastSlotUsed + 1;
                    if( iend8 < istart8) {
                        iend8 = istart8;
                    }
                }
                lastSlotUsed = iend8;
            }
            FillGradientRGB( &(entries[0]), istart8, rgbstart, iend8, rgbend);
            indexstart = indexend;
            rgbstart = rgbend;
        }
        return *this;
    }
    CRGBPalette16& loadDynamicGradientPalette( TDynamicRGBGradientPalette_bytes gpal )
    {
        TRGBGradientPaletteEntryUnion* ent = (TRGBGradientPaletteEntryUnion*)(gpal);
        TRGBGradientPaletteEntryUnion u;

        // Count entries
        uint16_t count = 0;
        do {
            u = *(ent + count);
            count++;;
        } while ( u.index != 255);

        int8_t lastSlotUsed = -1;


        u = *ent;
        CRGBSmall rgbstart( u.r, u.g, u.b);

        int indexstart = 0;
        uint8_t istart8 = 0;
        uint8_t iend8 = 0;
        while( indexstart < 255) {
            ent++;
            u = *ent;
            int indexend  = u.index;
            CRGBSmall rgbend( u.r, u.g, u.b);
            istart8 = indexstart / 16;
            iend8   = indexend   / 16;
            if( count < 16) {
                if( (istart8 <= lastSlotUsed) && (lastSlotUsed < 15)) {
                    istart8 = lastSlotUsed + 1;
                    if( iend8 < istart8) {
                        iend8 = istart8;
                    }
                }
                lastSlotUsed = iend8;
            }
            FillGradientRGB( &(entries[0]), istart8, rgbstart, iend8, rgbend);
            indexstart = indexend;
            rgbstart = rgbend;
        }
        return *this;
    }

};



class CHSVPalette32 {
public:
    CHSV entries[32];
    CHSVPalette32() {};
    CHSVPalette32( const CHSV& c00,const CHSV& c01,const CHSV& c02,const CHSV& c03,
                  const CHSV& c04,const CHSV& c05,const CHSV& c06,const CHSV& c07,
                  const CHSV& c08,const CHSV& c09,const CHSV& c10,const CHSV& c11,
                  const CHSV& c12,const CHSV& c13,const CHSV& c14,const CHSV& c15 )
    {
        for( uint8_t i = 0; i < 2; i++) {
            entries[0+i]=c00; entries[2+i]=c01; entries[4+i]=c02; entries[6+i]=c03;
            entries[8+i]=c04; entries[10+i]=c05; entries[12+i]=c06; entries[14+i]=c07;
            entries[16+i]=c08; entries[18+i]=c09; entries[20+i]=c10; entries[22+i]=c11;
            entries[24+i]=c12; entries[26+i]=c13; entries[28+i]=c14; entries[30+i]=c15;
        }
    };
    
    CHSVPalette32( const CHSVPalette32& rhs)
    {
        for (int i=0;i<32;i++){
            entries[i] = rhs.entries[i];
        }
        // GCC complains BB
       //memmove8( &(entries[0]), &(rhs.entries[0]), sizeof( entries));
    }
    CHSVPalette32& operator=( const CHSVPalette32& rhs)
    {
        for (int i=0;i<32;i++) {
            entries[i] = rhs.entries[i];
        }
        //memmove8( &(entries[0]), &(rhs.entries[0]), sizeof( entries));
        return *this;
    }
    
    CHSVPalette32( const HSVPalette32& rhs)
    {
        for( uint8_t i = 0; i < 32; i++) {
            CRGBSmall xyz   =  FL_PGM_READ_DWORD_NEAR( rhs + i);
            entries[i].hue = xyz.r;
            entries[i].sat = xyz.g;
            entries[i].val = xyz.b;
        }
    }
    CHSVPalette32& operator=( const HSVPalette32& rhs)
    {
        for( uint8_t i = 0; i < 32; i++) {
            CRGBSmall xyz   =  FL_PGM_READ_DWORD_NEAR( rhs + i);
            entries[i].hue = xyz.r;
            entries[i].sat = xyz.g;
            entries[i].val = xyz.b;
        }
        return *this;
    }
    
    inline CHSV& operator[] (uint8_t x) __attribute__((always_inline))
    {
        return entries[x];
    }
    inline const CHSV& operator[] (uint8_t x) const __attribute__((always_inline))
    {
        return entries[x];
    }
    
    inline CHSV& operator[] (int x) __attribute__((always_inline))
    {
        return entries[(uint8_t)x];
    }
    inline const CHSV& operator[] (int x) const __attribute__((always_inline))
    {
        return entries[(uint8_t)x];
    }
    
    operator CHSV*()
    {
        return &(entries[0]);
    }
    
    bool operator==( const CHSVPalette32 rhs)
    {
        const uint8_t* p = (const uint8_t*)(&(this->entries[0]));
        const uint8_t* q = (const uint8_t*)(&(rhs.entries[0]));
        if( p == q) return true;
        for( uint8_t i = 0; i < (sizeof( entries)); i++) {
            if( *p != *q) return false;
            p++;
            q++;
        }
        return true;
    }
    bool operator!=( const CHSVPalette32 rhs)
    {
        return !( *this == rhs);
    }
    
    CHSVPalette32( const CHSV& c1)
    {
        FillSolid( &(entries[0]), 32, c1);
    }
    CHSVPalette32( const CHSV& c1, const CHSV& c2)
    {
        FillGradient( &(entries[0]), 32, c1, c2);
    }
    CHSVPalette32( const CHSV& c1, const CHSV& c2, const CHSV& c3)
    {
        FillGradient( &(entries[0]), 32, c1, c2, c3);
    }
    CHSVPalette32( const CHSV& c1, const CHSV& c2, const CHSV& c3, const CHSV& c4)
    {
        FillGradient( &(entries[0]), 32, c1, c2, c3, c4);
    }
    
};

class CRGBPalette32 {
public:
    CRGBSmall entries[32];
    CRGBPalette32() {};
    CRGBPalette32( const CRGBSmall& c00,const CRGBSmall& c01,const CRGBSmall& c02,const CRGBSmall& c03,
                  const CRGBSmall& c04,const CRGBSmall& c05,const CRGBSmall& c06,const CRGBSmall& c07,
                  const CRGBSmall& c08,const CRGBSmall& c09,const CRGBSmall& c10,const CRGBSmall& c11,
                  const CRGBSmall& c12,const CRGBSmall& c13,const CRGBSmall& c14,const CRGBSmall& c15 )
    {
        for( uint8_t i = 0; i < 2; i++) {
            entries[0+i]=c00; entries[2+i]=c01; entries[4+i]=c02; entries[6+i]=c03;
            entries[8+i]=c04; entries[10+i]=c05; entries[12+i]=c06; entries[14+i]=c07;
            entries[16+i]=c08; entries[18+i]=c09; entries[20+i]=c10; entries[22+i]=c11;
            entries[24+i]=c12; entries[26+i]=c13; entries[28+i]=c14; entries[30+i]=c15;
        }
    };
    
    CRGBPalette32( const CRGBPalette32& rhs)
    {
        for (int i=0;i<32;i++) {
            entries[i] = rhs.entries[i];
        }
        //memmove8( &(entries[0]), &(rhs.entries[0]), sizeof( entries));
    }
    CRGBPalette32( const CRGBSmall rhs[32])
    {
        for (int i=0;i<32;i++) {
            entries[i] = rhs[i];
        }
        //memmove8( &(entries[0]), &(rhs[0]), sizeof( entries));
    }
    CRGBPalette32& operator=( const CRGBPalette32& rhs)
    {
        for (int i=0;i<32;i++) {
            entries[i] = rhs.entries[i];
        }
        //memmove8( &(entries[0]), &(rhs.entries[0]), sizeof( entries));
        return *this;
    }
    CRGBPalette32& operator=( const CRGBSmall rhs[32])
    {   
        for (int i=0;i<32;i++) {
            entries[i] = rhs[i];
        }
        //memmove8( &(entries[0]), &(rhs[0]), sizeof( entries));
        return *this;
    }
    
    CRGBPalette32( const CHSVPalette32& rhs)
    {
        for( uint8_t i = 0; i < 32; i++) {
            entries[i] = rhs.entries[i]; // implicit HSV-to-RGB conversion
        }
    }
    CRGBPalette32( const CHSV rhs[32])
    {
        for( uint8_t i = 0; i < 32; i++) {
            entries[i] = rhs[i]; // implicit HSV-to-RGB conversion
        }
    }
    CRGBPalette32& operator=( const CHSVPalette32& rhs)
    {
        for( uint8_t i = 0; i < 32; i++) {
            entries[i] = rhs.entries[i]; // implicit HSV-to-RGB conversion
        }
        return *this;
    }
    CRGBPalette32& operator=( const CHSV rhs[32])
    {
        for( uint8_t i = 0; i < 32; i++) {
            entries[i] = rhs[i]; // implicit HSV-to-RGB conversion
        }
        return *this;
    }
    
    CRGBPalette32( const RGBPalette32& rhs)
    {
        for( uint8_t i = 0; i < 32; i++) {
            entries[i] =  FL_PGM_READ_DWORD_NEAR( rhs + i);
        }
    }
    CRGBPalette32& operator=( const RGBPalette32& rhs)
    {
        for( uint8_t i = 0; i < 32; i++) {
            entries[i] =  FL_PGM_READ_DWORD_NEAR( rhs + i);
        }
        return *this;
    }
    
    bool operator==( const CRGBPalette32 rhs)
    {
        const uint8_t* p = (const uint8_t*)(&(this->entries[0]));
        const uint8_t* q = (const uint8_t*)(&(rhs.entries[0]));
        if( p == q) return true;
        for( uint8_t i = 0; i < (sizeof( entries)); i++) {
            if( *p != *q) return false;
            p++;
            q++;
        }
        return true;
    }
    bool operator!=( const CRGBPalette32 rhs)
    {
        return !( *this == rhs);
    }
    
    inline CRGBSmall& operator[] (uint8_t x) __attribute__((always_inline))
    {
        return entries[x];
    }
    inline const CRGBSmall& operator[] (uint8_t x) const __attribute__((always_inline))
    {
        return entries[x];
    }
    
    inline CRGBSmall& operator[] (int x) __attribute__((always_inline))
    {
        return entries[(uint8_t)x];
    }
    inline const CRGBSmall& operator[] (int x) const __attribute__((always_inline))
    {
        return entries[(uint8_t)x];
    }
    
    operator CRGBSmall*()
    {
        return &(entries[0]);
    }
    
    CRGBPalette32( const CHSV& c1)
    {
        FillSolid( &(entries[0]), 32, c1);
    }
    CRGBPalette32( const CHSV& c1, const CHSV& c2)
    {
        FillGradient( &(entries[0]), 32, c1, c2);
    }
    CRGBPalette32( const CHSV& c1, const CHSV& c2, const CHSV& c3)
    {
        FillGradient( &(entries[0]), 32, c1, c2, c3);
    }
    CRGBPalette32( const CHSV& c1, const CHSV& c2, const CHSV& c3, const CHSV& c4)
    {
        FillGradient( &(entries[0]), 32, c1, c2, c3, c4);
    }
    
    CRGBPalette32( const CRGBSmall& c1)
    {
        FillSolid( &(entries[0]), 32, c1);
    }
    CRGBPalette32( const CRGBSmall& c1, const CRGBSmall& c2)
    {
        FillGradientRGB( &(entries[0]), 32, c1, c2);
    }
    CRGBPalette32( const CRGBSmall& c1, const CRGBSmall& c2, const CRGBSmall& c3)
    {
        FillGradientRGB( &(entries[0]), 32, c1, c2, c3);
    }
    CRGBPalette32( const CRGBSmall& c1, const CRGBSmall& c2, const CRGBSmall& c3, const CRGBSmall& c4)
    {
        FillGradientRGB( &(entries[0]), 32, c1, c2, c3, c4);
    }
    
    
    CRGBPalette32( const CRGBPalette16& rhs16)
    {
        UpscalePalette( rhs16, *this);
    }
    CRGBPalette32& operator=( const CRGBPalette16& rhs16)
    {
        UpscalePalette( rhs16, *this);
        return *this;
    }
    
    CRGBPalette32( const RGBPalette16& rhs)
    {
        CRGBPalette16 p16(rhs);
        *this = p16;
    }
    CRGBPalette32& operator=( const RGBPalette16& rhs)
    {
        CRGBPalette16 p16(rhs);
        *this = p16;
        return *this;
    }
    
    
    // Gradient palettes are loaded into CRGB16Palettes in such a way
    // that, if possible, every color represented in the gradient palette
    // is also represented in the CRGBPalette32.
    // For example, consider a gradient palette that is all black except
    // for a single, one-element-wide (1/256th!) spike of red in the middle:
    //     0,   0,0,0
    //   124,   0,0,0
    //   125, 255,0,0  // one 1/256th-palette-wide red stripe
    //   126,   0,0,0
    //   255,   0,0,0
    // A naive conversion of this 256-element palette to a 16-element palette
    // might accidentally completely eliminate the red spike, rendering the
    // palette completely black.
    // However, the conversions provided here would attempt to include a
    // the red stripe in the output, more-or-less as faithfully as possible.
    // So in this case, the resulting CRGBPalette32 palette would have a red
    // stripe in the middle which was 1/16th of a palette wide -- the
    // narrowest possible in a CRGBPalette32.
    // This means that the relative width of stripes in a CRGBPalette32
    // will be, by definition, different from the widths in the gradient
    // palette.  This code attempts to preserve "all the colors", rather than
    // the exact stripe widths at the expense of dropping some colors.
    CRGBPalette32( RGBGradientPalette_bytes progpal )
    {
        *this = progpal;
    }
    CRGBPalette32& operator=( RGBGradientPalette_bytes progpal )
    {
        TRGBGradientPaletteEntryUnion* progent = (TRGBGradientPaletteEntryUnion*)(progpal);
        TRGBGradientPaletteEntryUnion u;
        
        // Count entries
        uint16_t count = 0;
        do {
            u.dword = FL_PGM_READ_DWORD_NEAR(progent + count);
            count++;;
        } while ( u.index != 255);
        
        int8_t lastSlotUsed = -1;
        
        u.dword = FL_PGM_READ_DWORD_NEAR( progent);
        CRGBSmall rgbstart( u.r, u.g, u.b);
        
        int indexstart = 0;
        uint8_t istart8 = 0;
        uint8_t iend8 = 0;
        while( indexstart < 255) {
            progent++;
            u.dword = FL_PGM_READ_DWORD_NEAR( progent);
            int indexend  = u.index;
            CRGBSmall rgbend( u.r, u.g, u.b);
            istart8 = indexstart / 8;
            iend8   = indexend   / 8;
            if( count < 16) {
                if( (istart8 <= lastSlotUsed) && (lastSlotUsed < 31)) {
                    istart8 = lastSlotUsed + 1;
                    if( iend8 < istart8) {
                        iend8 = istart8;
                    }
                }
                lastSlotUsed = iend8;
            }
            FillGradientRGB( &(entries[0]), istart8, rgbstart, iend8, rgbend);
            indexstart = indexend;
            rgbstart = rgbend;
        }
        return *this;
    }
    CRGBPalette32& loadDynamicGradientPalette( TDynamicRGBGradientPalette_bytes gpal )
    {
        TRGBGradientPaletteEntryUnion* ent = (TRGBGradientPaletteEntryUnion*)(gpal);
        TRGBGradientPaletteEntryUnion u;
        
        // Count entries
        uint16_t count = 0;
        do {
            u = *(ent + count);
            count++;;
        } while ( u.index != 255);
        
        int8_t lastSlotUsed = -1;
        
        
        u = *ent;
        CRGBSmall rgbstart( u.r, u.g, u.b);
        
        int indexstart = 0;
        uint8_t istart8 = 0;
        uint8_t iend8 = 0;
        while( indexstart < 255) {
            ent++;
            u = *ent;
            int indexend  = u.index;
            CRGBSmall rgbend( u.r, u.g, u.b);
            istart8 = indexstart / 8;
            iend8   = indexend   / 8;
            if( count < 16) {
                if( (istart8 <= lastSlotUsed) && (lastSlotUsed < 31)) {
                    istart8 = lastSlotUsed + 1;
                    if( iend8 < istart8) {
                        iend8 = istart8;
                    }
                }
                lastSlotUsed = iend8;
            }
            FillGradientRGB( &(entries[0]), istart8, rgbstart, iend8, rgbend);
            indexstart = indexend;
            rgbstart = rgbend;
        }
        return *this;
    }
    
};



class CRGBPalette256 {
public:
    CRGBSmall entries[256];
    CRGBPalette256() {};
    CRGBPalette256( const CRGBSmall& c00,const CRGBSmall& c01,const CRGBSmall& c02,const CRGBSmall& c03,
                  const CRGBSmall& c04,const CRGBSmall& c05,const CRGBSmall& c06,const CRGBSmall& c07,
                  const CRGBSmall& c08,const CRGBSmall& c09,const CRGBSmall& c10,const CRGBSmall& c11,
                  const CRGBSmall& c12,const CRGBSmall& c13,const CRGBSmall& c14,const CRGBSmall& c15 )
    {
        CRGBPalette16 p16(c00,c01,c02,c03,c04,c05,c06,c07,
                          c08,c09,c10,c11,c12,c13,c14,c15);
        *this = p16;
    };

    CRGBPalette256( const CRGBPalette256& rhs)
    {
        for (int i=0;i<256;i++) {
            entries[i] = rhs.entries[i];
        }
        //memmove8( &(entries[0]), &(rhs.entries[0]), sizeof( entries));
    }
    CRGBPalette256( const CRGBSmall rhs[256])
    {
        for (int i=0;i<256;i++) {
            entries[i] = rhs[i];
        }
        //memmove8( &(entries[0]), &(rhs[0]), sizeof( entries));
    }
    CRGBPalette256& operator=( const CRGBPalette256& rhs)
    {
        for (int i=0;i<256;i++) {
            entries[i] = rhs.entries[i];
        }
        //memmove8( &(entries[0]), &(rhs.entries[0]), sizeof( entries));
        return *this;
    }
    CRGBPalette256& operator=( const CRGBSmall rhs[256])
    {
        for (int i=0;i<256;i++) {
            entries[i] = rhs[i];
        }
        //memmove8( &(entries[0]), &(rhs[0]), sizeof( entries));
        return *this;
    }

    CRGBPalette256( const CHSVPalette256& rhs)
    {
    	for( int i = 0; i < 256; i++) {
	    	entries[i] = rhs.entries[i]; // implicit HSV-to-RGB conversion
    	}
    }
    CRGBPalette256( const CHSV rhs[256])
    {
        for( int i = 0; i < 256; i++) {
            entries[i] = rhs[i]; // implicit HSV-to-RGB conversion
        }
    }
    CRGBPalette256& operator=( const CHSVPalette256& rhs)
    {
    	for( int i = 0; i < 256; i++) {
	    	entries[i] = rhs.entries[i]; // implicit HSV-to-RGB conversion
    	}
        return *this;
    }
    CRGBPalette256& operator=( const CHSV rhs[256])
    {
        for( int i = 0; i < 256; i++) {
            entries[i] = rhs[i]; // implicit HSV-to-RGB conversion
        }
        return *this;
    }

    CRGBPalette256( const CRGBPalette16& rhs16)
    {
        UpscalePalette( rhs16, *this);
    }
    CRGBPalette256& operator=( const CRGBPalette16& rhs16)
    {
        UpscalePalette( rhs16, *this);
        return *this;
    }

    CRGBPalette256( const RGBPalette16& rhs)
    {
        CRGBPalette16 p16(rhs);
        *this = p16;
    }
    CRGBPalette256& operator=( const RGBPalette16& rhs)
    {
        CRGBPalette16 p16(rhs);
        *this = p16;
        return *this;
    }

    bool operator==( const CRGBPalette256 rhs)
    {
        const uint8_t* p = (const uint8_t*)(&(this->entries[0]));
        const uint8_t* q = (const uint8_t*)(&(rhs.entries[0]));
        if( p == q) return true;
        for( uint16_t i = 0; i < (sizeof( entries)); i++) {
            if( *p != *q) return false;
            p++;
            q++;
        }
        return true;
    }
    bool operator!=( const CRGBPalette256 rhs)
    {
        return !( *this == rhs);
    }
    
    inline CRGBSmall& operator[] (uint8_t x) __attribute__((always_inline))
    {
        return entries[x];
    }
    inline const CRGBSmall& operator[] (uint8_t x) const __attribute__((always_inline))
    {
        return entries[x];
    }

    inline CRGBSmall& operator[] (int x) __attribute__((always_inline))
    {
        return entries[(uint8_t)x];
    }
    inline const CRGBSmall& operator[] (int x) const __attribute__((always_inline))
    {
        return entries[(uint8_t)x];
    }

    operator CRGBSmall*()
    {
        return &(entries[0]);
    }

    CRGBPalette256( const CHSV& c1)
    {
        FillSolid( &(entries[0]), 256, c1);
    }
    CRGBPalette256( const CHSV& c1, const CHSV& c2)
    {
        FillGradient( &(entries[0]), 256, c1, c2);
    }
    CRGBPalette256( const CHSV& c1, const CHSV& c2, const CHSV& c3)
    {
        FillGradient( &(entries[0]), 256, c1, c2, c3);
    }
    CRGBPalette256( const CHSV& c1, const CHSV& c2, const CHSV& c3, const CHSV& c4)
    {
        FillGradient( &(entries[0]), 256, c1, c2, c3, c4);
    }

    CRGBPalette256( const CRGBSmall& c1)
    {
        FillSolid( &(entries[0]), 256, c1);
    }
    CRGBPalette256( const CRGBSmall& c1, const CRGBSmall& c2)
    {
        FillGradientRGB( &(entries[0]), 256, c1, c2);
    }
    CRGBPalette256( const CRGBSmall& c1, const CRGBSmall& c2, const CRGBSmall& c3)
    {
        FillGradientRGB( &(entries[0]), 256, c1, c2, c3);
    }
    CRGBPalette256( const CRGBSmall& c1, const CRGBSmall& c2, const CRGBSmall& c3, const CRGBSmall& c4)
    {
        FillGradientRGB( &(entries[0]), 256, c1, c2, c3, c4);
    }

    CRGBPalette256( RGBGradientPalette_bytes progpal )
    {
        *this = progpal;
    }
    CRGBPalette256& operator=( RGBGradientPalette_bytes progpal )
    {
        TRGBGradientPaletteEntryUnion* progent = (TRGBGradientPaletteEntryUnion*)(progpal);
        TRGBGradientPaletteEntryUnion u;
        u.dword = FL_PGM_READ_DWORD_NEAR( progent);
        CRGBSmall rgbstart( u.r, u.g, u.b);

        int indexstart = 0;
        while( indexstart < 255) {
            progent++;
            u.dword = FL_PGM_READ_DWORD_NEAR( progent);
            int indexend  = u.index;
            CRGBSmall rgbend( u.r, u.g, u.b);
            FillGradientRGB( &(entries[0]), indexstart, rgbstart, indexend, rgbend);
            indexstart = indexend;
            rgbstart = rgbend;
        }
        return *this;
    }
    CRGBPalette256& loadDynamicGradientPalette( TDynamicRGBGradientPalette_bytes gpal )
    {
        TRGBGradientPaletteEntryUnion* ent = (TRGBGradientPaletteEntryUnion*)(gpal);
        TRGBGradientPaletteEntryUnion u;
        u = *ent;
        CRGBSmall rgbstart( u.r, u.g, u.b);

        int indexstart = 0;
        while( indexstart < 255) {
            ent++;
            u = *ent;
            int indexend  = u.index;
            CRGBSmall rgbend( u.r, u.g, u.b);
            FillGradientRGB( &(entries[0]), indexstart, rgbstart, indexend, rgbend);
            indexstart = indexend;
            rgbstart = rgbend;
        }
        return *this;
    }
};



typedef enum { NOBLEND=0, LINEARBLEND=1 } TBlendType;

CRGBSmall ColorFromPalette( const CRGBPalette16& pal,
                      uint8_t index,
                      uint8_t brightness=255,
                      TBlendType blendType=LINEARBLEND);

CRGBSmall ColorFromPalette( const RGBPalette16& pal,
                       uint8_t index,
                       uint8_t brightness=255,
                       TBlendType blendType=LINEARBLEND);

CRGBSmall ColorFromPalette( const CRGBPalette256& pal,
                       uint8_t index,
                       uint8_t brightness=255,
                       TBlendType blendType=NOBLEND );

CHSV ColorFromPalette( const CHSVPalette16& pal,
                       uint8_t index,
                       uint8_t brightness=255,
                       TBlendType blendType=LINEARBLEND);

CHSV ColorFromPalette( const CHSVPalette256& pal,
                       uint8_t index,
                       uint8_t brightness=255,
                       TBlendType blendType=NOBLEND );

CRGBSmall ColorFromPalette( const CRGBPalette32& pal,
                      uint8_t index,
                      uint8_t brightness=255,
                      TBlendType blendType=LINEARBLEND);

CRGBSmall ColorFromPalette( const RGBPalette32& pal,
                      uint8_t index,
                      uint8_t brightness=255,
                      TBlendType blendType=LINEARBLEND);

CHSV ColorFromPalette( const CHSVPalette32& pal,
                      uint8_t index,
                      uint8_t brightness=255,
                      TBlendType blendType=LINEARBLEND);


// Fill a range of LEDs with a sequece of entryies from a palette
template <typename PALETTE>
void fill_palette(CRGB* L, uint16_t N, uint8_t startIndex, uint8_t incIndex,
                  const PALETTE& pal, uint8_t brightness, TBlendType blendType)
{
    uint8_t colorIndex = startIndex;
    for( uint16_t i = 0; i < N; i++) {
        L[i] = ColorFromPalette( pal, colorIndex, brightness, blendType);
        colorIndex += incIndex;
    }
}

template <typename PALETTE>
void map_data_into_colors_through_palette(
	uint8_t *dataArray, uint16_t dataCount,
	CRGBSmall* targetColorArray,
	const PALETTE& pal,
	uint8_t brightness=255,
	uint8_t opacity=255,
	TBlendType blendType=LINEARBLEND)
{
	for( uint16_t i = 0; i < dataCount; i++) {
		uint8_t d = dataArray[i];
		CRGBSmall rgb = ColorFromPalette( pal, d, brightness, blendType);
		if( opacity == 255 ) {
			targetColorArray[i] = rgb;
		} else {
			targetColorArray[i].nscale8( 256 - opacity);
			rgb.nscale8_video( opacity);
			targetColorArray[i] += rgb;
		}
	}
}

// nblendPaletteTowardPalette:
//               Alter one palette by making it slightly more like
//               a 'target palette', used for palette cross-fades.
//
//               It does this by comparing each of the R, G, and B channels
//               of each entry in the current palette to the corresponding
//               entry in the target palette and making small adjustments:
//                 If the Red channel is too low, it will be increased.
//                 If the Red channel is too high, it will be slightly reduced.
//                 ... and likewise for Green and Blue channels.
//
//               Additionally, there are two significant visual improvements
//               to this algorithm implemented here.  First is this:
//                 When increasing a channel, it is stepped up by ONE.
//                 When decreasing a channel, it is stepped down by TWO.
//               Due to the way the eye perceives light, and the way colors
//               are represented in RGB, this produces a more uniform apparent
//               brightness when cross-fading between most palette colors.
//
//               The second visual tweak is limiting the number of changes
//               that will be made to the palette at once.  If all the palette
//               entries are changed at once, it can give a muddled appearance.
//               However, if only a few palette entries are changed at once,
//               you get a visually smoother transition: in the middle of the
//               cross-fade your current palette will actually contain some
//               colors from the old palette, a few blended colors, and some
//               colors from the new palette.
//               The maximum number of possible palette changes per call
//               is 48 (sixteen color entries time three channels each).
//               The default 'maximim number of changes' here is 12, meaning
//               that only approximately a quarter of the palette entries
//               will be changed per call.
void nblendPaletteTowardPalette( CRGBPalette16& currentPalette,
                                CRGBPalette16& targetPalette,
                                uint8_t maxChanges=24);



#define DEFINE_GRADIENT_PALETTE(X) extern const RGBGradientPalette_byte X[]  =

/// Forward-declaration macro for DEFINE_GRADIENT_PALETTE(X)
#define DECLARE_GRADIENT_PALETTE(X) extern const RGBGradientPalette_byte X[] 