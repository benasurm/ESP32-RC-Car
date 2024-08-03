#include "../include/SerialIO/serial_io.h"
#include "../include/WiFiConnection/wifi_conn.h"
#include "../include/HTTPServer/http_server.h"

void setup() 
{
    // Start Serial IO with 115200 baud rate
    Serial.begin(115200);
    SerialIO::PrintLn(boot_ok_msg);

    // Begin Wifi connection process
    ConnectToWifi();

    // TODO - call a function that creates an instance of HTTP Server
    StartHTTPServer();
}

void loop() 
{

}