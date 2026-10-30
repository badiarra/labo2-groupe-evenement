
#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

class Motor {
public:
    Motor(int ENA, int IN1, int IN2);

    void init();
    void setSpeed(float val);
    void forward();
    void reverse();
    void stop();

private:
    int enable;
    int input_1;
    int input_2;
};

#endif
