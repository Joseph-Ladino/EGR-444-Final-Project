#include "joint.h"
#include <Arduino.h>

#define PIN_GRIPPER 13
#define PIN_WRIST_ROTATE 12
#define PIN_WRIST_FLEX 14
#define PIN_ELBOW 27
#define PIN_SHOULDER_L 26
#define PIN_SHOULDER_R 25
#define PIN_BASE 33

Joint gripper{.servo_pin = PIN_GRIPPER, .min_us = 500, .max_us = 2500};
Joint wrist_rotate{.servo_pin = PIN_WRIST_ROTATE, .min_us = 500, .max_us = 2500};
Joint wrist_flex{.servo_pin = PIN_WRIST_FLEX, .min_us = 500, .max_us = 2500};
Joint elbow{.servo_pin = PIN_ELBOW};
Joint shoulder_l{.servo_pin = PIN_SHOULDER_L};
Joint shoulder_r{.servo_pin = PIN_SHOULDER_R};
Joint base{.servo_pin = PIN_BASE};

Joint *joints[] = {&gripper, &wrist_rotate, &wrist_flex, &elbow, &shoulder_l, &shoulder_r, &base};
constexpr auto num_joints = sizeof(joints) / sizeof(Joint *);

void setup() {
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);

    Serial.begin(115'200);

    for (int i = 0; i < num_joints; i++) {
        auto &joint = *joints[i];
        auto &servo = joint.servo;
        servo.setPeriodHertz(50);

        if (joint.min_us > 0 && joint.max_us > 0)
            servo.attach(joint.servo_pin, joint.min_us, joint.max_us);
        else servo.attach(joint.servo_pin);
    }
}

void loop() {
    if (Serial.available() > 0) {
        auto cmd = Serial.readStringUntil('\n');

        if (cmd.startsWith("set ")) {
            auto joint_idx = cmd.substring(4, 5).toInt();
            auto deg = cmd.substring(6).toFloat();

            if (joint_idx == 4) {
                joints[4]->servo.write(deg);
                joints[5]->servo.write(180-deg);
            } else if (joint_idx == 5) {
                joints[4]->servo.write(180 - deg);
                joints[5]->servo.write(deg);
            } else {
                joints[joint_idx]->servo.write(deg);
            }

            Serial.printf("setting joint %d to %f deg\r\n", joint_idx, deg);
            delay(100);
        }
    }
}
