#include <Solver.h>
#include <UserInterface.h>
#include <ControlMessage.h>

Solver solver{&Serial1};
UserInterface user_interface{&Serial1};

void setup() {
  Serial1.begin(9600);
  Serial1.println("starting demo...");
  user_interface.begin();
  solver.begin();
}

void loop() {
  if(user_interface.hasMessage()){
      Command next_message = user_interface.getNextMessage();
      solver.execute(next_message);
  }
}
