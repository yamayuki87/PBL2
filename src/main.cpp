
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
int n=5;
int FRONT_HOME = 90;

int FRONT_HOME2 = 25;// a

// 回転角度
int ROTATE_ANGLE = 45;

// 上下サーボの初期角度
int DOWN_ANGLE = 25;

// 脚を上げる角度
int UP_ANGLE = 120;
int UP_ANGLE2 = 80;

// 動作時間
int MOVE_TIME = 1500;

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
    if((ch%2)==1){
      motor(ch, FRONT_HOME2);
    }else{
    motor(ch, FRONT_HOME);
  }
}

  delay(5000);
}

// ==========================
// グループA
// 左前 CH0・CH1
// 右後 CH4・CH5
// ==========================

// Aの脚を上げる
void liftA() {
  for (int angle=FRONT_HOME2; angle <=UP_ANGLE; angle+=n) {
    motor(1, angle);
    delay(15);
  }
  //motor(1, UP_ANGLE);
    for (int angle=FRONT_HOME2; angle <=UP_ANGLE2; angle+=n) {
    motor(5, angle);
    delay(15);
  }
  //motor(5, UP_ANGLE2);
}

// Aを45度回転
void rotateA() {
  for (int angle=FRONT_HOME; angle >=FRONT_HOME - ROTATE_ANGLE; angle-=n ) {
    motor(0, angle);
    delay(15);
  }

  //motor(0, FRONT_HOME - ROTATE_ANGLE);
  for (int angle=FRONT_HOME; angle <=FRONT_HOME + ROTATE_ANGLE; angle+=n) {
    motor(4, angle);
    delay(15);
  }
  //motor(4, FRONT_HOME + ROTATE_ANGLE);
  for (int angle=FRONT_HOME2; angle <=UP_ANGLE; angle+=n) {
    motor(5, angle);
    delay(15);
  }
  //motor(5, UP_ANGLE);
}



// Aの脚を下ろす
void lowerA() {
  for (int angle=UP_ANGLE; angle >=DOWN_ANGLE; angle-=n) {
    motor(1, angle);
    delay(15);
  }
  //motor(1, DOWN_ANGLE);
  for (int angle=UP_ANGLE2; angle >=DOWN_ANGLE; angle-=n) {
    motor(5, angle);
    delay(15);
  }
 // motor(5, DOWN_ANGLE);
}

// Aを元の角度に戻す
void resetA() {
  for (int angle=FRONT_HOME + ROTATE_ANGLE; angle >=FRONT_HOME; angle-=n) {
    motor(0, angle);
    delay(15);
  }
  //motor(0, FRONT_HOME);
  for (int angle=FRONT_HOME - ROTATE_ANGLE; angle <=FRONT_HOME; angle+=n) {
    motor(4, angle);
    delay(15);
  }
  //motor(4, FRONT_HOME);
}

// ==========================
// グループB
// 右前 CH6・CH7
// 左後 CH2・CH3
// ==========================

// Bの脚を上げる
void liftB() {
  for (int angle=FRONT_HOME2; angle <=UP_ANGLE; angle+=10) {
    motor(7, angle);
    delay(15);
  }
  //motor(7, UP_ANGLE);
  for (int angle=FRONT_HOME2; angle <=90; angle+=10) {
    motor(3, angle);
    delay(15);
  }
  //motor(3, 90);
}

// Bを45度回転
void rotateB() {
  for (int angle=FRONT_HOME; angle <=FRONT_HOME + ROTATE_ANGLE; angle+=10) {
    motor(6, angle);
    delay(15);
  }
  //motor(6, FRONT_HOME + ROTATE_ANGLE);
   for (int angle=FRONT_HOME; angle >= FRONT_HOME - ROTATE_ANGLE; angle-=10) {
    motor(2, angle);
    delay(15);
  }
  //motor(2, FRONT_HOME - ROTATE_ANGLE);
  for (int angle=25; angle <= UP_ANGLE; angle+=10) {
    motor(3, angle);
    delay(15);
  }
  //motor(3, UP_ANGLE);
}

// Bの脚を下ろす
void lowerB() {
  for (int angle=UP_ANGLE; angle >= DOWN_ANGLE; angle-=n) {
    motor(7, angle);
    delay(15);
  }
  //motor(7, DOWN_ANGLE);
   for (int angle=UP_ANGLE2; angle >= DOWN_ANGLE; angle-=n) {
    motor(3, angle);
    delay(15);
  }
  //motor(3, DOWN_ANGLE);
}

// Bを元の角度に戻す
void resetB() {
  for (int angle=FRONT_HOME + ROTATE_ANGLE; angle >= FRONT_HOME ; angle-=n) {
    motor(6, angle);
    delay(15);
  }
  //motor(6, FRONT_HOME);
  for (int angle=FRONT_HOME - ROTATE_ANGLE; angle <= FRONT_HOME ; angle+=n) {
    motor(2, angle);
    delay(15);
  }
  //motor(2, FRONT_HOME);
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
  liftB();
  delay(500);
  resetA();
  delay(500);
  lowerB();
  delay(500);
  liftB();
  delay(MOVE_TIME);
  liftB();

  // ④ Bグループを45度回転
  rotateB();
  delay(MOVE_TIME);

  // ⑤ Bグループを下ろす
  lowerB();
  delay(MOVE_TIME);

  // ⑥ Bを元に戻しながらAを上げる
  liftA();
  delay(500);
  resetB();
  delay(500);
  lowerA();
  delay(500);
  liftA();
  delay(MOVE_TIME);

  // 最初に戻り、Aグループを回転させる
}


