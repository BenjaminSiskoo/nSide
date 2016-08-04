//Atari 2600 controller port pinout:
//  ___________________
// /(1) (2) (3) (4) (5)\
//  \ (6) (7) (8) (9) /
//   \---------------/
//
// pin  name     port1      port2
//  1:  up       $0280.d4   $0280.d0
//  2:  down     $0280.d5   $0280.d1
//  3:  left     $0280.d6   $0280.d2
//  4:  right    $0280.d7   $0280.d3
//  5:  inpt0/2  $0038.d7   $003a.d7
//  6:  inpt4/5  $003c.d7   $003d.d7
//  7:  +5v
//  8:  gnd
//  9:  inpt1/3  $0039.d7   $003b.d7

struct Controller : Thread {
  enum : uint { Port1 = 0, Port2 = 1 };

  Controller(bool port);
  virtual ~Controller();
  static auto Enter() -> void;

  virtual auto main() -> void;
  virtual auto pot0() -> bool { return 1; }
  virtual auto pot1() -> bool { return 1; }
  virtual auto fire() -> bool { return 1; }
  virtual auto direction() -> uint4 { return 0xf; }

  const bool port;
};

#include "joystick/joystick.hpp"
