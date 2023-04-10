/* Blink C++ Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <cstdlib>
#include <thread>
#include <nvs_flash.h>
#include "esp_log.h"
#include "driver/gpio.h"
#include "src/IceLight_config.h"
#include "src/IceLED/IceLED.h"
#include "src/Preferences/Preferences.hpp"
#include "src/IceLED/lib8tion.h"

#include "src/IceNetwork/IceNetwork.h"
#include "src/Utilities/MemUsage.h"

using namespace std;
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

CRGBPalette256 currentPalette;

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

static int palletIndex = 0;
void ChangePalettePeriodically()
{
    //static int secondHand = 0;
    
    EVERY_N_SECONDS_I(pc, 5){
        currentPalette = gGradientPalettes[palletIndex];
        printf("Palett index: %i\n", palletIndex);
        palletIndex++;
        if(palletIndex == GradientPaletteCount)
        {
            palletIndex = 0;
        }
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

static const char *MAIN_TAG = "ICELIGHT";
void Init_NVS()
{
    //Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGI(MAIN_TAG, "Init_NVS: nvs_flash_init returned %i\n", err);
        ESP_LOGI(MAIN_TAG, "Erasing flash\n");
        
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_LOGI(MAIN_TAG, "Re initing flash\n");

        esp_err_t ret = nvs_flash_init();
        ESP_ERROR_CHECK(ret);
        ESP_LOGI(MAIN_TAG, "Done rebuilding flash\n");
        
        Pref_RebuildPreferences();
    }
}


int g_nvsCounter = 0;
extern "C" void app_main(void)
{
    Init_NVS();
    printf("Start:\n");
    PrintMemUsage();
    gpio_reset_pin(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(BLINK_GPIO, 0);

    StartNetwork();

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
    FillSolid(segment2.pixels, segment2.pixelCount, CRGB::Azure);
    
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
            
            uint8_t brightness = 255;
            colorIndex = 0;
            for( int i = 0; i < segment1.pixelCount; ++i) {
                CRGBSmall colour = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
                colorIndex += 3;
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





