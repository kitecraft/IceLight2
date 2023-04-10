#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <cstdlib>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "esp_log.h"

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



#ifdef __cplusplus
}
#endif
