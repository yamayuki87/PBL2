
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pca(0x40);

// PCA9685設定
#define SERVO_FREQ 50
#define SERVO_MIN 150
#define SERVO_MAX 550

// ==========================
// 角度設定（自由に変更可能）
// ==========================

// 前後サーボの初期角度
int FRONT_HOME = 90;

// 回転角度
int ROTATE_ANGLE = 45;

// 上下サーボの初期角度
int DOWN_ANGLE = 90;

// 脚を上げる角度
int UP_ANGLE = 70;

// 動作時間
int MOVE_TIME = 300;

// ==========================
// サーボ個別制御
// ==========================

void motor(int ch, int angle) {

  angle = constrain(angle, 0, 180);

  int pulse = map(angle, 0, 180,
                  SERVO_MIN, SERVO_MAX);

  pca.setPWM(ch, 0, pulse);
}

// ==========================
// 初期位置
// ==========================

void homePosition() {

  for (int ch = 0; ch < 8; ch++) {
    motor(ch, FRONT_HOME);
  }

  delay(1000);
}

// ==========================
// グループA
// 左前 CH0・CH1
// 右後 CH4・CH5
// ==========================

// Aの脚を上げる
void liftA() {
  motor(1, UP_ANGLE);
  motor(5, UP_ANGLE);
}

// Aを45度回転
void rotateA() {
  motor(0, FRONT_HOME + ROTATE_ANGLE);
  motor(4, FRONT_HOME + ROTATE_ANGLE);
}

// Aの脚を下ろす
void lowerA() {
  motor(1, DOWN_ANGLE);
  motor(5, DOWN_ANGLE);
}

// Aを元の角度に戻す
void resetA() {
  motor(0, FRONT_HOME);
  motor(4, FRONT_HOME);
}

// ==========================
// グループB
// 右前 CH6・CH7
// 左後 CH2・CH3
// ==========================

// Bの脚を上げる
void liftB() {
  motor(7, UP_ANGLE);
  motor(3, UP_ANGLE);
}

// Bを45度回転
void rotateB() {
  motor(6, FRONT_HOME + ROTATE_ANGLE);
  motor(2, FRONT_HOME + ROTATE_ANGLE);
}

// Bの脚を下ろす
void lowerB() {
  motor(7, DOWN_ANGLE);
  motor(3, DOWN_ANGLE);
}

// Bを元の角度に戻す
void resetB() {
  motor(6, FRONT_HOME);
  motor(2, FRONT_HOME);
}

// ==========================
// 初期設定
// ==========================

void setup() {

  Wire.begin();

  pca.begin();
  pca.setPWMFreq(SERVO_FREQ);

  delay(500);

  homePosition();

  // 最初にAグループを上げる
  liftA();
  delay(MOVE_TIME);
}

// ==========================
// トロット歩容
// ==========================

void loop() {

  // ① Aグループを45度回転
  rotateA();
  delay(MOVE_TIME);

  // ② Aグループを下ろす
  lowerA();
  delay(MOVE_TIME);

  // ③ Aを元に戻しながらBを上げる
  resetA();
  liftB();
  delay(MOVE_TIME);

  // ④ Bグループを45度回転
  rotateB();
  delay(MOVE_TIME);

  // ⑤ Bグループを下ろす
  lowerB();
  delay(MOVE_TIME);

  // ⑥ Bを元に戻しながらAを上げる
  resetB();
  liftA();
  delay(MOVE_TIME);

  // 最初に戻り、Aグループを回転させる
}
