#pragma once

#include <nall/nall.hpp>
#include <nall/vfs.hpp>
using namespace nall;

#include "types.hpp"
#include <libco/libco.h>
#include <audio/audio.hpp>
#include <video/video.hpp>
#include <resource/resource.hpp>

namespace Emulator {
  static const string Name           = "nSide";
  static const string OriginalName   = "higan";
  static const string Version        = "009.13";
  static const string FromVersion    = "103.10";
  static const string Author         = "hex_usr";
  static const string OriginalAuthor = "byuu";
  static const string License        = "GPLv3";
  static const string Website        = "http://byuu.org/";
  static const string_vector Contributors = {
    "Alyosha_TAS (Atari 2600 PIA, TIA, timing details)",
    "Andreas Naive (S-DD1 decompression algorithm)",
    //"Ange Albertini (higan logo)",  //not used in nSide
    "AWJ (Super Famicom PPUcounter NTSC/PAL timing, MMC5 CHR banking)",
    "Bisqwit (Famicom PPU color generation for NTSC systems)",
    "blargg (Famicom testing, Super Famicom MUL/DIV and Balanced DSP)",
    "Cydrak (ST-018 discoveries and bug fixes, Game Boy Advance emulation, Mega Drive sound)",
    "_Demo_",
    "Derrick Sobodash",
    "DMV27",
    "Dr. Decapitator (DSP-1,2,3,4, ST-010,011 ROM dumps)",
    "endrift (Game Boy, Game Boy Advance fixes)",
    "FirebrandX",
    "FitzRoy (Famicom, Super Famicom bug-testing)",
    "gekkio (Game Boy STAT IRQ and other fixes)",
    "GIGO",
    "ilari (Super Famicom CPU MDR tests)",
    "jchadwick (Game Boy Advance fixes)",
    "Jonas Quinn (Super Famicom DSP, Game Boy, Game Boy Advance fixes)",
    "kevtris (Famicom PPU voltage levels)",
    "kode54",
    "krom",
    "LostTemplar (ST-018 program ROM analysis)",
    "Matthew Callis",
    "MerryMage (nall: elliptic curve cryptography)"
    "Nach (libco: setjmp jmpbuf support)",
    "OV2 (ruby: XAudio2 support)"
    "Overload (Cx4 data ROM dump)",
    "p4plus2 (Super Famicom CPU MDR tests)",
    "RedDwarf",
    "Richard Bannister",
    "Ryphecha (Famicom APU emulation)",
    "segher (Cx4 reverse engineering)",
    "tetsuo55",
    "Total (Super Famicom CPU MDR tests)",
    "TRAC",
    "trap15 (WonderSwan information and fixes)",
    "wareya (WASAPI driver)",
    "zones",
    "anomie (No longer used: sample-based DSP implementation)",
    "blargg (No longer used: snes_ntsc library)",
    "loopy (No longer used: MMC5 CHR banking)",
    "neviksti (No longer used: SPC7110 decompression support)",
    "Ryphecha (No longer used: polyphase audio resampler)",
  };

  //incremented only when serialization format changes
  static const string SerializerVersion = "009.13(103.10)";

  namespace Constants {
    namespace Colorburst {
      static constexpr double NTSC = 315.0 / 88.0 * 1'000'000.0;
      static constexpr double PAL  = 283.75 * 15'625.0 + 25.0;
    }
  }

  //nall/vfs shorthand constants for open(), load()
  namespace File {
    static const auto Read = vfs::file::mode::read;
    static const auto Write = vfs::file::mode::write;
    static const auto Optional = false;
    static const auto Required = true;
  };
}

#include "platform.hpp"
#include "interface.hpp"
#include "debugger.hpp"
