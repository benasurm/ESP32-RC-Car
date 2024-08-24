#pragma once

#include <WiFi.h>
#include "../SerialIO/serial_io.h"
#include "Arduino.h"

const char enter_ssid_msg[] = "Enter the network SSID: ";
const char enter_passwd_msg[] = "Enter the password of the network: ";
const char begin_conn_msg[] = "** Connecting";
const char already_conn_msg[] = "** Connection already established.";
const char conn_fail_msg[] = "** Connection to the network failed";
const char no_ssid_avail_msg[] = "** Network by given SSID is not found";
const char conn_ok_msg[] = "** Connection to the network successfull";
const char try_again_msg[] = "** Try connecting to network again";

// Declaring SSID and password char array structure
struct
{
    char ssid[msg_size];
    char passwd[msg_size];
} curr_net_info;

void ReadNetInfo();
void ConnectToWifi();
