#pragma once
#include "Pixel_Types.h"
#include <string.h>
#include <stdint.h>
#include "Colour_Utilities.h"

#ifdef __cplusplus
extern "C" {
#endif

class IceLED_Segment
{
private:
    uint8_t* _raw = nullptr;
    int _rawSize = 0;

public:
    CRGB* pixels = nullptr;
    uint16_t pixelCount = 0;

    IceLED_Segment(){}
    IceLED_Segment(CRGB* pixelHead, uint16_t numPixels, uint8_t* raw){
        pixels = pixelHead;
        pixelCount = numPixels;
        _raw = raw;
        _rawSize = sizeof(uint8_t) * pixelCount * 3;
    }
    
    uint16_t size() { return pixelCount; }
    
    void ClearLedData() {
        memset((uint8_t*)_raw, 0, _rawSize);
    }

    // Reference to the n'th CRGB pixel
    CRGB &operator[](int x) { return pixels[x % pixelCount]; }

    void FadeToBlackBy(uint8_t fadeBy) {
        nscale8( pixels, pixelCount, 255 - fadeBy);
     }

};



#ifdef __cplusplus
}
#endif
