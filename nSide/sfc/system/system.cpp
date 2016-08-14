#include <sfc/sfc.hpp>

namespace SuperFamicom {

System system;
Scheduler scheduler;
Cheat cheat;
#include "video.cpp"
#include "peripherals.cpp"
#include "random.cpp"
#include "serialization.cpp"

auto System::run() -> void {
  if(scheduler.enter() == Scheduler::Event::Frame) ppu.refresh();
}

auto System::runToSave() -> void {
  scheduler.synchronize(cpu);
  scheduler.synchronize(smp);
  scheduler.synchronize(ppu);
  scheduler.synchronize(dsp);
  for(auto coprocessor : cpu.coprocessors) scheduler.synchronize(*coprocessor);
  for(auto peripheral : cpu.peripherals) scheduler.synchronize(*peripheral);
}

auto System::init() -> void {
  assert(interface != nullptr);

  icd2.init();
  mcc.init();
  nss.init();
  event.init();
  sa1.init();
  superfx.init();
  armdsp.init();
  hitachidsp.init();
  necdsp.init();
  epsonrtc.init();
  sharprtc.init();
  spc7110.init();
  sdd1.init();
  obc1.init();
  msu1.init();

  bsmemory.init();
}

auto System::term() -> void {
}

auto System::load() -> bool {
  information = Information();

  if(auto fp = interface->open(ID::System, "manifest.bml", File::Read, File::Required)) {
    information.manifest = fp->reads();
  } else return false;

  auto document = BML::unserialize(information.manifest);
  auto system = document["system"];

  bus.reset();
  if(!cpu.load(system)) return false;
  if(!smp.load(system)) return false;
  if(!ppu.load(system)) return false;
  if(!dsp.load(system)) return false;
  if(!cartridge.load()) return false;

  information.region = cartridge.region() == Cartridge::Region::NTSC ? Region::NTSC : Region::PAL;
  if(system["region"].text() == "NTSC") information.region = Region::NTSC;
  if(system["region"].text() == "PAL" ) information.region = Region::PAL;

  information.colorburst = region() == Region::NTSC
  ? Emulator::Constants::Colorburst::NTSC
  : Emulator::Constants::Colorburst::PAL * 4.0 / 5.0;

  if(cartridge.has.ICD2) icd2.load();
  if(cartridge.has.MCC) mcc.load();
  if(cartridge.has.NSSDIP) nss.load();
  if(cartridge.has.Event) event.load();
  if(cartridge.has.SA1) sa1.load();
  if(cartridge.has.SuperFX) superfx.load();
  if(cartridge.has.ARMDSP) armdsp.load();
  if(cartridge.has.HitachiDSP) hitachidsp.load();
  if(cartridge.has.NECDSP) necdsp.load();
  if(cartridge.has.EpsonRTC) epsonrtc.load();
  if(cartridge.has.SharpRTC) sharprtc.load();
  if(cartridge.has.SPC7110) spc7110.load();
  if(cartridge.has.SDD1) sdd1.load();
  if(cartridge.has.OBC1) obc1.load();
  if(cartridge.has.MSU1) msu1.load();

  if(cartridge.has.BSMemorySlot) bsmemory.load();
  if(cartridge.has.SufamiTurboSlots) sufamiturboA.load(), sufamiturboB.load();

  serializeInit();
  return information.loaded = true;
}

auto System::save() -> void {
  if(!loaded()) return;
  cartridge.save();
}

auto System::unload() -> void {
  if(!loaded()) return;
  peripherals.unload();

  if(cartridge.has.ICD2) icd2.unload();
  if(cartridge.has.MCC) mcc.unload();
  if(cartridge.has.NSSDIP) nss.unload();
  if(cartridge.has.Event) event.unload();
  if(cartridge.has.SA1) sa1.unload();
  if(cartridge.has.SuperFX) superfx.unload();
  if(cartridge.has.ARMDSP) armdsp.unload();
  if(cartridge.has.HitachiDSP) hitachidsp.unload();
  if(cartridge.has.NECDSP) necdsp.unload();
  if(cartridge.has.EpsonRTC) epsonrtc.unload();
  if(cartridge.has.SharpRTC) sharprtc.unload();
  if(cartridge.has.SPC7110) spc7110.unload();
  if(cartridge.has.SDD1) sdd1.unload();
  if(cartridge.has.OBC1) obc1.unload();
  if(cartridge.has.MSU1) msu1.unload();

  if(cartridge.has.BSMemorySlot) bsmemory.unload();
  if(cartridge.has.SufamiTurboSlots) sufamiturboA.unload(), sufamiturboB.unload();

  cartridge.unload();
  information.loaded = false;
}

auto System::power() -> void {
  random.seed((uint)time(0));

  cpu.power();
  smp.power();
  dsp.power();
  ppu.power();

  if(cartridge.has.ICD2) icd2.power();
  if(cartridge.has.MCC) mcc.power();
  if(cartridge.has.NSSDIP) nss.power();
  if(cartridge.has.Event) event.power();
  if(cartridge.has.SA1) sa1.power();
  if(cartridge.has.SuperFX) superfx.power();
  if(cartridge.has.ARMDSP) armdsp.power();
  if(cartridge.has.HitachiDSP) hitachidsp.power();
  if(cartridge.has.NECDSP) necdsp.power();
  if(cartridge.has.EpsonRTC) epsonrtc.power();
  if(cartridge.has.SharpRTC) sharprtc.power();
  if(cartridge.has.SPC7110) spc7110.power();
  if(cartridge.has.SDD1) sdd1.power();
  if(cartridge.has.OBC1) obc1.power();
  if(cartridge.has.MSU1) msu1.power();

  if(cartridge.has.BSMemorySlot) bsmemory.power();

  reset();
}

auto System::reset() -> void {
  Emulator::video.reset();
  Emulator::video.setInterface(interface);
  configureVideoPalette();
  configureVideoEffects();

  Emulator::audio.reset();
  Emulator::audio.setInterface(interface);

  scheduler.reset();
  cpu.reset();
  smp.reset();
  dsp.reset();
  ppu.reset();

  if(cartridge.has.ICD2) icd2.reset();
  if(cartridge.has.MCC) mcc.reset();
  if(cartridge.has.NSSDIP) nss.reset();
  if(cartridge.has.Event) event.reset();
  if(cartridge.has.SA1) sa1.reset();
  if(cartridge.has.SuperFX) superfx.reset();
  if(cartridge.has.ARMDSP) armdsp.reset();
  if(cartridge.has.HitachiDSP) hitachidsp.reset();
  if(cartridge.has.NECDSP) necdsp.reset();
  if(cartridge.has.EpsonRTC) epsonrtc.reset();
  if(cartridge.has.SharpRTC) sharprtc.reset();
  if(cartridge.has.SPC7110) spc7110.reset();
  if(cartridge.has.SDD1) sdd1.reset();
  if(cartridge.has.OBC1) obc1.reset();
  if(cartridge.has.MSU1) msu1.reset();

  if(cartridge.has.BSMemorySlot) bsmemory.reset();

  if(cartridge.has.ICD2) cpu.coprocessors.append(&icd2);
  if(cartridge.has.Event) cpu.coprocessors.append(&event);
  if(cartridge.has.SA1) cpu.coprocessors.append(&sa1);
  if(cartridge.has.SuperFX) cpu.coprocessors.append(&superfx);
  if(cartridge.has.ARMDSP) cpu.coprocessors.append(&armdsp);
  if(cartridge.has.HitachiDSP) cpu.coprocessors.append(&hitachidsp);
  if(cartridge.has.NECDSP) cpu.coprocessors.append(&necdsp);
  if(cartridge.has.EpsonRTC) cpu.coprocessors.append(&epsonrtc);
  if(cartridge.has.SharpRTC) cpu.coprocessors.append(&sharprtc);
  if(cartridge.has.SPC7110) cpu.coprocessors.append(&spc7110);
  if(cartridge.has.MSU1) cpu.coprocessors.append(&msu1);

  scheduler.primary(cpu);
  peripherals.reset();
}

}
