#include "SimServoController.h"
#include "Arduino.h"
#include "ArmConfig.h"
#include <stdint.h>

SimServoController::SimServoController(HardwareSerial* serial) : serial{serial} {}

void SimServoController::begin() {}

void SimServoController::initializeState(const double servo_state[]) {}

bool SimServoController::executeAction(const int action) { return true; }

bool SimServoController::executeServoAbs(const double servo_state[]) {
  serial->print("servo");
  for (int i = 0; i < N_JOINTS; ++i) {
    serial->print(' ');
    serial->print(servo_state[i]);
  }
  serial->println("");
  serial->flush();
}

bool SimServoController::executeServoDelta(const double servo_delta[]) {
  return executeServoAbs(servo_delta);
}

void SimServoController::saveState() {}
void SimServoController::reset() {}
