#include "IceServer.h"
#include "esp_log.h"

static const char *ICESERVER_TAG = "ICESERVER";

/* An HTTP GET handler */
static esp_err_t hello_get_handler(httpd_req_t *req)
{
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
                ESP_LOGI(ICESERVER_TAG, "Found URL query parameter => query1=%s", param);
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


esp_err_t StartIceServer()
{
    
    httpd_handle_t g_Httpserver = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(ICESERVER_TAG, "Starting server on port: '%d'", config.server_port);
    esp_err_t err = httpd_start(&g_Httpserver, &config);
    if (err == ESP_OK) {
        
    httpd_uri_t hello = {
        .uri       = "/hello",
        .method    = HTTP_GET,
        .handler   = hello_get_handler,
        /* Let's pass response string in user
        * context to demonstrate it's usage */
        .user_ctx  = nullptr,
    };

        // Set URI handlers
        ESP_LOGI(ICESERVER_TAG, "Registering URI handlers");
        httpd_register_uri_handler(g_Httpserver, &hello);
        //httpd_register_uri_handler(server, &echo);
        //httpd_register_uri_handler(server, &ctrl);

        
        //httpd_register_basic_auth(server);
        
        return ESP_OK;
    }

    ESP_LOGI(ICESERVER_TAG, "Error starting server!");
    return err;
}