#include "UserInterface.h"
#include <Arduino.h>

#define SERVO_COMMAND_HEADER 's'
#define DELTA_COMMAND_HEADER 'd'
#define ACTION_COMMAND_HEADER 'a'
#define POSTION_COMMAND_HEADER 'p'
#define INITIALIZE_COMMAND_HEADER 'i'
#define SAVE_COMMAND_HEADER 'v'
#define RESET_COMMAND_HEADER 'r'

UserInterface::UserInterface(HardwareSerial* software_serial) : serial{software_serial} {}

void UserInterface::begin() {}

bool UserInterface::hasMessage() {
  static uint8_t idx = 0;
  char endMarker = '\n';
  char rc;
  if (serial->available()) {
    rc = serial->read();
    if (rc == endMarker) {
      input_buffer[idx] = '\0';
      idx = 0;
      return true;
    } else {
      input_buffer[idx] = rc;
      idx++;
      if (idx >= MAX_CHARS - 1) {
        idx = MAX_CHARS - 1;
      }
    }
  }
  return false;
}

COMMAND_TYPE UserInterface::getCommandType(const char* serial_input) const {
  char header = serial_input[0];
  if (header == SERVO_COMMAND_HEADER) {
    return SERVO_COMMAND;
  } else if (header == ACTION_COMMAND_HEADER) {
    return ACTION_COMMAND;
  } else if (header == POSTION_COMMAND_HEADER) {
    return POSITION_COMMAND;
  } else if (header == INITIALIZE_COMMAND_HEADER) {
    return INITIALIZE_COMMAND;
  } else if (header == SAVE_COMMAND_HEADER) {
    return SAVE_COMMAND;
  } else if (header == RESET_COMMAND_HEADER) {
    return RESET_COMMAND;
  } else if (header == DELTA_COMMAND_HEADER) {
    return DELTA_COMMAND;
  }
  return INVALID_COMMAND;
}

Command UserInterface::parseActionCommand(char* command_string) const {
  ActionCommand action_command;
  if (strcmp(command_string, "lock") == 0) {
    action_command.action = LOCK_SELF;
  } else if (strcmp(command_string, "unlock") == 0) {
    action_command.action = UNLOCK_SELF;
  } else if (strcmp(command_string, "pull") == 0) {
    action_command.action = LOCK_OTHER;
  } else if (strcmp(command_string, "push") == 0) {
    action_command.action = UNLOCK_OTHER;
  }
  struct Command message;
  message.data.action_command = action_command;
  message.type = ACTION_COMMAND;
#ifdef DEBUG
  serial->println("sent action message!");
#endif
  return message;
}

void splitLineToFloatArray(char* command_string, double* arr, int size) {
  char delimiters[] = ",";
  char* ptr = strtok(command_string, delimiters);

  for (int i = 0; i < size; i++) {
    arr[i] = atof(ptr);
    ptr = strtok(NULL, delimiters);
  }
}

Command UserInterface::parseServoCommand(char* command_string) const {
  ServoCommand servo_command;
  splitLineToFloatArray(command_string, &servo_command.servo_delta[0], N_JOINTS);

  struct Command message;
  message.data.servo_command = servo_command;
  message.type = SERVO_COMMAND;
#ifdef DEBUG
  serial->println("sent servo message!");
#endif
  return message;
}

Command UserInterface::parsePositionCommand(char* command_string) const {
  double position[3];
  splitLineToFloatArray(command_string, &position[0], 3);
  // double m[4][4];
  // splitLineToFloatArray(command_string, &m[0][0], 16);
  // Pose pose{Matrix{{
  //               {m[0][0], m[0][1], m[0][2]},
  //               {m[1][0], m[1][1], m[1][2]},
  //               {m[2][0], m[2][1], m[2][2]},
  //           }},
  //           Point{
  //               m[0][0],
  //               m[0][1],
  //               m[0][2],
  //           }};
  struct Point point {
    position[0], position[1], position[2]
  };

  PositionCommand position_command;
  position_command.pos = point;
  position_command.tool_angle = 0.0;

  struct Command message;
  message.data.position_command = position_command;
  message.type = POSITION_COMMAND;
#ifdef DEBUG
  serial->println("sent position command");
#endif
  return message;
}

Command UserInterface::parseInitializeCommand(char* command_string) const {
  Command position_command = parsePositionCommand(command_string);
  position_command.type = INITIALIZE_COMMAND;
#ifdef DEBUG
  serial->println("sent initialize command");
#endif
  return position_command;
}

Command UserInterface::parseSaveCommand(char* command_string) const {
  struct Command save_command;
  save_command.data.error_message = 0;
  save_command.type = SAVE_COMMAND;
#ifdef DEBUG
  serial->println("sent save command");
#endif
  return save_command;
}

Command UserInterface::parseResetCommand(char* command_string) const {
  struct Command reset_command;
  reset_command.data.error_message = 0;
  reset_command.type = RESET_COMMAND;
#ifdef DEBUG
  serial->println("sent reset command");
#endif
  return reset_command;
}

Command UserInterface::parseDeltaCommand(char* command_string) const {
  struct Command delta_command = parseServoCommand(command_string);
  delta_command.type = DELTA_COMMAND;
}

void getDataFromCommand(char* serial_input) {
  for (int i = 2; i < MAX_CHARS; i++) {
    serial_input[i - 2] = serial_input[i];
  }
}

Command UserInterface::getErrorCommand() const {
  Command error_command;
  error_command.type = INVALID_COMMAND;
  error_command.data.error_message = 1;
  return error_command;
}

Command UserInterface::getNextMessage() {
  COMMAND_TYPE type = getCommandType(input_buffer);
  getDataFromCommand(input_buffer); // can just do input_buffer + 2
  if (type == ACTION_COMMAND) {
    return parseActionCommand(input_buffer);
  } else if (type == SERVO_COMMAND) {
    return parseServoCommand(input_buffer);
  } else if (type == POSITION_COMMAND) {
    return parsePositionCommand(input_buffer);
  } else if (type == INITIALIZE_COMMAND) {
    return parseInitializeCommand(input_buffer);
  } else if (type == SAVE_COMMAND) {
    return parseSaveCommand(input_buffer);
  } else if (type == RESET_COMMAND) {
    return parseResetCommand(input_buffer);
  } else if (type == DELTA_COMMAND) {
    return parseDeltaCommand(input_buffer);
  } else {
#ifdef DEBUG
    serial->println("invalid command detected!");
#endif
    return getErrorCommand();
  }
}
