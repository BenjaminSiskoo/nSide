auto Cartridge::parseMarkup(const char* markup) -> void {
  auto document = BML::unserialize(markup);
  auto information = document["information"];
  auto boardNode = document["board"];

  this->information.title.cartridge = information["title"].text();
  if(boardNode["region"].text() == "ntsc")  _region = Region::NTSC;
  if(boardNode["region"].text() == "pal")   _region = Region::PAL;
  if(boardNode["region"].text() == "dendy") _region = Region::Dendy;

  if(system.revision() == System::Revision::VSSystem) parseMarkupVS(document, boardNode);

  Board::load(boardNode);  //this call will set Cartridge::board if successful
  parseMarkupMemory(board->prgrom, boardNode["prg/rom"], ID::ProgramROM, false);
  parseMarkupMemory(board->prgram, boardNode["prg/ram"], ID::ProgramRAM, true);
  parseMarkupMemory(board->chrrom, boardNode["chr/rom"], ID::CharacterROM, false);
  parseMarkupMemory(board->chrram, boardNode["chr/ram"], ID::CharacterRAM, true);
  if(board->chip) {
    parseMarkupMemory(board->chip->ram, boardNode["chip/ram"], ID::ChipRAM, true);
  }
  if(system.pc10()) {
    auto rom = boardNode["pc10"].find("rom");
    parseMarkupMemory(board->instrom, rom(0), ID::InstructionROM, false);
    parseMarkupMemory(board->keyrom, rom(1), ID::KeyROM, false);
  }
}

auto Cartridge::parseMarkupVS(Markup::Node& document, Markup::Node& boardNode) -> void {
  uint ppus = 0;
  auto side = document.find("side");
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
    peripherals.connect(0, Device::Gamepad);
  } else if(device1 == "none") {
    peripherals.connect(0, Device::None);
  }
  if(device2 == "gamepad") {
    peripherals.connect(1, Device::Gamepad);
  } else if(device2 == "beamgun") {
    peripherals.connect(1, Device::BeamGun);
  } else if(device2 == "none") {
    peripherals.connect(1, Device::None);
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

auto Cartridge::parseMarkupMemory(MappedRAM& ram, Markup::Node node, uint id, bool writable) -> void {
  string name = node["name"].text();
  uint size = node["size"].natural();
  bool save = !(bool)node["volatile"];
  ram.map(allocate<uint8>(size, 0xff), size);
  if(name) {
    interface->loadRequest(id, name, !writable);
    if(writable && save) memory.append({id, name});
  }
}
