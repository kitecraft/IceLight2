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
esp_err_t Pref_GetItem(const char *key, T &value) {
    esp_err_t err;
    std::unique_ptr<nvs::NVSHandle> handle = nvs::open_nvs_handle(IL_PREF_NAMESPACE_GENERAL, NVS_READONLY, &err);
    if (err != ESP_OK) {
        ESP_LOGE(PREF_TAG, "Pref_GetItem: Error (%s) opening NVS handle: '%s'!\n", esp_err_to_name(err), IL_PREF_NAMESPACE_GENERAL);
    } else {
        err = handle->get_item(key, value);
    }
    return err;
}

template<typename T>
esp_err_t Pref_GetItem(const char *key, T def, T &value) {
    esp_err_t err = Pref_GetItem(key, value);
    if(err != ESP_OK){
        value = def;
    }

    return ESP_OK;
}

template<typename T>
esp_err_t Pref_SetItem(const char *key, T value) {
    esp_err_t err;
    std::unique_ptr<nvs::NVSHandle> handle = nvs::open_nvs_handle(IL_PREF_NAMESPACE_GENERAL, NVS_READWRITE, &err);
    if (err != ESP_OK) {
        ESP_LOGE(PREF_TAG, "Pref_SetItem: Error (%s) opening NVS handle (%s)!\n", esp_err_to_name(err), IL_PREF_NAMESPACE_GENERAL);
    } else {
        err = handle->set_item(key, value);
        if(err == ESP_OK){
            err = handle->commit();
        }
    }

    return err;
}


esp_err_t Pref_GetString(const char *key, char *val, size_t len)
{
    memset(val, '\0', len);
    esp_err_t err;
    std::unique_ptr<nvs::NVSHandle> handle = nvs::open_nvs_handle(IL_PREF_NAMESPACE_GENERAL, NVS_READONLY, &err);
    if (err != ESP_OK) {
        ESP_LOGE(PREF_TAG, "Pref_GetString: Error (%s) opening NVS handle: '%s'!\n", esp_err_to_name(err), IL_PREF_NAMESPACE_GENERAL);
    } else {
        size_t l;
        handle->get_item_size(nvs::ItemType::SZ, key, l);
        if(l == 0){
            return ESP_FAIL;
        }
        err = handle->get_string(key, val, len);
    }
    return err;
}

esp_err_t Pref_GetString(const char *key, const char* def, char *val, size_t len)
{
    esp_err_t err = Pref_GetString(key, val, len);
    if(err != ESP_OK){
        int s = strlen(def);
        int l = s > len ? len : s;
        strncpy(val, def, l);
    }
    return ESP_OK;
}

esp_err_t Pref_SetString(const char *key, const char *val) {
    esp_err_t err;
    std::unique_ptr<nvs::NVSHandle> handle = nvs::open_nvs_handle(IL_PREF_NAMESPACE_GENERAL, NVS_READWRITE, &err);
    if (err != ESP_OK) {
        ESP_LOGE(PREF_TAG, "Pref_SetItem: Error (%s) opening NVS handle (%s)!\n", esp_err_to_name(err), IL_PREF_NAMESPACE_GENERAL);
    } else {
        err = handle->set_string(key, val);
        if(err == ESP_OK){
            err = handle->commit();
        }
    }
    return err;
}



esp_err_t Pref_GetDeviceName(char *val, size_t len)
{
    return Pref_GetString(IL_PREF_KEY_DEVICE_NAME, CONFIG_ICELIGHT_DEVICE_NAME, val, len);
}
esp_err_t Pref_SetDeviceName(const char *val)
{
    return Pref_SetString(IL_PREF_KEY_DEVICE_NAME, val);
}

esp_err_t Pref_GetSoftAPPassword(char *val, size_t len)
{
    return Pref_GetString(IL_PREF_KEY_AP_PASSWORD, "", val, len);
}
esp_err_t Pref_SetSoftAPPassword(const char *val)
{
    if(strlen(val) < 8){
        return ESP_FAIL;
    }
    return Pref_SetString(IL_PREF_KEY_AP_PASSWORD, val);
}


//IceLED
uint8_t Pref_GetDefaultBrightness(){
    int val;
    Pref_GetItem(IL_PREF_KEY_LAST_BRIGHTNESS, CONFIG_ICELIGHT_DEFAULT_BRIGHTNESS, val);
    return val;
}
void Pref_SetDefaultBrightness(int val){
    Pref_SetItem(IL_PREF_KEY_LAST_BRIGHTNESS, val);
}


//Wifi
bool Pref_GetSTAEnabled(){
    bool val;
    Pref_GetItem(IL_PREF_KEY_STA_ENABLED, false, val);
    return val;
}
void Pref_SetSTAEnabled(bool val){
    Pref_SetItem(IL_PREF_KEY_STA_ENABLED, val);
}