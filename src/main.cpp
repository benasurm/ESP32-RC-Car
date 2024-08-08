#include "../include/SerialIO/serial_io.h"
#include "../include/WiFiConnection/wifi_conn.h"
#include "../include/HTTPServer/http_server.h"
#include "../include/Motors/motors.h"
#include "../include/Camera/camera.h"

void setup() 
{
    // Start Serial IO with 115200 baud rate
    Serial.begin(115200);
    SerialIO::PrintLn(boot_ok_msg);

    // Initialize motors
    InitializeMotors();

    // Initialize camera module
    esp_err_t cam_init_res = CameraInit();
    // Check whether camera initialization is successfull
    if(cam_init_res != ESP_OK)
    {
        Serial.printf("** Camera initialization failed, error code: 0x%x", cam_init_res);
        SerialIO::PrintEmptyLn();
        return;
    }

    // Begin Wifi connection process
    ConnectToWifi();

    // Creating an instance of HTTP server and assigning URI handlers
    StartHTTPServer();
}

void loop() 
{

}