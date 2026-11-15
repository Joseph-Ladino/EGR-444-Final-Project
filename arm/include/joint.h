#pragma once

#include <ESP32Servo.h>

struct Joint {
    int servo_pin;
    int min_deg, max_deg;
    int min_us, max_us;
    Servo servo;
};