#include "tomoko.hpp"
unique_pointer<Video> video;
unique_pointer<Audio> audio;
unique_pointer<Input> input;
Emulator::Interface* emulator = nullptr;

auto locate(string name) -> string {
  string location = {programpath(), name};
  if(inode::exists(location)) return location;

  location = {configpath(), "nSide/", name};
  if(inode::exists(location)) return location;

  location = {localpath(), "nSide/", name};
  if(inode::exists(location)) return location;

  location = {configpath(), "higan/", name};
  if(inode::exists(location)) {
    if(suffixname(location) == ".bml" && suffixname(dirname(location)) != ".sys") {
      directory::create({configpath(), "nSide/"});
      file::copy(location, {configpath(), "nSide/", name});
      return {configpath(), "nSide/", name};
    }
    return location;
  }

  location = {localpath(), "higan/", name};
  if(inode::exists(location)) {
    if(suffixname(location) == ".bml" && suffixname(dirname(location)) != ".sys") {
      directory::create({localpath(), "nSide/"});
      file::copy(location, {localpath(), "nSide/", name});
      return {localpath(), "nSide/", name};
    }
    return location;
  }

  directory::create({localpath(), "nSide/"});
  return {localpath(), "nSide/", name};
}

auto locateSystem(string name) -> string {
  string location = {settings["Library/Location"].text(), "System/", name};
  if(inode::exists(location)) return location;

  return locate(name);
}

#include <nall/main.hpp>
auto nall::main(lstring args) -> void {
  Application::setName("nSide");
  new Program(args);
  Application::run();
}
