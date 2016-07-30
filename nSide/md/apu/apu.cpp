#include <md/md.hpp>

namespace MegaDrive {

APU apu;

auto APU::Enter() -> void {
  while(true) scheduler.synchronize(), apu.main();
}

auto APU::main() -> void {
  step(system.colorburst());
}

auto APU::step(uint clocks) -> void {
}

auto APU::stop() -> bool {
}

auto APU::power() -> void {
}

auto APU::reset() -> void {
  create(APU::Enter, system.colorburst());
}

auto APU::io() -> void {
}

auto APU::read(uint16 addr) -> uint8 {
  return 0x00;
}

auto APU::write(uint16 addr, uint8 data) -> void {
}

auto APU::portRead(uint8 port) -> uint8 {
  return 0x00;
}

auto APU::portWrite(uint8 port, uint8 data) -> void {
}

}
