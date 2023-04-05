#pragma once
#include "CRGB.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

class IceLED_Segment
{
private:
    CRGB* _pixels = nullptr;
    uint16_t _pixelCount = 0;
    uint8_t* _raw = nullptr;
    int _rawSize = 0;

public:
    IceLED_Segment(){}
    IceLED_Segment(CRGB* pixelHead, uint16_t pixelCount, uint8_t* raw){
        _pixels = pixelHead;
        _pixelCount = pixelCount;
        _raw = raw;
        _rawSize = sizeof(uint8_t) * _pixelCount * 3;
    }
    
    uint16_t size() { return _pixelCount; }
    
    void ClearLedData() {
        memset((uint8_t*)_raw, 0, _rawSize);
    }

    // Reference to the n'th CRGB pixel
    CRGB &operator[](int x) { return _pixels[x % _pixelCount]; }

};



#ifdef __cplusplus
}
#endif
