#include "tomoko.hpp"
Video* video = nullptr;
Audio* audio = nullptr;
Input* input = nullptr;
Emulator::Interface* emulator = nullptr;

//if file already exists in the same path as the binary; use it (portable mode)
//if not, use default requested path (*nix/user mode)
auto locate(string pathname, string filename) -> string {
  string location = {programpath(), filename};
  if(storage::exists(location)) return location;
  return {pathname, filename};
}

#include <nall/main.hpp>
auto nall::main(lstring args) -> void {
  Application::setName("nSide-t");
  new Program;
  Application::run();
}
