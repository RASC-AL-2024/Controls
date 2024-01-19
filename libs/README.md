# Library Descriptions

## ArmConfig
* defines high-level constants used throughout all other libraries

## Comms:
* this code runs on the assembly-module microcontroller
* it reads and returns messages passed to it

## ControlMessage:
* Defines the different types of message packets

## Plan
* The Plan class is more or less a list of commands, for example, you can chain together multiple different move and action commands

## Radio:
* Currently unused, this class allows for wireless communication using nrf24l01

## ServoController:
* Code to write to the Dynamixel servos

## ServoControllerAbstract:
* abstract class which defines the neccessary functions for a servo controller

## SimServoController:
* This is used when we are running the embedded code in "sim"
* Running in "sim" means we will run all code as usual on the actual hardware (the microcontroller), except instead of commands to the servos, we will intercept the values and send them back to our simulator (Unity3D game engine). 

## LockController:
* This code is used on the assembly modules microcontroller. 
* When a command is recieved to actuate one of the locks on the assembly module, it actuates the correct servo

## Solver:
* This code gets a command, creats a plan, and executes it

# Library Interactions:
main.ino works as follows:
1. **user_interface** recieves a command (for now from the UI)
2. the command is passed to **solver**
3. **solver** creates a plan or an action
4. this plan or action is passed to **servo_controller**
5. **servo_controller** sends commands to the servos, or the assembly module microcontroller 
6. the assembly module microcontroller also uses the **Comms** library to check if a message has been passed to it
7. if so, it uses the **LockController** library, to actuate the correct lock, and sends a confirmation back



