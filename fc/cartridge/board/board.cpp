#include "bandai-74-161-02-74.cpp"
#include "bandai-74-161-161-32.cpp"
#include "bandai-fcg.cpp"
#include "hvc-axrom.cpp"
#include "hvc-cxrom.cpp"
#include "hvc-exrom.cpp"
#include "hvc-fxrom.cpp"
#include "hvc-gxrom.cpp"
#include "hvc-nrom.cpp"
#include "hvc-pxrom.cpp"
#include "hvc-sxrom.cpp"
#include "hvc-txrom.cpp"
#include "hvc-uxrom.cpp"
#include "irem-74-161-161-21-138.cpp"
#include "irem-g101.cpp"
#include "irem-h3001.cpp"
#include "irem-holydiver.cpp"
#include "irem-i-im.cpp"
#include "irem-tam-s1.cpp"
#include "jaleco-jf-0x.cpp"
#include "jaleco-jf-16.cpp"
#include "jaleco-jf-2x.cpp"
#include "konami-vrc1.cpp"
#include "konami-vrc2.cpp"
#include "konami-vrc3.cpp"
#include "konami-vrc4.cpp"
#include "konami-vrc6.cpp"
#include "konami-vrc7.cpp"
#include "namco-163.cpp"
#include "namco-34xx.cpp"
#include "nes-event.cpp"
#include "nes-hkrom.cpp"
#include "nes-qj.cpp"
#include "pal-zz.cpp"
#include "sunsoft-4.cpp"
#include "sunsoft-5b.cpp"
#include "taito-tc.cpp"
#include "fds.cpp"
#include "vs.cpp"

// Unlicensed board definitions; feel free to remove
#include "unlicensed/camerica.cpp"
#include "unlicensed/colordreams-74-377.cpp"
#include "unlicensed/mlt-action52.cpp"
#include "unlicensed/nina.cpp"
// Homebrew board definitions; feel free to remove
#include "unlicensed/noconflicts-cnrom.cpp"
#include "unlicensed/single-chip.cpp"

Board::Board(Markup::Node& board_node) {
  cartridge.board = this;
}

Board::~Board() {
}

auto Board::read(MappedRAM& memory, uint addr) -> uint8 {
  return memory.read(mirror(addr, memory.size()));
}

auto Board::write(MappedRAM& memory, uint addr, uint8 byte) -> void {
  memory.write(mirror(addr, memory.size()), byte);
}

auto Board::mirror(uint addr, uint size) -> uint {
  uint base = 0;
  if(size) {
    uint mask = 1 << 23;
    while(addr >= size) {
      while(!(addr & mask)) mask >>= 1;
      addr -= mask;
      if(size > mask) {
        size -= mask;
        base += mask;
      }
      mask >>= 1;
    }
    base += addr;
  }
  return base;
}

auto Board::enter() -> void {
  while(true) {
    if(scheduler.sync == Scheduler::SynchronizeMode::All) {
      scheduler.exit(Scheduler::ExitReason::SynchronizeEvent);
    }

    tick();
  }
}

auto Board::tick() -> void {
  cartridge.clock += (system.region() == System::Region::NTSC ? 12 : 16);
  if(cartridge.clock >= 0 && scheduler.sync != Scheduler::SynchronizeMode::All) co_switch(cpu.thread);
}

auto Board::chr_read(uint addr) -> uint8 {
  if(chrram.size()) return read(chrram, addr);
  if(chrrom.size()) return read(chrrom, addr);
  return ppu.status.mdr;
}

auto Board::chr_write(uint addr, uint8 data) -> void {
  if(chrram.size()) write(chrram, addr, data);
}

auto Board::power() -> void {
}

auto Board::reset() -> void {
}

auto Board::serialize(serializer& s) -> void {
  if(prgram.size()) s.array(prgram.data(), prgram.size());
  if(chrram.size()) s.array(chrram.data(), chrram.size());
}

Board* Board::load(Markup::Node board_node) {
  if(system.revision == System::Revision::VSSystem) return new VS(board_node);

  string type = board_node["id"].text();

  if(slice(type,0,4) == "HVC-"
  || slice(type,0,4) == "NES-"
  || slice(type,0,4) == "PAL-") {
    type = slice(type,4);
    if(type == "AMROM"   ) return new HVC_AxROM(board_node);
    if(type == "ANROM"   ) return new HVC_AxROM(board_node);
    if(type == "AN1ROM"  ) return new HVC_AxROM(board_node);
    if(type == "AOROM"   ) return new HVC_AxROM(board_node);

    if(type == "BNROM"   ) return new IremIIM(board_node);

    if(type == "BTR"     ) return new Sunsoft5B(board_node);

    if(type == "CNROM"   ) return new HVC_CxROM(board_node);
    if(type == "CPROM"   ) return new HVC_CxROM(board_node);

    if(type == "DEROM"   ) return new Namco34xx(board_node);
    if(type == "DE1ROM"  ) return new Namco34xx(board_node);
    if(type == "DRROM"   ) return new Namco34xx(board_node);

    if(type == "EVENT"   ) return new NES_Event(board_node);

    if(type == "EKROM"   ) return new HVC_ExROM(board_node);
    if(type == "ELROM"   ) return new HVC_ExROM(board_node);
    if(type == "ETROM"   ) return new HVC_ExROM(board_node);
    if(type == "EWROM"   ) return new HVC_ExROM(board_node);

    if(type == "FJROM"   ) return new HVC_FxROM(board_node);
    if(type == "FKROM"   ) return new HVC_FxROM(board_node);

    if(type == "GNROM"   ) return new HVC_GxROM(board_node);
    if(type == "MHROM"   ) return new HVC_GxROM(board_node);

    if(type == "HKROM"   ) return new NES_HKROM(board_node);

    if(type == "JLROM"   ) return new Sunsoft5B(board_node);
    if(type == "JSROM"   ) return new Sunsoft5B(board_node);

    if(type == "FAMILYBASIC") return new HVC_NROM(board_node);
    if(type == "HROM"       ) return new HVC_NROM(board_node);
    if(type == "NROM"       ) return new HVC_NROM(board_node);
    if(type == "NROM-128"   ) return new HVC_NROM(board_node);
    if(type == "NROM-256"   ) return new HVC_NROM(board_node);
    if(type == "RROM"       ) return new HVC_NROM(board_node);
    if(type == "RROM-128"   ) return new HVC_NROM(board_node);
    if(type == "RTROM"      ) return new HVC_NROM(board_node);
    if(type == "SROM"       ) return new HVC_NROM(board_node);
    if(type == "STROM"      ) return new HVC_NROM(board_node);

    if(type == "PEEOROM" ) return new HVC_PxROM(board_node);
    if(type == "PNROM"   ) return new HVC_PxROM(board_node);

    if(type == "QJ"      ) return new NES_QJ(board_node);

    if(type == "SAROM"   ) return new HVC_SxROM(board_node);
    if(type == "SBROM"   ) return new HVC_SxROM(board_node);
    if(type == "SCROM"   ) return new HVC_SxROM(board_node);
    if(type == "SC1ROM"  ) return new HVC_SxROM(board_node);
    if(type == "SEROM"   ) return new HVC_SxROM(board_node);
    if(type == "SFROM"   ) return new HVC_SxROM(board_node);
    if(type == "SF1ROM"  ) return new HVC_SxROM(board_node);
    if(type == "SFEXPROM") return new HVC_SxROM(board_node);
    if(type == "SGROM"   ) return new HVC_SxROM(board_node);
    if(type == "SHROM"   ) return new HVC_SxROM(board_node);
    if(type == "SH1ROM"  ) return new HVC_SxROM(board_node);
    if(type == "SIROM"   ) return new HVC_SxROM(board_node);
    if(type == "SJROM"   ) return new HVC_SxROM(board_node);
    if(type == "SKROM"   ) return new HVC_SxROM(board_node);
    if(type == "SLROM"   ) return new HVC_SxROM(board_node);
    if(type == "SL1ROM"  ) return new HVC_SxROM(board_node);
    if(type == "SL2ROM"  ) return new HVC_SxROM(board_node);
    if(type == "SL3ROM"  ) return new HVC_SxROM(board_node);
    if(type == "SLRROM"  ) return new HVC_SxROM(board_node);
    if(type == "SMROM"   ) return new HVC_SxROM(board_node);
    if(type == "SNROM"   ) return new HVC_SxROM(board_node);
    if(type == "SNWEPROM") return new HVC_SxROM(board_node);
    if(type == "SOROM"   ) return new HVC_SxROM(board_node);
    if(type == "SUROM"   ) return new HVC_SxROM(board_node);
    if(type == "SXROM"   ) return new HVC_SxROM(board_node);

    if(type == "TBROM"   ) return new HVC_TxROM(board_node);
    if(type == "TEROM"   ) return new HVC_TxROM(board_node);
    if(type == "TFROM"   ) return new HVC_TxROM(board_node);
    if(type == "TGROM"   ) return new HVC_TxROM(board_node);
    if(type == "TKROM"   ) return new HVC_TxROM(board_node);
    if(type == "TKEPROM" ) return new HVC_TxROM(board_node);
    if(type == "TKSROM"  ) return new HVC_TxROM(board_node);
    if(type == "TLROM"   ) return new HVC_TxROM(board_node);
    if(type == "TL1ROM"  ) return new HVC_TxROM(board_node);
    if(type == "TL2ROM"  ) return new HVC_TxROM(board_node);
    if(type == "TLSROM"  ) return new HVC_TxROM(board_node);
    if(type == "TNROM"   ) return new HVC_TxROM(board_node);
    if(type == "TQROM"   ) return new HVC_TxROM(board_node);
    if(type == "TR1ROM"  ) return new HVC_TxROM(board_node);
    if(type == "TSROM"   ) return new HVC_TxROM(board_node);
    if(type == "TVROM"   ) return new HVC_TxROM(board_node);

    if(type == "UNROM"   ) return new HVC_UxROM(board_node);
    if(type == "UN1ROM"  ) return new HVC_UxROM(board_node);
    if(type == "UOROM"   ) return new HVC_UxROM(board_node);

    if(type == "ZZ"      ) return new PAL_ZZ(board_node);
  }

  if(type == "ACCLAIM-MC-ACC") return new HVC_TxROM(board_node);

  if(type == "BANDAI-74*161/02/74" ) return new Bandai74_161_02_74(board_node);
  if(type == "BANDAI-74*161/32"    ) return new HVC_CxROM(board_node);
  if(type == "BANDAI-74*161/161/32") return new Bandai74_161_161_32(board_node);
  if(type == "BANDAI-CNROM"        ) return new HVC_CxROM(board_node);
  if(type == "BANDAI-FCG"          ) return new BandaiFCG(board_node);
  if(type == "BANDAI-FCG-1"        ) return new BandaiFCG(board_node);
  if(type == "BANDAI-FCG-2"        ) return new BandaiFCG(board_node);
  if(type == "BANDAI-GNROM"        ) return new HVC_GxROM(board_node);
  if(type == "BANDAI-JUMP2"        ) return new BandaiFCG(board_node);
  if(type == "BANDAI-LZ93D50"      ) return new BandaiFCG(board_node);
  if(type == "BANDAI-NROM-128"     ) return new HVC_NROM(board_node);
  if(type == "BANDAI-NROM-256"     ) return new HVC_NROM(board_node);
  if(type == "BANDAI-PT-554"       ) return new HVC_CxROM(board_node);

  if(type == "IREM-74*161/161/21/138") return new Irem74_161_161_21_138(board_node);
  if(type == "IREM-BNROM"            ) return new IremIIM(board_node);
  if(type == "IREM-FCG-1"            ) return new BandaiFCG(board_node);
  if(type == "IREM-G101"             ) return new IremG101(board_node);
  if(type == "IREM-H3001"            ) return new IremH3001(board_node);
  if(type == "IREM-HOLYDIVER"        ) return new IremHolyDiver(board_node);
  if(type == "IREM-NROM-128"         ) return new HVC_NROM(board_node);
  if(type == "IREM-NROM-256"         ) return new HVC_NROM(board_node);
  if(type == "IREM-TAM-S1"           ) return new IremTamS1(board_node);
  if(type == "IREM-UNROM"            ) return new HVC_UxROM(board_node);

  if(type == "JALECO-JF-05") return new JalecoJF0x(board_node);
  if(type == "JALECO-JF-06") return new JalecoJF0x(board_node);
  if(type == "JALECO-JF-07") return new JalecoJF0x(board_node);
  if(type == "JALECO-JF-08") return new JalecoJF0x(board_node);
  if(type == "JALECO-JF-09") return new JalecoJF0x(board_node);
  if(type == "JALECO-JF-10") return new JalecoJF0x(board_node);
  if(type == "JALECO-JF-11") return new JalecoJF0x(board_node);
  if(type == "JALECO-JF-14") return new JalecoJF0x(board_node);
  if(type == "JALECO-JF-16") return new JalecoJF16(board_node);
  if(type == "JALECO-JF-22") return new KonamiVRC1(board_node);
  if(type == "JALECO-JF-23") return new JalecoJF2x(board_node);
  if(type == "JALECO-JF-24") return new JalecoJF2x(board_node);
  if(type == "JALECO-JF-25") return new JalecoJF2x(board_node);
  if(type == "JALECO-JF-27") return new JalecoJF2x(board_node);
  if(type == "JALECO-JF-29") return new JalecoJF2x(board_node);
  if(type == "JALECO-JF-37") return new JalecoJF2x(board_node);
  if(type == "JALECO-JF-40") return new JalecoJF2x(board_node);

  if(type == "KONAMI-74*139/74") return new JalecoJF0x(board_node);
  if(type == "KONAMI-CNROM"    ) return new HVC_CxROM(board_node);
  if(type == "KONAMI-NROM-128" ) return new HVC_NROM(board_node);
  if(type == "KONAMI-SLROM"    ) return new HVC_SxROM(board_node);
  if(type == "KONAMI-TLROM"    ) return new HVC_TxROM(board_node);
  if(type == "KONAMI-UNROM"    ) return new HVC_UxROM(board_node);
  if(type == "KONAMI-VRC-1"    ) return new KonamiVRC1(board_node);
  if(type == "KONAMI-VRC-2"    ) return new KonamiVRC2(board_node);
  if(type == "KONAMI-VRC-3"    ) return new KonamiVRC3(board_node);
  if(type == "KONAMI-VRC-4"    ) return new KonamiVRC4(board_node);
  if(type == "KONAMI-VRC-6"    ) return new KonamiVRC6(board_node);
  if(type == "KONAMI-VRC-7"    ) return new KonamiVRC7(board_node);

  if(type == "NAMCOT-129" ) return new Namco163(board_node);
  if(type == "NAMCOT-163" ) return new Namco163(board_node);
  if(type == "NAMCOT-175" ) return new Namco163(board_node);
  if(type == "NAMCOT-340" ) return new Namco163(board_node);
  if(type == "NAMCOT-3301") return new HVC_NROM(board_node);
  if(type == "NAMCOT-3302") return new HVC_NROM(board_node);
  if(type == "NAMCOT-3305") return new HVC_NROM(board_node);
  if(type == "NAMCOT-3311") return new HVC_NROM(board_node);
  if(type == "NAMCOT-3312") return new HVC_NROM(board_node);
  if(type == "NAMCOT-3401") return new Namco34xx(board_node);
  if(type == "NAMCOT-3406") return new Namco34xx(board_node);
  if(type == "NAMCOT-3407") return new Namco34xx(board_node);
  if(type == "NAMCOT-3413") return new Namco34xx(board_node);
  if(type == "NAMCOT-3414") return new Namco34xx(board_node);
  if(type == "NAMCOT-3415") return new Namco34xx(board_node);
  if(type == "NAMCOT-3416") return new Namco34xx(board_node);
  if(type == "NAMCOT-3417") return new Namco34xx(board_node);
  if(type == "NAMCOT-3425") return new Namco34xx(board_node);
  if(type == "NAMCOT-3443") return new Namco34xx(board_node);
  if(type == "NAMCOT-3446") return new Namco34xx(board_node);
  if(type == "NAMCOT-3451") return new Namco34xx(board_node);
  if(type == "NAMCOT-3453") return new Namco34xx(board_node);

  if(type == "SUNSOFT-4" ) return new Sunsoft4(board_node);
  if(type == "SUNSOFT-5B") return new Sunsoft5B(board_node);

  if(type == "TAITO-TC0190FMC") return new TaitoTC(board_node);
  if(type == "TAITO-TC0350FMR") return new TaitoTC(board_node);
  if(type == "TAITO-TC0690FMR") return new TaitoTC(board_node);

  if(type == "FDS") return new FDS(board_node);
  if(type == "VS" ) return new VS(board_node);

  // Unlicensed boards below; feel free to remove
  if(type == "AVE-NINA-01"       ) return new Nina(board_node);
  if(type == "AVE-NINA-02"       ) return new Nina(board_node);
  if(type == "AVE-NINA-03"       ) return new Nina(board_node);
  if(type == "AVE-NINA-06"       ) return new Nina(board_node);
  if(type == "CAMERICA-ALGN"     ) return new Camerica(board_node);
  if(type == "CAMERICA-ALGQ"     ) return new Camerica(board_node);
  if(type == "CAMERICA-BF9093"   ) return new Camerica(board_node);
  if(type == "CAMERICA-BF9096"   ) return new Camerica(board_node);
  if(type == "CAMERICA-BF9097"   ) return new Camerica(board_node);
  if(type == "CODEMASTERS-NR8N"  ) return new Camerica(board_node);
  if(type == "COLORDREAMS-74*377") return new ColorDreams74_377(board_node);
  if(type == "MLT-ACTION52"      ) return new MLT_Action52(board_node);
  if(type == "TENGEN-800002"     ) return new Namco34xx(board_node);
  if(type == "TENGEN-800003"     ) return new HVC_NROM(board_node);
  if(type == "TENGEN-800004"     ) return new Namco34xx(board_node);
  if(type == "TENGEN-800008"     ) return new HVC_CxROM(board_node);
  if(type == "TENGEN-800030"     ) return new Namco34xx(board_node);
  if(type == "TENGEN-800042"     ) return new Sunsoft4(board_node);
  // Homebrew boards; feel free to remove
  if(type == "NoConflicts-CNROM") return new NoConflicts_CNROM(board_node);
  if(type == "SingleChip") return new SingleChip(board_node);

  return nullptr;
}
