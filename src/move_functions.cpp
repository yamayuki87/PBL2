#include "move_functions.h"

constexpr int SERVOMIN = 140;
constexpr int SERVOMAX = 550;
constexpr uint8_t NUM_SERVOS = 8;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(PCA9685_ADDRESS);

static const int homeAngles[NUM_SERVOS] = {
  90, // 0: 右前 前後
  90, // 1: 右前 上下
  90, // 2: 右後 前後
  90, // 3: 右後 上下
  90, // 4: 左前 前後
  90, // 5: 左前 上下
  90, // 6: 左後 前後
  90  // 7: 左後 上下
};

// 数値はサーボ角度[deg]の仮設定。30mmを意味する値ではありません。
// lift: 足を持ち上げる角度、landing: 接地時の上下角度の差分
// landing>0: 通常より足を上げた位置へ着地
// landing<0: 通常より足を下げた位置へ着地
// reachFront/Rear: 振り出す角度、drive: 接地脚を後方へ蹴る角度
struct GaitSettings {
  int lift;
  int landing;
  int reachFront;
  int reachRear;
  int drive;
  unsigned int waitMs;
};

static const GaitSettings FLAT = {35,   0, 20, 30, 25, 300};
static const GaitSettings UP   = {50,  12, 25, 35, 20, 450};
static const GaitSettings DOWN = {45, -12, 20, 25, 18, 500};

static int angleToPulse(int angle) {
  // サーボ機構自体の可動範囲も別途確認してください。
  angle = constrain(angle, 0, 180);
  return map(angle, 0, 180, SERVOMIN, SERVOMAX);
}

void moveServo(uint8_t servo, int angle) {
  if (servo >= NUM_SERVOS) return;
  pwm.setPWM(servo, 0, angleToPulse(angle));
}

static void moveHome(uint8_t servo, int offset = 0) {
  if (servo >= NUM_SERVOS) return;
  const int angle = homeAngles[servo] + offset;
  if (angle < 0 || angle > 180) {
    Serial.print("WARNING: angle clipped, servo=");
    Serial.println(servo);
  }
  moveServo(servo, angle);
}

void homeposition() {
  for (uint8_t i = 0; i < NUM_SERVOS; ++i) moveHome(i);
  delay(400);
}

void stop() {
  homeposition(); // PWM信号は出し続け、静止姿勢を保持する
}

// liftDirection は、元コードで使っていた足上げ方向を維持する。
// 左前(5): -1、右後(3): +1、右前(1): +1、左後(7): -1
static void moveTrotPhase(
    uint8_t liftA, int liftDirectionA, uint8_t hipA, int reachA,
    uint8_t liftB, int liftDirectionB, uint8_t hipB, int reachB,
    uint8_t stanceHipA, int driveA, uint8_t stanceHipB, int driveB,
    const GaitSettings& gait) {

  // 1. 対角の2脚を上げ、残り2脚で前進
  moveHome(liftA, liftDirectionA * gait.lift);
  moveHome(liftB, liftDirectionB * gait.lift);
  moveHome(stanceHipA, driveA);
  moveHome(stanceHipB, driveB);
  delay(gait.waitMs);

  // 2. 浮かせた2脚を前方へ振り出す
  moveHome(hipA, reachA);
  moveHome(hipB, reachB);
  delay(gait.waitMs);

  // 3. 上りは少し高い位置、下りは少し低い位置に足を下ろす
  moveHome(liftA, liftDirectionA * gait.landing);
  moveHome(liftB, liftDirectionB * gait.landing);
  delay(gait.waitMs);
}

static void trotCycle(const GaitSettings& gait) {
  // 位相A: 左前(4,5)・右後(2,3)を振り出す
  moveTrotPhase(
    5, -1, 4,  gait.reachFront,
    3, +1, 2, -gait.reachRear - 10,
    0,  gait.drive, 6, -gait.drive,
    gait
  );

  // 位相B: 右前(0,1)・左後(6,7)を振り出す
  moveTrotPhase(
    1, +1, 0, -gait.reachFront + 5,
    7, -1, 6,  gait.reachRear,
    4, -gait.drive, 2, gait.drive,
    gait
  );
}

void walk()     { trotCycle(FLAT); }
void walkUp()   { trotCycle(UP); }
void walkDown() { trotCycle(DOWN); }
