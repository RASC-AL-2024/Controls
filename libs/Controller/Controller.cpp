#include <Controller.h>

// Module

float module_radius_m = 0.10; // m

static Point offsets[6] = {
    {module_radius_m, 0, 0},  {-module_radius_m, 0, 0}, {0, module_radius_m, 0},
    {0, -module_radius_m, 0}, {0, 0, module_radius_m},  {0, 0, -module_radius_m},
};

Point Module::connection(Module::Face face) {
  auto offset = offsets[static_cast<uint8_t>(face)];
  return pose * offset;
}

// Controller

Controller::Controller(HardwareSerial& serial) : solver{&serial} {}

void Controller::register_module(uint8_t id, const Pose& pose) { modules.insert(id, pose); }
