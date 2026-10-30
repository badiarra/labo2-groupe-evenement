
#include "Motor.h"

Motor::Motor(int ENA, int IN1, int IN2) {
    
    enable = ENA;
    input_1 = IN1;
    input_2 = IN2;
}

void Motor::init() {
    pinMode(enable, OUTPUT);
    pinMode(input_1, OUTPUT);
    pinMode(input_2, OUTPUT);

    stop();
}

void Motor::setSpeed(float val) {
    // val compris entre 0.0 et 1.0
    val = constrain(val, 0.0f, 1.0f);

    int pwm = (int)(val * 255.0f);

    analogWrite(enable, pwm);
}

void Motor::forward() {
    digitalWrite(input_1, HIGH);
    digitalWrite(input_2, LOW);
}

void Motor::reverse() {
    digitalWrite(input_1, LOW);
    digitalWrite(input_2, HIGH);
}

void Motor::stop() {
    analogWrite(enable, 0);

    digitalWrite(input_1, LOW);
    digitalWrite(input_2, LOW);
}
