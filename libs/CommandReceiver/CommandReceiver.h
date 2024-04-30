#ifndef CommandReceiver_h
#define CommandReceiver_h

#include <ControlMessage.h>

class CommandReceiver {
public:
  bool hasMessage() { return {}; };
  Command getNextMessage() { return {}; };
};

#endif // !CommandReceiver_h
