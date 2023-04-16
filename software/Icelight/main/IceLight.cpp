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
#include "IceLedTask.h"

#include "src/IceNetwork/IceNetwork.h"
#include "src/IceServer/IceServer.h"
#include "src/Utilities/MemUsage.h"
#include "src/IceFS/IceFS.h"

using namespace std;


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

void Reset()
{
    Pref_SetDefaultBrightness(CONFIG_ICELIGHT_DEFAULT_BRIGHTNESS);
    Pref_SetDeviceName(CONFIG_ICELIGHT_DEVICE_NAME);
    Pref_SetSoftAPPassword("");
    Pref_SetSTAEnabled(false);
}

extern "C" void app_main(void)
{
    printf("\n*** Starting the whole thing now:\n");
    PrintMemUsage();

    Init_NVS();
    if(InitFS() != ESP_OK){
        ESP_LOGI(MAIN_TAG, "Failed to init filesystem.  Spinning...");
        while(true){vTaskDelay(TickType_t(1));}
    }

    TaskHandle_t xHandle = NULL;
    xTaskCreatePinnedToCore( LedTask, "LedTask", STACK_SIZE, NULL, tskIDLE_PRIORITY, &xHandle,  0);
    
    StartNetwork();
    //StartIceServer();
    while (true) {
        
        vTaskDelay(TickType_t(1));
    }
}





