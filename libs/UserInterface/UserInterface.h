
#ifndef UserInterface_h
#define UerInterface_h
#include <CommandReceiver.h>
#include <ControlMessage.h>

#define MAX_CHARS 20
class UserInterface : CommandReceiver {

  HardwareSerial* serial;
  char input_buffer[MAX_CHARS];

  Command parsePositionCommand(char*) const;
  Command parseActionCommand(char*) const;
  Command parseServoCommand(char*) const;
  Command parseInitializeCommand(char*) const;
  Command parseSaveCommand(char* command_string) const;
  Command parseResetCommand(char* command_string) const;
  Command parseDeltaCommand(char* command_string) const;
  Command getErrorCommand() const;
  COMMAND_TYPE getCommandType(const char*) const;

public:
  UserInterface(HardwareSerial* serial);
  void begin();
  bool hasMessage();
  Command getNextMessage();
};
#endif // !UserInterface_h
