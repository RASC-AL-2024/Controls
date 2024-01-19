#ifndef ArmConfig_h
#define ArmConfig_h
#include <Arduino.h>

#define N_JOINTS 4
#define GEAR_RATIO 5.5
extern int JOINT_LENGTHS[N_JOINTS-1];
extern uint8_t DXL_ID_LIST[N_JOINTS];
extern double INIT_POSITIONS[N_JOINTS];

//#define DEBUG
#define IS_SIMULATOR
#define IS_INITIALIZED

#endif // !ArmConfig_h
