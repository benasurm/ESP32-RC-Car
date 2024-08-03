#pragma once

#include <WiFi.h>
#include "../SerialIO/serial_io.h"
#include "Arduino.h"

const char enter_ssid_msg[] = "Enter the network SSID: ";
const char enter_passwd_msg[] = "Enter the password of the network : ";
const char begin_conn_msg[] = "** Connecting";
const char already_conn_msg[] = "** Connection already established. Closing connection attempt.";
const char conn_fail_msg[] = "** Connection to the network failed";
const char no_ssid_avail_msg[] = "** Network by given SSID is not found";
const char conn_ok_msg[] = "** Connection to the network successfull";

struct NetInfo
{
    char ssid[msg_size];
    char passwd[msg_size];
};

void ReadNetInfo(NetInfo &net_info);
void ConnectToWifi();