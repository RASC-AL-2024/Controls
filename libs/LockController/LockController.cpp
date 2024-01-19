#include "LockController.h"
#include <Arduino.h>
#include <ActionMessage.h>

#define SELF_SOLENOID_PIN 5
#define SELF_MIN 70
#define SELF_MAX 90

#define OTHER_SOLENOID_PIN 7
#define OTHER_MIN 20
#define OTHER_MAX 100

LockController::LockController() {}

void LockController::begin() {
	self_servo.attach(SELF_SOLENOID_PIN);
	other_servo.attach(OTHER_SOLENOID_PIN);
	self_servo.write(SELF_MIN);
	other_servo.write(OTHER_MIN);
}

bool LockController::executeAction(uint8_t action) {
	switch (action) {
	case LOCK_SELF:
		self_servo.write(SELF_MAX);
		return true;
		break;
	case UNLOCK_SELF:
		self_servo.write(SELF_MIN);
		return true;
		break;
	case LOCK_OTHER:
		other_servo.write(OTHER_MAX);
		return true;
		break;
	case UNLOCK_OTHER:
		other_servo.write(OTHER_MIN);
		return true;
		break;
	default:
		break;
	}
	return false;
}