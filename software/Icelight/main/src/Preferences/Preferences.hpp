#pragma once
#include "nvs.h"
#include "nvs_handle.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "IceLight_NVS_Keys.h"
#include "esp_log.h"

static const char *PREF_TAG = "Preferences";

void Pref_RebuildPreferences()
{
    //Use this to build all of the preferences on a 'factory' reset
    
}

template<typename T>
esp_err_t Pref_GetItem(const char *name, const char *key, T &value) {
    esp_err_t err;
    std::unique_ptr<nvs::NVSHandle> handle = nvs::open_nvs_handle(name, NVS_READONLY, &err);
    if (err != ESP_OK) {
        ESP_LOGE(PREF_TAG, "Pref_GetItem: Error (%s) opening NVS handle: '%s'!\n", esp_err_to_name(err), name);
    } else {
        err = handle->get_item(key, value);
    }
    return err;
}

template<typename T>
esp_err_t Pref_GetItem(const char *name, const char *key, T def, T &value) {
    esp_err_t err = Pref_GetItem(name, key, value);
    if(err != ESP_OK){
        value = def;
    }

    return ESP_OK;
}

template<typename T>
esp_err_t Pref_SetItem(const char *name, const char *key, T value) {
    esp_err_t err;
    std::unique_ptr<nvs::NVSHandle> handle = nvs::open_nvs_handle(name, NVS_READWRITE, &err);
    if (err != ESP_OK) {
        ESP_LOGE(PREF_TAG, "Pref_SetItem: Error (%s) opening NVS handle (%s)!\n", esp_err_to_name(err), name);
    } else {
        err = handle->set_item(key, value);
        if(err == ESP_OK){
            err = handle->commit();
        }
    }

    return err;
}

/*
class Preferences {
private:
    std::unique_ptr<nvs::NVSHandle> _handle;
    bool _started;
    bool _readOnly;

public:
    Preferences();
};
*/