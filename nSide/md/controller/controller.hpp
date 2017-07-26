//Mega Drive controller port pinout:
//  ___________________
// /(1) (2) (3) (4) (5)\
//  \ (6) (7) (8) (9) /
//   \---------------/
//
// pin  name   port1        port2
//  1:  up     $a10003.d0   $a10005.d0
//  2:  down   $a10003.d1   $a10005.d1
//  3:  left   $a10003.d2   $a10005.d2
//  4:  right  $a10003.d3   $a10005.d3
//  5:  +5v
//  6:  tl     $a10003.d4   $a10005.d4
//  7:  th     $a10003.d6   $a10005.d6
//  8:  gnd
//  9:  tr     $a10003.d5   $a10005.d5

struct Controller : Thread {
  Controller(uint port);
  virtual ~Controller();

  static auto Enter() -> void;
  virtual auto main() -> void;

  virtual auto readData() -> uint8 { return 0xff; }
  virtual auto writeData(uint8 data) -> void {}

  const uint port;
};

struct ControllerPort {
  auto connect(uint deviceID) -> void;

  auto readControl() -> uint8;
  auto writeControl(uint8 data) -> void;

  auto power(uint port) -> void;
  auto unload() -> void;
  auto serialize(serializer&) -> void;

  uint port;
  uint8 control;
  Controller* device = nullptr;
};

extern ControllerPort controllerPort1;
extern ControllerPort controllerPort2;
extern ControllerPort extensionPort;

#include "control-pad/control-pad.hpp"
#include "fighting-pad-6b/fighting-pad-6b.hpp"
#include "sega-tap/sega-tap.hpp"
