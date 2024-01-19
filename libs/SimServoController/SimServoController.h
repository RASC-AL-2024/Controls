#ifndef SimServoController_h
#define SimServoController_h

#include "ServoControllerAbstract.h"

class HardwareSerial;
class SimServoController : public ServoControllerAbstract {
public:
	SimServoController(HardwareSerial* software_serial) {};
	void begin() override {};
	void initializeState(const double servo_state[]) override {};
	bool executeAction(const int action) override { return true; };
	bool executeServoAbs(const double servo_state[]) override { return true; };
	bool executeServoDelta(const double servo_delta[]) override { return true; };
	void saveState() override {};
	void reset() override {};
};

#endif