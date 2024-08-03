#pragma once

#include "../SerialIO/serial_io.h"
#include "../FieldValue/field_value.h"
#include <stdlib.h>
#include <stdio.h>

// Constants

const int max_pwm = 255;
const int min_pwm = -255;

// Predefined messages

const char change_pwm_val_msg[] = "** The values of the motor PWM have been modified:";

// Structure that stores motor PWM values

struct
{
    int x = 0;
    int y = 0;
} MotorPWMValues;

// Functionality methods

void SetNewPWMValues(FieldValue field_val[], int len);
void ValidatePWMValue(int &value);
void PrintPWMValues();
