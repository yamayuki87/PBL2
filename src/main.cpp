#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "move_functions.h"

//基本性能のコード

//手順
//初期姿勢
//前進
//物体検知→旋回、さける
//とめる


float objectDistance=0;
int8_t turnCounter = 0;



void setup() {
  Serial.begin(9600);
  Wire.begin();

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  // PCA9685の初期化
  pwm.begin();
  // PCA9685のPWM周波数をサーボに適した50Hzに設定
  pwm.setPWMFreq(50);
  homeposition();
  delay(1000);
}

void loop() {
    objectDistance = readSonar();

    //ゴール検知
    if (isGoal(objectDistance)) {
        stop();
        return;
    }



    // 障害物回避
    // if (objectDistance < 30) {
    //     if (++turnCounter >= 2) { 
    //         delay(5000);
    //         rotate(8, false); 
    //         walk();
    //         walk();
            
    //         rotate(8, true);
    //         turnCounter = 0;
    //         return; 
    //     }
    // } else {
    //     turnCounter = 0;
    // }

    // 通常歩行
    walk();
}