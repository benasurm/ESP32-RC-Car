#pragma once

#include "esp_http_server.h"
#include "../include/SerialIO/serial_io.h"
#include "../include/Motors/motors.h"
#include "../include/FrontEnd/html_embed.h"
#include "../FieldValue/field_value.h"

// Predefined constants

const size_t motor_val_msg_size = 32;
const char http_body_delim[] = "=&";

// Server handle enumeration (flag)
enum ServerHandleFlag
{
    PAGE,
    CAPTURE
};


// Result messages

const char ok_msg[] = "** HTTP Server instance created successfully";
const char null_arg_msg[] = "** Creating HTTP Server instance failed: Null argument(s)";
const char mem_alloc_fail_msg[] = "** Creating HTTP Server instance failed: Failed to allocate memory";
const char httpd_task_fail_msg[] = "** Creating HTTP Server instance failed: Failed to launch server task";
const char print_http_req_msg[] = "** Printing HTTP request message content:";
const char empty_http_req_msg[] = "** HTTP request message is empty";
const char inv_req_arg_msg[] = "** Invalid arguments in httpd_req_recv()";
const char sock_err_msg[] = "** Socket timeout/interrupt";
const char sock_fail_msg[] = "** Unrecoverable socket error";
const char motor_resp_msg[] = "** Motor speed data successfully read";
const char resp_ok_msg[] = "** HTTP response packet sent successfully";
const char null_req_ptr_msg[] = "** Null request pointer";
const char too_large_buff_msg[] = "** Essential headers are too large for internal buffer";
const char raw_send_err_msg[] = "** Error in raw send";
const char inval_req_msg[] = "** Invalid request";
const char conn_url_msg[] = "** To access controls, connect to: http://";

// Functionality methods

void PrintResultCode(esp_err_t &result);
void StartHTTPServer();
void InitializeServer(esp_err_t &result, httpd_uri_t handlers[], size_t count, const ServerHandleFlag &handle_flag);
void RegisterHandlers(httpd_uri_t handlers[], size_t count, const ServerHandleFlag &handle_flag);
void PrintRecvResult(httpd_req_t* req, size_t recv_size);
void PrintRespResult(httpd_req_t* req, int resp_res);
void ExtractAxisValues(char *http_msg_body);
esp_err_t IndexHandler(httpd_req_t* req);