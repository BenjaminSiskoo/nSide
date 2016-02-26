auto Cartridge::parseMarkup(const char* markup) -> void {
  auto document = BML::unserialize(markup);
  auto information = document["information"];
  auto board_node = document["board"];

  this->information.title.cartridge = information["title"].text();
  _region = board_node["region"].text() == "pal" ? Region::PAL : Region::NTSC;

  if(system.revision() == System::Revision::VSSystem) {
    uint ppus = 0;
    auto side = document.find("side");
    bool primary_side;
    if(side(0)["ppu"]) {
      primary_side = 2 - side.size();
      board_node = side(0);
      ppus++;
    }
    if(side(1)["ppu"]) {
      if(ppus == 0) {
        primary_side = 1;
        board_node = side(1);
      }
      ppus++;
    }
    cpu.side = primary_side;
    apu.side = primary_side;
    ppu.side = primary_side;

    auto controller = board_node.find("controller");
    vsarcadeboard.swap_controllers = controller(0)["port"].integer() == 2;
    string device1 = board_node.find("controller(port=1)/device")(0).text();
    string device2 = board_node.find("controller(port=2)/device")(0).text();
    if(device1 == "gamepad") {
      device.connect(0, Famicom::Device::ID::Gamepad);
    } else if(device1 == "none") {
      device.connect(0, Famicom::Device::ID::None);
    }
    if(device2 == "gamepad") {
      device.connect(1, Famicom::Device::ID::Gamepad);
    } else if(device2 == "beamgun") {
      device.connect(1, Famicom::Device::ID::BeamGun);
    } else if(device2 == "none") {
      device.connect(1, Famicom::Device::ID::None);
    }
    vsarcadeboard.set_dip(primary_side, interface->dipSettings(board_node));
    string ppu_revision = board_node["ppu/revision"].text();
    if(ppu_revision == "RP2C02C")     ppu.revision = PPU::Revision::RP2C02C;
    if(ppu_revision == "RP2C02G")     ppu.revision = PPU::Revision::RP2C02G;
    if(ppu_revision == "RP2C03B")     ppu.revision = PPU::Revision::RP2C03B;
    if(ppu_revision == "RP2C03G")     ppu.revision = PPU::Revision::RP2C03G;
    if(ppu_revision == "RP2C04-0001") ppu.revision = PPU::Revision::RP2C04_0001;
    if(ppu_revision == "RP2C04-0002") ppu.revision = PPU::Revision::RP2C04_0002;
    if(ppu_revision == "RP2C04-0003") ppu.revision = PPU::Revision::RP2C04_0003;
    if(ppu_revision == "RP2C04-0004") ppu.revision = PPU::Revision::RP2C04_0004;
    if(ppu_revision == "RC2C03B")     ppu.revision = PPU::Revision::RC2C03B;
    if(ppu_revision == "RC2C03C")     ppu.revision = PPU::Revision::RC2C03C;
    if(ppu_revision == "RC2C05-01")   ppu.revision = PPU::Revision::RC2C05_01;
    if(ppu_revision == "RC2C05-02")   ppu.revision = PPU::Revision::RC2C05_02;
    if(ppu_revision == "RC2C05-03")   ppu.revision = PPU::Revision::RC2C05_03;
    if(ppu_revision == "RC2C05-04")   ppu.revision = PPU::Revision::RC2C05_04;
    if(ppu_revision == "RC2C05-05")   ppu.revision = PPU::Revision::RC2C05_05;

    if(ppus == 2) interface->information.width = 512;
  }

  Board::load(board_node);  //this call will set Cartridge::board if successful
  parseMarkupMemory(board->prgrom, board_node["prg/rom"], ID::ProgramROM, false);
  parseMarkupMemory(board->prgram, board_node["prg/ram"], ID::ProgramRAM, true);
  parseMarkupMemory(board->chrrom, board_node["chr/rom"], ID::CharacterROM, false);
  parseMarkupMemory(board->chrram, board_node["chr/ram"], ID::CharacterRAM, true);
  if(board->chip) {
    parseMarkupMemory(board->chip->ram, board_node["chip/ram"], ID::ChipRAM, true);
  }
  if(system.pc10()) {
    auto rom = board_node["pc10"].find("rom");
    parseMarkupMemory(board->instrom, rom(0), ID::InstructionROM, false);
    parseMarkupMemory(board->keyrom, rom(1), ID::KeyROM, false);
  }
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
