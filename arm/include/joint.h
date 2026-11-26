#pragma once

#include <ESP32Servo.h>

struct Joint {
    int servo_pin;
    int min_deg = 0, max_deg = 180, neutral_deg = 90;
    int min_us = 0, max_us = 0;
    Servo servo;
};