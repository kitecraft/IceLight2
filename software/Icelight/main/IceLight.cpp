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
#include "src/Utilities/MemUsage.h"

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



extern "C" void app_main(void)
{
    Init_NVS();
    printf("Start:\n");
    PrintMemUsage();

    TaskHandle_t xHandle = NULL;
    xTaskCreatePinnedToCore( LedTask, "LedTask", STACK_SIZE, NULL, tskIDLE_PRIORITY, &xHandle,  0);
    StartNetwork();
    
    while (true) {
        
        vTaskDelay(TickType_t(1));
    }
}





