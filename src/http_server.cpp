#include "../include/HTTPServer/http_server.h"
#include "../include/Camera/camera.h"

// Index server handle
httpd_handle_t page_server_handle = NULL;
httpd_handle_t capture_server_handle = NULL;

// HTML page string
String index_text;

void PrintResultCode(esp_err_t &result)
{
    switch(result)
    {
        case ESP_OK:
            SerialIO::PrintLn(ok_msg);
        break;
        case ESP_ERR_INVALID_ARG:
            SerialIO::PrintLn(null_arg_msg);
        break;
        case ESP_ERR_HTTPD_ALLOC_MEM:
            SerialIO::PrintLn(mem_alloc_fail_msg);
        break;
        case ESP_ERR_HTTPD_TASK:
            SerialIO::PrintLn(httpd_task_fail_msg);
        break;
        default:
        break;
    }
}

void StartHTTPServer()
{
    // Instantiate default server configuration object (struct)
    httpd_config_t server_config = HTTPD_DEFAULT_CONFIG();

    // Empty the page and capture server handles
    page_server_handle = NULL;
    capture_server_handle = NULL;

    // Start the page httpd server
    esp_err_t result = httpd_start(&page_server_handle, &server_config);

    // Definition of URI handlers

    httpd_uri_t motor_axis_uri =
    {
        .uri = "/motor_axis",
        .method = HTTP_POST,
        .handler = AxisHandler,
        .user_ctx = NULL
    };

    httpd_uri_t index_uri
    {
        .uri = "/",
        .method = HTTP_GET,
        .handler = IndexHandler,
        .user_ctx = NULL
    };

    httpd_uri_t capture_uri
    {
        .uri = "/jpg_capture",
        .method = HTTP_GET,
        .handler = CaptureHandler,
        .user_ctx = NULL 
    };

    // Declare page handlers
    httpd_uri_t page_handlers[] = { motor_axis_uri, index_uri };
    // Register page URI handlers
    InitializeServer(result, page_handlers, 2, PAGE);
    PrintResultCode(result);

    if(result == ESP_OK)
    {
        // Declare capture handlers
        httpd_uri_t capture_handlers[] = { capture_uri };
        // Set capture server port to 81
        server_config.server_port++;
        server_config.ctrl_port++;
        // Start capture server instance
        result = httpd_start(&capture_server_handle, &server_config);
        // Register capture URI handlers
        InitializeServer(result, capture_handlers, 1, CAPTURE);
        PrintResultCode(result);
        // Print the page URL
        if(result == ESP_OK)
        {
            SerialIO::Print(conn_url_msg);
            Serial.println(WiFi.localIP());
        }
    }
}

void InitializeServer(esp_err_t &result, httpd_uri_t handlers[], size_t count, const ServerHandleFlag &handle_flag)
{
    if(result == ESP_OK)
    {
        RegisterHandlers(handlers, count, handle_flag);
    }
}

void RegisterHandlers(httpd_uri_t handlers[], size_t count, const ServerHandleFlag &handle_flag)
{
    for(size_t i = 0; i < count; i++)
    {
        switch (handle_flag)
        {
            case PAGE:
                httpd_register_uri_handler(page_server_handle, &handlers[i]);
                break;

            case CAPTURE:
                httpd_register_uri_handler(capture_server_handle, &handlers[i]);
                break;
        
            default:
                break;
        }
    }
}

void PrintRecvResult(httpd_req_t* req, size_t recv_size)
{
    switch (recv_size)
    {
        case 0:
            SerialIO::PrintLn(empty_http_req_msg);
            break;
        case HTTPD_SOCK_ERR_INVALID:
            SerialIO::PrintLn(inv_req_arg_msg);
            break;
        case HTTPD_SOCK_ERR_TIMEOUT:
            SerialIO::PrintLn(sock_err_msg);
            httpd_resp_send_408(req);
            break;
        case HTTPD_SOCK_ERR_FAIL:
            SerialIO::PrintLn(sock_fail_msg);
            break;
        default: 
            break;
    }
}

void PrintRespResult(httpd_req_t* req, int resp_res)
{
    switch (resp_res)
    {
        case ESP_OK:
            SerialIO::PrintLn(resp_ok_msg);
            break;
        case ESP_ERR_INVALID_ARG:
            SerialIO::PrintLn(null_req_ptr_msg);
            break;
        case ESP_ERR_HTTPD_RESP_HDR:
            SerialIO::PrintLn(too_large_buff_msg);
            break;
        case ESP_ERR_HTTPD_RESP_SEND:
            SerialIO::PrintLn(raw_send_err_msg);
            break;
        case ESP_ERR_HTTPD_INVALID_REQ:
            SerialIO::PrintLn(inval_req_msg);
            break;
        default: 
            break;
    }
}

void ExtractAxisValues(char *http_msg_body)
{
    // Flag that marks if loop has detected a parameter
    bool in_param = false;
    if(http_msg_body != NULL)
    {
        // Temp token of the http message body field/values
        char *token = strtok(http_msg_body, http_body_delim);
        
        // Temp pointers to fields, values
        char *field;
        char *value;

        int count = 0;
        FieldValue temp[motor_val_msg_size];

        // Read http message body fields/values until token is null
        while(token != nullptr)
        {
            // Case when no parameter value is being read
            if(!in_param)
            {
                in_param = true;
                field = token;
            }
            // Case when a parameter is read and it's value also must be read
            else
            {
                in_param = false;
                value = token;
                temp[count].field = field;
                temp[count].value = value;
                count++;
            }
            // Returns a pointer to a char range between delimiters
            token = strtok(NULL, http_body_delim);
        }
        // Set new PWM values
        SetNewPWMValues(temp, count);
    }
}

esp_err_t IndexHandler(httpd_req_t* req)
{
    String html_page;
    httpd_resp_set_type(req, "text/html");
    InitializeHTMLForm(html_page);
    int resp_res = httpd_resp_send(req, &html_page[0], strlen(&html_page[0]));
    PrintRespResult(req, resp_res);
    return resp_res;
}

