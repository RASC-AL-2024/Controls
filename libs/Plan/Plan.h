#ifndef Plan_h
#define Plan_h

#include "ControlMessage.h"

#define MAX_STATES 5

class Plan {
private:
    int max_time;
    int current_time;
    struct Command* states[MAX_STATES];
public:
    Plan();
    void addAction(int action);
    void addPosition(const struct PositionCommand& pos);
    bool hasNextState() const;
    struct Command* getNextState();
    void returnToPlanStart();
    ~Plan();
};
#endif 
