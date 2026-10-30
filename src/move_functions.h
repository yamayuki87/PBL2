#ifndef __HOKOU_SENKAI_H__
#define __HOKOU_SENKAI_H__

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>


#define PCA9685_ADDRESS 0x40

extern Adafruit_PWMServoDriver pwm;
extern const int trigPin;
extern const int echoPin;

void homeposition();
float readSonar();
bool isGoal(float distance);
void stop();
void rotate(unsigned int step , bool reverse = false);
void walk();
void moveServo(uint8_t servo, int angle);

#endif // __HOKOU_SENKAI_H__