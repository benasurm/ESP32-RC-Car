#include "../include/Motors/motors.h"
#include "../include/HTTPServer/http_server.h"

// Motor GPIO pins (camera is away from observer's perspective)

int RF_GPIO = 14;
int RB_GPIO = 2;
int LF_GPIO = 13;
int LB_GPIO = 15;

// Functionality methods

void InitializeMotors()
{
    // Configure motors to behave like outputs
    pinMode(RF_GPIO, OUTPUT);
    pinMode(RB_GPIO, OUTPUT);
    pinMode(LF_GPIO, OUTPUT);
    pinMode(LB_GPIO, OUTPUT);

    // Disable all motors
    digitalWrite(RF_GPIO, LOW);
    digitalWrite(RB_GPIO, LOW);
    digitalWrite(LF_GPIO, LOW);
    digitalWrite(LB_GPIO, LOW);
}

void SetNewPWMValues(FieldValue field_val[], int len)
{
    for(int i = 0; i <= len; i++)
    {
        switch (field_val[i].field[0])
        {
            case 'x':
                MotorPWMValues.x = std::atoi(field_val[i].value); 
                ValidatePWMValue(MotorPWMValues.x);
                break;
            case 'y':
                MotorPWMValues.y = std::atoi(field_val[i].value); 
                ValidatePWMValue(MotorPWMValues.y);
                break;
            default:
                break;
        }
    }
    // Apply the newly read PWM values
    ApplyPWMValues();
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

int SmallerPWMVal()
{
    int new_val = 100 + std::abs(MotorPWMValues.y) - std::abs(MotorPWMValues.x) / 2;
    return std::min(max_pwm, new_val);
}

int GreaterPWMVal()
{
    int new_val = 100 + std::abs(MotorPWMValues.y) + std::abs(MotorPWMValues.x) / 2; 
    return std::min(max_pwm, new_val);
}

void ApplyPWMValues()
{
    if(MotorPWMValues.y < 0)
    {
        analogWrite(LF_GPIO, 0);
        analogWrite(RF_GPIO, 0);
        if(MotorPWMValues.x >= 0)
        {
            analogWrite(LB_GPIO, GreaterPWMVal());
            analogWrite(RB_GPIO, SmallerPWMVal());
        }
        else
        {
            analogWrite(LB_GPIO, SmallerPWMVal());
            analogWrite(RB_GPIO, GreaterPWMVal());
        }
    }
    else if(MotorPWMValues.y > 0)
    {
        analogWrite(LB_GPIO, 0);
        analogWrite(RB_GPIO, 0);
        if(MotorPWMValues.x >= 0)
        {
            analogWrite(LF_GPIO, GreaterPWMVal());
            analogWrite(RF_GPIO, SmallerPWMVal());
        }
        else
        {
            analogWrite(LF_GPIO, SmallerPWMVal());
            analogWrite(RF_GPIO, GreaterPWMVal());
        } 
    }
    else if(MotorPWMValues.y == 0)
    {
        analogWrite(LF_GPIO, 0);
        analogWrite(RF_GPIO, 0);
        analogWrite(LB_GPIO, 0);
        analogWrite(RB_GPIO, 0);
        if(MotorPWMValues.x >= 0)
        {
            analogWrite(LF_GPIO, MotorPWMValues.x);
            analogWrite(RB_GPIO, MotorPWMValues.x);
        }
        else
        {
            analogWrite(LB_GPIO, std::abs(MotorPWMValues.x));
            analogWrite(RF_GPIO, std::abs(MotorPWMValues.x));          
        }
    }
}