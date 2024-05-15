#include <Solver.h>
#include <UserInterface.h>
#include <ControlMessage.h>
#include <Controller.h>
 
Solver solver{&Serial};
UserInterface user_interface{&Serial};
Controller controller{Serial};

void setup() {
  Serial.begin(9600, SERIAL_8N2);
  user_interface.begin();
  solver.begin();
}

void loop() {
  if (user_interface.hasMessage()) {
    Command next_message = user_interface.getNextMessage();
    solver.execute(next_message);
  }
}
