#include "ActionMessage.h"

const uint8_t LOCK_SELF_PING_ID = 100;
const uint8_t UNLOCK_SELF_PING_ID = 101;
const uint8_t LOCK_OTHER_PING_ID = 103;
const uint8_t UNLOCK_OTHER_PING_ID = 102;

struct PingIdActionPair {
	ACTIONS action;
	uint8_t ping_id;
};

const extern PingIdActionPair action_to_ping_id_map[N_ACTIONS] = { {LOCK_SELF, LOCK_SELF_PING_ID}, {UNLOCK_SELF, UNLOCK_SELF_PING_ID}, {LOCK_OTHER, LOCK_OTHER_PING_ID}, {UNLOCK_OTHER, UNLOCK_OTHER_PING_ID} };

bool getActionFromPingId(uint8_t id, uint8_t& action_message) {
	for (uint8_t i = 0; i < N_ACTIONS; i++) {
		if (action_to_ping_id_map[i].ping_id == id) {
			action_message = action_to_ping_id_map[i].action;
			return true;
		}
	}
	return false;
}

bool pingIdFromAction(uint8_t action, uint8_t& ping_id) {
	for (uint8_t i = 0; i < N_ACTIONS; i++) {
		if (action_to_ping_id_map[i].action == action) {
			ping_id = action_to_ping_id_map[i].ping_id;
			return true;
		}
	}
	return false;
}
