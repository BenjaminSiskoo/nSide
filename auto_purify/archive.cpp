vector<uint8_t> AutoPurify::extractROM() {
  Decode::ZIP archive;
  if(archive.open(information.archive)) {
    for(auto& file : archive.file) {
      if(file.name.endsWith(".fc")  || file.name.endsWith(".nes")
      || file.name.endsWith(".sfc") || file.name.endsWith(".smc")
      || file.name.endsWith(".st")  || file.name.endsWith(".bs")
      || file.name.endsWith(".gb")  || file.name.endsWith(".gbc")
      || file.name.endsWith(".gba")
      ) {
        information.name = filename(file.name);
        return archive.extract(file);
      }
    }
  }
  return vector<uint8_t>();
}

vector<uint8_t> AutoPurify::extractFile(const string& name) {
  Decode::ZIP archive;
  if(archive.open(information.archive)) {
    for(auto& file : archive.file) {
      if(filename(file.name) == name) {
        return archive.extract(file);
      }
    }
  }
  return vector<uint8_t>();
}
