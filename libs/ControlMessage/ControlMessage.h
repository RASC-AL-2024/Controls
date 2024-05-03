#ifndef ControlMessage_h
#define ControlMessage_h

#include <ActionMessage.h>
#include <Arduino.h>
#include <ArmConfig.h>
#include <Pose.h>

#define N_DIMENSIONS 3

enum ACTION_TYPE { ATTACH = 1, DETACH, ASSEMBLE, RESET, INVALID };
enum FACE { TOP, RIGHT, BOTTOM, LEFT, FRONT, BACK };
enum DIRECTION { NORTH, EAST, SOUTH, WEST };
enum COMMAND_TYPE {
  SERVO_COMMAND,
  DELTA_COMMAND,
  POSITION_COMMAND,
  ACTION_COMMAND,
  EXT_POSITION_COMMAND,
  INITIALIZE_COMMAND,
  SAVE_COMMAND,
  RESET_COMMAND,
  INVALID_COMMAND
};

struct PositionCommand {
  Point pos;
  double tool_angle;
};

struct ExtPositionCommand {
  ACTION_TYPE type;
  struct PositionCommand pos_command;
  uint8_t face;
  uint8_t direction;
};

struct ServoCommand {
  double servo_delta[N_JOINTS];
};

struct ActionCommand {
  ACTIONS action;
};

struct Command {
  union {
    ExtPositionCommand ext_position_command;
    PositionCommand position_command;
    ServoCommand servo_command;
    ActionCommand action_command;
    int error_message;
  } data;
  COMMAND_TYPE type;
};

#endif
