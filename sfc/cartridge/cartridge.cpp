#include <sfc/sfc.hpp>

#define CARTRIDGE_CPP
namespace SuperFamicom {

#include "markup.cpp"
#include "serialization.cpp"
Cartridge cartridge;

string Cartridge::title() {
  if(information.title.gameBoy.empty() == false) {
    return {information.title.cartridge, " + ", information.title.gameBoy};
  }

  if(information.title.satellaview.empty() == false) {
    return {information.title.cartridge, " + ", information.title.satellaview};
  }

  if(information.title.sufamiTurboA.empty() == false) {
    if(information.title.sufamiTurboB.empty() == true) {
      return {information.title.cartridge, " + ", information.title.sufamiTurboA};
    } else {
      return {information.title.cartridge, " + ", information.title.sufamiTurboA, " + ", information.title.sufamiTurboB};
    }
  }

  return information.title.cartridge;
}

void Cartridge::load() {
  region = Region::NTSC;

  has_gb_slot    = false;
  has_bs_cart    = false;
  has_bs_slot    = false;
  has_st_slots   = false;
  has_nss_dip    = false;
  has_event      = false;
  has_sa1        = false;
  has_superfx    = false;
  has_armdsp     = false;
  has_hitachidsp = false;
  has_necdsp     = false;
  has_epsonrtc   = false;
  has_sharprtc   = false;
  has_spc7110    = false;
  has_sdd1       = false;
  has_obc1       = false;
  has_hsu1       = false;
  has_msu1       = false;

  information.markup.cartridge    = "";
  information.markup.gameBoy      = "";
  information.markup.satellaview  = "";
  information.markup.sufamiTurboA = "";
  information.markup.sufamiTurboB = "";

  information.title.cartridge     = "";
  information.title.gameBoy       = "";
  information.title.satellaview   = "";
  information.title.sufamiTurboA  = "";
  information.title.sufamiTurboB  = "";

  interface->loadRequest(ID::Manifest, "manifest.bml");
  parse_markup(information.markup.cartridge);

  //Super Game Boy
  if(cartridge.has_gb_slot()) {
    sha256 = Hash::SHA256(GameBoy::cartridge.romdata, GameBoy::cartridge.romsize).digest();
  }

  //Broadcast Satellaview
  else if(cartridge.has_bs_cart() && cartridge.has_bs_slot()) {
    sha256 = Hash::SHA256(satellaviewcartridge.memory.data(), satellaviewcartridge.memory.size()).digest();
  }

  //Sufami Turbo
  else if(cartridge.has_st_slots()) {
    Hash::SHA256 sha;
    sha.data(sufamiturboA.rom.data(), sufamiturboA.rom.size());
    sha.data(sufamiturboB.rom.data(), sufamiturboB.rom.size());
    sha256 = sha.digest();
  }

  //Super Famicom
  else {
    Hash::SHA256 sha;
    //hash each ROM image that exists; any with size() == 0 is ignored by sha256_chunk()
    sha.data(rom.data(), rom.size());
    sha.data(bsxcartridge.rom.data(), bsxcartridge.rom.size());
    sha.data(sa1.rom.data(), sa1.rom.size());
    sha.data(superfx.rom.data(), superfx.rom.size());
    sha.data(hitachidsp.rom.data(), hitachidsp.rom.size());
    sha.data(spc7110.prom.data(), spc7110.prom.size());
    sha.data(spc7110.drom.data(), spc7110.drom.size());
    sha.data(sdd1.rom.data(), sdd1.rom.size());
    //hash all firmware that exists
    vector<uint8> buffer;
    buffer = armdsp.firmware();
    sha.data(buffer.data(), buffer.size());
    buffer = hitachidsp.firmware();
    sha.data(buffer.data(), buffer.size());
    buffer = necdsp.firmware();
    sha.data(buffer.data(), buffer.size());
    //finalize hash
    sha256 = sha.digest();
  }

  rom.write_protect(true);
  ram.write_protect(false);

  system.load();
  loaded = true;
}

void Cartridge::load_super_game_boy() {
  interface->loadRequest(ID::SuperGameBoyManifest, "manifest.bml");
  auto document = BML::unserialize(information.markup.gameBoy);
  information.title.gameBoy = document["information/title"].text();

  auto rom = document["cartridge/rom"];
  auto ram = document["cartridge/ram"];

  GameBoy::cartridge.information.markup = information.markup.gameBoy;
  GameBoy::cartridge.load(GameBoy::System::Revision::SuperGameBoy);

  if(auto name = rom["name"].text()) interface->loadRequest(ID::SuperGameBoyROM, name);
  if(auto name = ram["name"].text()) interface->loadRequest(ID::SuperGameBoyRAM, name);
  if(auto name = ram["name"].text()) memory.append({ID::SuperGameBoyRAM, name});
}

void Cartridge::load_satellaview() {
  interface->loadRequest(ID::SatellaviewManifest, "manifest.bml");
  auto document = BML::unserialize(information.markup.satellaview);
  information.title.satellaview = document["information/title"].text();

  auto rom = document["cartridge/rom"];

  if(rom["name"]) {
    unsigned size = rom["size"].decimal();
    satellaviewcartridge.memory.map(allocate<uint8>(size, 0xff), size);
    interface->loadRequest(ID::SatellaviewROM, rom["name"].text());

    satellaviewcartridge.readonly = (rom["type"].text() == "MaskROM");
  }
}

void Cartridge::load_sufami_turbo_a() {
  interface->loadRequest(ID::SufamiTurboSlotAManifest, "manifest.bml");
  auto document = BML::unserialize(information.markup.sufamiTurboA);
  information.title.sufamiTurboA = document["information/title"].text();

  auto rom = document["cartridge/rom"];
  auto ram = document["cartridge/ram"];

  if(rom["name"]) {
    unsigned size = rom["size"].decimal();
    sufamiturboA.rom.map(allocate<uint8>(size, 0xff), size);
    interface->loadRequest(ID::SufamiTurboSlotAROM, rom["name"].text());
  }

  if(ram["name"]) {
    unsigned size = ram["size"].decimal();
    sufamiturboA.ram.map(allocate<uint8>(size, 0xff), size);
    interface->loadRequest(ID::SufamiTurboSlotARAM, ram["name"].text());
    memory.append({ID::SufamiTurboSlotARAM, ram["name"].text()});
  }

  if(document["cartridge/linkable"]) {
    interface->loadRequest(ID::SufamiTurboSlotB, "Sufami Turbo - Slot B", "st");
  }
}

void Cartridge::load_sufami_turbo_b() {
  interface->loadRequest(ID::SufamiTurboSlotBManifest, "manifest.bml");
  auto document = BML::unserialize(information.markup.sufamiTurboB);
  information.title.sufamiTurboB = document["information/title"].text();

  auto rom = document["cartridge/rom"];
  auto ram = document["cartridge/ram"];

  if(rom["name"]) {
    unsigned size = rom["size"].decimal();
    sufamiturboB.rom.map(allocate<uint8>(size, 0xff), size);
    interface->loadRequest(ID::SufamiTurboSlotBROM, rom["name"].text());
  }

  if(ram["name"]) {
    unsigned size = ram["size"].decimal();
    sufamiturboB.ram.map(allocate<uint8>(size, 0xff), size);
    interface->loadRequest(ID::SufamiTurboSlotBRAM, ram["name"].text());
    memory.append({ID::SufamiTurboSlotBRAM, ram["name"].text()});
  }
}

void Cartridge::unload() {
  if(loaded == false) return;

  system.unload();
  rom.reset();
  ram.reset();

  loaded = false;
  memory.reset();
}

Cartridge::Cartridge() {
  loaded = false;
}

Cartridge::~Cartridge() {
  unload();
}

}
