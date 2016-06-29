auto Cartridge::loadCartridge(Markup::Node node) -> void {
  information.title.cartridge = node["information/title"].text();
  auto boardNode = node["board"];
  if(boardNode["region"].text() == "ntsc")  information.region = Region::NTSC;
  if(boardNode["region"].text() == "pal")   information.region = Region::PAL;
  if(boardNode["region"].text() == "dendy") information.region = Region::Dendy;

  if(system.revision() == System::Revision::VSSystem) setupVS(node, boardNode);

  Board::load(boardNode);  //this call will set Cartridge::board if successful
  if(!board) return;
  if(auto node = boardNode["prg/rom"]) loadMemory(board->prgrom, node, File::Required, pathID());
  if(auto node = boardNode["prg/ram"]) loadMemory(board->prgram, node, File::Optional, pathID());
  if(auto node = boardNode["chr/rom"]) loadMemory(board->chrrom, node, File::Required, pathID());
  if(auto node = boardNode["chr/ram"]) loadMemory(board->chrram, node, File::Optional, pathID());
  if(board->chip) if(auto node = boardNode["chip/ram"]) loadMemory(board->chip->ram, node, File::Optional, pathID());

  if(system.pc10()) {
    auto rom = boardNode["pc10"].find("rom");
    loadMemory(board->instrom, rom(0), File::Required, pathID());
    loadMemory(board->keyrom,  rom(1), File::Required, pathID());
  }
}

auto Cartridge::setupVS(Markup::Node& node, Markup::Node& boardNode) -> void {
  uint ppus = 0;
  auto side = node.find("side");
  bool primarySide;
  if(side(0)["ppu"]) {
    primarySide = 2 - side.size();
    boardNode = side(0);
    ppus++;
  }
  if(side(1)["ppu"]) {
    if(ppus == 0) {
      primarySide = 1;
      boardNode = side(1);
    }
    ppus++;
  }
  cpu.side = primarySide;
  apu.side = primarySide;
  ppu.side = primarySide;

  auto controller = boardNode.find("controller");
  vssystem.swapControllers = controller(0)["port"].integer() == 2;
  string device1 = boardNode.find("controller(port=1)/device")(0).text();
  string device2 = boardNode.find("controller(port=2)/device")(0).text();
  if(device1 == "gamepad") {
    peripherals.connect(ID::Port::Controller1, ID::Device::Gamepad);
  } else if(device1 == "none") {
    peripherals.connect(ID::Port::Controller1, ID::Device::None);
  }
  if(device2 == "gamepad") {
    peripherals.connect(ID::Port::Controller2, ID::Device::Gamepad);
  } else if(device2 == "zapper") {
    peripherals.connect(ID::Port::Controller2, ID::Device::Zapper);
  } else if(device2 == "none") {
    peripherals.connect(ID::Port::Controller2, ID::Device::None);
  }
  vssystem.setDip(primarySide, interface->dipSettings(boardNode));

  string cpuVersion = side(0)["cpu/version"].text();
  vssystem.forceSubRAM = cpuVersion == "RP2A04";

  string ppuVersion = boardNode["ppu/version"].text();
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

  if(ppus == 2) interface->information.canvasWidth = 512;
}

//

auto Cartridge::loadMemory(MappedRAM& ram, Markup::Node node, bool required, maybe<uint> id) -> void {
  string name = node["name"].text();
  uint size = node["size"].natural();
  ram.allocate(size);
  if(auto fp = interface->open(id(), name, File::Read, required)) {
    fp->read(ram.data(), ram.size());
  }
}
