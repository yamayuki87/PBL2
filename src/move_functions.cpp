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
  homeposition(); 
}

static void moveTrotPhase(
    uint8_t liftA, int liftDirectionA, uint8_t hipA, int reachA,
    uint8_t liftB, int liftDirectionB, uint8_t hipB, int reachB,
    uint8_t stanceHipA, int driveA, uint8_t stanceHipB, int driveB,
    const GaitSettings& gait) {

  // 対角の2脚を上げ、残り2脚で前進
  moveHome(liftA, liftDirectionA * gait.lift);
  moveHome(liftB, liftDirectionB * gait.lift);
  moveHome(stanceHipA, driveA);
  moveHome(stanceHipB, driveB);
  delay(gait.waitMs);

  // 浮かせた2脚を前方へ振り出す
  moveHome(hipA, reachA);
  moveHome(hipB, reachB);
  delay(gait.waitMs);

  // 上りは少し高い位置、下りは少し低い位置に足を下ろす
  moveHome(liftA, liftDirectionA * gait.landing);
  moveHome(liftB, liftDirectionB * gait.landing);
  delay(gait.waitMs);
}

static void trotCycle(const GaitSettings& gait) {
  // 左前(4,5)・右後(2,3)を振り出す
  moveTrotPhase(
    5, -1, 4,  gait.reachFront,
    3, +1, 2, -gait.reachRear - 10,
    0,  gait.drive, 6, -gait.drive,
    gait
  );

  // 右前(0,1)・左後(6,7)を振り出す
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
