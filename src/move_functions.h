#ifndef __HOKOU_H__
#define __HOKOU_H__

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>


#define PCA9685_ADDRESS 0x40

extern Adafruit_PWMServoDriver pwm;
extern const int trigPin;
extern const int echoPin;

void homeposition();
float readSonar();
//bool isGoal(float distance);
void stop();
void walk();
void moveServo(uint8_t servo, int angle);

#endif // __HOKOU_H__