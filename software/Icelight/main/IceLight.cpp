/* Blink C++ Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <cstdlib>
#include <thread>
#include "esp_log.h"
#include "src/IceLED/IceLED.h"
#include "driver/gpio.h"
#include "src/IceLED/lib8tion.h"

using namespace std;
static gpio_num_t BLINK_GPIO = GPIO_NUM_5;
const TickType_t xDelay = (1000/20) / portTICK_PERIOD_MS;

const int g_fps = 1000/40;

uint32_t getHeapSize(void)
{
    multi_heap_info_t info;
    heap_caps_get_info(&info, MALLOC_CAP_INTERNAL);
    return info.total_free_bytes + info.total_allocated_bytes;
}

uint32_t getFreeHeap(void)
{
    return heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
}

uint32_t getMinFreeHeap(void)
{
    return heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL);
}

uint32_t getMaxAllocHeap(void)
{
    return heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL);
}

uint32_t getPsramSize(void)
{
    multi_heap_info_t info;
    heap_caps_get_info(&info, MALLOC_CAP_SPIRAM);
    return info.total_free_bytes + info.total_allocated_bytes;
}

uint32_t getFreePsram(void)
{
    return heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
}

uint32_t getMinFreePsram(void)
{
    return heap_caps_get_minimum_free_size(MALLOC_CAP_SPIRAM);
}

uint32_t getMaxAllocPsram(void)
{
    return heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM);
}

void PrintMemUsage()
{
    printf("Heap size: %li\n", getHeapSize());
    printf("Free heap: %li\n", getFreeHeap());
    printf("Used heap: %li\n", getHeapSize() - getFreeHeap());
    printf("Min free heap: %li\n", getMinFreeHeap());

    printf("PSRam size: %li\n", getPsramSize());
    printf("Free PSRam: %li\n", getFreePsram());
    printf("Used PSRam: %li\n", getPsramSize() - getFreePsram());
    printf("Min free PSRam: %li\n", getMinFreePsram());
}


extern "C" void app_main(void)
{
    printf("Start:\n");
    PrintMemUsage();
    gpio_reset_pin(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(BLINK_GPIO, 0);

    IceLED IceLed;


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
    offSet += numLeds;
    IceLED_Segment segment3 = IceLed.GetSegment(offSet, length);
    offSet += numLeds;
    IceLED_Segment segment4 = IceLed.GetSegment(offSet, length);
    
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
    
    for(uint16_t i = 0; i < segment1.size(); i++){
        segment1[i] = CRGB::White;
    }
    //for(uint16_t i = 0; i < segment2.size(); i++){
    //    segment2[i] = WhiteSmoke;
    //}

    for(uint16_t i = 0; i < segment3.size(); i++){
        segment3[i] = CRGB::WhiteSmoke;
    }
    for(uint16_t i = 0; i < segment4.size(); i++){
        segment4[i] = CRGB::AliceBlue;
    }
    /*
    for(uint16_t i = 0; i < segment5.size(); i++){
        segment5[i] = WhiteSmoke;
    }
    for(uint16_t i = 0; i < segment6.size(); i++){
        segment6[i] = WhiteSmoke;
    }
    for(uint16_t i = 0; i < segment7.size(); i++){
        segment7[i] = WhiteSmoke;
    }
    for(uint16_t i = 0; i < segment8.size(); i++){
        segment8[i] = WhiteSmoke;
    }
    */

    IceLed.Show();

    bool curr = true;

    static uint8_t hue;
    int currPixel = 0;

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


            segment2.pixels[currPixel++] = CHSV(hue, 255, 255);
            hue += 5;
            if(currPixel == segment2.pixelCount){
                currPixel = 0;
            }


            //FillRainbow(segment2.pixels, segment2.pixelCount, 0, 256/segment2.pixelCount);


            //FillGradientRGB(segment2.pixels, 0, CRGB_SMALL(rand()), 40, CRGB_SMALL(rand()));
            //FillGradientRGB(segment2.pixels, 40, CRGB_SMALL(rand()), CRGB_SMALL(rand()));
            //FillGradientRGB(segment2.pixels, 60, CRGB_SMALL(rand()), CRGB_SMALL(rand()), CRGB_SMALL(rand()));
            //FillGradientRGB(segment2.pixels, 50, CRGB_SMALL(rand()), CRGB_SMALL(rand()), CRGB_SMALL(rand()), CRGB_SMALL(rand()));

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
            
            
            segment1.ClearLedData();
            for(uint16_t i = 0; i < segment1.size(); i++){
                segment1[i] = random();
            }

            if(currPixel>1)
                Blend(segment2[currPixel-2], CRGB_SMALL(CRGB::Green), 4);
            if(currPixel>2)
                Blend(segment2[currPixel-3], CRGB_SMALL(CRGB::Green), 4);
            if(currPixel>3)
                Blend(segment2[currPixel-4], CRGB_SMALL(CRGB::Green), 4);
            if(currPixel>4)
                Blend(segment2[currPixel-5], CRGB_SMALL(CRGB::Green), 4);

            segment2.FadeToBlackBy(1);


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
