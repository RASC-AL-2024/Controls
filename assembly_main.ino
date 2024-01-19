#include <Comms.h>
#include <LockController.h>

Comms comms;
LockController lock_controller;

void setup()
{
    comms.begin();
    lock_controller.begin();
}

void loop()
{
    if (comms.hasMessage())
    {
        uint8_t next_message = comms.getNextMessage();
        lock_controller.executeAction(next_message);
    }
}
