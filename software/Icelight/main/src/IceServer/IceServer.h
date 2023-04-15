#pragma once
#include <string.h>
#include <fcntl.h>
#include "esp_http_server.h"
#include "cJSON.h"

static httpd_handle_t g_Httpserver = NULL;
esp_err_t StartIceServer();