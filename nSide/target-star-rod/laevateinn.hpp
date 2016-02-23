#if !defined(PROFILE_ACCURACY)
  #error "debugger is only compatible with accuracy profile"
#endif

#include <nall/config.hpp>
#include <nall/directory.hpp>
#include <nall/dsp.hpp>
#include <nall/file.hpp>
#include <nall/filemap.hpp>
#include <nall/stream/file.hpp>
#include <nall/stream/memory.hpp>
#include <nall/stream/mmap.hpp>
#include <nall/stream/vector.hpp>
#include <ruby/ruby.hpp>
#include <hiro/hiro.hpp>
using namespace nall;
using namespace ruby;
using namespace hiro;
extern unique_pointer<Video> video;
extern unique_pointer<Audio> audio;
extern unique_pointer<Input> input;

#include <emulator/emulator.hpp>
#include <sfc/sfc.hpp>
extern Emulator::Interface* emulator;

#include "program/program.hpp"
#include "settings/settings.hpp"
#include "debugger/debugger.hpp"
#include "tracer/tracer.hpp"
#include "window/window.hpp"
#include "console/console.hpp"
#include "presentation/presentation.hpp"
#include "cpu/cpu.hpp"
#include "smp/smp.hpp"
#include "memory/memory.hpp"
#include "breakpoint/breakpoint.hpp"
#include "properties/properties.hpp"
#include "vram/vram.hpp"
#include "resource/resource.hpp"

auto locate(string name) -> string;
auto locateHigan(string name) -> string;
auto locateSystem(string name) -> string;
