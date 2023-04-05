#pragma once
#include "freertos/FreeRTOS.h"
#include <cstdlib>

#include "IceLED_config.h"


typedef enum
{
    ILS_CHAN_0 = 0,
    ILS_CHAN_1,
#ifdef ICELIGHT_PIN_C
    ILS_CHAN_2,
#endif
#ifdef ICELIGHT_PIN_D
    ILS_CHAN_3,
#endif
#ifdef ICELIGHT_PIN_E
    ILS_CHAN_4,
#endif
#ifdef ICELIGHT_PIN_F
    ILS_CHAN_5,
#endif
#ifdef ICELIGHT_PIN_G
    ILS_CHAN_6,
#endif
#ifdef ICELIGHT_PIN_H
    ILS_CHAN_7,
#endif

    ILS_CHAN_MAX,
    ILS_CHAN_NO_CHAN
} iceled_channels_t;
