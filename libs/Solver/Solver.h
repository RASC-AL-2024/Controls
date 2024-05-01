

#ifndef Solver_h
#define Solver_h

#include "ServoController.h"
#include "SimServoController.h"
#include <Arduino.h>
#include <ArmConfig.h>
#include <FABRIK2D.h>

using ServoState = double[N_JOINTS];

class Plan;
struct Command;

class Solver {
public:
  HardwareSerial* serial;

  Fabrik2D fabrik2D;
  ServoControllerAbstract* servo_controller;

  void solveServoCommand(const struct Command& servo_command);
  void solveDeltaCommand(const struct Command& servo_command);
  void solvePositionCommand(const struct Command& position_command);
  void solveExtPositionCommand(const struct Command& ext_position_command);
  void solveActionCommand(const struct Command& action_command);
  void solveInitializeCommand(const struct Command& command);
  void solveSaveCommand(const struct Command& command);
  void solveResetCommand(const struct Command& command);

  void getServoState(const struct PositionCommand& position, ServoState state);
  bool getPlan(const struct Command& command, Plan& plan);

  bool solveAttach(const Command& command, Plan& plan);
  bool solveDetach(const Command& command, Plan& plan);
  bool solveAssemble(const Command& command, Plan& plan);
  bool solveReset(const Command& command, Plan& plan);

public:
  Solver(HardwareSerial* software_serial);
  void begin();
  void execute(const struct Command& command);
};
#endif
