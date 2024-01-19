#ifndef Radio_h
#define Radio_h

#include <RF24.h>
#include <CommandReceiver.h>

#define MESSAGE_SIZE 10

struct Command;
class Radiov : CommandReceiver {
private:
    bool is_sender;
    const byte self_address[5] = {'R','x','A','A','A'};
    char data_recieved[MESSAGE_SIZE];
    char previous_data[MESSAGE_SIZE];
    char data_to_send[MESSAGE_SIZE];
    RF24 radio;
    void copyToSendBuffer(const struct Command& command);

public:
    Radio(bool is_sender);
    void begin();
    bool hasMessage();
    bool hasMessage(const char expected_message[MESSAGE_SIZE]);
    struct Command getNextMessage();
    bool sendMessage(const struct Command& message);
    bool sendMessage(const char* message);
};
#endif
