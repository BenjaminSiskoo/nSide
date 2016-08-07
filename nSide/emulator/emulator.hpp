#pragma once

#include <nall/nall.hpp>
#include <nall/vfs.hpp>
using namespace nall;

#include <libco/libco.h>
#include <audio/audio.hpp>
#include <video/video.hpp>
#include <resource/resource.hpp>

namespace Emulator {
  static const string Name = "nSide";
  static const string OriginalName = "higan";
  static const string Version = "009.10";
  static const string FromVersion = "100.05";
  static const string Author = "hex_usr";
  static const string OriginalAuthor = "byuu";
  static const string_vector Contributors = {
    "Alyosha_TAS (Atari 2600 PIA timer and TIA sprite implementation details)",
    "Andreas Naive (S-DD1 decompression algorithm)",
    //"Ange Albertini (higan logo)",  //not used in nSide
    "anomie",
    "AWJ (Super Famicom PPUcounter NTSC/PAL timing)",
    "Bisqwit (Famicom PPU color generation for NTSC systems)",
    "blargg (Famicom testing, Super Famicom testing and Balanced DSP)",
    "Cydrak (ST-018 discoveries and bug fixes, Game Boy Advance emulation)",
    "_Demo_",
    "Derrick Sobodash",
    "DMV27",
    "Dr. Decapitator (DSP-1,2,3,4, ST-010,011 ROM dumps)",
    "endrift (Game Boy, Game Boy Advance fixes)",
    "FirebrandX",
    "FitzRoy (Famicom, Super Famicom bug-testing)",
    "gekkio (Game Boy STAT IRQ and other fixes)",
    "GIGO",
    "jchadwick (Game Boy Advance fixes)",
    "Jonas Quinn (Super Famicom DSP, Game Boy, Game Boy Advance fixes)",
    "kevtris (Famicom PPU voltage levels)",
    "kode54",
    "krom",
    "LostTemplar (ST-018 program ROM analysis)",
    "Matthew Callis",
    "Nach",
    "neviksti (SPC7110 decompression algorithm)",
    "Overload (Cx4 data ROM dump)",
    "RedDwarf",
    "Richard Bannister",
    "Ryphecha (Famicom emulation)",
    "segher (Cx4 reverse engineering)",
    "tetsuo55",
    "TRAC",
    "trap15 (WonderSwan information and fixes)",
    "wareya (WASAPI driver)",
    "zones",
  };
  static const string License = "GPLv3";
  static const string Website = "http://byuu.org/";

  //incremented only when serialization format changes
  static const string SerializerVersion = "009.10(100.15)";

  namespace Constants {
    namespace Colorburst {
      static constexpr double NTSC = 315.0 / 88.0 * 1'000'000.0;
      static constexpr double PAL  = 283.75 * 15'625.0 + 25.0;
    }
  }
}

#include "interface.hpp"
#include "debugger.hpp"
