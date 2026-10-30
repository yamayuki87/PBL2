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
  60,
  5,
  120,
  7,
  120,
  125,
  60,
  127
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

static void moveAndWait(uint8_t servo, int offset, unsigned long waitMs) {
  moveHome(servo, offset);
  delay(waitMs);
}

static void moveDriveServos(int offset0, int offset2, int offset4, int offset6) {
  moveAndWait(0, offset0, 10);
  moveAndWait(2, offset2, 10);
  moveAndWait(4, offset4, 10);
  moveAndWait(6, offset6, 10);
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


void walk() {
  Serial.println("start walk");

  // --- 左前(4,5) ---
  moveAndWait(5, -WALK_LIFT, WAIT_TIME);
  moveAndWait(4, WALK_REACH_FRONT, WAIT_TIME);
  moveAndWait(5, 0, WAIT_TIME);
  Serial.println("A");

  // --- 右前(0,1) ---
  moveAndWait(1, WALK_LIFT, WAIT_TIME);
  moveAndWait(0, -WALK_REACH_FRONT+5, WAIT_TIME);
  moveAndWait(1, 0, WAIT_TIME);
  Serial.println("B");

  // --- 前2脚を接地したまま駆動 ---
  moveDriveServos(0, WALK_DRIVE, 0, -WALK_DRIVE);
  Serial.println("C");
  delay(300);

  // --- 左後(6,7) ---
  moveAndWait(7, -WALK_LIFT, WAIT_TIME);
  moveAndWait(6, WALK_REACH_REAR, WAIT_TIME);
  moveAndWait(7, 0, WAIT_TIME);
  Serial.println("D");

  // --- 右後(2,3) ---
  moveAndWait(3, WALK_LIFT, WAIT_TIME);
  moveAndWait(2, -WALK_REACH_REAR-10, WAIT_TIME);
  moveAndWait(3, 0, WAIT_TIME);
  Serial.println("E");

  // --- 後ろ2脚を接地したまま駆動 ---
  moveDriveServos(WALK_DRIVE, 0, -WALK_DRIVE, 0);
  delay(WAIT_TIME);
  Serial.println("1 step completed.");
}