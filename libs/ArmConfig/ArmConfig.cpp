#include "ArmConfig.h"

// 7.8 cm, 5.0cm, 2.5cm
int JOINT_LENGTHS[] = {78, 50, 25};
uint8_t DXL_ID_LIST[] = {5, 4, 6, 3}; // base, shoulder, wrist, finger

double INIT_POSITIONS[N_JOINTS] = {0.4, 0.716, 1.1, 1};
