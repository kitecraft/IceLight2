#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "esp_system.h"
#include "../IceLight_config.h"


#ifdef __cplusplus
extern "C" {
#endif

static const char *ICEFS_TAG = "ICEFS";
wl_handle_t s_wl_handle = WL_INVALID_HANDLE;
const char* ffsBase = FFS_BASE_PATH;

esp_err_t InitFS()
{
    const esp_vfs_fat_mount_config_t mount_config = {
            .format_if_mount_failed = true,
            .max_files = 4,
            .allocation_unit_size = CONFIG_WL_SECTOR_SIZE
    };
    esp_err_t err = esp_vfs_fat_spiflash_mount_rw_wl(ffsBase, FFS_PARTITION_LABEL, &mount_config, &s_wl_handle);
    if (err != ESP_OK) {
        ESP_LOGE(ICEFS_TAG, "Failed to mount FATFS (%s)", esp_err_to_name(err));
    }
    return err;
}


#ifdef __cplusplus
}
#endif

