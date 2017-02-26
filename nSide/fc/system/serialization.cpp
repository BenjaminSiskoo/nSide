auto System::serialize() -> serializer {
  serializer s(serializeSize);

  uint signature = 0x31545342;
  char version[16] = {};
  char hash[64] = {};
  char description[512] = {};
  memory::copy(&version, (const char*)Emulator::SerializerVersion, Emulator::SerializerVersion.size());
  memory::copy(&hash, (const char*)cartridgeSlot[model() == Model::VSSystem].sha256(), 64);

  s.integer(signature);
  s.array(version);
  s.array(hash);
  s.array(description);

  serializeAll(s);
  return s;
}

auto System::unserialize(serializer& s) -> bool {
  uint signature = 0;
  char version[16] = {};
  char hash[64] = {};
  char description[512] = {};

  s.integer(signature);
  s.array(version);
  s.array(hash);
  s.array(description);

  if(signature != 0x31545342) return false;
  if(string{version} != Emulator::SerializerVersion) return false;

  power();
  serializeAll(s);
  return true;
}

//internal

auto System::serialize(serializer& s) -> void {
  s.integer((uint&)information.region);
}

auto System::serializeAll(serializer& s) -> void {
  for(auto& cartridge : cartridgeSlot) cartridge.serialize(s);
  system.serialize(s);
  random.serialize(s);

  cpu0.serialize(s);
  apu0.serialize(s);
  ppu0.serialize(s);

  if(model() == Model::VSSystem) {
    cpu1.serialize(s);
    apu1.serialize(s);
    ppu1.serialize(s);
    vssystem.serialize(s);
  }

  if(model() == Model::PlayChoice10) playchoice10.serialize(s);
  if(model() == Model::FamicomBox)   famicombox.serialize(s);
}

//perform dry-run state save:
//determines exactly how many bytes are needed to save state for this cartridge,
//as amount varies per game (eg different RAM sizes, etc.)
auto System::serializeInit() -> void {
  serializer s;

  uint signature = 0;
  char version[16] = {};
  char hash[64] = {};
  char description[512] = {};

  s.integer(signature);
  s.array(version);
  s.array(hash);
  s.array(description);

  serializeAll(s);
  serializeSize = s.size();
}
