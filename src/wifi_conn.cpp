#include "../include/WiFiConnection/wifi_conn.h"

void ReadNetInfo(NetInfo &net_info)
{
    // Reading the network SSID and password
    SerialIO::PrintLn(enter_ssid_msg);
    SerialIO::ReadLn(net_info.ssid);

    SerialIO::PrintLn(enter_passwd_msg);
    SerialIO::ReadLn(net_info.passwd);
}

void ConnectToWifi()
{
    // Declaring SSID and password char array structure
    NetInfo curr_net_info;
    // Reading network info from user
    ReadNetInfo(curr_net_info);

    // Checking if no connection is already established
    if(WiFi.status() != WL_NO_SHIELD)
    {
        SerialIO::PrintLn(already_conn_msg);
        return;
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
    }
    
    // If connection to network was successfull
    if(WiFi.status() == WL_CONNECTED)
    {
        SerialIO::PrintEmptyLn();
        SerialIO::PrintLn(conn_ok_msg);
    }
}