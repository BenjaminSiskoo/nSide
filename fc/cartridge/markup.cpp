#ifdef CARTRIDGE_CPP

void Cartridge::parse_markup(const char* markup) {
  auto document = Markup::Document(markup);
  information.title.cartridge = document["information/title"].text();

  auto cartridge = document["cartridge"];
  region = cartridge["region"].data != "PAL" ? Region::NTSC : Region::PAL;

  if(system.revision == System::Revision::VSSystem) {
    unsigned ppus = 0;
    if(document["cartridge/vs[0]"]["ppu"].exists()) {
      cartridge = cartridge["vs[0]"];
      ppus++;
    }
    if(document["cartridge/vs[1]"]["ppu"].exists()) {
      if(ppus == 0) cartridge = cartridge["vs[1]"];
      ppus++;
    }
    vsarcadeboard.swap_controllers = cartridge["controller[0]/port"].integer() == 2;
    string device1 = cartridge["controller(port=1)/device"].text();
    string device2 = cartridge["controller(port=2)/device"].text();
    if(device1 == "joypad") {
      input.connect(0, Input::Device::Joypad);
    } else if(device1 == "none") {
      input.connect(0, Input::Device::None);
    }
    if(device2 == "joypad") {
      input.connect(1, Input::Device::Joypad);
    } else if(device2 == "beamgun") {
      input.connect(1, Input::Device::BeamGun);
    } else if(device2 == "none") {
      input.connect(1, Input::Device::None);
    }
    vsarcadeboard.set_dip(interface->dipSettings(cartridge));
    string ppu_revision = cartridge["ppu/revision"].data;
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
  Board::load(document["cartridge"]);  //this call will set Cartridge::board if successful
  parse_markup_cartridge(cartridge);
}

//

void Cartridge::parse_markup_memory(MappedRAM& ram, Markup::Node node, unsigned id, bool writable) {
  string name = node["name"].data;
  unsigned size = numeral(node["size"].data);
  ram.map(allocate<uint8>(size, 0xff), size);
  if(name.empty() == false) {
    interface->loadRequest(id, name);
    if(writable) memory.append({id, name});
  }
}

//

void Cartridge::parse_markup_cartridge(Markup::Node root) {
  parse_markup_memory(board->prgrom, root["prg/rom"], ID::ProgramROM, false);
  parse_markup_memory(board->prgram, root["prg/ram"], ID::ProgramRAM, true);
  parse_markup_memory(board->chrrom, root["chr/rom"], ID::CharacterROM, false);
  parse_markup_memory(board->chrram, root["chr/ram"], ID::CharacterRAM, true);
  if(system.pc10()) {
    parse_markup_memory(board->instrom, root["pc10/rom[0]"], ID::InstructionROM, false);
    parse_markup_memory(board->keyrom, root["pc10/rom[1]"], ID::KeyROM, false);
  }
}

#endif
