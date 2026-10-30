#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "move_functions.h"

//階段踏破のコード

//手順
//初期姿勢
//前進
//とめる

//float objectDistance=0;



void setup() {
  Serial.begin(9600);
  Wire.begin();

  // PCA9685の初期化
  pwm.begin();
  // PCA9685のPWM周波数をサーボに適した50Hzに設定
  pwm.setPWMFreq(50);
  homeposition();
  delay(1000);
}

void loop() {
    //objectDistance = readSonar();

    // //ゴール検知
    // if (isGoal(objectDistance)) {
    //     stop();
    //     return;
    // }

    // 通常歩行
    walk();
}