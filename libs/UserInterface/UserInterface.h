
#ifndef UserInterface_h
#define UerInterface_h
#include <CommandReceiver.h>
#include <ControlMessage.h>

#define MAX_CHARS 20
class UserInterface : CommandReceiver {

	HardwareSerial* serial;
	char input_buffer[MAX_CHARS];

	Command parsePositionCommand(const char*) const;
	Command parseActionCommand(const char*) const;
	Command parseServoCommand(const char*) const;
	Command parseInitializeCommand(const char*) const;
	Command parseSaveCommand(const char* command_string) const;
	Command parseResetCommand(const char* command_string) const;
	Command parseDeltaCommand(const char* command_string) const;
	Command getErrorCommand() const;
	COMMAND_TYPE getCommandType(const char*) const;

public:
	UserInterface(HardwareSerial* serial);
	void begin();
	bool hasMessage();
	Command getNextMessage();
};
#endif // !UserInterface_h
