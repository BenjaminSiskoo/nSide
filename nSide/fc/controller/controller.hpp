// NES controller port pinout:
// ┌────
// │(7)  \
// │(2)(1)│
// │(3)(5)│
// │(4)(6)│
// └──────┘
// pin    name     port1                            port2
//  1:    +5v
//  2:    clock    $4016 read                       $4017 read
//  3:    latch    $4016.d0 write                   $4016.d0 write
//  4:    data0    $4016.d0 read                    $4017.d0 read
//  5:    data3    $4016.d3 read                    $4017.d3 read
//  6:    data4    $4016.d4 read                    $4017.d4 read
//  7:    gnd

// Famicom expansion port pinout:
// /--------------------------------\
// \(08)(07)(06)(05)(04)(03)(02)(01)/
//  \ (15)(14)(13)(12)(11)(10)(09) /
//   \----------------------------/
// pin    name     register
// 01:    gnd
// 02:    sound
// 03:    irq
// 04:    data2.4  $4017.d4 read
// 05:    data2.3  $4017.d3 read
// 06:    data2.2  $4017.d2 read
// 07:    data2.1  $4017.d1 read
// 08:    data2.0  $4017.d0 read
// 09:    /oe 2
// 10:    out 2    $4016.d2 write
// 11:    out 1    $4016.d1 write
// 12:    latch    $4016.d0 write
// 13:    data1.1  $4016.d1 read
// 14:    /oe 1
// 15:    +5v

struct Controller : Thread {
  enum : uint { Port1 = 0, Port2 = 1, ExpansionPort = 2 };

  Controller(uint port, uint device_id);
  Controller(uint port);

  static auto Enter() -> void;
  virtual auto enter() -> void;

  auto step(uint clocks) -> void;
  auto synchronizeCPU() -> void;

  virtual auto data() -> uint5 { return 0; }
  virtual auto data1() -> uint2 { return 0; }
  virtual auto data2() -> uint5 { return 0; }
  virtual auto latch(bool data) -> void {}

  const uint port;
  const uint device_id;

protected:
  auto poll(uint input) -> int16;
};

#include "gamepad/gamepad.hpp"
#include "fourplayers/fourplayers.hpp"
#include "fourscore/fourscore.hpp"
#include "beamgun/beamgun.hpp"
#include "familytrainer/familytrainer.hpp"
#include "sfcgamepad/sfcgamepad.hpp"
#include "mouse/mouse.hpp"
#include "vspanel/vspanel.hpp"
