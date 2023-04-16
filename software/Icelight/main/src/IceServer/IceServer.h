#pragma once
#include <string.h>
#include <fcntl.h>
#include "esp_log.h"
#include "esp_http_server.h"
#include "cJSON.h"
#include "esp_vfs.h"
#include "../IceLight_config.h"
#include "src/Utilities/MemUsage.h"
#include "src/Utilities/StringUtilities.h"

static httpd_handle_t g_Httpserver = NULL;
static const char *ICESERVER_TAG = "ICESERVER";

#define FILE_PATH_MAX (ESP_VFS_PATH_MAX + 128)
#define SCRATCH_BUFSIZE (10240)

typedef struct rest_server_context {
    char base_path[ESP_VFS_PATH_MAX + 1];
    //char scratch[SCRATCH_BUFSIZE];
} rest_server_context_t;

/* An HTTP GET handler */
static esp_err_t hello_get_handler(httpd_req_t *req)
{
    ESP_LOGI(ICESERVER_TAG, "Starting hello_get_handler...");
    char*  buf;
    size_t buf_len;


    /* Read URL query string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = (char*)malloc(buf_len);
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            ESP_LOGI(ICESERVER_TAG, "Found URL query => %s", buf);
            char param[32];
            /* Get value of expected key from query string */
            if (httpd_query_key_value(buf, "query1", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(ICESERVER_TAG, "Found URL query parameter => query1%s", param);
            }
            if (httpd_query_key_value(buf, "query3", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(ICESERVER_TAG, "Found URL query parameter => query3=%s", param);
            }
            if (httpd_query_key_value(buf, "query2", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(ICESERVER_TAG, "Found URL query parameter => query2=%s", param);
            }
        }
        free(buf);
    }

    /* Set some custom headers */
    httpd_resp_set_hdr(req, "Custom-Header-1", "Custom-Value-1");
    httpd_resp_set_hdr(req, "Custom-Header-2", "Custom-Value-2");

    /* Send response with custom headers and body set as the
     * string passed in user context*/
    const char* resp_str = "Hello World"; //(const char*) req->user_ctx;
    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(ICESERVER_TAG, "Request headers lost");
    }
    return ESP_OK;
}

#define CHECK_FILE_EXTENSION(filename, ext) (strcasecmp(&filename[strlen(filename) - strlen(ext)], ext) == 0)

/* Set HTTP response content type according to file extension */
static esp_err_t set_content_type_from_file(httpd_req_t *req, const char *filepath)
{
    const char *type = "text/plain";
    if (CHECK_FILE_EXTENSION(filepath, ".htm")) {
        type = "text/html";
    }
    else if (CHECK_FILE_EXTENSION(filepath, ".js")) {
        type = "application/javascript";
    }
    else if (CHECK_FILE_EXTENSION(filepath, ".css")) {
        type = "text/css";
    }
    // else if (CHECK_FILE_EXTENSION(filepath, ".png")) {
    //    type = "image/png";
    //}
    else if (CHECK_FILE_EXTENSION(filepath, ".ico")) {
        type = "image/x-icon";
    }
    //else if (CHECK_FILE_EXTENSION(filepath, ".svg")) {
    //    type = "text/xml";
    //}
    return httpd_resp_set_type(req, type);
}


static esp_err_t HandleIndex(httpd_req_t *req)
{
    ESP_LOGI(ICESERVER_TAG, "Starting HandleIndex...");
    char filepath[FILE_PATH_MAX];

    rest_server_context_t *rest_context = (rest_server_context_t *)req->user_ctx;
    strlcpy(filepath, rest_context->base_path, sizeof(filepath));
    strlcat(filepath, "/index.htm", sizeof(filepath));

    int fd = open(filepath, O_RDONLY, 0);
    ESP_LOGI(ICESERVER_TAG, "Opening file %s", filepath);
    if (fd == -1) {
        ESP_LOGE(ICESERVER_TAG, "Failed to open file : %s", filepath);
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read existing file");
        return ESP_FAIL;
    }

    set_content_type_from_file(req, filepath);

    //char *chunk = rest_context->scratch;
    char *chunk = (char*)ps_calloc(SCRATCH_BUFSIZE, sizeof(char));
    ssize_t read_bytes;
    do {
        /* Read file in chunks into the scratch buffer */
        read_bytes = read(fd, chunk, SCRATCH_BUFSIZE);
        if (read_bytes == -1) {
            ESP_LOGE(ICESERVER_TAG, "Failed to read file : %s", filepath);
        } else if (read_bytes > 0) {
            //Do the replacement of %__ICELIGHT__%
            char* newStr = repl_str(chunk, "%{__DEVICE_NAME__}%", "IceLight");
            if(newStr == NULL){
                free(chunk);
                ESP_LOGE(ICESERVER_TAG, "Failed to do a replacement op");
                return ESP_FAIL;
            }
            /* Send the buffer contents as HTTP response chunk */
            if (httpd_resp_send_chunk(req, newStr, strlen(newStr)) != ESP_OK) {
                free(chunk);
                free(newStr);
                close(fd);
                ESP_LOGE(ICESERVER_TAG, "File sending failed!");
                /* Abort sending file */
                httpd_resp_sendstr_chunk(req, NULL);
                /* Respond with 500 Internal Server Error */
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file");
                return ESP_FAIL;
            }
            free(newStr);
        }
    } while (read_bytes > 0);
    free(chunk);
    /* Close file after sending complete */
    close(fd);
    ESP_LOGI(ICESERVER_TAG, "File sending complete");
    /* Respond with an empty chunk to signal HTTP response completion */
    httpd_resp_send_chunk(req, NULL, 0);
    
    return ESP_OK;
}

/* Send HTTP response with the contents of the requested file */
static esp_err_t rest_common_get_handler(httpd_req_t *req)
{
    ESP_LOGI(ICESERVER_TAG, "Starting rest handler...");
    PrintMemUsage();
    char filepath[FILE_PATH_MAX];

    rest_server_context_t *rest_context = (rest_server_context_t *)req->user_ctx;
    strlcpy(filepath, rest_context->base_path, sizeof(filepath));


    if(strlen(req->uri) == 1 || strcasecmp(req->uri,"/index.htm") == 0){
        return HandleIndex(req);
    }
    else {
        strlcat(filepath, req->uri, sizeof(filepath));
    }
    int fd = open(filepath, O_RDONLY, 0);
    ESP_LOGI(ICESERVER_TAG, "Opening file %s", filepath);
    if (fd == -1) {
        ESP_LOGE(ICESERVER_TAG, "Failed to open file : %s", filepath);
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read existing file");
        return ESP_FAIL;
    }

    set_content_type_from_file(req, filepath);

    //char *chunk = rest_context->scratch;
    char *chunk = (char*)ps_calloc(SCRATCH_BUFSIZE, sizeof(char));
    ssize_t read_bytes;
    do {
        /* Read file in chunks into the scratch buffer */
        read_bytes = read(fd, chunk, SCRATCH_BUFSIZE);
        if (read_bytes == -1) {
            ESP_LOGE(ICESERVER_TAG, "Failed to read file : %s", filepath);
        } else if (read_bytes > 0) {
            /* Send the buffer contents as HTTP response chunk */
            if (httpd_resp_send_chunk(req, chunk, read_bytes) != ESP_OK) {
                close(fd);
                free(chunk);
                ESP_LOGE(ICESERVER_TAG, "File sending failed!");
                /* Abort sending file */
                httpd_resp_sendstr_chunk(req, NULL);
                /* Respond with 500 Internal Server Error */
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file");
                return ESP_FAIL;
            }
        }
    } while (read_bytes > 0);
    /* Close file after sending complete */
    close(fd);
    free(chunk);
    ESP_LOGI(ICESERVER_TAG, "File sending complete");
    /* Respond with an empty chunk to signal HTTP response completion */
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static esp_err_t RegisterUriHandler( char* path, httpd_method_t method, esp_err_t (*handler)(httpd_req_t*), void* ctx)
{
    httpd_uri_t uri = {
        .uri       = path,
        .method    = method,
        .handler   = handler,
        .user_ctx  = ctx,
    };
    esp_err_t err = httpd_register_uri_handler(g_Httpserver, &uri);
    if(err != ESP_OK){
        ESP_LOGE(ICESERVER_TAG, "Failed to register the uri %s %i(%s)", path, err, esp_err_to_name(err));
    }
    return err;
}


static esp_err_t StartIceServer()
{
    g_Httpserver = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.core_id = 1;
    config.lru_purge_enable = true;
    config.uri_match_fn = httpd_uri_match_wildcard;

    rest_server_context_t *rest_context = (rest_server_context_t *)calloc(1, sizeof(rest_server_context_t));
    char buf[ESP_VFS_PATH_MAX + 1] = FFS_BASE_PATH;
    strcat(buf, WWW_FILE_PATH);
    strlcpy(rest_context->base_path, buf, sizeof(rest_context->base_path));

    // Start the httpd server
    ESP_LOGI(ICESERVER_TAG, "Starting server on port: '%d'", config.server_port);
    esp_err_t err = httpd_start(&g_Httpserver, &config);
    if (err == ESP_OK) {
        RegisterUriHandler("/hello", HTTP_GET, hello_get_handler, rest_context);
        RegisterUriHandler("/*", HTTP_GET, rest_common_get_handler, rest_context);


        //httpd_register_basic_auth(server);
    
        return ESP_OK;
    }

    ESP_LOGI(ICESERVER_TAG, "Error starting server!");
    return err;
}