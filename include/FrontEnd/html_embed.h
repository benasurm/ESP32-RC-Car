#pragma once 

#include "../WiFiConnection/wifi_conn.h"
#include <Arduino.h>


// Message constants
const char html_init_fail_msg[] = "** Failed to initialize HTML form";
const char wifi_err_code[] = "WiFi error code: ";


// Functionality methods
void InitializeHTMLForm(String &html_embed);