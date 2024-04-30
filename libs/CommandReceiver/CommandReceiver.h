#ifndef CommandReceiver_h
#define CommandReceiver_h

struct Command;

class CommandReceiver {
public:
	virtual bool hasMessage() = 0;
	virtual Command getNextMessage() = 0;
};

#endif // !CommandReceiver_h
