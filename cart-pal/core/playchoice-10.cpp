auto CartPal::playchoice10Manifest(string location) -> string {
  vector<uint8_t> buffer;
  concatenate(buffer, {location, "ines.rom"});
  concatenate(buffer, {location, "program.rom"});
  concatenate(buffer, {location, "character.rom"});
  concatenate(buffer, {location, "instructions.rom"});
  concatenate(buffer, {location, "key.rom"});
  return playchoice10Manifest(buffer, location);
}

auto CartPal::playchoice10Manifest(vector<uint8_t>& buffer, string location,
  uint* prgrom, uint* chrrom,
  uint* instrom, uint* keyrom
) -> string {
  string manifest;
  unsigned offset = (buffer.size() & 0x1fff) == 0 ? 0 : 16;
  if(buffer.data()[0] != 'N'
  || buffer.data()[1] != 'E'
  || buffer.data()[2] != 'S'
  || buffer.data()[3] !=  26) offset = 0;

  if(settings["cart-pal/UseDatabase"].boolean() && !manifest) {
    //PlayChoice-10 ROMs in iNES format store 16 bytes for the key,
    //and 16 more bytes for Counter Out.
    //However, only 9 key bytes actually exist in the cartridge.
    //The remaining 7 key bytes, as well as the entire Counter Out,
    //follow a pattern based on hardware behavior.
    //TODO: Detect extraneous bytes and exclude them from SHA256
    string digest = Hash::SHA256(buffer.data() + offset, buffer.size() - offset).digest();
    for(auto node : database.playchoice10) {
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
  if(prgrom)  *prgrom  = document["board/prg/rom/size"].natural();  //0 if node does not exist
  if(chrrom)  *chrrom  = document["board/chr/rom/size"].natural();  //0 if node does not exist
  if(instrom) *instrom = document.find("board/pc10/rom[0]/size")(0).natural();
  if(keyrom)  *keyrom  = document.find("board/pc10/rom[1]/size")(0).natural();

  return manifest;
}

auto CartPal::playchoice10ManifestScan(vector<Markup::Node>& roms, Markup::Node node) -> void {
  if(node["name"].text().endsWith(".rom")) roms.append(node);
  for(auto leaf : node) playchoice10ManifestScan(roms, leaf);
}

auto CartPal::playchoice10Import(vector<uint8_t>& buffer, string location) -> string {
  bool has_ines_header = true;
  if(buffer.data()[0] != 'N'
  || buffer.data()[1] != 'E'
  || buffer.data()[2] != 'S'
  || buffer.data()[3] !=  26) has_ines_header = false;
  unsigned offset = has_ines_header ? 16 : 0;

  auto name = Location::prefix(location);
  auto source = Location::path(location);
  string target{settings["Library/Location"].text(), "PlayChoice-10/", name, ".pc10/"};
//if(directory::exists(target)) return failure("game already exists");

  uint prgrom = 0;
  uint chrrom = 0;
  uint instrom = 0;
  uint keyrom = 0;
  auto manifest = playchoice10Manifest(buffer, location, &prgrom, &chrrom, &instrom, &keyrom);
  if(!manifest) return failure("failed to parse ROM image");

  auto document = BML::unserialize(manifest);
  vector<Markup::Node> roms;
  if(has_ines_header) roms.append(BML::unserialize("rom name=ines.rom size=0x10")["rom"]);
  playchoice10ManifestScan(roms, document["board"]);

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
