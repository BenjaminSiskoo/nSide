#include <a2600/a2600.hpp>

namespace Atari2600 {

TIA tia;

#include "io.cpp"
#include "serialization.cpp"

TIA::TIA() {
  output = new uint32[160 * 228];
}

TIA::~TIA() {
  delete[] output;
}

auto TIA::step(uint clocks) -> void {
  while(clocks--) {
    io.hcounter++;
    if(io.hcounter >= 228) {
      io.hcounter = 0;
      if(io.vcounter < 511) io.vcounter++;
      //Safe-guard against software that does not use VSYNC properly
      if(io.vcounter >= (system.region() == System::Region::NTSC ? 262 : 312)) io.vcounter = 0;
    }
    Thread::step(1);
    synchronize(cpu);
  }
}

auto TIA::Enter() -> void {
  while(true) scheduler.synchronize(), tia.main();
}

auto TIA::main() -> void {
  scanline();

  io.hcounter = 0;
  for(uint x : range(57)) {
    if(x == 0) cpu.rdyLine(1);
    for(uint sub : range(4)) {
      run();
      step(1);
    }
  }
}

auto TIA::load(Markup::Node node) -> bool {
  return true;
}

auto TIA::power() -> void {
  create(Enter, system.colorburst());
  stream = Emulator::audio.createStream(1, system.colorburst());
  memory::fill(output, 160 * 228 * sizeof(uint32));

  io.playerMissile0Color   = 0x00;
  io.playerMissile1Color   = 0x00;
  io.playfieldBallColor    = 0x00;
  io.backgroundColor       = 0x00;

  io.playfieldBallPriority = false;

  player[0].numberSize = 0;
  player[0].reflect    = 0;
  player[0].graphic    = 0;
  player[0].position   = 0;
  player[0].motion     = 0;

  player[1].numberSize = 0;
  player[1].reflect    = 0;
  player[1].graphic    = 0;
  player[1].position   = 0;
  player[1].motion     = 0;

  missile[0].enable   = false;
  missile[0].size     = 0;
  missile[0].position = 0;
  missile[0].motion   = 0;

  missile[1].enable   = false;
  missile[1].size     = 0;
  missile[1].position = 0;
  missile[1].motion   = 0;

  ball.enable   = false;
  ball.size     = 0;
  ball.position = 0;
  ball.motion   = 0;

  playfield.reflect = false;
  playfield.score   = false;

  playfield.graphic0 = 0x00;
  playfield.graphic1 = 0x00;
  playfield.graphic2 = 0x00;

  collision.m0p0 = false;
  collision.m0p1 = false;
  collision.m1p0 = false;
  collision.m1p1 = false;
  collision.p0bl = false;
  collision.p0pf = false;
  collision.p1bl = false;
  collision.p1pf = false;
  collision.m0bl = false;
  collision.m0pf = false;
  collision.m1bl = false;
  collision.m1pf = false;
  collision.blpf = false;
  collision.m0m1 = false;
  collision.p0p1 = false;
}

auto TIA::run() -> void {
  stream->sample(0.0 / 32768.0);
  if(missile[0].reset) missile[0].position = player[0].position + 3;
  if(missile[1].reset) missile[1].position = player[1].position + 3;

  uint offsetX = 68;
  uint offsetY = system.region() == System::Region::NTSC ? 19 : 37;
  if(io.hcounter < offsetX || io.vcounter < offsetY) return;

  uint x = io.hcounter - offsetX;
  uint y = io.vcounter - offsetY;
  if(y >= 228) return;

  uint7 pixel = 0;

  if(!io.vblank) {
    pixel = io.backgroundColor;
    uint size;

    bool p0 = false;
    bool p1 = false;
    bool m0 = false;
    bool m1 = false;
    bool bl = false;
    bool pf = false;

    if(player[0].position >= x - 7 && player[0].position <= x) {
      uint bit = !player[0].reflect ? 7 - (x - player[0].position) : x - player[0].position;
      p0 = player[0].graphic.bit(bit);
    }

    if(player[1].position >= x - 7 && player[1].position <= x) {
      uint bit = !player[1].reflect ? 7 - (x - player[1].position) : x - player[1].position;
      p1 = player[1].graphic.bit(bit);
    }

    if(missile[0].enable) {
      size = 1 << missile[0].size;
      if(missile[0].position >= x - (size - 1) && missile[0].position <= x) m0 = true;
    }

    if(missile[1].enable) {
      size = 1 << missile[1].size;
      if(missile[1].position >= x - (size - 1) && missile[1].position <= x) m1 = true;
    }

    if(ball.enable) {
      size = 1 << ball.size;
      if(ball.position >= x - (size - 1) && ball.position <= x) bl = true;
    }

    uint playfieldX = x >> 2;
    if(playfieldX >= 20) playfieldX = !playfield.reflect ? playfieldX - 20 : 39 - playfieldX;
    if(playfieldX >=  0 && playfieldX <  4) pf = playfield.graphic0.bit(playfieldX -  0);
    if(playfieldX >=  4 && playfieldX < 12) pf = playfield.graphic1.bit(11 - playfieldX);
    if(playfieldX >= 12 && playfieldX < 20) pf = playfield.graphic2.bit(playfieldX - 12);

    if(m0 && p0) collision.m0p0 = true;
    if(m0 && p1) collision.m0p1 = true;
    if(m1 && p0) collision.m1p0 = true;
    if(m1 && p1) collision.m1p1 = true;
    if(p0 && bl) collision.p0bl = true;
    if(p0 && pf) collision.p0pf = true;
    if(p1 && bl) collision.p1bl = true;
    if(p1 && pf) collision.p1pf = true;
    if(m0 && bl) collision.m0bl = true;
    if(m0 && pf) collision.m0pf = true;
    if(m1 && bl) collision.m1bl = true;
    if(m1 && pf) collision.m1pf = true;
    if(bl && pf) collision.blpf = true;
    if(m0 && m1) collision.m0m1 = true;
    if(p0 && p1) collision.p0p1 = true;

    if(io.playfieldBallPriority && (bl || pf)) pixel = io.playfieldBallColor;
    else if(p0 || m0) pixel = io.playerMissile0Color;
    else if(p1 || m1) pixel = io.playerMissile1Color;
    else if(bl || pf) pixel = io.playfieldBallColor;
  }

  output[y * 160 + x] = pixel;
}

auto TIA::scanline() -> void {
  if(io.vcounter == 0) frame();

  if(io.vcounter == 228 + (system.region() == System::Region::NTSC ? 19 : 44)) {
    //dirty hack to prevent controls for hardware switches from being polled
    //19912 (262 * 228 / 3) times as fast as joystick/paddle controls and other emulators' controls
    if(io.vblank) pia.updateIO();

    scheduler.exit(Scheduler::Event::Frame);
  }
}

auto TIA::frame() -> void {
}

auto TIA::refresh() -> void {
  auto output = this->output;
  auto pitch = 160;
  auto width = 160;
  auto height = 228;
  Emulator::video.refresh(output, pitch * sizeof(uint32), width, height);
}

}
