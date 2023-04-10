#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <cstdlib>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "../IceLight_config.h"
#include "../Preferences/Preferences.hpp"


static const char* NETWORK_TAG = "IceNetwork";
#define MAX_ESP_NETIFS 3
static esp_netif_t* esp_netifs[MAX_ESP_NETIFS] = {nullptr, nullptr};

static size_t _wifi_strncpy(char * dst, const char * src, size_t dst_len){
    if(!dst || !src || !dst_len){
        return 0;
    }
    size_t src_len = strlen(src);
    if(src_len >= dst_len){
        src_len = dst_len;
    } else {
        src_len += 1;
    }
    memcpy(dst, src, src_len);
    return src_len;
}

static void IceNetworkEventHandler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(NETWORK_TAG, "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(NETWORK_TAG, "station "MACSTR" leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
}

esp_err_t SetHostname(esp_interface_t interface, const char * hostname){
	if(interface < MAX_ESP_NETIFS){
		return esp_netif_set_hostname(esp_netifs[interface], hostname);
	}
	return ESP_FAIL;
}

struct IPAddress{
    union {
        uint8_t bytes[4];  // IPv4 address
        uint32_t dword;
    };
    IPAddress()
    {
        dword = 0;
    }
    IPAddress(uint32_t address)
    {
        dword = address;
    }
    void ToString(char *val) const
    {
        sprintf(val,"%u.%u.%u.%u", bytes[0], bytes[1], bytes[2], bytes[3]);
    }
};

IPAddress SoftAPIP()
{
	esp_netif_ip_info_t ip;
    if(esp_netif_get_ip_info(esp_netifs[ESP_IF_WIFI_AP], &ip) != ESP_OK){
        ESP_LOGE(NETWORK_TAG, "Failed to get ip");
    	return IPAddress();
    }
    return IPAddress(ip.ip.addr);
}

static esp_err_t InitSoftAP()
{
    esp_err_t err;
    if(esp_netifs[ESP_IF_WIFI_AP] != nullptr){
        //do a disconnect here
    }

    char deviceName[CONFIG_ICELIGHT_MAX_STRING_LEN] = {'\0'};
    Pref_GetDeviceName(deviceName, CONFIG_ICELIGHT_MAX_STRING_LEN);

    bool validPassword = true;
    char softApPassword[CONFIG_ICELIGHT_MAX_STRING_LEN] = {'\0'};
    Pref_GetSoftAPPassword(softApPassword, CONFIG_ICELIGHT_MAX_STRING_LEN);

    if(strlen(softApPassword) == 0){
        validPassword = false;
    }

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    err = esp_wifi_init(&cfg);
    if(err != ESP_OK){
        ESP_LOGE(NETWORK_TAG, "Failed to init softAP config");
        return err;
    }

    err = esp_event_handler_instance_register(WIFI_EVENT,
                                            ESP_EVENT_ANY_ID,
                                            &IceNetworkEventHandler,
                                            NULL,
                                            NULL);
    if(err != ESP_OK){
        ESP_LOGE(NETWORK_TAG, "Failed to register event handler");
        return err;
    }
    

    wifi_config_t wifi_config;
    _wifi_strncpy((char*)wifi_config.ap.ssid, deviceName, 32);
    wifi_config.ap.ssid_len = strlen(deviceName);
    if(validPassword){
        _wifi_strncpy((char*)wifi_config.ap.password, softApPassword, 32);
        wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
        wifi_config.ap.pairwise_cipher = WIFI_CIPHER_TYPE_CCMP;
    } else {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
        wifi_config.ap.password[0] = 0;
    }
    wifi_config.ap.max_connection = CONFIG_ICENETWORK_MAXIMUM_MAX_CONNECTIONS;
    wifi_config.ap.pmf_cfg.required = false;
    wifi_config.ap.beacon_interval = 100;
    wifi_config.ap.ftm_responder = false;
    wifi_config.ap.channel = CONFIG_ICENETWORK_SOFTAP_DEFAULT_CHANNEL;
    
    err = esp_wifi_set_mode(WIFI_MODE_AP);
    if(err != ESP_OK){
        ESP_LOGE(NETWORK_TAG, "Failed to set wifi mode");
        return err;
    }

    err = esp_wifi_set_config(WIFI_IF_AP, &wifi_config);
    if(err != ESP_OK){
        ESP_LOGE(NETWORK_TAG, "Failed to set wifi config");
        return err;
    }
    
    err = esp_wifi_start();
    if(err != ESP_OK){
        ESP_LOGE(NETWORK_TAG, "Failed to start WifiAP");
        return err;
    }

    IPAddress ipa = SoftAPIP();
    char ipAddress[17] = {'\0'};
    ipa.ToString(ipAddress);
    ESP_LOGI(NETWORK_TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d ip:%s\n",
             deviceName, softApPassword, CONFIG_ICENETWORK_SOFTAP_DEFAULT_CHANNEL, ipAddress);
             
    return ESP_OK;
}

static void StartNetwork()
{
    if(esp_netif_init() != ESP_OK){
        ESP_LOGE(NETWORK_TAG, "Failed to init network");
        return;
    }
    if(esp_event_loop_create_default() != ESP_OK){
        ESP_LOGE(NETWORK_TAG, "Failed to create network event loop");
        return;
    }

    esp_netifs[ESP_IF_WIFI_STA] = esp_netif_create_default_wifi_sta();
    esp_netifs[ESP_IF_WIFI_AP] = esp_netif_create_default_wifi_ap();

    char deviceName[CONFIG_ICELIGHT_MAX_STRING_LEN] = {'\0'};
    Pref_GetDeviceName(deviceName, CONFIG_ICELIGHT_MAX_STRING_LEN);
    printf("Got device name: %s\n", deviceName);
    
    if(Pref_SetSoftAPPassword("Test1234") != ESP_OK){
            ESP_LOGE(NETWORK_TAG, "Failed to set password\n" );
    }
    
	esp_wifi_set_ps(WIFI_PS_NONE);
    if(Pref_GetSTAEnabled()){
        
    } else {
        esp_err_t ret = InitSoftAP();
        if(ret != ESP_OK){
            ESP_LOGE(NETWORK_TAG, "Failed to start soft ap. '%i': '%s'  rebooting...", ret, esp_err_to_name(ret));
            vTaskDelay(TickType_t(2000));
            esp_restart();
        } else {

        }
    }
}



#ifdef __cplusplus
}
#endif
