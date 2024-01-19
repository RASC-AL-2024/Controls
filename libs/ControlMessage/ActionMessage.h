#ifndef ActionMessage_h
#define ActionMessage_h
#include <Arduino.h>

#define N_ACTIONS 4
enum ACTIONS { LOCK_SELF, UNLOCK_SELF, LOCK_OTHER, UNLOCK_OTHER };
bool getActionFromPingId(uint8_t id, uint8_t& action_message);
bool pingIdFromAction(uint8_t action, uint8_t& ping_id);
#endif // !ActionMessage_h
