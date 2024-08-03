#include "../include/SerialIO/serial_io.h"

namespace SerialIO
{
    void Print(char text[])
    {
        int length = 0;
        while(text[length] != '\0')
        {
            Serial.print(text[length]);
            length++;   
        }
    }

    void Print(const char text[])
    {
        int length = 0;
        while(text[length] != '\0')
        {
            Serial.print(text[length]);
            length++;   
        }
    }

    void Print(int a)
    {
        Serial.print(a);
    }

    void PrintLn(char text[])
    {
        Print(text);
        PrintEmptyLn();
    }

    void PrintLn(const char text[])
    {
        Print(text);
        PrintEmptyLn();
    }

    void PrintEmptyLn()
    {
        Serial.println();
    }

    void ReadLn(char buffer[])
    {
        int size = 0;
        char temp_ch;
        bool wait = true;
        while(true)
        {
            if(Serial.available() > 0)
            {
                temp_ch = Serial.read();
                if(temp_ch == '\n' || temp_ch == '\r')
                {
                    while (Serial.available() > 0 && 
                        (Serial.peek() == '\n' || Serial.peek() == '\r')) 
                    {
                        Serial.read();
                    }
                    break;
                }
                else if(size < msg_size - 1)
                {
                    buffer[size] = temp_ch;
                    size++;
                }
            }
        }
        buffer[size] = '\0';
    }
}
