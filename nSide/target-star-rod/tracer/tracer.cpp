#include "../laevateinn.hpp"
unique_pointer<Tracer> tracer;

auto Tracer::resetMask() -> void {
  memset(cpuMask, 0, 0x200000);
  memset(smpMask, 0, 0x2000);
}

auto Tracer::maskCPU(uint24 addr) -> bool {
  if(mask == false) return false;
  if(cpuMask[addr >> 3] & (1 << (addr & 7))) return true;
  cpuMask[addr >> 3] |= 1 << (addr & 7);
  return false;
}

auto Tracer::maskSMP(uint16 addr) -> bool {
  if(mask == false) return false;
  if(smpMask[addr >> 3] & (1 << (addr & 7))) return true;
  smpMask[addr >> 3] |= 1 << (addr & 7);
  return false;
}

auto Tracer::enabled() -> bool {
  return fp.open();
}

auto Tracer::enable(bool state) -> void {
  if(state == false) {
    debugger->print("Tracer disabled\n");
    fp.close();
    return;
  }

  //try not to overwrite existing traces: scan from 001-999.
  //if all files exist, use 000, even if it overwrites another log.
  uint n = 1;
  do {
    if(file::exists({program->folderPaths(0), "debug/trace-", natural(n, 3L), ".log"}) == false) break;
  } while(++n <= 999);

  string filename = {program->folderPaths(0), "debug/trace-", natural(n, 3L), ".log"};
  if(fp.open(filename, file::mode::write) == false) return;
  debugger->print("Tracing to ", filename, "\n");
}

Tracer::Tracer() {
  tracer = this;
  mask = false;
  cpuMask = new uint8[0x200000]();
  smpMask = new uint8[0x2000]();
}

Tracer::~Tracer() {
  delete[] cpuMask;
  delete[] smpMask;
}