#pragma once

#include "esp_camera.h"
#include "../HTTPServer/http_server.h"

// Status messages

const char cam_init_ok_msg[] = "** Camera initialization successfull";
const char err_500_msg[] = "** Frame retrieval failed";
const char capture_ok_msg[] = "** ESP Camera capture successfull";
const char hdr_set_ok_msg[] = "** Header set successfully";
const char inv_req_ptr_msg[] = "** Invalid request pointer";
const char inv_img_format_msg[] = "** Invalid image format";
const char pkt_sent_ok_msg[] = "** JPEG Packet set successfully";
const char img_ct[] = "image/jpeg";

// Functionality method declaration

esp_err_t CameraInit();
void PrintInitRes(esp_err_t &init_res);
size_t ConvertTokB(size_t &len);
size_t ConvertToms(size_t &begin, size_t &end);
esp_err_t CaptureHandler(httpd_req_t* req);