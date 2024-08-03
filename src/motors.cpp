#include "../include/Motors/motors.h"
#include "../include/HTTPServer/http_server.h"

// Functionality methods

void SetNewPWMValues(FieldValue field_val[], int len)
{
    for(int i = 0; i <= len; i++)
    {
        switch (field_val[i].field[0])
        {
            case 'x':
                MotorPWMValues.x = std::atoi(field_val[i].value); 
                ValidatePWMValue(MotorPWMValues.x);
                // Do something with the changed values
                break;
            case 'y':
                MotorPWMValues.y = std::atoi(field_val[i].value); 
                ValidatePWMValue(MotorPWMValues.y);
                // Do something with the changed values
                break;
            default:
                break;
        }
    }
}

void ValidatePWMValue(int &value)
{
    if(value > max_pwm)
    {
        value = max_pwm;
    }
    else if(value < min_pwm)
    {
        value = min_pwm;
    }
}

void PrintPWMValues()
{
    SerialIO::PrintLn(change_pwm_val_msg);

    SerialIO::Print("x: \0");
    SerialIO::Print(MotorPWMValues.x);
    SerialIO::PrintEmptyLn();

    SerialIO::Print("y: \0");
    SerialIO::Print(MotorPWMValues.y);
    SerialIO::PrintEmptyLn();
}