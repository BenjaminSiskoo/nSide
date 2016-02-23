#include "tomoko.hpp"
unique_pointer<Video> video;
unique_pointer<Audio> audio;
unique_pointer<Input> input;
Emulator::Interface* emulator = nullptr;

auto locate(string name) -> string {
  string location = {programpath(), name};
  if(file_system_object::exists(location)) return location;

  location = {configpath(), "nSide/", name};
  if(file_system_object::exists(location)) return location;

  location = {localpath(), "nSide/", name};
  if(file_system_object::exists(location)) return location;

  location = {configpath(), "higan/", name};
  if(file_system_object::exists(location)) return location;

  location = {localpath(), "higan/", name};
  if(file_system_object::exists(location)) return location;

  directory::create({localpath(), "nSide/"});
  return {localpath(), "nSide/", name};
}

auto locateSystem(string name) -> string {
  string location = {settings["Library/Location"].text(), "System/", name};
  if(file_system_object::exists(location)) return location;

  return locate(name);
}

#include <nall/main.hpp>
auto nall::main(lstring args) -> void {
  Application::setName("nSide");
  new Program(args);
  Application::run();
}
