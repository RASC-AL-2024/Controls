#ifndef SimServoController_h
#define SimServoController_h

#include "ServoControllerAbstract.h"

class HardwareSerial;
class SimServoController : public ServoControllerAbstract {
public:
  SimServoController(HardwareSerial* serial);
  void begin() override;
  void initializeState(const double servo_state[]) override;
  bool executeAction(const int action) override;
  bool executeServoAbs(const double servo_state[]) override;
  bool executeServoDelta(const double servo_delta[]) override;
  void saveState() override;
  void reset() override;

private:
  HardwareSerial* serial;
};

#endif
