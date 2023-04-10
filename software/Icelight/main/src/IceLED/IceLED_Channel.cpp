#include "IceLED_Channel.h"
#include "esp_log.h"
#include "driver/rmt_tx.h"
#include "lib8tion.h"
#include "IceLED_Channel.h"

static const char* ICELED_CHANNEL_TAG = "IceLED_Channel";

IRAM_ATTR static size_t rmt_encode_led_strip(rmt_encoder_t *encoder, rmt_channel_handle_t channel, const void *primary_data, size_t data_size, rmt_encode_state_t *ret_state)
{
    rmt_led_strip_encoder_t *led_encoder = __containerof(encoder, rmt_led_strip_encoder_t, base);
    rmt_encoder_handle_t bytes_encoder = led_encoder->bytes_encoder;
    rmt_encoder_handle_t copy_encoder = led_encoder->copy_encoder;
    rmt_encode_state_t session_state = RMT_ENCODING_COMPLETE;
    rmt_encode_state_t state = RMT_ENCODING_COMPLETE;
    size_t encoded_symbols = 0;
    switch (led_encoder->state) {
    case 0: // send RGB data
        encoded_symbols += bytes_encoder->encode(bytes_encoder, channel, primary_data, data_size, &session_state);

        //encoded_symbols += bytes_encoder->encode(bytes_encoder, channel, primary_data, data_size, &session_state);
        if (session_state & RMT_ENCODING_COMPLETE) {
            led_encoder->state = 1; // back to the initial encoding session
        }
        if (session_state & RMT_ENCODING_MEM_FULL) {
            state = RMT_ENCODING_MEM_FULL;
            goto out; // yield if there's no free space for encoding artifacts
        }
    // fall-through
    case 1: // send reset code
        encoded_symbols += copy_encoder->encode(copy_encoder, channel, &led_encoder->reset_code,
                                                sizeof(led_encoder->reset_code), &session_state);
        if (session_state & RMT_ENCODING_COMPLETE) {
            led_encoder->state = 0; // back to the initial encoding session
            state = RMT_ENCODING_COMPLETE;
        }
        if (session_state & RMT_ENCODING_MEM_FULL) {
            state = RMT_ENCODING_MEM_FULL;
            goto out; // yield if there's no free space for encoding artifacts
        }
    }
out:
    *ret_state = state;
    return encoded_symbols;
}

esp_err_t rmt_del_led_strip_encoder(rmt_encoder_t *encoder)
{
    rmt_led_strip_encoder_t *led_encoder = __containerof(encoder, rmt_led_strip_encoder_t, base);
    rmt_del_encoder(led_encoder->bytes_encoder);
    rmt_del_encoder(led_encoder->copy_encoder);
    free(led_encoder);
    return ESP_OK;
}

esp_err_t rmt_led_strip_encoder_reset(rmt_encoder_t *encoder)
{
    rmt_led_strip_encoder_t *led_encoder = __containerof(encoder, rmt_led_strip_encoder_t, base);
    rmt_encoder_reset(led_encoder->bytes_encoder);
    rmt_encoder_reset(led_encoder->copy_encoder);
    led_encoder->state = 0;
    return ESP_OK;
}

esp_err_t rmt_new_led_strip_encoder(const led_strip_encoder_config_t *config, rmt_encoder_handle_t *ret_encoder)
{
    rmt_led_strip_encoder_t *led_encoder = NULL;
        
    led_encoder = (rmt_led_strip_encoder_t *)calloc(1, sizeof(rmt_led_strip_encoder_t));
    
    if(led_encoder == nullptr){
        ESP_LOGE(ICELED_CHANNEL_TAG, "led_encoder is NULL");
        return ESP_ERR_NO_MEM;
    }

    led_encoder->base.encode = rmt_encode_led_strip;
    led_encoder->base.del = rmt_del_led_strip_encoder;
    led_encoder->base.reset = rmt_led_strip_encoder_reset;
    
   rmt_bytes_encoder_config_t bytes_encoder_config;
   bytes_encoder_config.bit0 = {
            .duration0 = static_cast<unsigned int>(0.325 * config->resolution / 1000000), // T0H=0.3us
            .level0 = 1,
            .duration1 = static_cast<unsigned int>(0.925 * config->resolution / 1000000), // T0L=0.9us
            .level1 = 0,
        };
    bytes_encoder_config.bit1 = {
            .duration0 = static_cast<unsigned int>(0.925 * config->resolution / 1000000), // T1H=0.9us
            .level0 = 1,
            .duration1 = static_cast<unsigned int>(0.325 * config->resolution / 1000000), // T1L=0.3us
            .level1 = 0,
        };
    bytes_encoder_config.flags.msb_first = 1; // WS2812 transfer bit order: G7...G0R7...R0B7...B0

    if(rmt_new_bytes_encoder(&bytes_encoder_config, &led_encoder->bytes_encoder) != ESP_OK){
        if (led_encoder) {
            if (led_encoder->bytes_encoder) {
                rmt_del_encoder(led_encoder->bytes_encoder);
            }
            if (led_encoder->copy_encoder) {
                rmt_del_encoder(led_encoder->copy_encoder);
            }
            free(led_encoder);
        }
        ESP_LOGE(ICELED_CHANNEL_TAG, "failed rmt_new_bytes_encoder");
        return ESP_FAIL;
    }

    
    rmt_copy_encoder_config_t copy_encoder_config = {};
    if(rmt_new_copy_encoder(&copy_encoder_config, &led_encoder->copy_encoder) != ESP_OK){
        if (led_encoder) {
            if (led_encoder->bytes_encoder) {
                rmt_del_encoder(led_encoder->bytes_encoder);
            }
            if (led_encoder->copy_encoder) {
                rmt_del_encoder(led_encoder->copy_encoder);
            }
            free(led_encoder);
        }
        ESP_LOGE(ICELED_CHANNEL_TAG, "failed rmt_new_copy_encoder");
        return ESP_FAIL;
    }

    uint32_t reset_ticks = config->resolution / 1000000 * 50 / 2; // reset code duration defaults to 50us
    led_encoder->reset_code = (rmt_symbol_word_t) {
        .duration0 = reset_ticks,
        .level0 = 0,
        .duration1 = reset_ticks,
        .level1 = 0,
    };
    *ret_encoder = &led_encoder->base;

    ESP_LOGI(ICELED_CHANNEL_TAG, "rmt_new_led_strip_encoder success");
    return ESP_OK;
}

IceLED_Channel* CreateIceLED_Channel(iceled_channel_config_t config_)
{
    IceLED_Channel *newChannel = nullptr;
    newChannel = new IceLED_Channel();
    
    rmt_tx_channel_config_t tx_chan_config = {
        .gpio_num = config_.gpio,
        .clk_src = RMT_CLK_SRC_DEFAULT, // select source clock
        .resolution_hz = RMT_LED_STRIP_RESOLUTION_HZ,
        .mem_block_symbols = 128, // increase the block size can make the LED less flickering
        .trans_queue_depth = 8, // set the number of transactions that can be pending in the background
    };
    esp_err_t err = rmt_new_tx_channel(&tx_chan_config, &newChannel->rmt_channel);
    if(err != ESP_OK){
        ESP_LOGE(ICELED_CHANNEL_TAG, "Failed to create TX channel");
        return nullptr;
    }
    
    led_strip_encoder_config_t encoder_config = {
        .resolution = RMT_LED_STRIP_RESOLUTION_HZ,
    };
    err = rmt_new_led_strip_encoder(&encoder_config, &newChannel->led_encoder);
    if(err != ESP_OK){
        ESP_LOGE(ICELED_CHANNEL_TAG, "Failed to create create encoder");
        return nullptr;
    }


    ESP_LOGI(ICELED_CHANNEL_TAG, "Enable RMT TX channel");
    err = rmt_enable(newChannel->rmt_channel);
    if(err != ESP_OK){
        return nullptr;
    }

    newChannel->count = config_.count;

    return newChannel;    
}
