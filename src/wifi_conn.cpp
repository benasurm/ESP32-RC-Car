#include "../include/WiFiConnection/wifi_conn.h"

const u8_t max_fail_count = 5;
u8_t fail_count;

void ReadNetInfo()
{
    // Reading the network SSID and password
    SerialIO::PrintLn(enter_ssid_msg);
    SerialIO::ReadLn(curr_net_info.ssid);

    SerialIO::PrintLn(enter_passwd_msg);
    SerialIO::ReadLn(curr_net_info.passwd);
}

void ConnectToWifi()
{
    // Reading network info from user
    ReadNetInfo();
    // Connection fail counter
    fail_count = 0;

    // Checking if no connection is already established
    if(WiFi.status() != WL_NO_SHIELD)
    {
        SerialIO::PrintLn(already_conn_msg);
        WiFi.disconnect();
    }

    // Initialization of WiFi library's network settings
    WiFi.begin(curr_net_info.ssid, curr_net_info.passwd);
    SerialIO::PrintLn(begin_conn_msg);
    
    // Checking if WiFi is connected
    while(WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        switch (WiFi.status())
        {
            // Waiting for connection case
            case WL_IDLE_STATUS:
                SerialIO::Print(".");
                delay(1000);
            break;
            // Connection failure case
            case WL_CONNECT_FAILED:
                SerialIO::PrintEmptyLn();
                SerialIO::PrintLn(conn_fail_msg);
            break;
            // Unavailable SSID case
            case WL_NO_SSID_AVAIL:
                SerialIO::PrintLn(no_ssid_avail_msg);
            break;
            default:
            break;
        }
        fail_count++;
        if(fail_count == max_fail_count)
        {
            SerialIO::PrintLn(try_again_msg);
            ConnectToWifi();
        }
    }
    
    // If connection to network was successfull
    if(WiFi.status() == WL_CONNECTED)
    {
        SerialIO::PrintEmptyLn();
        SerialIO::PrintLn(conn_ok_msg);
    }
}