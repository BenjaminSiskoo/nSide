#include "tomoko.hpp"
Emulator::Interface* emulator = nullptr;

#include <nall/main.hpp>
auto nall::main(lstring args) -> void {
  Application::setName("nSide-t");
  new Program;
  Application::run();
}
