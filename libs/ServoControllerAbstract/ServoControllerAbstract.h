#ifndef ServoControllerAbstract_h
#define ServoControllerAbstract_h

class HardwareSerial;

class ServoControllerAbstract {
public:
  virtual void begin() = 0;
  virtual void initializeState(const double servo_state[]) = 0;
  virtual bool executeAction(const int action) = 0;
  virtual bool executeServoAbs(const double servo_state[]) = 0;
  virtual bool executeServoDelta(const double servo_delta[]) = 0;
  virtual void saveState() = 0;
  virtual void reset() = 0;
};

#endif
