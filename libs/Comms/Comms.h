#ifndef Comms_h
#define Comms_h

#include <SoftHalfDuplexSerial.h>

#define HEADER_SIZE 4

class Comms {
private:
	softHalfDuplexSerial serial;
	uint8_t bytes_read;
	uint8_t action_message;
	uint8_t action_id;

	bool checkHeader(char next_byte);
	bool processMessage();

public:
	Comms();
	void begin();
	bool hasMessage();
	uint8_t getNextMessage();
	void sendMessage();
};
#endif