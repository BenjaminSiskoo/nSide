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
  static const string Version = "009.09";
  static const string FromVersion = "099.11";
  static const string Author = "hex_usr";
  static const string OriginalAuthor = "byuu";
  static const lstring Contributors = {
    "Andreas Naive (S-DD1 decompression algorithm)",
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
    "FitzRoy (Famicom bug-testing)",
    "gekkio (Game Boy fixes)",
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
  static const string SerializerVersion = "009.09(099.07)";
}

#include "interface.hpp"

//debugging function hook:
//no overhead (and no debugger invocation) if not compiled with -DDEBUGGER
//wraps testing of function to allow invocation without a defined callback
template<typename T> struct hook;
template<typename R, typename... P> struct hook<auto (P...) -> R> {
  function<auto (P...) -> R> callback;

  auto operator()(P... p) const -> R {
    #if defined(DEBUGGER)
    if(callback) return callback(forward<P>(p)...);
    #endif
    return R();
  }

  hook() {}
  hook(const hook& hook) { callback = hook.callback; }
  hook(void* function) { callback = function; }
  hook(auto (*function)(P...) -> R) { callback = function; }
  template<typename C> hook(auto (C::*function)(P...) -> R, C* object) { callback = {function, object}; }
  template<typename C> hook(auto (C::*function)(P...) const -> R, C* object) { callback = {function, object}; }
  template<typename L> hook(const L& function) { callback = function; }

  auto operator=(const hook& source) -> hook& { callback = source.callback; return *this; }
};

#if defined(DEBUGGER)
  #define privileged public
#else
  #define privileged private
#endif
