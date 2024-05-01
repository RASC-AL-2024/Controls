#include <Solver.h>
#include <UserInterface.h>
#include <ControlMessage.h>

Solver solver{&Serial};
UserInterface user_interface{&Serial};

void setup() {
  Serial.begin(9600);
  user_interface.begin();
  solver.begin();
}

double state[N_JOINTS] = {0.0, 1.0, 2.0, 3.0};

void loop() {
  solver.servo_controller->executeServoAbs(state);
  delay(5000);
  // if (user_interface.hasMessage()) {
  //     Command next_message = user_interface.getNextMessage();
  //     solver->servo_controller->executeServoAbs(state);
  //     solver.execute(next_message);
  // }
}
