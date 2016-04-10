#include <nall/nall.hpp>
#include <ruby/ruby.hpp>
#include <hiro/hiro.hpp>
using namespace nall;
using namespace ruby;
using namespace hiro;
extern unique_pointer<Video> video;
extern unique_pointer<Audio> audio;
extern unique_pointer<Input> input;

#include <emulator/emulator.hpp>
extern Emulator::Interface* emulator;

#include <sfc/sfc.hpp>
namespace SFC = SuperFamicom;

auto locate(string name) -> string;

template<typename... Args> auto echo(Args&&... args) -> void {
  //terminal->print({std::forward<Args>(args)...});
}

#include "program/program.hpp"
#include "settings/settings.hpp"
#include "input/input.hpp"
#include "debugger/debugger.hpp"
#include "terminal/terminal.hpp"
#include "presentation/presentation.hpp"
#include "resource/resource.hpp"
