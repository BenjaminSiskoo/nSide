#include "laevateinn.hpp"
#include "resource/resource.cpp"
unique_pointer<Video> video;
unique_pointer<Audio> audio;
unique_pointer<Input> input;
Emulator::Interface* emulator = nullptr;

auto locate(string name) -> string {
  string location = {programpath(), "star-rod-", name};
  if(inode::exists(location)) return location;

  location = {configpath(), "star-rod/", name};
  if(inode::exists(location)) return location;

  directory::create({localpath(), "star-rod/"});
  return {localpath(), "star-rod/", name};
}

auto locateHigan(string name) -> string {
  string location = {programpath(), name};
  if(inode::exists(location)) return location;

  location = {configpath(), "nSide/", name};
  if(inode::exists(location)) return location;

  return {localpath(), "nSide/", name};
}

auto locateSystem(string name) -> string {
  string location = {program->higan_settings["Library/Location"].text(), "System/", name};
  if(inode::exists(location)) return location;

  return locate(name);
}

#include <nall/main.hpp>
auto nall::main(lstring args) -> void {
  Application::setName("star-rod");
  new Program(args);
  Application::run();
}
