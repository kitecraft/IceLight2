#pragma once
#define DEVICE_NAME "IceLight"
#define DEVICE_AUTHOR "Kitecraft"
#define DEVICE_VERSION "0.1.0" //Major.Minor.Patch

#define B_TO_S(b)(b ? "true" : "false")

#define CONFIG_ICELIGHT_DEVICE_NAME "IceLight"
#define CONFIG_ICELIGHT_MAX_STRING_LEN 32

#define STACK_SIZE  8192    //Task stack size

//Wifi
#define CONFIG_ICENETWORK_MAXIMUM_RETRY 5
#define CONFIG_ICENETWORK_MAXIMUM_MAX_CONNECTIONS 4
#define CONFIG_ICENETWORK_SOFTAP_DEFAULT_CHANNEL 6

//LED centered
#define CONFIG_ICELIGHT_DEFAULT_BRIGHTNESS 64

//IceFS - FatFS
#define FFS_PARTITION_LABEL "storage"
#define FFS_BASE_PATH "/ffs"

//IceServer
#define WWW_FILE_PATH "/html"

