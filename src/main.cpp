#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "move_functions.h"



constexpr uint8_t STAIR_COUNT = 3;
constexpr uint8_t APPROACH_CYCLES = 0;       // 最初の段までの平地歩行
constexpr uint8_t UP_CYCLES_PER_STAIR = 1;   // 1段を進むトロット周期数（要実測）
constexpr uint8_t TOP_CYCLES = 2;            // 3段上ったあとの平地歩行（要実測）
constexpr uint8_t DOWN_CYCLES_PER_STAIR = 1; // 1段を下る周期数（要実測）
constexpr uint8_t EXIT_CYCLES = 1;           // 下り終わったあとの平地歩行

static bool finished = false;

static void repeatCycles(void (*gait)(), uint8_t count) {
  for (uint8_t i = 0; i < count; ++i) {
    gait();
  }
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  pwm.begin();
  pwm.setPWMFreq(50);

  if (!MOTION_ENABLED) {
    Serial.println("Safety lock: homeAngles, leg directions and travel clearance must be checked.");
    Serial.println("Set MOTION_ENABLED = true after checking on a raised robot.");
    return;
  }

  homeposition();
  delay(1000);
}

void loop() {
  if (!MOTION_ENABLED || finished) return;
  finished = true; // Arduino loop() が繰り返されても再実行しない

  Serial.println("Approaching stairs");
  repeatCycles(walk, APPROACH_CYCLES);

  for (uint8_t step = 0; step < STAIR_COUNT; ++step) {
    Serial.print("Ascending stair ");
    Serial.println(step + 1);
    repeatCycles(walkUp, UP_CYCLES_PER_STAIR);
  }

  Serial.println("Walking across the top");
  repeatCycles(walk, TOP_CYCLES);

  for (uint8_t step = 0; step < STAIR_COUNT; ++step) {
    Serial.print("Descending stair ");
    Serial.println(step + 1);
    repeatCycles(walkDown, DOWN_CYCLES_PER_STAIR);
  }

  Serial.println("Moving away from stairs");
  repeatCycles(walk, EXIT_CYCLES);

  stop();
  Serial.println("Route completed.");
}
