#include <ms/ms.hpp>

namespace MasterSystem {

VDP vdp;
#include "io.cpp"
#include "render.cpp"

auto VDP::Enter() -> void {
  while(true) scheduler.synchronize(), vdp.main();
}

auto VDP::main() -> void {
  scanline();
  if(state.y < activeHeight()) {
    for(uint x : range(activeWidth())) {
      run();
      step(4);
    }
    step(344);
    if(state.y == screenY() + screenHeight()) scheduler.exit(Scheduler::Event::Frame);
  } else {
    step(1368);
  }
}

auto VDP::step(uint clocks) -> void {
  Thread::step(clocks);
  synchronize(cpu);
}

auto VDP::refresh() -> void {
  Emulator::video.refresh(buffer, 256 * sizeof(uint32), screenWidth(), screenHeight());
}

auto VDP::in(uint8 addr) -> uint8 {
  switch(addr) {
  case 0x40: case 0x42: case 0x44: case 0x46:
  case 0x48: case 0x4a: case 0x4c: case 0x4e:
  case 0x50: case 0x52: case 0x54: case 0x56:
  case 0x58: case 0x5a: case 0x5c: case 0x5e:
  case 0x60: case 0x62: case 0x64: case 0x66:
  case 0x68: case 0x6a: case 0x6c: case 0x6e:
  case 0x70: case 0x72: case 0x74: case 0x76:
  case 0x78: case 0x7a: case 0x7c: case 0x7e:  //port $7e
    return 0xb0/*hcounter*/;
  case 0x41: case 0x43: case 0x45: case 0x47:
  case 0x49: case 0x4b: case 0x4d: case 0x4f:
  case 0x51: case 0x53: case 0x55: case 0x57:
  case 0x59: case 0x5b: case 0x5d: case 0x5f:
  case 0x61: case 0x63: case 0x65: case 0x67:
  case 0x69: case 0x6b: case 0x6d: case 0x6f:
  case 0x71: case 0x73: case 0x75: case 0x77:
  case 0x79: case 0x7b: case 0x7d: case 0x7f:  //port $7f
    return 0xb0/*vcounter*/;
  case 0x80: case 0x82: case 0x84: case 0x86:
  case 0x88: case 0x8a: case 0x8c: case 0x8e:
  case 0x90: case 0x92: case 0x94: case 0x96:
  case 0x98: case 0x9a: case 0x9c: case 0x9e:
  case 0xa0: case 0xa2: case 0xa4: case 0xa6:
  case 0xa8: case 0xaa: case 0xac: case 0xae:
  case 0xb0: case 0xb2: case 0xb4: case 0xb6:
  case 0xb8: case 0xba: case 0xbc: case 0xbe:  //port $be
    return readData();
  case 0x81: case 0x83: case 0x85: case 0x87:
  case 0x89: case 0x8b: case 0x8d: case 0x8f:
  case 0x91: case 0x93: case 0x95: case 0x97:
  case 0x99: case 0x9b: case 0x9d: case 0x9f:
  case 0xa1: case 0xa3: case 0xa5: case 0xa7:
  case 0xa9: case 0xab: case 0xad: case 0xaf:
  case 0xb1: case 0xb3: case 0xb5: case 0xb7:
  case 0xb9: case 0xbb: case 0xbd: case 0xbf:  //port $bf
    return readControl();
  }

  return 0xb0;
}

auto VDP::out(uint8 addr, uint8 data) -> void {
  switch(addr) {

  case 0x80: case 0x82: case 0x84: case 0x86:
  case 0x88: case 0x8a: case 0x8c: case 0x8e:
  case 0x90: case 0x92: case 0x94: case 0x96:
  case 0x98: case 0x9a: case 0x9c: case 0x9e:
  case 0xa0: case 0xa2: case 0xa4: case 0xa6:
  case 0xa8: case 0xaa: case 0xac: case 0xae:
  case 0xb0: case 0xb2: case 0xb4: case 0xb6:
  case 0xb8: case 0xba: case 0xbc: case 0xbe: {  //port $be
    writeData(data);
    break;
  }

  case 0x81: case 0x83: case 0x85: case 0x87:
  case 0x89: case 0x8b: case 0x8d: case 0x8f:
  case 0x91: case 0x93: case 0x95: case 0x97:
  case 0x99: case 0x9b: case 0x9d: case 0x9f:
  case 0xa1: case 0xa3: case 0xa5: case 0xa7:
  case 0xa9: case 0xab: case 0xad: case 0xaf:
  case 0xb1: case 0xb3: case 0xb5: case 0xb7:
  case 0xb9: case 0xbb: case 0xbd: case 0xbf: {  //port $bf
    writeControl(data);
    break;
  }

  }
}

auto VDP::power() -> void {
}

auto VDP::reset() -> void {
  create(VDP::Enter, system.colorburst() * 6.0);
}

auto VDP::vblank() -> bool {
  return false;
}

inline auto VDP::screenX() -> uint {
  return system.model() != Model::GameGear ? 0 : (256 - 160) / 2;
}

inline auto VDP::screenY() -> uint {
  return system.model() != Model::GameGear ? 0 : (240 - 144) / 2;
}

inline auto VDP::screenWidth() -> uint {
  return system.model() != Model::GameGear ? 256 : 160;
}

inline auto VDP::screenHeight() -> uint {
  switch(system.model()) {
  case Model::SG1000: return 192;
  case Model::MasterSystem: return 240;
  case Model::GameGear: return 144;
  }
}

inline auto VDP::activeWidth() -> uint {
  return 256;
}

inline auto VDP::activeHeight() -> uint {
  return system.model() == Model::SG1000 ? 192 : 240;
}

}
