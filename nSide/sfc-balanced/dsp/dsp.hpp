#include "SPC_DSP.h"

struct DSP : Thread {
  shared_pointer<Emulator::Stream> stream;

  DSP();

  alwaysinline auto step(uint clocks) -> void;
  alwaysinline auto synchronizeSMP() -> void;

  auto mute() -> bool;
  auto read(uint8 addr) -> uint8;
  auto write(uint8 addr, uint8 data) -> void;

  auto main() -> void;
  auto load(Markup::Node) -> bool;
  auto power() -> void;
  auto reset() -> void;

  auto serialize(serializer&) -> void;

private:
  SPC_DSP spc_dsp;
  int16_t samplebuffer[8192];
};

extern DSP dsp;
