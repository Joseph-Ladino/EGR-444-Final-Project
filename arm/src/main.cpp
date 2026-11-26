#include "joint.h"
#include <Arduino.h>

#define PIN_GRIPPER 13
#define PIN_WRIST_ROTATE 12
#define PIN_WRIST_FLEX 14
#define PIN_ELBOW 27
#define PIN_SHOULDER_L 26
#define PIN_SHOULDER_R 25
#define PIN_BASE 33

Joint gripper{.servo_pin = PIN_GRIPPER, .max_deg=60, .neutral_deg = 0};
Joint wrist_rotate{.servo_pin = PIN_WRIST_ROTATE};
Joint wrist_flex{.servo_pin = PIN_WRIST_FLEX};
Joint elbow{.servo_pin = PIN_ELBOW, .neutral_deg=180};
Joint shoulder_l{.servo_pin = PIN_SHOULDER_L, .neutral_deg=0};
Joint shoulder_r{.servo_pin = PIN_SHOULDER_R, .neutral_deg=180};
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

void set_arm_neutral() {
    for (int i = 0; i < num_joints; i++) {
        auto &joint = *joints[i];
        auto &servo = joint.servo;

        servo.write(joint.neutral_deg);
    }
}
void set_arm_detached() {
    for (int i = 0; i < num_joints; i++) {
        auto &joint = *joints[i];
        auto &servo = joint.servo;

        servo.detach();
    }
}

void set_arm_released() {
    for (int i = 0; i < num_joints; i++) {
        auto &joint = *joints[i];
        auto &servo = joint.servo;

        servo.release();
    }
}

void set_arm_attached() {
    for (int i = 0; i < num_joints; i++) {
        auto &joint = *joints[i];
        auto &servo = joint.servo;

        servo.attach(joint.servo_pin);
    }
}

void arm_lunge() {
    gripper.servo.write(gripper.max_deg);
    wrist_flex.servo.write(wrist_flex.neutral_deg);
    wrist_rotate.servo.write(wrist_flex.neutral_deg);
    elbow.servo.write(elbow.min_deg);
    shoulder_l.servo.write(shoulder_l.max_deg);
    shoulder_r.servo.write(180 - shoulder_r.max_deg);
}

void loop() {
    if (Serial.available() > 0) {
        auto cmd = Serial.readStringUntil('\n');

        if(cmd.startsWith("lunge")) arm_lunge();
        else if(cmd.startsWith("set arm ")) {
            auto subcmd = cmd.substring(8);
            subcmd.trim();

            if(subcmd == "neutral") set_arm_neutral();
            else if(subcmd == "attached") set_arm_attached();
            else if(subcmd == "detached") set_arm_detached();
            else if(subcmd == "released") set_arm_released();
        }
        else if (cmd.startsWith("set ")) {
            auto joint_idx = cmd.substring(4, 5).toInt();
            auto subcmd = cmd.substring(6);
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
