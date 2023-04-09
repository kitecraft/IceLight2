#pragma once
#include "Pixel_Types.h"
#include "Colour_Utilities.h"


/// Cloudy color pallete
extern const RGBPalette16 CloudColors_p;
/// Lava colors
extern const RGBPalette16 LavaColors_p;
/// Ocean colors, blues and whites
extern const RGBPalette16 OceanColors_p;
/// Forest colors, greens
extern const RGBPalette16 ForestColors_p;

/// HSV Rainbow
extern const RGBPalette16 RainbowColors_p;

#define RainbowStripesColors_p RainbowStripeColors_p
/// HSV Rainbow colors with alternatating stripes of black
extern const RGBPalette16 RainbowStripeColors_p;

/// HSV color ramp: blue purple ping red orange yellow (and back)
/// Basically, everything but the greens, which tend to make
/// people's skin look unhealthy.  This palette is good for
/// lighting at a club or party, where it'll be shining on people.
extern const RGBPalette16 PartyColors_p;

/// Approximate "black body radiation" palette, akin to
/// the FastLED 'HeatColor' function.
/// Recommend that you use values 0-240 rather than
/// the usual 0-255, as the last 15 colors will be
/// 'wrapping around' from the hot end to the cold end,
/// which looks wrong.
extern const RGBPalette16 HeatColors_p;

extern const uint8_t ib_jul01_gp[];
extern const uint8_t es_vintage_57_gp[];
extern const uint8_t es_vintage_01_gp[];
extern const uint8_t es_rivendell_15_gp[];
extern const uint8_t rgi_15_gp[];
extern const uint8_t retro2_16_gp[];
extern const uint8_t Analogous_1_gp[];
extern const uint8_t es_pinksplash_08_gp[];
extern const uint8_t es_ocean_breeze_036_gp[];
extern const uint8_t departure_gp[];
extern const uint8_t es_landscape_64_gp[];
extern const uint8_t es_landscape_33_gp[];
extern const uint8_t rainbowsherbet_gp[];
extern const uint8_t gr65_hult_gp[];
extern const uint8_t gr64_hult_gp[];
extern const uint8_t GMT_drywet_gp[];
extern const uint8_t ib15_gp[];
extern const uint8_t Tertiary_01_gp[];
extern const uint8_t lava_gp[];
extern const uint8_t fierce_ice_gp[];
extern const uint8_t Colorfull_gp[];
extern const uint8_t Pink_Purple_gp[];
extern const uint8_t Sunset_Real_gp[];
extern const uint8_t Sunset_Yellow_gp[];
extern const uint8_t Beech_gp[];
extern const uint8_t Another_Sunset_gp[];
extern const uint8_t es_autumn_19_gp[];
extern const uint8_t BlacK_Blue_Magenta_White_gp[];
extern const uint8_t BlacK_Magenta_Red_gp[];
extern const uint8_t BlacK_Red_Magenta_Yellow_gp[];
extern const uint8_t Blue_Cyan_Yellow_gp[];
extern const uint8_t Orange_Teal_gp[];
extern const uint8_t Tiamat_gp[];
extern const uint8_t April_Night_gp[];
extern const uint8_t Orangery_gp[];
extern const uint8_t C9_gp[];
extern const uint8_t Sakura_gp[];
extern const uint8_t Aurora_gp[];
extern const uint8_t Atlantica_gp[];
extern const uint8_t C9_2_gp[];
extern const uint8_t C9_new_gp[];
extern const uint8_t temperature_gp[];
extern const uint8_t Aurora2_gp[];
extern const uint8_t retro_clown_gp[];
extern const uint8_t candy_gp[];
extern const uint8_t toxy_reaf_gp[];
extern const uint8_t  fairy_reaf_gp[];
extern const uint8_t semi_blue_gp[];
extern const uint8_t pink_candy_gp[];
extern const uint8_t red_reaf_gp[];
extern const uint8_t aqua_flash_gp[];
extern const uint8_t yelblu_hot_gp[];
extern const uint8_t lite_light_gp[];
extern const uint8_t red_flash_gp[];
extern const uint8_t blink_red_gp[];
extern const uint8_t red_shift_gp[];
extern const uint8_t red_tide_gp[];
extern const uint8_t candy2_gp[];

extern const RGBGradientPalettePtr gGradientPalettes[];
const int GradientPaletteCount = 58;
