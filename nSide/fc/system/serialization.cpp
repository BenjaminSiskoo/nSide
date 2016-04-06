auto System::serialize() -> serializer {
  serializer s(_serializeSize);

  uint signature = 0x31545342;
  uint version = Info::SerializerVersion;
  char hash[64] = {0};
  char description[512] = {0};
  memory::copy(&hash, (const char*)cartridge.sha256(), 64);

  s.integer(signature);
  s.integer(version);
  s.array(hash);
  s.array(description);

  serializeAll(s);
  return s;
}

auto System::unserialize(serializer& s) -> bool {
  uint signature = 0;
  uint version = 0;
  char hash[64] = {0};
  char description[512] = {0};

  s.integer(signature);
  s.integer(version);
  s.array(hash);
  s.array(description);

  if(signature != 0x31545342) return false;
  if(version != Info::SerializerVersion) return false;

  power();
  serializeAll(s);
  return true;
}

//internal

auto System::serialize(serializer& s) -> void {
  s.integer((uint&)_region);
}

auto System::serializeAll(serializer& s) -> void {
  cartridge.serialize(s);
  system.serialize(s);
  cpu.serialize(s);
  apu.serialize(s);
  ppu.serialize(s);

  if(revision() == Revision::VSSystem) vssystem.serialize(s);
  if(revision() == Revision::PlayChoice10) playchoice10.serialize(s);
}

//perform dry-run state save:
//determines exactly how many bytes are needed to save state for this cartridge,
//as amount varies per game (eg different RAM sizes, etc.)
auto System::serializeInit() -> void {
  serializer s;

  uint signature = 0;
  uint version = 0;
  char hash[64] = {0};
  char description[512] = {0};

  s.integer(signature);
  s.integer(version);
  s.array(hash);
  s.array(description);

  serializeAll(s);
  _serializeSize = s.size();
}
