#ifndef LockController_h
#define LockController_h
#include <Servo.h>

class LockController {
private:
	Servo self_servo;
	Servo other_servo;

public:
	LockController();
	void begin();
	bool executeAction(uint8_t action);
};
#endif // !LockController_h
