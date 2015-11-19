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
#include <phoenix/phoenix.hpp>
using namespace nall;
using namespace ruby;
using namespace phoenix;
extern Video* video;
extern Audio* audio;
extern Input* input;

#include <sfc/sfc.hpp>

#include "settings/settings.hpp"
#include "interface/interface.hpp"
#include "debugger/debugger.hpp"
#include "tracer/tracer.hpp"
#include "window/window.hpp"
#include "console/console.hpp"
#include "video/video.hpp"
#include "cpu/cpu.hpp"
#include "smp/smp.hpp"
#include "memory/memory.hpp"
#include "breakpoint/breakpoint.hpp"
#include "properties/properties.hpp"
#include "vram/vram.hpp"
extern uint8_t laevateinnLogo[121905];

struct Program {
  Emulator::Interface* active = nullptr;

  string basepath;
  string userpath;
  string sharedpath;

  string path(string name);
  void main();
  Program(string pathname);
};

extern Program* program;
extern DSP dspaudio;
