#pragma once

#include "Arduino.h"

const int msg_size = 64; 

const char boot_ok_msg[] = "** ESP32 Serial is ready!";

namespace SerialIO
{
    void Print(char text[]);
    void Print(const char text[]);
    void Print(int a);
    void PrintLn(char text[]);
    void PrintLn(const char text[]);
    void PrintEmptyLn();
    void ReadLn(char buffer[]);
}




