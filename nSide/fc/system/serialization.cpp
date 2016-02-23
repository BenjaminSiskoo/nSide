auto System::serialize() -> serializer {
  serializer s(_serializeSize);

  uint signature = 0x31545342, version = Info::SerializerVersion;
  char hash[64], description[512];
  memcpy(&hash, (const char*)cartridge.sha256(), 64);
  memset(&description, 0, sizeof description);

  s.integer(signature);
  s.integer(version);
  s.array(hash);
  s.array(description);

  serializeAll(s);
  return s;
}

auto System::unserialize(serializer& s) -> bool {
  uint signature, version;
  char hash[64], description[512];

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

  if(revision() == Revision::VSSystem) vsarcadeboard.serialize(s);
  if(revision() == Revision::PlayChoice10) pc10arcadeboard.serialize(s);
}

//perform dry-run state save:
//determines exactly how many bytes are needed to save state for this cartridge,
//as amount varies per game (eg different RAM sizes, etc.)
auto System::serializeInit() -> void {
  serializer s;

  uint signature = 0, version = 0;
  char hash[64], description[512];

  s.integer(signature);
  s.integer(version);
  s.array(hash);
  s.array(description);

  serializeAll(s);
  _serializeSize = s.size();
}
