#pragma once

#include "esp_camera.h"

// Status messages

const char cam_init_ok_msg[] = "** Camera initialization successfull";

// Functionality method declaration

esp_err_t CameraInit();
void PrintInitRes(esp_err_t &init_res);