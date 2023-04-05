#pragma once
#include "freertos/FreeRTOS.h"
#include "driver/rmt_tx.h"
#include <cstdlib>
#include "IceLED_Pins.h"
#include "LED_Types.h"
#include "Pixel_Types.h"
#include "freertos/semphr.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
    uint8_t gpio = NOT_A_PIN;
    iceled_channels_t channel = ILS_CHAN_NO_CHAN;
    uint16_t count = 0;
    iceled_type_t pixelType = ILT_WS2812;

} iceled_channel_config_t;

typedef struct {
    uint32_t resolution; /*!< Encoder resolution, in Hz */
} led_strip_encoder_config_t;

typedef struct {
    rmt_encoder_t base;
    rmt_encoder_t *bytes_encoder;
    rmt_encoder_t *copy_encoder;
    int state;
    rmt_symbol_word_t reset_code;
} rmt_led_strip_encoder_t;

typedef struct{
    iceled_channels_t channel = ILS_CHAN_NO_CHAN;
    uint16_t count = 0;
    uint16_t offset = 0;
    iceled_type_t pixelType;
    rmt_channel_handle_t rmt_channel = NULL;
    rmt_encoder_handle_t led_encoder = NULL;

    //raw array of bytes that are sent to the rmt encoder
    //length is this->count*3
    uint8_t* _pixelData = nullptr; 
}IceLED_Channel;

#define RMT_LED_STRIP_RESOLUTION_HZ 10000000 // 10MHz resolution, 1 tick = 0.1us (led strip needs a high resolution)
IceLED_Channel* CreateIceLED_Channel(iceled_channel_config_t config_);



esp_err_t rmt_del_led_strip_encoder(rmt_encoder_t *encoder);
esp_err_t rmt_led_strip_encoder_reset(rmt_encoder_t *encoder);
esp_err_t rmt_new_led_strip_encoder(const led_strip_encoder_config_t *config, rmt_encoder_handle_t *ret_encoder);
IceLED_Channel* CreateIceLED_Channel(iceled_channel_config_t config_);

#ifdef __cplusplus
}
#endif
