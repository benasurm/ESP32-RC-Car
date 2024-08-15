#include "../include/Camera/camera.h"
#include "esp_camera.h"
#include "../include/SerialIO/serial_io.h"
#include <Arduino.h>
#include "esp_timer.h"

// ESP32 AI_THINKER Pin Map

#define CAMERA_MODEL_AI_THINKER
//#define CAMERA_MODEL_WROVER_KIT

#if defined(CAMERA_MODEL_WROVER_KIT)
#define CAM_PIN_PWDN    -1
#define CAM_PIN_RESET   -1 
#define CAM_PIN_XCLK    21
#define CAM_PIN_SIOD    26
#define CAM_PIN_SIOC    27

#define CAM_PIN_D7      35
#define CAM_PIN_D6      34
#define CAM_PIN_D5      39
#define CAM_PIN_D4      36
#define CAM_PIN_D3      19
#define CAM_PIN_D2      18
#define CAM_PIN_D1       5
#define CAM_PIN_D0       4
#define CAM_PIN_VSYNC   25
#define CAM_PIN_HREF    23
#define CAM_PIN_PCLK    22

#elif defined(CAMERA_MODEL_AI_THINKER)
#define CAM_PIN_PWDN    32
#define CAM_PIN_RESET   -1 
#define CAM_PIN_XCLK    0
#define CAM_PIN_SIOD    26
#define CAM_PIN_SIOC    27

#define CAM_PIN_D7      35
#define CAM_PIN_D6      34
#define CAM_PIN_D5      39
#define CAM_PIN_D4      36
#define CAM_PIN_D3      21
#define CAM_PIN_D2      19
#define CAM_PIN_D1      18
#define CAM_PIN_D0       5
#define CAM_PIN_VSYNC   25
#define CAM_PIN_HREF    23
#define CAM_PIN_PCLK    22

#endif

camera_config_t camera_config =
{
    .pin_pwdn  = CAM_PIN_PWDN,
    .pin_reset = CAM_PIN_RESET,
    .pin_xclk = CAM_PIN_XCLK,
    .pin_sccb_sda = CAM_PIN_SIOD,
    .pin_sccb_scl = CAM_PIN_SIOC,
    .pin_d7 = CAM_PIN_D7,
    .pin_d6 = CAM_PIN_D6,
    .pin_d5 = CAM_PIN_D5,
    .pin_d4 = CAM_PIN_D4,
    .pin_d3 = CAM_PIN_D3,
    .pin_d2 = CAM_PIN_D2,
    .pin_d1 = CAM_PIN_D1,
    .pin_d0 = CAM_PIN_D0,
    .pin_vsync = CAM_PIN_VSYNC,
    .pin_href = CAM_PIN_HREF,
    .pin_pclk = CAM_PIN_PCLK,
    .xclk_freq_hz = 20000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,
    .pixel_format = PIXFORMAT_JPEG,
    .frame_size = FRAMESIZE_VGA, // 640x480 resolution
    .jpeg_quality = 35,
    .fb_count = 1,
    .fb_location = CAMERA_FB_IN_DRAM,
    .grab_mode = CAMERA_GRAB_LATEST
};

esp_err_t CameraInit()
{
    // Initialize pin mode and power on the camera if PWDN pin is defined
    if(CAM_PIN_PWDN != -1)
    {
        pinMode(CAM_PIN_PWDN, OUTPUT);
        digitalWrite(CAM_PIN_PWDN, LOW);
    }
    // Initialize the camera with given configuration
    esp_err_t init_res = esp_camera_init(&camera_config);
    PrintInitRes(init_res);
    return init_res;
}

void PrintInitRes(esp_err_t &init_res)
{
    switch (init_res)
    {
        case ESP_OK:
            SerialIO::PrintLn(cam_init_ok_msg);
            break;
        default:
            break;
    }
}

size_t ConvertTokB(size_t &len)
{
    return len / 1000;
}

size_t ConvertToms(int64_t &begin, int64_t &end)
{
    return (end - begin) / 1000;
}

esp_err_t CaptureHandler(httpd_req_t *req)
{
    // Declaration of pointer to the camera frame buffer
    camera_fb_t *fb = NULL;
    // Declaration of HTTP Response result
    esp_err_t resp_res;
    // Size of the JPG image in the frame buffer
    size_t fb_len = 0;
    // Save the starting time of capture
    int64_t begin_time = esp_timer_get_time();

    // A shitty hack to fix frame buffer storing old images
    fb = esp_camera_fb_get();
    esp_camera_fb_return(fb);
    fb = NULL;
    fb = esp_camera_fb_get();
    if(!fb)
    {
        SerialIO::PrintLn(err_500_msg);
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    //SerialIO::PrintLn(capture_ok_msg);

    // Setting 'Content-Type' header to 'image/jpeg' 
    resp_res = httpd_resp_set_type(req, img_ct);
    // Checking result of operation
    switch (resp_res)
    {
        case ESP_OK:
            //SerialIO::PrintLn(hdr_set_ok_msg);
            break;
        default:
            PrintResultCode(resp_res);
            esp_camera_fb_return(fb);
            httpd_resp_send_500(req);
            return ESP_FAIL;
    }

    // Setting 'Access-Control-Allow-Origin' header to '*' 
    resp_res = httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    // Checking result of operation
    switch (resp_res)
    {
        case ESP_OK:
            //SerialIO::PrintLn(hdr_set_ok_msg);
            break;
        default:
            PrintResultCode(resp_res);
            esp_camera_fb_return(fb);
            httpd_resp_send_500(req);
            return ESP_FAIL;
    }
    
    if(fb->format == PIXFORMAT_JPEG)
    {
        resp_res = httpd_resp_send(req, (const char *)fb->buf, fb->len);
        switch (resp_res)
        {
            case ESP_OK:
                //SerialIO::PrintLn(pkt_sent_ok_msg);
                fb_len = fb->len;
                break;
            default:
                PrintResultCode(resp_res);
                httpd_resp_send_500(req);
                esp_camera_fb_return(fb);
                return ESP_FAIL;
        }
    }
    else
    {
        SerialIO::PrintLn(inv_img_format_msg);
        httpd_resp_send_500(req);
        esp_camera_fb_return(fb);
        return ESP_FAIL;
    }

    // Return camera buffer to free memory
    esp_camera_fb_return(fb);

    // Stop the timer, print sent data size and duration to Serial
    int64_t end_time = esp_timer_get_time();

    // Format and print result message
    //Serial.printf("JPG packet sent: %uKb; %u ms", ConvertTokB(fb_len), ConvertToms(begin_time, end_time));
    return ESP_OK;
}

