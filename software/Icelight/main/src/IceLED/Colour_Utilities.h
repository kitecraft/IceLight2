#pragma once
#include <stdint.h>
#include <math.h>
#include "Pixel_Types.h"


void FillSolid( CRGB * leds, int numToFill, const CRGB& color);
void FillSolid( CRGB * leds, int numToFill, const uint32_t color);
//void FillSolid( struct CHSV * targetArray, int numToFill, const struct CHSV& hsvColor);

void FillRainbow( struct CRGB * pFirstLED, int numToFill, uint8_t initialhue, uint8_t deltahue);
//void FillRainbow( struct CHSV * targetArray, int numToFill, uint8_t initialhue, uint8_t deltahue);

void FillGradientRGB( CRGB* leds, uint16_t startpos, CRGB_SMALL startcolor, uint16_t endpos, CRGB_SMALL endcolor);
