#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "driver/rmt_tx.h"

#include "IceLED_Channel.h"
#include "Pixel_Types.h"
#include "LED_Types.h"
#include "Pixel_Types.h"
#include "IceLED_Segment.h"
#include "Colour_Helpers.h"
#include "IceLED_Delay.h"
#include "Colour_Utilities.h"

#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

using namespace std;

typedef struct{
    iceled_type_t ledType;
    uint8_t gpio;
    uint16_t numLeds;
} iceled_config_t;

typedef struct{
    CMinWait<55>   wait;
    SemaphoreHandle_t channelActivitySem;
} ChannelCBData_t;

//static const uint8_t gMCU_mW  =  25 * 5; // 25mA @ 5v = 125 mW
static const uint8_t gRed_mW   = 16 * 5; ///< 16mA @ 5v = 80mW
static const uint8_t gGreen_mW = 11 * 5; ///< 11mA @ 5v = 55mW
static const uint8_t gBlue_mW  = 15 * 5; ///< 15mA @ 5v = 75mW
static const uint8_t gDark_mW  =  1 * 5; ///<  1mA @ 5v =  5mW

class IceLED
{
private:
    CRGB* _pixels = nullptr;
    uint16_t _pixelCount;
    uint8_t *_rawPixels = nullptr;
    uint8_t *_rmtPixels = nullptr;
    //used to ensure the channels aren't altered while show() is running
    SemaphoreHandle_t _synchroSem;

    //Up to 8 channnels available
    IceLED_Channel *_channels[8];
    
    //single byte used as a mask for 8 channels
    uint8_t _channelsInUse = 0x00;

    //count of channels actually in use
    size_t _pixelChannelsInUseCount = 0;
    
    //counting sempahore handle. used to lock out 
    //another show() if the rmt channels are still 
    //working on a current show
    ChannelCBData_t _channelCBData;

    iceled_channels_t GetNextAvailableChannel();
    bool IsChannelInUse(iceled_channels_t channel);
    void MarkChannelInUse(iceled_channels_t channel){_channelsInUse |= (1 << channel);}
    void ClearChannelInUse(iceled_channels_t channel){_channelsInUse |= (0 << channel);}


    uint8_t _brightness = 0;
    CRGB_SMALL GetAdjustment(uint8_t scale);
    CRGB_SMALL ComputeAdjustment(uint8_t scale, const CRGB_SMALL & colorCorrection, const CRGB_SMALL & colorTemperature);
    uint8_t ComputeSingleAdjustment(uint8_t scale, uint8_t colorCorrection, uint8_t colorTemperature);

    // sets brightness to
    uint8_t calculate_max_brightness_for_power_mW( uint8_t target_brightness, uint32_t max_power_mW);
    uint32_t calculate_unscaled_power_mW();

    //self explanatory
    CRGB_SMALL _colourCorrection = IceLEDColourCorrection::TypicalSMD5050;
    CRGB_SMALL _colourTemperature = ColourTemperature::UncorrectedTemperature;
    
public:
    IceLED();
    
    bool ConfigureIceLed(iceled_config_t* configArray, uint8_t configArrayCount);
    IceLED_Segment GetSegment(uint16_t startOffset, uint16_t count);
    
    void SetBrightness(uint8_t brightness){_brightness = brightness;}
    uint8_t GetBrightness(){return _brightness;}

    void SetColourTemperature(uint32_t temperature){_colourTemperature = temperature;}

    bool Show(bool wait = false);

};

#ifdef __cplusplus
}
#endif
