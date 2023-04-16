#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <cstdlib>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "../IceLight_config.h"
#include "../Preferences/Preferences.hpp"
#include "../IceServer/IceServer.h"


static const char* NETWORK_TAG = "IceNetwork";
#define MAX_ESP_NETIFS 3
static esp_netif_t* esp_netifs[MAX_ESP_NETIFS] = {nullptr, nullptr};
static int g_retryNum = 0;
//static EventGroupHandle_t s_wifi_event_group;
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1


static esp_err_t InitSoftAP();

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
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (g_retryNum < CONFIG_ICENETWORK_MAXIMUM_RETRY) {
            esp_wifi_connect();
            g_retryNum++;
            ESP_LOGI(NETWORK_TAG, "**** retry to connect to the AP");
        } else {
            ESP_LOGI(NETWORK_TAG,"**** connect to the AP fail");
            //xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
            InitSoftAP();
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(NETWORK_TAG, "**** got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        g_retryNum = 0;
        StartIceServer();
        //xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }

    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(NETWORK_TAG, "**** station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(NETWORK_TAG, "**** station "MACSTR" leave, AID=%d",
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

IPAddress STAIP()
{
	esp_netif_ip_info_t ip;
    if(esp_netif_get_ip_info(esp_netifs[ESP_IF_WIFI_STA], &ip) != ESP_OK){
        ESP_LOGE(NETWORK_TAG, "Failed to get ip");
    	return IPAddress();
    }
    return IPAddress(ip.ip.addr);
}

static esp_err_t InitSoftAP()
{
    char deviceName[CONFIG_ICELIGHT_MAX_STRING_LEN] = {'\0'};
    Pref_GetDeviceName(deviceName, CONFIG_ICELIGHT_MAX_STRING_LEN);
    SetHostname(ESP_IF_WIFI_AP, deviceName);
    
    bool validPassword = true;
    char softApPassword[CONFIG_ICELIGHT_MAX_STRING_LEN] = {'\0'};
    Pref_GetSoftAPPassword(softApPassword, CONFIG_ICELIGHT_MAX_STRING_LEN);

    if(strlen(softApPassword) == 0){
        validPassword = false;
    }

    esp_err_t err;
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
    StartIceServer();
    
    IPAddress ipa = SoftAPIP();
    char ipAddress[17] = {'\0'};
    ipa.ToString(ipAddress);
    ESP_LOGI(NETWORK_TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d ip:%s\n",
             deviceName, softApPassword, CONFIG_ICENETWORK_SOFTAP_DEFAULT_CHANNEL, ipAddress);
             
    return ESP_OK;
}

static esp_err_t ConnectToWifiNetworkWithCreds(char *ssid, char *password)
{
    //ESP_LOGI(NETWORK_TAG, "Connecting to network: '%s' %i, with password '%s' %i", ssid, strlen(ssid),  password, strlen(password));
    //s_wifi_event_group = xEventGroupCreate();
    char deviceName[CONFIG_ICELIGHT_MAX_STRING_LEN] = {'\0'};
    Pref_GetDeviceName(deviceName, CONFIG_ICELIGHT_MAX_STRING_LEN);
    SetHostname(ESP_IF_WIFI_STA, deviceName);
    
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    esp_event_handler_instance_register(WIFI_EVENT,
                                    ESP_EVENT_ANY_ID,
                                    &IceNetworkEventHandler,
                                    NULL,
                                    &instance_any_id);

    esp_event_handler_instance_register(IP_EVENT,
                                    IP_EVENT_STA_GOT_IP,
                                    &IceNetworkEventHandler,
                                    NULL,
                                    &instance_got_ip);



    wifi_config_t wifi_config;
    _wifi_strncpy((char*)wifi_config.sta.ssid, ssid, 32);
    _wifi_strncpy((char*)wifi_config.sta.password, password, 32);
    /* Authmode threshold resets to WPA2 as default if password matches WPA2 standards (pasword len => 8).
        * If you want to connect the device to deprecated WEP/WPA networks, Please set the threshold value
        * to WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK and set the password with length and format matching to
        * WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK standards.
        */
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    wifi_config.sta.sae_pwe_h2e = WPA3_SAE_PWE_BOTH;
    
    esp_err_t err;
    err = esp_wifi_set_mode(WIFI_MODE_STA);
    if(err != ESP_OK){
        ESP_LOGE(NETWORK_TAG, "Failed to start WifiAP");
        return err;
    }
    err = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    if(err != ESP_OK){
        ESP_LOGE(NETWORK_TAG, "Failed to start WifiAP");
        return err;
    }
    err = esp_wifi_start();
    if(err != ESP_OK){
        ESP_LOGE(NETWORK_TAG, "Failed to start WifiAP");
        return err;
    }

    ESP_LOGI(NETWORK_TAG, "****  wifi_init_sta finished. ***");
#if 0
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(NETWORK_TAG, "connected to ap SSID: ");
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGE(NETWORK_TAG, "Failed to connect to SSID");
    } else {
        ESP_LOGE(NETWORK_TAG, "UNEXPECTED EVENT");
    }
#endif
    return ESP_OK;
}

static esp_err_t ConnectToCurrentlyKnownWifiNetwork()
{
    char networkName[CONFIG_ICELIGHT_MAX_STRING_LEN];
    Pref_GetStaSSID(networkName, CONFIG_ICELIGHT_MAX_STRING_LEN);

    char networkPassword[CONFIG_ICELIGHT_MAX_STRING_LEN];
    Pref_GetStaSSIDPassword(networkPassword, CONFIG_ICELIGHT_MAX_STRING_LEN);

    /*
    For some UNKNOWN reason, if the follow log line is removed,
    then it fails to connect to the wifi network.  
    WHY!!!  WHY why WHY WHY WHWasdfasasdn34q
    */
    ESP_LOGI(NETWORK_TAG, "");
    return ConnectToWifiNetworkWithCreds(networkName, networkPassword);
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

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    if(esp_wifi_init(&cfg) != ESP_OK){
        ESP_LOGE(NETWORK_TAG, "Failed to init esp wifi. rebooting...");
        vTaskDelay(TickType_t(2000));
        esp_restart();
    }
    
    //this turns off wifi power saving and is necessary for best ESPNow performance
	esp_wifi_set_ps(WIFI_PS_NONE); 


    //char deviceName[CONFIG_ICELIGHT_MAX_STRING_LEN] = {'\0'};
    //Pref_GetDeviceName(deviceName, CONFIG_ICELIGHT_MAX_STRING_LEN);
    //printf("Got device name: %s\n", deviceName);
    
    //if(Pref_SetSoftAPPassword("Test1234") != ESP_OK){
    //        ESP_LOGE(NETWORK_TAG, "Failed to set password\n" );
    //}
    
    Pref_SetSTAEnabled(true);

    if(Pref_GetSTAEnabled()){
        esp_err_t ret = ConnectToCurrentlyKnownWifiNetwork();
        //esp_err_t ret = ConnectToWifiNetworkWithCreds("sdf", "sgf");
        if(ret != ESP_OK){
            ESP_LOGE(NETWORK_TAG, "Failed to start wifi. '%i': '%s'  rebooting...", ret, esp_err_to_name(ret));
            vTaskDelay(TickType_t(2000));
            esp_restart();
        } else {
            /*
            IPAddress ipa = STAIP();
            char ipAddress[17] = {'\0'};
            ipa.ToString(ipAddress);
            printf("Successfully connected to Wifi with ip: %s\n", ipAddress);
            */
        }
    } else {
        esp_err_t ret = InitSoftAP();
        if(ret != ESP_OK){
            ESP_LOGE(NETWORK_TAG, "Failed to start soft ap. '%i': '%s'  rebooting...", ret, esp_err_to_name(ret));
            vTaskDelay(TickType_t(2000));
            esp_restart();
        } else {
            printf("Successfully started wifi access point");
        }
    }
}



#ifdef __cplusplus
}
#endif
