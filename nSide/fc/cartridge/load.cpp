auto Cartridge::loadCartridge(Markup::Node node) -> void {
  information.title.cartridge = node["information/title"].text();
  auto boardNode = node["board"];
  if(boardNode["region"].text() == "ntsc")  information.region = Region::NTSC;
  if(boardNode["region"].text() == "pal")   information.region = Region::PAL;
  if(boardNode["region"].text() == "dendy") information.region = Region::Dendy;

  if(system.revision() == System::Revision::VSSystem) setupVS(node, boardNode);

  Board::load(boardNode);  //this call will set Cartridge::board if successful
  if(auto node = boardNode["prg/rom"]) loadMemory(board->prgrom, node, File::Required, pathID());
  if(auto node = boardNode["prg/ram"]) loadMemory(board->prgram, node, File::Optional, pathID());
  if(auto node = boardNode["chr/rom"]) loadMemory(board->chrrom, node, File::Required, pathID());
  if(auto node = boardNode["chr/ram"]) loadMemory(board->chrram, node, File::Optional, pathID());
  if(board->chip) if(auto node = boardNode["chip/ram"]) loadMemory(board->chip->ram, node, File::Optional, pathID());

  if(system.pc10()) {
    auto rom = boardNode["pc10"].find("rom");
    loadMemory(board->instrom, rom(0), File::Required, ID::PlayChoice10);
    loadMemory(board->keyrom,  rom(1), File::Required, ID::PlayChoice10);
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

  string cpuRevision = side(0)["cpu/revision"].text();
  vssystem.forceSubRAM = cpuRevision == "RP2A04";

  string ppuRevision = boardNode["ppu/revision"].text();
  if(ppuRevision == "RP2C02C")     ppu.revision = PPU::Revision::RP2C02C;
  if(ppuRevision == "RP2C02G")     ppu.revision = PPU::Revision::RP2C02G;
  if(ppuRevision == "RP2C03B")     ppu.revision = PPU::Revision::RP2C03B;
  if(ppuRevision == "RP2C03G")     ppu.revision = PPU::Revision::RP2C03G;
  if(ppuRevision == "RP2C04-0001") ppu.revision = PPU::Revision::RP2C04_0001;
  if(ppuRevision == "RP2C04-0002") ppu.revision = PPU::Revision::RP2C04_0002;
  if(ppuRevision == "RP2C04-0003") ppu.revision = PPU::Revision::RP2C04_0003;
  if(ppuRevision == "RP2C04-0004") ppu.revision = PPU::Revision::RP2C04_0004;
  if(ppuRevision == "RC2C03B")     ppu.revision = PPU::Revision::RC2C03B;
  if(ppuRevision == "RC2C03C")     ppu.revision = PPU::Revision::RC2C03C;
  if(ppuRevision == "RC2C05-01")   ppu.revision = PPU::Revision::RC2C05_01;
  if(ppuRevision == "RC2C05-02")   ppu.revision = PPU::Revision::RC2C05_02;
  if(ppuRevision == "RC2C05-03")   ppu.revision = PPU::Revision::RC2C05_03;
  if(ppuRevision == "RC2C05-04")   ppu.revision = PPU::Revision::RC2C05_04;
  if(ppuRevision == "RC2C05-05")   ppu.revision = PPU::Revision::RC2C05_05;

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
