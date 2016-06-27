#include <fc/fc.hpp>

namespace Famicom {

PPU ppu;

#include "memory.cpp"
#include "mmio.cpp"
#include "render.cpp"
#include "serialization.cpp"

PPU::PPU() {
  output = new uint32[256 * 312];
}

PPU::~PPU() {
  delete[] output;
}

auto PPU::step(uint clocks) -> void {
  clock += clocks;
}

auto PPU::synchronizeCPU() -> void {
  if(clock >= 0 && !scheduler.synchronizing()) co_switch(cpu.thread);
}

auto PPU::Enter() -> void {
  while(true) scheduler.synchronize(), ppu.main();
}

auto PPU::main() -> void {
  renderScanline();
}

auto PPU::addClocks(uint clocks) -> void {
  uint vbl = system.region() != System::Region::Dendy ? 241 : 291;
  uint pre = system.region() == System::Region::NTSC ? 261 : 311;

  while(clocks--) {
    if(vcounter() == vbl - 1 && hcounter() == 340) r.nmiHold = 1;

    if(vcounter() == vbl && hcounter() ==   0) r.chrAddressBus = r.vaddr & 0x3fff;
    if(vcounter() == vbl && hcounter() ==   0) r.nmiFlag = r.nmiHold;
    if(vcounter() == vbl && hcounter() ==   2) cpu.nmiLine(r.nmiEnable && r.nmiFlag);

    if(vcounter() == pre - 1 && hcounter() == 340) r.nmiHold = 0;
    if(vcounter() == pre     && hcounter() ==   0) r.nmiFlag = r.nmiHold;
    if(vcounter() == pre     && hcounter() ==   1) r.spriteZeroHit = 0, r.spriteOverflow = 0;
    if(vcounter() == pre     && hcounter() ==   2) cpu.nmiLine(r.nmiEnable && r.nmiFlag);

    step(system.region() == System::Region::NTSC ? 4 : 5);
    synchronizeCPU();

    for(uint i = 0; i < 8; i++) {
      if(--r.mdrDecay[i] == 0) r.mdr &= ~(1 << i);
    }

    tick(1);
  }
}

auto PPU::load(Markup::Node node) -> bool {
  if(system.vs()) return true;

  string versionString;
  if(system.fc()) {
    print("Famicom\n");
    print("Region ID: ", (uint)system.region(), "\n");
    if(system.region() == System::Region::NTSC)  versionString = node["ppu/ntsc-version"].text();
    if(system.region() == System::Region::PAL)   versionString = node["ppu/pal-version"].text();
    if(system.region() == System::Region::Dendy) versionString = node["ppu/dendy-version"].text();
  } else {
    print("VS. System, PlayChoice-10, or FamicomBox\n");
    versionString = node["ppu/version"].text();
  }
  print("Version: ", versionString, "\n");

  //YIQ
  if(versionString == "RP2C02C")     version = Version::RP2C02C;
  if(versionString == "RP2C02G")     version = Version::RP2C02G;

  //RGB
  if(versionString == "RP2C03B")     version = Version::RP2C03B;
  if(versionString == "RP2C03G")     version = Version::RP2C03G;
  if(versionString == "RP2C04-0001") version = Version::RP2C04_0001;
  if(versionString == "RP2C04-0002") version = Version::RP2C04_0002;
  if(versionString == "RP2C04-0003") version = Version::RP2C04_0003;
  if(versionString == "RP2C04-0004") version = Version::RP2C04_0004;
  if(versionString == "RC2C03B")     version = Version::RC2C03B;
  if(versionString == "RC2C03C")     version = Version::RC2C03C;
  if(versionString == "RC2C05-01")   version = Version::RC2C05_01;
  if(versionString == "RC2C05-02")   version = Version::RC2C05_02;
  if(versionString == "RC2C05-03")   version = Version::RC2C05_03;
  if(versionString == "RC2C05-04")   version = Version::RC2C05_04;
  if(versionString == "RC2C05-05")   version = Version::RC2C05_05;

  //YUV
  if(versionString == "RP2C07")      version = Version::RP2C07;
  if(versionString == "UA6538")      version = Version::UA6538;

  return true;
}

auto PPU::power() -> void {
  r.vaddr = 0x0000;

  r.nmiHold = 0;
  r.nmiFlag = 1;

  //$2003  OAMADDR
  r.oamAddress = 0x00;

  for(auto& n : ciram) n = 0xff;
}

auto PPU::reset() -> void {
  create(PPU::Enter, system.cpuFrequency());
  PPUcounter::reset();
  memory::fill(output, 256 * 312 * sizeof(uint32));

  function<auto (uint16, uint8) -> uint8> reader{&PPU::read, this};
  function<auto (uint16, uint8) -> void> writer{&PPU::write, this};
  bus.map(reader, writer, "2000-3fff");

  r.mdr = 0x00;
  r.busData = 0x00;
  r.addressLatch = 0;

  r.taddr = 0x0000;
  r.xaddr = 0x00;

  //$2000  PPUCTRL
  r.nmiEnable = false;
  r.masterSelect = 0;
  r.spriteHeight = 8;
  r.bgAddress = 0x0000;
  r.objAddress = 0x0000;
  r.vramIncrement = 1;

  //$2001  PPUMASK
  r.emphasis = 0;
  r.objEnable = false;
  r.bgEnable = false;
  r.objEdgeEnable = false;
  r.bgEdgeEnable = false;
  r.grayscale = false;

  //$2002  PPUSTATUS
  r.spriteZeroHit = false;
  r.spriteOverflow = false;

  for(auto& n : cgram) n = 0;
  for(auto& n : oam) n = 0;
}

auto PPU::scanline() -> void {
  if(vcounter() == 0) frame();
  cartridge.scanline(vcounter());

  if(vcounter() == 241) {
    scheduler.exit(Scheduler::Event::Frame);
  }
}

auto PPU::frame() -> void {
  //TODO: Verify whether putting the scheduler exit event at vcounter() == 241 reduces lag as opposed to here
}

auto PPU::origin_x() -> uint {
  return (system.vs() && interface->information.canvasWidth == 512) ? side * 256 : 0;
}

auto PPU::origin_y() -> uint {
  if(system.pc10()) {
    switch(playchoice10.screenConfig) {
    case PlayChoice10::ScreenConfig::Dual:   return 240;
    case PlayChoice10::ScreenConfig::Single: return   0;
    }
  } else {
    return 0;
  }
}

auto PPU::refresh() -> void {
  auto output = this->output;
  Emulator::video.refreshRegion(output, 256 * sizeof(uint32), origin_x(), origin_y(), 256, 240);
}

//

auto PPU::ext() -> uint4 {
  return 0; //EXT pins are tied to ground
}

// Arcade RGB palettes

const uint9 PPU::RP2C03[16 * 4] = {
  0333,0014,0006,0326,0403,0503,0510,0420,0320,0120,0031,0040,0022,0000,0000,0000,
  0555,0036,0027,0407,0507,0704,0700,0630,0430,0140,0040,0053,0044,0000,0000,0000,
  0777,0357,0447,0637,0707,0737,0740,0750,0660,0360,0070,0276,0077,0000,0000,0000,
  0777,0567,0657,0757,0747,0755,0764,0772,0773,0572,0473,0276,0467,0000,0000,0000,
};

const uint9 PPU::RP2C04_0001[16 * 4] = {
  0755,0637,0700,0447,0044,0120,0222,0704,0777,0333,0750,0503,0403,0660,0320,0777,
  0357,0653,0310,0360,0467,0657,0764,0027,0760,0276,0000,0200,0666,0444,0707,0014,
  0003,0567,0757,0070,0077,0022,0053,0507,0000,0420,0747,0510,0407,0006,0740,0000,
  0000,0140,0555,0031,0572,0326,0770,0630,0020,0036,0040,0111,0773,0737,0430,0473,
};

const uint9 PPU::RP2C04_0002[16 * 4] = {
  0000,0750,0430,0572,0473,0737,0044,0567,0700,0407,0773,0747,0777,0637,0467,0040,
  0020,0357,0510,0666,0053,0360,0200,0447,0222,0707,0003,0276,0657,0320,0000,0326,
  0403,0764,0740,0757,0036,0310,0555,0006,0507,0760,0333,0120,0027,0000,0660,0777,
  0653,0111,0070,0630,0022,0014,0704,0140,0000,0077,0420,0770,0755,0503,0031,0444,
};

const uint9 PPU::RP2C04_0003[16 * 4] = {
  0507,0737,0473,0555,0040,0777,0567,0120,0014,0000,0764,0320,0704,0666,0653,0467,
  0447,0044,0503,0027,0140,0430,0630,0053,0333,0326,0000,0006,0700,0510,0747,0755,
  0637,0020,0003,0770,0111,0750,0740,0777,0360,0403,0357,0707,0036,0444,0000,0310,
  0077,0200,0572,0757,0420,0070,0660,0222,0031,0000,0657,0773,0407,0276,0760,0022,
};

const uint9 PPU::RP2C04_0004[16 * 4] = {
  0430,0326,0044,0660,0000,0755,0014,0630,0555,0310,0070,0003,0764,0770,0040,0572,
  0737,0200,0027,0747,0000,0222,0510,0740,0653,0053,0447,0140,0403,0000,0473,0357,
  0503,0031,0420,0006,0407,0507,0333,0704,0022,0666,0036,0020,0111,0773,0444,0707,
  0757,0777,0320,0700,0760,0276,0777,0467,0000,0750,0637,0567,0360,0657,0077,0120,
};

}
