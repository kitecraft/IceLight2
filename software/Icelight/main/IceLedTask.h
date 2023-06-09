#pragma once
#include <cstdlib>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "src/IceLight_config.h"
#include "src/Utilities/MemUsage.h"
#include "src/IceLED/IceLED.h"
#include "src/Preferences/Preferences.hpp"
#include "src/IceLED/lib8tion.h"

static gpio_num_t BLINK_GPIO = GPIO_NUM_5;
const TickType_t xDelay = (1000/20) / portTICK_PERIOD_MS;

const int g_fps = 1000/40;



const Palette16 myRedWhiteBluePalette_p =
{
    CRGB::Red,
    CRGB::Gray, // 'white' is too bright compared to red and blue
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Red,
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Black,
    CRGB::Black
};

static int palletIndex = 0;
CRGBPalette256 currentPalette = gGradientPalettes[palletIndex];

TBlendType    currentBlending = LINEARBLEND;
// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
    for( int i = 0; i < 16; ++i) {
        currentPalette[i] = CHSV( rand()%256, 255, rand()%256);
    }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
    // 'black out' all 16 palette entries...
    FillSolid( currentPalette.entries, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::White;
    currentPalette[4] = CRGB::White;
    currentPalette[8] = CRGB::White;
    currentPalette[12] = CRGB::White;
    
}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
    CRGBSmall purple, green, black;
    purple = CHSV( HUE_PURPLE, 255, 255);
    green  = CHSV( HUE_GREEN, 255, 255);
    black  = CRGB::Black;
    
    currentPalette = CRGBPalette16(
                                   green,  green,  black,  black,
                                   purple, purple, black,  black,
                                   green,  green,  black,  black,
                                   purple, purple, black,  black );
}

void ChangePalettePeriodically()
{
    //static int secondHand = 0;
    
    EVERY_N_SECONDS_I(pc, 5){
        palletIndex++;
        printf("Palett index: %i\n", palletIndex);
        if(palletIndex == GradientPaletteCount)
        {
            palletIndex = 0;
        }
        currentPalette = gGradientPalettes[palletIndex];
        /*
        if( secondHand ==  0)  { currentPalette = RainbowColors_p;         currentBlending = LINEARBLEND; }
        if( secondHand == 10)  { currentPalette = RainbowStripeColors_p;   currentBlending = NOBLEND;  }
        if( secondHand == 15)  { currentPalette = RainbowStripeColors_p;   currentBlending = LINEARBLEND; }
        if( secondHand == 20)  { SetupPurpleAndGreenPalette();             currentBlending = LINEARBLEND; }
        if( secondHand == 25)  { SetupTotallyRandomPalette();              currentBlending = LINEARBLEND; }
        if( secondHand == 30)  { SetupBlackAndWhiteStripedPalette();       currentBlending = NOBLEND; }
        if( secondHand == 35)  { SetupBlackAndWhiteStripedPalette();       currentBlending = LINEARBLEND; }
        if( secondHand == 40)  { currentPalette = CloudColors_p;           currentBlending = LINEARBLEND; }
        if( secondHand == 45)  { currentPalette = PartyColors_p;           currentBlending = LINEARBLEND; }
        if( secondHand == 50)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = NOBLEND;  }
        if( secondHand == 55)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = LINEARBLEND; }
        secondHand += 5;
        if(secondHand == 60){
            secondHand = 0;
        }
        */
    }
}



int g_nvsCounter = 0;
#define GPIO_BIT_MASK ( (1ULL<<GPIO_NUM_5) | (1ULL<<GPIO_NUM_23) | (1ULL<<GPIO_NUM_25)  | (1ULL<<GPIO_NUM_21) | (1ULL<<GPIO_NUM_22) )

static void LedTask(void *pvParameters)
{
    /*
    gpio_config_t io_conf;
	io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pin_bit_mask = GPIO_BIT_MASK;
	gpio_config(&io_conf);
    */
    gpio_reset_pin(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(BLINK_GPIO, 0);
    gpio_set_pull_mode(BLINK_GPIO, GPIO_PULLDOWN_ONLY);
    
    IceLED IceLed;

/*
    int val = 0;
    if(Pref_GetDefaultBrightness(val) != ESP_OK){
        ESP_LOGE(MAIN_TAG, "Something has gone horribly wrong\n");
    } else {
        ESP_LOGI(MAIN_TAG, "Brightness value: %i\n", val);
        g_nvsCounter = val;
    }

    bool bVal = false;
    if(Pref_GetSTAEnabled(bVal) != ESP_OK){
        ESP_LOGE(MAIN_TAG, "Something has gone horribly wrong\n");
    } else {
        ESP_LOGI(MAIN_TAG, "STA connected: %s\n", B_TO_S(bVal));
    }
    g_nvsCounter++;
    Pref_SetDefaultBrightness(g_nvsCounter);
    Pref_SetSTAEnabled(!bVal);
*/


    uint16_t numLeds = 100;
    int numChannels = 4;
    iceled_config_t configArray[numChannels];

    iceled_config_t configA{
    .ledType = ILT_WS2812,
    .gpio = ICELIGHT_PIN_A,
    .numLeds = numLeds,
    };
    configArray[0] = configA;

    iceled_config_t configB{
        .ledType = ILT_WS2812,
        .gpio = ICELIGHT_PIN_B,
        .numLeds = numLeds,
    };
    configArray[1] = configB;


    iceled_config_t configC{
        .ledType = ILT_WS2812,
        .gpio = ICELIGHT_PIN_C,
        .numLeds = numLeds,
    };
    configArray[2] = configC;

    iceled_config_t configD{
        .ledType = ILT_WS2812,
        .gpio = ICELIGHT_PIN_D,
        .numLeds = numLeds,
    };
    configArray[3] = configD;

    /*
    iceled_config_t configE{
        .ledType = ILT_WS2812,
        .gpio = ICELIGHT_PIN_E,
        .numLeds = numLeds,
    };
    configArray[4] = configE;
    
    iceled_config_t configF{
        .ledType = ILT_WS2812,
        .gpio = ICELIGHT_PIN_F,
        .numLeds = numLeds,
    };
    configArray[5] = configF;

    iceled_config_t configG{
        .ledType = ILT_WS2812,
        .gpio = ICELIGHT_PIN_G,
        .numLeds = numLeds,
    };
    configArray[6] = configG;
    
    iceled_config_t configH{
        .ledType = ILT_WS2812,
        .gpio = ICELIGHT_PIN_H,
        .numLeds = numLeds,
    };
    configArray[7] = configH;
    */


    IceLed.ConfigureIceLed(configArray, numChannels);
    
    int length = numLeds;
    int offSet = 0;
    IceLED_Segment segment1 = IceLed.GetSegment(offSet, length);
    offSet += numLeds;
    IceLED_Segment segment2 = IceLed.GetSegment(offSet, length);
    //offSet += numLeds;
    //IceLED_Segment segment3 = IceLed.GetSegment(offSet, length);
    //offSet += numLeds;
    //IceLED_Segment segment4 = IceLed.GetSegment(offSet, length);
    
    /*
    offSet += numLeds;
    IceLED_Segment segment5 = IceLed.GetSegment(offSet, length);
    offSet += numLeds;
    IceLED_Segment segment6 = IceLed.GetSegment(offSet, length);
    offSet += numLeds;
    IceLED_Segment segment7 = IceLed.GetSegment(offSet, length);
    offSet += numLeds;
    IceLED_Segment segment8 = IceLed.GetSegment(offSet, length);
    */

    printf("Configured:\n");
    PrintMemUsage();

    int colorIndex = 0;
    IceLed.SetBrightness(40);
    segment1.ClearLedData();
    segment2.ClearLedData();
    //FillSolid(segment1.pixels, segment1.pixelCount, CRGB::Black);
    //FillSolid(segment2.pixels, segment2.pixelCount, CRGB::Azure);
    
    //for(uint16_t i = 0; i < segment1.size(); i++){
    //    segment1[i] = CRGB::White;
    //}
    //for(uint16_t i = 0; i < segment2.size(); i++){
    //    segment2[i] = WhiteSmoke;
    //}

    /*
    for(uint16_t i = 0; i < segment3.size(); i++){
        segment3[i] = CRGB::WhiteSmoke;
    }
    for(uint16_t i = 0; i < segment4.size(); i++){
        segment4[i] = CRGB::AliceBlue;
    }
    */

    IceLed.Show();

    //bool curr = true;

    //static uint8_t hue;
    //int currPixel = 0;
    
    //currentPalette = PartyColors_p;
    TBlendType currentBlending = LINEARBLEND;

    //uint8_t brightness = 255;
    //int palletPixel = 0;
    //for( int i = 0; i < segment1.pixelCount; ++i) {
    //    segment1.pixels[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
    //    colorIndex += 3;
    //}

    while (true) {
        //printf("Hello World\n");
        
        /*
        EVERY_N_SECONDS(1){
            int b = IceLed.GetBrightness() + 5;
            printf("Brightness: %i\n\n",b);
            IceLed.SetBrightness(b);
        }
        */

       /*
        EVERY_N_SECONDS(1){
            if(curr){
                IceLed.SetColourTemperature(CoolWhiteFluorescent);
                printf("CoolWhiteFluorescent\n");
            } else {
                IceLed.SetColourTemperature(ClearBlueSky);
                printf("OvercastSky\n");
            }
            curr = !curr;
        }
        */

        EVERY_N_SECONDS_I(solidChange, 1){
            //for(uint16_t i = 0; i < segment2.size(); i++){
            //    segment2[i] = ICE_COLOR_ARRAY[colorIndex];
            //}
            //   colorIndex++;
            //   if(colorIndex == ICE_COLOUR_COUNT){
            //       colorIndex = 0;


            //FillSolid(segment2.pixels, segment2.pixelCount, rand());

            //segment2.pixels[currPixel++] = CHSV(hue, 255, 255);
            //hue += 5;
            //if(currPixel == 50){
            //    currPixel = 0;
            //}


            //FillRainbow(segment2.pixels, segment2.pixelCount, 0, 256/segment2.pixelCount);


            //FillGradientRGB(segment2.pixels, 0, CRGBSmall(rand()), 40, CRGBSmall(rand()));
            //FillGradientRGB(segment2.pixels, 40, CRGBSmall(rand()), CRGBSmall(rand()));
            //FillGradientRGB(segment2.pixels, 60, CRGBSmall(rand()), CRGBSmall(rand()), CRGBSmall(rand()));
            //FillGradientRGB(segment2.pixels, 50, CRGBSmall(rand()), CRGBSmall(rand()), CRGBSmall(rand()), CRGBSmall(rand()));

        }



        EVERY_N_MILLISECONDS(g_fps){
            /*
            for(uint16_t i = 0; i < segment2.size(); i++){
                segment2[i] = ICE_COLOR_ARRAY[colorIndex];
            }
            colorIndex++;
            if(colorIndex == ICE_COLOUR_COUNT){
                colorIndex = 0;
            }
            */
            
            
            //ChangePalettePeriodically();
            //currentPalette = gGradientPalettes[24];
            //segment1.FadeToBlackBy(50);
            //segment1.ClearLedData();
            //for(uint16_t i = 0; i < segment1.size()/20; i++){
                //segment1[rand()%segment1.size()] = random();
            //    segment1[rand()%segment1.size()] = ColorFromPalette( currentPalette, rand()%255, brightness, currentBlending);
            //}
            
            /*
            if(currPixel>1)
                Blend(segment2[currPixel-2], CRGBSmall(CRGB::Green), 3);
            if(currPixel>2)
                Blend(segment2[currPixel-3], CRGBSmall(CRGB::Green), 2);
            if(currPixel>3)
                Blend(segment2[currPixel-4], CRGBSmall(CRGB::Green), 1);
            if(currPixel>4)
                Blend(segment2[currPixel-5], CRGBSmall(CRGB::Green), 1);
            */

           
            //segment1.FadeToBlackBy(25);
            
            //segment2.FadeToBlackBy(1);

            
            ChangePalettePeriodically();
            
            //currentPalette = gGradientPalettes[40];
            segment1.ClearLedData();
            segment2.ClearLedData();
            
            uint8_t brightness = 255;
            colorIndex = 0;
            float increment = 256/numLeds;
            for( int i = 0; i < segment1.pixelCount; ++i) {
                CRGBSmall colour = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
                colorIndex = i * increment;
                segment1.pixels[i] = colour;
                segment2.pixels[i] = colour;
            }

            /*
            FillRainbow(segment1.pixels, segment1.pixelCount, palletPixel, 1);
            palletPixel--;
            if(currPixel == 0){
                palletPixel = 255;
            }
            */



            gpio_set_level(BLINK_GPIO, 1);
            if(!IceLed.Show()){
                printf("Main show: IceLed failed to Show()\n");
            }
            gpio_set_level(BLINK_GPIO, 0);
        }



        /*
            for(uint16_t i = 0; i < segment.size(); i++){
                segment[i] = Green;
            }
            for(uint16_t i = 0; i < segment2.size(); i++){
                segment2[i] = Yellow;
            }
            IceLed.Show();

            TickType_t tDelay = 250;
            vTaskDelay(tDelay);
            
            segment.ClearLedData();
            IceLed.Show();
            vTaskDelay(tDelay);

            segment2.ClearLedData();
            IceLed.Show();
            vTaskDelay(tDelay);



            
            for(uint16_t i = 0; i < segment.size(); i++){
                segment[i] = Orange;
            }
            for(uint16_t i = 0; i < segment2.size(); i++){
                segment2[i] = Lime;
            }
            IceLed.Show();

            vTaskDelay(tDelay);
            
            segment2.ClearLedData();
            IceLed.Show();
            vTaskDelay(tDelay);

            segment.ClearLedData();
            IceLed.Show();
            vTaskDelay(tDelay);
            */
     
        vTaskDelay(TickType_t(1));
    }
}