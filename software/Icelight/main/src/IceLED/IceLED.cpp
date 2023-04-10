#include "IceLED.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "lib8tion.h"

#define POWER_DEBUG_PRINT 1


IRAM_ATTR static bool channelEndCallback(rmt_channel_handle_t channel, const rmt_tx_done_event_data_t *edata, void *user_data)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    ChannelCBData_t* cbData = (ChannelCBData_t*) user_data;
    xSemaphoreGiveFromISR(cbData->channelActivitySem, &xHigherPriorityTaskWoken);
    cbData->wait.mark();
    return true;
}

static const char* ICELED_TAG = "IceLED";
IceLED::IceLED()
{
    _synchroSem = xSemaphoreCreateBinary();
    xSemaphoreGive( _synchroSem );
}

iceled_channels_t IceLED::GetNextAvailableChannel()
{
    for(int i = 0; i < ILS_CHAN_MAX; i++){
        if(!IsChannelInUse(static_cast<iceled_channels_t>(i))){
            return static_cast<iceled_channels_t>(i);
        }
    }
    return ILS_CHAN_NO_CHAN;
}

bool IceLED::IsChannelInUse(iceled_channels_t channel)
{
    return ((_channelsInUse & (1 << channel )) == (1 << channel)) ? true : false;
}

bool IceLED::ConfigureIceLed(iceled_config_t* configArray, uint8_t configArrayCount)
{
    //not sure how long it might take perform a full Show
    if( xSemaphoreTake( _synchroSem, ( TickType_t ) 10 ) != pdTRUE )
    {
        ESP_LOGE(ICELED_TAG, "Faled to take semaphore");
        return false;
    }

    /*
        clean up and remove any/all existing stuff first
    */
   _pixelChannelsInUseCount = 0;



    if(configArrayCount > ICELED_MAX_CHANNELS){
        configArrayCount = ICELED_MAX_CHANNELS;
    }

    uint16_t totalLedCount = 0;
    for(int i = 0; i < configArrayCount; i++){
        iceled_channels_t availChannel = GetNextAvailableChannel();
        if(availChannel == ILS_CHAN_NO_CHAN){
            //no available channel, so don't add it
            ESP_LOGE(ICELED_TAG, "No channel available");
            break;
        }
        
        iceled_channel_config_t channelConfig{
            .gpio = configArray[i].gpio,
            .channel = availChannel,
            .count = configArray[i].numLeds,
            .pixelType = configArray[i].ledType,
        };

        IceLED_Channel* channel = CreateIceLED_Channel(channelConfig);
        if(channel != nullptr){
            _pixelChannelsInUseCount++;
            channel->offset = totalLedCount;
            
            _channels[i] = channel;
            totalLedCount += channel->count;
            MarkChannelInUse(static_cast<iceled_channels_t>(i));
        ESP_LOGI(ICELED_TAG, "Total count is: %i",totalLedCount);
        } else {
            ESP_LOGI(ICELED_TAG, "Got a nullptr");
        }
    }

    _pixelCount = totalLedCount;
    ESP_LOGI(ICELED_TAG, "Pixel count is: %i, %i",_pixelCount, totalLedCount);
    _pixels =  (CRGB*)heap_caps_calloc(_pixelCount, sizeof(CRGB), MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
    
    //_rawPixels holds the actual G, R, and B uint8_t variables behind the CRGB pixels
    _rawPixels =  (uint8_t*)heap_caps_calloc(_pixelCount*3, sizeof(uint8_t), MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);

    //_rmtPixels is the buffers used by the rmt channel encoders. 
    _rmtPixels =  (uint8_t*)heap_caps_calloc(_pixelCount*3, sizeof(uint8_t), MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);

    int rawPixelCount = 0;
    for(int i=0; i< _pixelCount; i++){
        _pixels[i].g = &_rawPixels[rawPixelCount++];
        _pixels[i].r = &_rawPixels[rawPixelCount++];
        _pixels[i].b = &_rawPixels[rawPixelCount++];
    }

    _channelCBData.channelActivitySem = xSemaphoreCreateCounting(_pixelChannelsInUseCount, _pixelChannelsInUseCount);
    if( _channelCBData.channelActivitySem == NULL )
    {
        ESP_LOGE(ICELED_TAG, "Failed to create _channelActivitySem");
    }

    int runningOffset = 0;
    for(int i = 0; i < _pixelChannelsInUseCount; i++){
        _channels[i]->_pixelData = &_rmtPixels[runningOffset];
        runningOffset += _channels[i]->count*3;

        rmt_tx_event_callbacks_t cbs = {
            .on_trans_done = channelEndCallback,
        };
        if(rmt_tx_register_event_callbacks(_channels[i]->rmt_channel, &cbs, &_channelCBData) != ESP_OK){
            ESP_LOGE(ICELED_TAG, "Failed to register the callback");
        }
    }
    

    xSemaphoreGive( _synchroSem );
    ESP_LOGI(ICELED_TAG, "ConfigureIceLed success");
    return true;
}

IceLED_Segment IceLED::GetSegment(uint16_t startOffset, uint16_t count)
{
    if(startOffset > _pixelCount || startOffset + count > _pixelCount){
        ESP_LOGE(ICELED_TAG, "Failed to get segment: %i %i %i", startOffset, count, _pixelCount);
        return IceLED_Segment();
    }
    IceLED_Segment newSegment(&_pixels[startOffset], count, &_rawPixels[startOffset*3]);
    return newSegment;
}

/*
Starts the process of updating the physical pixels.

*/
bool IceLED::Show(bool wait)
{
    if( xSemaphoreTake( _synchroSem, ( TickType_t ) 10 ) != pdTRUE )
    {
        ESP_LOGE(ICELED_TAG, "Show:  failed to get sempahore");
        return false;
    }

    if(uxSemaphoreGetCount(_channelCBData.channelActivitySem) == 0){
        xSemaphoreGive( _synchroSem );
        return false;
    }
    
    memcpy(_rmtPixels, _rawPixels, sizeof(uint8_t)*_pixelCount*3);

    //NOT gonna use the max power stuff for now.  It's wayyyyy wrong
    //uint8_t bScale = calculate_max_brightness_for_power_mW(_brightness, 28000);
    
    CRGBSmall adjustment = GetAdjustment(_brightness);
    int count = _pixelCount*3;
    while(count){
        count--;
        _rmtPixels[count] = scale8(_rmtPixels[count], adjustment.b);
        count--;
        _rmtPixels[count] = scale8(_rmtPixels[count], adjustment.r);
        count--;
        _rmtPixels[count] = scale8(_rmtPixels[count], adjustment.g);
    };   

    rmt_transmit_config_t tx_config = {.loop_count = 0,};
    _channelCBData.wait.wait();
    
    for(int i = 0; i < 8; i++){
        if(IsChannelInUse(static_cast<iceled_channels_t>(i))){
            xSemaphoreTake(_channelCBData.channelActivitySem, portMAX_DELAY );
            rmt_transmit(_channels[i]->rmt_channel, _channels[i]->led_encoder, _channels[i]->_pixelData, _channels[i]->count*3, &tx_config);
        }
    }
/*
    if(wait){
        for(int i = 0; i < 8; i++){
            if(IsChannelInUse(static_cast<iceled_channels_t>(i))){
                rmt_tx_wait_all_done(_channels[i]->rmt_channel, -1);
            }
        }
    }
*/
    xSemaphoreGive( _synchroSem );
    return true;
}


/// Get the combined brightness/color adjustment for this controller
CRGBSmall IceLED::GetAdjustment(uint8_t scale)
{
    return ComputeAdjustment(scale, _colourCorrection, _colourTemperature);
}

CRGBSmall IceLED::ComputeAdjustment(uint8_t scale, const CRGBSmall & colorCorrection, const CRGBSmall & colourTemperature)
{
    CRGBSmall adj(0,0,0);
    if(scale > 0){
        adj.r = ComputeSingleAdjustment(scale, colorCorrection.r, colourTemperature.r);
        adj.g = ComputeSingleAdjustment(scale, colorCorrection.g, colourTemperature.b);
        adj.b = ComputeSingleAdjustment(scale, colorCorrection.b, colourTemperature.b);
    }
    return adj;
}

uint8_t IceLED::ComputeSingleAdjustment(uint8_t scale, uint8_t colorCorrection, uint8_t colorTemperature)
{
    if(colorCorrection > 0 && colorTemperature > 0) {
        uint32_t work = (((uint32_t)colorCorrection)+1) * (((uint32_t)colorTemperature)+1) * scale;
        work /= 0x10000L;
        return (work & 0xFF);
    }
    return 0;
}

uint8_t IceLED::calculate_max_brightness_for_power_mW( uint8_t target_brightness, uint32_t max_power_mW)
{
    uint32_t total_mW = gMCU_mW;
    total_mW += calculate_unscaled_power_mW();

    //uint32_t total_mW = calculate_unscaled_power_mW();
#if POWER_DEBUG_PRINT == 1
    printf("\n----------------\npower demand at full brightness mW = %li\n", total_mW);
#endif

    uint32_t requested_power_mW = ((uint32_t)total_mW * target_brightness) / 256;
#if POWER_DEBUG_PRINT == 1
    if( target_brightness != 255 ) {
        printf("power demand at scaled brightness mW = %li\n", requested_power_mW);
    }
    printf("power limit mW = %li\n", max_power_mW);
#endif

    if( requested_power_mW < max_power_mW) {
#if POWER_LED > 0
        if( gMaxPowerIndicatorLEDPinNumber ) {
            Pin(gMaxPowerIndicatorLEDPinNumber).lo(); // turn the LED off
        }
#endif
#if POWER_DEBUG_PRINT == 1
        printf("demand is under the limit\n");
#endif
        return target_brightness;
    }

    uint8_t recommended_brightness = (uint32_t)((uint8_t)(target_brightness) * (uint32_t)(max_power_mW)) / ((uint32_t)(requested_power_mW));
#if POWER_DEBUG_PRINT == 1
    printf("recommended brightness # =  %i\n", recommended_brightness);

    uint32_t resultant_power_mW = (total_mW * recommended_brightness) / 256;
    printf("resultant power demand mW =  %li\n\n", resultant_power_mW);
#endif

#if POWER_LED > 0
    if( gMaxPowerIndicatorLEDPinNumber ) {
        Pin(gMaxPowerIndicatorLEDPinNumber).hi(); // turn the LED on
    }
#endif

    return recommended_brightness;
}


uint32_t IceLED::calculate_unscaled_power_mW() //25354
{
    uint32_t red32 = 0, green32 = 0, blue32 = 0;
    uint16_t count = _pixelCount - 1;

    // This loop might benefit from an AVR assembly version -MEK
    while( count) {
        red32   += *_pixels[count].r;
        green32 += *_pixels[count].g;
        blue32  += *_pixels[count].b;
        --count;
    }

    red32   *= gRed_mW;
    green32 *= gGreen_mW;
    blue32  *= gBlue_mW;

    red32   >>= 8;
    green32 >>= 8;
    blue32  >>= 8;

    uint32_t total = red32 + green32 + blue32 + (gDark_mW * _pixelCount);

    return total;
}
