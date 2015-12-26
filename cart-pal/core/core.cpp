CartPal::CartPal() {
  //database.famicom = BML::unserialize(string::read(locate({configpath(), "cart-pal/"}, "Database/Famicom.bml")));
  database.superFamicom = BML::unserialize(string::read(locate({configpath(), "cart-pal/"}, "Database/Super Famicom.bml")));
  database.gameBoy = BML::unserialize(string::read(locate({configpath(), "cart-pal/"}, "Database/Game Boy.bml")));
  database.gameBoyColor = BML::unserialize(string::read(locate({configpath(), "cart-pal/"}, "Database/Game Boy Color.bml")));
  database.gameBoyAdvance = BML::unserialize(string::read(locate({configpath(), "cart-pal/"}, "Database/Game Boy Advance.bml")));
  database.bsMemory = BML::unserialize(string::read(locate({configpath(), "cart-pal/"}, "Database/BS Memory.bml")));
  database.sufamiTurbo = BML::unserialize(string::read(locate({configpath(), "cart-pal/"}, "Database/Sufami Turbo.bml")));
}

auto CartPal::error() const -> string {
  return errorMessage;
}

auto CartPal::success() -> bool {
  errorMessage = "";
  return true;
}

auto CartPal::failure(string message) -> bool {
  errorMessage = message;
  return false;
}

auto CartPal::manifest(string location) -> string {
  location.transform("\\", "/").rtrim("/").append("/");
  if(!directory::exists(location)) return "";

  auto type = suffixname(location).downcase();
  //if(type == ".fc") return famicomManifest(location);
  if(type == ".sfc") return superFamicomManifest(location);
  if(type == ".gb") return gameBoyManifest(location);
  if(type == ".gbc") return gameBoyColorManifest(location);
  if(type == ".gba") return gameBoyAdvanceManifest(location);
  if(type == ".bs") return bsMemoryManifest(location);
  if(type == ".st") return sufamiTurboManifest(location);

  return "";
}

auto CartPal::import(string location) -> bool {
  location.transform("\\", "/").rtrim("/");
  if(!file::exists(location)) return failure("file does not exist");
  if(!file::readable(location)) return failure("file is unreadable");

  auto name = prefixname(location);
  auto type = suffixname(location).downcase();
  if(!name || !type) return failure("invalid file name");

  auto buffer = file::read(location);
  if(!buffer) return failure("file is empty");

  if(type == ".zip") {
    Decode::ZIP zip;
    if(!zip.open(location)) return failure("ZIP archive is invalid");
    if(!zip.file) return failure("ZIP archive is empty");

    name = prefixname(zip.file[0].name);
    type = suffixname(zip.file[0].name).downcase();
    buffer = zip.extract(zip.file[0]);
  }

  //if(type == ".fc" || type == ".nes") return famicomImport(buffer, location);
  if(type == ".sfc" || type == ".smc") return superFamicomImport(buffer, location);
  if(type == ".gb") return gameBoyImport(buffer, location);
  if(type == ".gbc") return gameBoyColorImport(buffer, location);
  if(type == ".gba") return gameBoyAdvanceImport(buffer, location);
  if(type == ".bs") return bsMemoryImport(buffer, location);
  if(type == ".st") return sufamiTurboImport(buffer, location);

  return failure("unrecognized file extension");
}

auto CartPal::concatenate(vector<uint8>& output, string location) -> void {
  if(auto input = file::read(location)) {
    auto size = output.size();
    output.resize(size + input.size());
    memory::copy(output.data() + size, input.data(), input.size());
  }
}
