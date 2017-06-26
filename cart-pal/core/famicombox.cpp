auto CartPal::famicomboxManifest(string location) -> string {
  vector<uint8_t> buffer;
  concatenate(buffer, {location, "ines.rom"});
  concatenate(buffer, {location, "program.rom"});
  concatenate(buffer, {location, "character.rom"});
  return famicomboxManifest(buffer, location);
}

auto CartPal::famicomboxManifest(vector<uint8_t>& buffer, string location, uint* prgrom, uint* chrrom) -> string {
  string manifest;
  unsigned offset = (buffer.size() & 0x1fff) == 0 ? 0 : 16;
  if(buffer.data()[0] != 'N'
  || buffer.data()[1] != 'E'
  || buffer.data()[2] != 'S'
  || buffer.data()[3] !=  26) offset = 0;

  if(settings["cart-pal/UseDatabase"].boolean() && !manifest) {
    string digest = Hash::SHA256(buffer.data() + offset, buffer.size() - offset).digest();
    for(auto node : database.famicombox) {
      if(node["sha256"].text() == digest) {
        manifest.append(node.text(), "\n  sha256:   ", digest, "\n");
        break;
      }
    }
  }

  if(settings["cart-pal/UseHeuristics"].boolean() && !manifest) {
    FamicomCartridge cartridge{location, (uint8*)buffer.data(), buffer.size()};
    manifest = cartridge.manifest;
  }

  auto document = BML::unserialize(manifest);
  if(prgrom) *prgrom = document["board/prg/rom/size"].natural();  //0 if node does not exist
  if(chrrom) *chrrom = document["board/chr/rom/size"].natural();  //0 if node does not exist

  return manifest;
}

auto CartPal::famicomboxManifestScan(vector<Markup::Node>& roms, Markup::Node node) -> void {
  if(node["name"].text().endsWith(".rom")) roms.append(node);
  for(auto leaf : node) famicomboxManifestScan(roms, leaf);
}

auto CartPal::famicomboxImport(vector<uint8_t>& buffer, string location) -> string {
  bool has_ines_header = true;
  if(buffer.data()[0] != 'N'
  || buffer.data()[1] != 'E'
  || buffer.data()[2] != 'S'
  || buffer.data()[3] !=  26) has_ines_header = false;
  unsigned offset = has_ines_header ? 16 : 0;

  if(has_ines_header) {
    if(buffer.data()[7] & 0x01) return vsSystemImport(buffer, location);
    if((buffer.data()[7] & 0x0c) == 0x08) { // NES 2.0
      if(buffer.data()[7] & 0x02) return playchoice10Import(buffer, location);
    }
  }

  auto name = Location::prefix(location);
  auto source = Location::path(location);
  string target{settings["Library/Location"].text(), "FamicomBox/", name, ".fcb/"};
//if(directory::exists(target)) return failure("game already exists");

  uint prgrom = 0;
  uint chrrom = 0;
  auto manifest = famicomboxManifest(buffer, location, &prgrom, &chrrom);
  if(!manifest) return failure("failed to parse ROM image");

  auto document = BML::unserialize(manifest);
  vector<Markup::Node> roms;
  if(has_ines_header) roms.append(BML::unserialize("rom name=ines.rom size=0x10")["rom"]);
  famicomboxManifestScan(roms, document["board"]);

  if(!directory::create(target)) return failure("library path unwritable");
  if(file::exists({source, name, ".sav"}) && !file::exists({target, "save.ram"})) {
    file::copy({source, name, ".sav"}, {target, "save.ram"});
  }

  if(settings["cart-pal/CreateManifests"].boolean()) file::write({target, "manifest.bml"}, manifest);
  for(auto rom : roms) {
    auto name = rom["name"].text();
    auto size = rom["size"].natural();
    if(size > buffer.size() - offset) return failure("ROM image is missing data");
    file::write({target, name}, buffer.data() + offset, size);
    offset += size;
  }
  return success(target);
}
