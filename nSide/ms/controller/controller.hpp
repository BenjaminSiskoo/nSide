//Master System controller port pinout:
//  ___________________
// /(1) (2) (3) (4) (5)\
//  \ (6) (7) (8) (9) /
//   \---------------/
//
// pin  name   port1    port2
//  1:  up     $dc.d0   $dc.d6
//  2:  down   $dc.d1   $dc.d7
//  3:  left   $dc.d2   $dd.d0
//  4:  right  $dc.d3   $dd.d1
//  5:  +5v
//  6:  tl     $dc.d4   $dd.d2
//  7:  th     $dd.d6   $dd.d7
//  8:  gnd
//  9:  tr     $dc.d5   $dd.d3

struct Controller : Thread {
  Controller(bool port);
  virtual ~Controller();

  static auto Enter() -> void;
  virtual auto main() -> void;

  virtual auto readData() -> uint7 { return 0x7f; }
  virtual auto writeData(uint7 data) -> void {}

  virtual auto readControl() -> uint7 { return 0x00; }
  virtual auto writeControl(uint7 data) -> void {}

  const bool port;
};

#include "gamepad/gamepad.hpp"
