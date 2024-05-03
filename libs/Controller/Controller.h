#pragma once

#include <Arduino.h>
#include <Collections.h>
#include <Pose.h>
#include <Solver.h>

struct Module {
  enum class Kind { Normal, Elbow, Scoop };
  enum class Face : uint8_t {
    PositiveX = 0,
    NegativeX = 1,
    PositiveY = 2,
    NegativeY = 3,
    PositiveZ = 4,
    NegativeZ = 5
  };

  Point connection(Face face);

  Kind kind;
  Pose pose; // local space to world space
};

class Controller {
public:
  Controller(HardwareSerial& serial);
  void register_module(uint8_t id, const Pose& pose);
  void grab(uint8_t id, Module::Face face);

private:
  abc::Map<uint8_t, Pose, 4> modules;
  Solver solver;
};
