#include "SimServoController.h"
#include "Arduino.h"
#include "ArmConfig.h"
#include <stdint.h>

// Everything is little endian, assume we can just send this directly over the wire
struct __attribute__((packed)) SetServoMessage {
  uint8_t n_servos;
  float servo_state[8]; // radians
};

SimServoController::SimServoController(HardwareSerial* serial) : serial{serial} {}

void SimServoController::begin() {}

void SimServoController::initializeState(const double servo_state[]) {}

bool SimServoController::executeAction(const int action) { return true; }

bool SimServoController::executeServoAbs(const double servo_state[]) {
  SetServoMessage message{N_JOINTS, {}};
  serial->flush();
  serial->print("servo");
  for (int i = 0; i < N_JOINTS; ++i) {
    serial->print(' ');
    serial->print(servo_state[i]);
  }
  serial->println("");
  serial->flush();

  // serial->write(reinterpret_cast<uint8_t*>(&message), sizeof(message));
  // serial->flush();
}

bool SimServoController::executeServoDelta(const double servo_delta[]) {
  return executeServoAbs(servo_delta);
}

void SimServoController::saveState() {}
void SimServoController::reset() {}
