#pragma once
#include <stdint.h>
#include <math.h>
#include "Pixel_Types.h"

typedef enum { FORWARD_HUES, BACKWARD_HUES, SHORTEST_HUES, LONGEST_HUES } GradientDirectionCode_t;

void nscale8( CRGB* leds, uint16_t num_leds, uint8_t scale);
void FadeToBlackBy( CRGB* leds, uint16_t num_leds, uint8_t fadeBy);
void nscale8_video( CRGB* leds, uint16_t num_leds, uint8_t scale);
void FadeLightBy(CRGB* leds, uint16_t num_leds, uint8_t fadeBy);
void FadeWithColor( CRGB* leds, uint16_t numLeds, const CRGB_SMALL& colormask);

void Blend( CRGB& existing, const CRGB_SMALL& overlay, fract8 amountOfOverlay );
void Blend( CRGB& existing, const CRGB& overlay, fract8 amountOfOverlay );
//void Blend( CHSV& existing, const CHSV& overlay, fract8 amountOfOverlay, GradientDirectionCode_t directionCode);

void FillSolid( CRGB * leds, int numToFill, const CRGB& color);
void FillSolid( CRGB * leds, int numToFill, const uint32_t color);
//void FillSolid( struct CHSV * targetArray, int numToFill, const struct CHSV& hsvColor);

void FillRainbow( struct CRGB * pFirstLED, int numToFill, uint8_t initialhue, uint8_t deltahue);
//void FillRainbow( struct CHSV * targetArray, int numToFill, uint8_t initialhue, uint8_t deltahue);

void FillGradientRGB( CRGB* leds, uint16_t startpos, CRGB_SMALL startcolor, uint16_t endpos, CRGB_SMALL endcolor);
void FillGradientRGB( CRGB* leds, uint16_t numLeds, const CRGB_SMALL& c1, const CRGB_SMALL& c2);
void FillGradientRGB( CRGB* leds, uint16_t numLeds, const CRGB_SMALL& c1, const CRGB_SMALL& c2, const CRGB_SMALL& c3);
void FillGradientRGB( CRGB* leds, uint16_t numLeds, const CRGB_SMALL& c1, const CRGB_SMALL& c2, const CRGB_SMALL& c3, const CRGB_SMALL& c4);
