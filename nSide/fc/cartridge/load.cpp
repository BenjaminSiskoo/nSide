auto Cartridge::loadCartridge(Markup::Node node) -> void {
  information.title.cartridge = node["information/title"].text();
  auto boardNode = node["board"];
  if(!region() || region() == "Auto") {
    if(boardNode["region"].text() == "ntsc-j") information.region = "NTSC-J";
    if(boardNode["region"].text() == "ntsc-u") information.region = "NTSC-U";
    if(boardNode["region"].text() == "pal") information.region = "PAL";
    if(boardNode["region"].text() == "dendy") information.region = "Dendy";
  }

  if(Model::VSSystem()) {
    vssystem.gameCount = 0;
    auto sides = node.find("side");
    if(sides.size() == 2) cartridgeSlot[0].setupVS(sides(0), pathID());
    cartridgeSlot[1].setupVS(sides(sides.size() - 1), pathID());
    return;
  }

  board = Board::load(boardNode);
  if(!board) return;
  board->slot = slot;
  if(auto node = boardNode["prg/rom"]) loadMemory(board->prgrom, node, File::Required, pathID());
  if(auto node = boardNode["prg/ram"]) loadMemory(board->prgram, node, File::Optional, pathID());
  if(auto node = boardNode["chr/rom"]) loadMemory(board->chrrom, node, File::Required, pathID());
  if(auto node = boardNode["chr/ram"]) loadMemory(board->chrram, node, File::Optional, pathID());
  if(board->chip) {
    if(auto node = boardNode["chip/ram"]) loadMemory(board->chip->ram, node, File::Optional, pathID());
  }

  if(Model::PlayChoice10()) {
    auto rom = boardNode["pc10"].find("rom");
    loadMemory(board->instrom, rom(0), File::Required, pathID());
    loadMemory(board->keyrom,  rom(1), File::Required, pathID());
  }
}

auto Cartridge::setupVS(Markup::Node& side, uint _pathID) -> void {
  if(slot == 0) vssystem.forceSubRAM = side["cpu/version"].text() == "RP2A04";

  if(!side["ppu"]) return;

  vssystem.gameCount++;
  if(side["setting"]) vssystem.setDip(slot, platform->dipSettings(side));

  information.pathID = _pathID;
  information.region = "NTSC-U";

  auto& ppu = slot ? ppuS : ppuM;
  string ppuVersion = side["ppu/version"].text();
  if(ppuVersion == "RP2C02C")     ppu.version = PPU::Version::RP2C02C;
  if(ppuVersion == "RP2C02G")     ppu.version = PPU::Version::RP2C02G;
  if(ppuVersion == "RP2C03B")     ppu.version = PPU::Version::RP2C03B;
  if(ppuVersion == "RP2C03G")     ppu.version = PPU::Version::RP2C03G;
  if(ppuVersion == "RP2C04-0001") ppu.version = PPU::Version::RP2C04_0001;
  if(ppuVersion == "RP2C04-0002") ppu.version = PPU::Version::RP2C04_0002;
  if(ppuVersion == "RP2C04-0003") ppu.version = PPU::Version::RP2C04_0003;
  if(ppuVersion == "RP2C04-0004") ppu.version = PPU::Version::RP2C04_0004;
  if(ppuVersion == "RC2C03B")     ppu.version = PPU::Version::RC2C03B;
  if(ppuVersion == "RC2C03C")     ppu.version = PPU::Version::RC2C03C;
  if(ppuVersion == "RC2C05-01")   ppu.version = PPU::Version::RC2C05_01;
  if(ppuVersion == "RC2C05-02")   ppu.version = PPU::Version::RC2C05_02;
  if(ppuVersion == "RC2C05-03")   ppu.version = PPU::Version::RC2C05_03;
  if(ppuVersion == "RC2C05-04")   ppu.version = PPU::Version::RC2C05_04;
  if(ppuVersion == "RC2C05-05")   ppu.version = PPU::Version::RC2C05_05;

  board = Board::load(side);
  if(!board) return;
  board->slot = slot;
  if(auto node = side["prg/rom"]) loadMemory(board->prgrom, node, File::Required, pathID());
  if(auto node = side["prg/ram"]) loadMemory(board->prgram, node, File::Optional, pathID());
  if(auto node = side["chr/rom"]) loadMemory(board->chrrom, node, File::Required, pathID());
  if(auto node = side["chr/ram"]) loadMemory(board->chrram, node, File::Optional, pathID());
  if(board->chip) {
    if(auto node = side["chip/ram"]) loadMemory(board->chip->ram, node, File::Optional, pathID());
  }
}

//

auto Cartridge::loadMemory(MappedRAM& ram, Markup::Node node, bool required, maybe<uint> id) -> void {
  string name = node["name"].text();
  uint size = node["size"].natural();
  ram.allocate(size);
  if(auto fp = platform->open(id(), name, File::Read, required)) {
    fp->read(ram.data(), ram.size());
  }
}
