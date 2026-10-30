#include <Adafruit_PWMServoDriver.h>
#include "move_functions.h"


#define SERVOMIN 140
#define SERVOMAX 550
#define NUM_SERVOS 8


#define WALK_LIFT 100
#define WALK_REACH_FRONT 20    
#define WALK_REACH_REAR 30      
#define WALK_DRIVE 25    
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(PCA9685_ADDRESS);

static const int homeAngles[NUM_SERVOS] = {
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0
};


static int angleToPulse(int angle) {
  angle = constrain(angle, 0, 180);
  return map(angle, 0, 180, SERVOMIN, SERVOMAX);
}
void moveServo(uint8_t servo, int angle) {
  pwm.setPWM(servo, 0, angleToPulse(angle));
}

static void moveHome(uint8_t servo, int offset = 0) {
  moveServo(servo, homeAngles[servo] + offset);
}

void homeposition() {
  for (int i = 0; i < NUM_SERVOS; i++) {
    moveHome(i);
  }
  delay(300);
  Serial.println("All servos set to home position.");
}



void stop() {
  // for (int i = 0; i < NUM_SERVOS; i++) {
  //   pwm.setPWM(i, 0, 0);
  // }
  homeposition();
  Serial.println("All servos stopped.");
  while (true) {
    delay(1000);
  }
}


#define WAIT_TIME 300

static void moveTrotPhase(
  uint8_t liftServoA, int liftOffsetA, uint8_t hipServoA, int reachOffsetA,
  uint8_t liftServoB, int liftOffsetB, uint8_t hipServoB, int reachOffsetB,
  uint8_t stanceHipA, int driveOffsetA, uint8_t stanceHipB, int driveOffsetB
) {
  moveHome(liftServoA, liftOffsetA);
  moveHome(liftServoB, liftOffsetB);
  moveHome(stanceHipA, driveOffsetA);
  moveHome(stanceHipB, driveOffsetB);
  delay(WAIT_TIME);

  moveHome(hipServoA, reachOffsetA);
  moveHome(hipServoB, reachOffsetB);
  delay(WAIT_TIME);

  moveHome(liftServoA);
  moveHome(liftServoB);
  delay(WAIT_TIME);
}

void walk() {
  Serial.println("start walk");

  // 左前(4,5)と右後(2,3)を動かし、右前・左後で支持する。
  moveTrotPhase(
    5, -WALK_LIFT, 4, WALK_REACH_FRONT,
    3, WALK_LIFT, 2, -WALK_REACH_REAR - 10,
    0, WALK_DRIVE, 6, -WALK_DRIVE
  );

  // 右前(0,1)と左後(6,7)を動かし、左前・右後で支持する。
  moveTrotPhase(
    1, WALK_LIFT, 0, -WALK_REACH_FRONT + 5,
    7, -WALK_LIFT, 6, WALK_REACH_REAR,
    4, -WALK_DRIVE, 2, WALK_DRIVE
  );

  Serial.println("1 step completed.");
}