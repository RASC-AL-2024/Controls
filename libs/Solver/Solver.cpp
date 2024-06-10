#include "Solver.h"
#include <ControlMessage.h>
#include <Plan.h>

Solver::Solver(HardwareSerial* software_serial)
    : serial{software_serial}, fabrik2D{N_JOINTS, JOINT_LENGTHS} {
#ifdef IS_SIMULATOR
  servo_controller = new SimServoController{software_serial};
#else
  servo_controller = new ServoController{software_serial};
#endif

  fabrik2D.setTolerance(3);
}

void Solver::begin() {
#ifdef DEBUG
  serial->println("solver in debug mode");
#endif
  servo_controller->begin();
}

void Solver::solveServoCommand(const struct Command& servo_command) {
#ifdef DEBUG
  serial->println("executing servo command");
#endif
  servo_controller->executeServoAbs(servo_command.data.servo_command.servo_delta);
}

void Solver::solveDeltaCommand(const struct Command& servo_command) {
#ifdef DEBUG
  serial->println("executing delta command");
#endif
  servo_controller->executeServoDelta(servo_command.data.servo_command.servo_delta);
}

void Solver::solvePositionCommand(const struct Command& command) {
#ifdef DEBUG
  serial->println("executing position command");
#endif
  ServoState servo_state;
  PositionCommand position_command = command.data.position_command;
  getServoState(position_command, servo_state);
  servo_controller->executeServoAbs(servo_state);
}

void Solver::solveActionCommand(const struct Command& action_command) {
#ifdef DEBUG
  serial->print("executing action command: ");
  serial->println(action_command.data.action_command.action);
#endif
  servo_controller->executeAction(action_command.data.action_command.action);
}

void Solver::solveExtPositionCommand(const Command& ext_position_command) {
  Plan plan;
  bool error_code = getPlan(ext_position_command, plan);
#ifdef DEBUG
  serial->println("executing ext position command");
#endif
  while (error_code && plan.hasNextState()) {
    struct Command current_command = *plan.getNextState();
    if (current_command.type == ACTION_COMMAND) {
      solveActionCommand(current_command);
    } else if (current_command.type == POSITION_COMMAND) {
      solvePositionCommand(current_command);
    }
  }
}

void Solver::solveInitializeCommand(const struct Command& command) {
#ifdef DEBUG
  serial->println("executing position command");
#endif
  ServoState servo_state;
  struct PositionCommand position_command = command.data.position_command;
  getServoState(position_command, servo_state);
  servo_controller->initializeState(servo_state);
}

void Solver::solveSaveCommand(const struct Command& command) {
#ifdef DEBUG
  serial->println("executing save command");
#endif
  servo_controller->saveState();
}

void Solver::solveResetCommand(const struct Command& command) {
#ifdef DEBUG
  serial->println("executing reset command");
#endif
  servo_controller->reset();
}

void Solver::execute(const Command& command) {
#ifdef DEBUG
  serial->println("executing command");
  serial->println(command.type);
#endif

  if (command.type == SERVO_COMMAND) {
    solveServoCommand(command);
  } else if (command.type == POSITION_COMMAND) {
    solvePositionCommand(command);
  } else if (command.type == ACTION_COMMAND) {
    solveActionCommand(command);
  } else if (command.type == EXT_POSITION_COMMAND) {
    solveExtPositionCommand(command);
  } else if (command.type == INITIALIZE_COMMAND) {
    solveInitializeCommand(command);
  } else if (command.type == SAVE_COMMAND) {
    solveSaveCommand(command);
  } else if (command.type == RESET_COMMAND) {
    solveResetCommand(command);
  } else if (command.type == DELTA_COMMAND) {
    solveDeltaCommand(command);
  } else {
#ifdef DEBUG
    serial->println("invalid command!");
#endif
  }
}

bool Solver::getPlan(const Command& command, Plan& plan) {
  bool err_code = false;
  switch (command.type) {
    case ATTACH:
      err_code = solveAttach(command, plan);
      break;
    case DETACH:
      err_code = solveDetach(command, plan);
      break;
    case ASSEMBLE:
      err_code = solveAssemble(command, plan);
    case RESET:
      err_code = solveReset(command, plan);
      break;
    case INVALID:
#ifdef DEBUG
      serial->println("Recieved an invalid ext position command");
#endif
      break;
  }
  return err_code;
}

// Coordinate system is RUF
void Solver::getServoState(const PositionCommand& position, ServoState state) {
  float baseAngle = atan2(-position.pos[0], position.pos[2]);

  // Now fabrik +x is transformed.z, +y is transformed.y
  Point transformed = Matrix::fromEulerAngles(0, baseAngle, 0) * position.pos;

  serial->print(transformed[0]);
  serial->print(',');
  serial->print(transformed[1]);
  serial->print(',');
  serial->println(transformed[2]);

  // solve(126, 30, 0)

  fabrik2D.solve(transformed[1], transformed[2], 0, JOINT_LENGTHS);
  state[0] = baseAngle;
  for (int i = 0; i < 3; i++) {
    state[i + 1] = fabrik2D.getAngle(i); // these are in radians by default
  }
}

void findPlaneOffset(double& x, double& y, int direction) {}

void findFaceOffset(const Command& command, struct PositionCommand sliding_points[2]) {
  double x = command.data.ext_position_command.pos_command.pos[0];
  double y = command.data.ext_position_command.pos_command.pos[1];
  double z = command.data.ext_position_command.pos_command.pos[2];
  struct PositionCommand first_point {
    command.data.ext_position_command.pos_command.pos, 0.0
  };
  sliding_points[0] = first_point;
  sliding_points[1] = first_point;
}

bool Solver::solveAttach(const Command& command, Plan& plan) {
  struct PositionCommand sliding_points[2];
  findFaceOffset(command, sliding_points);
  plan.addPosition(sliding_points[0]);
  plan.addAction(UNLOCK_SELF);
  plan.addPosition(sliding_points[1]);
  plan.addAction(LOCK_SELF);
  return true;
}

bool Solver::solveAssemble(const Command& command, Plan& plan) { return true; }

bool Solver::solveDetach(const Command& command, Plan& plan) { return true; }

bool Solver::solveReset(const Command& command, Plan& plan) { return true; }
