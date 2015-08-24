#include <nall/nall.hpp>
#include <ruby/ruby.hpp>
#include <phoenix/phoenix.hpp>
using namespace nall;
using namespace ruby;
using namespace phoenix;
extern Video* video;
extern Audio* audio;
extern Input* input;

#include <emulator/emulator.hpp>
Emulator::Interface& system();

#include "configuration/configuration.hpp"
#include "interface/interface.hpp"
#include "utility/utility.hpp"
#include "input/input.hpp"
#include "window/window.hpp"
#include "general/general.hpp"
#include "settings/settings.hpp"
#include "tools/tools.hpp"
#include "resource/resource.hpp"

auto locate(string pathname, string filename) -> string;

struct Program {
  vector<Emulator::Interface*> emulator;
  Emulator::Interface* active = nullptr;
  library auto_purify;

  bool pause;
  bool autopause;
  unsigned depth;  //color depth; 24(bpp) or 30(bpp)

  string basepath;
  string userpath;
  string sharedpath;

  string normalFont;
  string boldFont;
  string titleFont;
  string monospaceFont;

  bool focused();
  string path(string filename);
  void main();
  void bootstrap();
  Program(int argc, char** argv);
};

extern Program* program;
extern DSP dspaudio;
