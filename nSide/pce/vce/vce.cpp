#include <pce/pce.hpp>

namespace PCEngine {

VCE vce;
#include "memory.cpp"
#include "io.cpp"

auto VCE::Enter() -> void {
  while(true) scheduler.synchronize(), vce.main();
}

auto VCE::main() -> void {
  vdc0.frame();
  vdc1.frame();
  timing.vclock = 0;

  while(timing.vclock < 262) {
    vdc0.scanline();
    vdc1.scanline();
    timing.hclock = 0;

    auto output = buffer + 1365 * timing.vclock;

    while(timing.hclock < 1360) {
      uint9 color;
      if(Model::PCEngine()) color = vdc0.bus();
      if(Model::SuperGrafx()) color = vpc.bus(timing.hclock);
      color = cram.read(color);

    //*output++ = color;
    //step(1);

      if(clock() >= 2) *output++ = color;
      if(clock() >= 2) *output++ = color;
      if(clock() >= 3) *output++ = color;
      if(clock() >= 4) *output++ = color;
      step(clock());
    }

    step(1365 - timing.hclock);
    timing.vclock++;
  }

  scheduler.exit(Scheduler::Event::Frame);
}

auto VCE::step(uint clocks) -> void {
  Thread::step(clocks);
  synchronize(cpu);

  timing.hclock += clocks;
}

auto VCE::refresh() -> void {
  Emulator::video.refresh(buffer + 1365 * 10, 1365 * sizeof(uint32), 1140, 242);
}

auto VCE::power() -> void {
  create(VCE::Enter, system.colorburst() * 6.0);

  for(auto& pixel : buffer) pixel = 0;
  memory::fill(&cram, sizeof(CRAM));
  memory::fill(&timing, sizeof(Timing));
  memory::fill(&io, sizeof(IO));
  io.clock = 4;
}

}
