#include "Plan.h"
#include <ActionMessage.h>
#include <Arduino.h>

Plan::Plan() : max_time{0}, current_time{0} {}

void Plan::addAction(int action) {
  if (max_time >= MAX_STATES) {
    return;
  }
  ActionCommand action_command{(ACTIONS)action};
  struct Command* next_command = new Command;
  next_command->data.action_command = action_command;
  next_command->type = ACTION_COMMAND;
  states[max_time++] = next_command;
}

void Plan::addPosition(const struct PositionCommand& pos) {
  if (max_time >= MAX_STATES) {
    return;
  }
  struct Command* next_command = new Command;
  next_command->data.position_command = pos;
  next_command->type = POSITION_COMMAND;
  states[max_time++] = next_command;
}

bool Plan::hasNextState() const { return current_time < max_time; }

struct Command* Plan::getNextState() {
  if (current_time >= max_time) {
    return nullptr;
  }
  return states[current_time++];
}

void Plan::returnToPlanStart() { current_time = 0; }

Plan::~Plan() {
  for (uint8_t i = 0; i < max_time; i++) {
    delete states[i]; // avoid memory leaks
  }
}
