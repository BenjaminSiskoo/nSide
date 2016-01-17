#include "../laevateinn.hpp"
Interface* interface = nullptr;
SuperFamicom::Interface* emulator = nullptr;

Interface::Interface() {
  interface = this;
  emulator = new SuperFamicom::Interface;
  emulator->bind = this;
}

bool Interface::loadCartridge(const string& foldername) {
  lpath(0) = locate(
    {program->higan_settings["Library/Location"].text(), "System/"},
    {localpath(), "nSide/"},
    {"Super Famicom.sys/"}
  );
  lpath(1) = foldername;

  if(SuperFamicom::cartridge.loaded()) {
    SuperFamicom::cartridge.unload();
    debugger->print("Cartridge unloaded\n");
  }

  pathName = foldername;
  directory::create({pathName, "debug/"});

  emulator->load(SuperFamicom::ID::SuperFamicom);
  emulator->power();
  emulator->connect(SuperFamicom::ID::ControllerPort1, (uint)SuperFamicom::Device::ID::Gamepad);
  emulator->set("Blur Emulation", false);
  emulator->set("Color Emulation", false);
  emulator->set("Scanline Emulation", false);

  string name = pathName;
  name = basename(name);
  name.rtrim("/");

  presentation->setTitle(name);
  debugger->print(SuperFamicom::cartridge.information.markup.cartridge, "\n");
  debugger->suspend();
  return true;
}

//request from emulation core to load non-volatile media file
void Interface::loadRequest(uint id, string path, bool required) {
  string pathname = {this->path(emulator->group(id)), path};
  if(file::exists(pathname) == false && required) {
    debugger->print(
      "Missing required file: ", path, "\n",
      "From location:\n", this->path(emulator->group(id)), "\n"
    );
    return;
  }
  mmapstream stream(pathname);
  debugger->print("Loaded ", pathname, "\n");
  return emulator->load(id, stream);
}

//request from emulation core to save non-volatile media file
void Interface::saveRequest(uint id, string path, bool required) {
  string pathname = {this->path(emulator->group(id)), path};
  filestream stream(pathname, file::mode::write);
  debugger->print("Saved ", pathname, "\n");
  return emulator->save(id, stream);
}

void Interface::unload() {
  emulator->unload();
  lpath.reset();
}

bool Interface::loadState(uint slot) {
  string filename = {pathName, "state-", slot, ".bst"};
  auto memory = file::read(filename);
  if(memory.empty()) return false;
  serializer s(memory.data(), memory.size());
  bool result = SuperFamicom::system.unserialize(s);
  if(result) debugger->print("Loaded state from ", filename, "\n");
  return result;
}

bool Interface::saveState(uint slot) {
  SuperFamicom::system.runToSave();
  serializer s = SuperFamicom::system.serialize();
  string filename = {pathName, "state-", slot, ".bst"};
  bool result = file::write(filename, s.data(), s.size());
  if(result) debugger->print("Saved state to ", filename, "\n");
  return result;
}

//hires is always true for accuracy core
//overscan is ignored for the debugger port
void Interface::videoRefresh(const uint32* data, uint pitch, uint width, uint height) {
  uint32* output;
  uint length;

  if(video->lock(output, length, width, height)) {
    pitch >>= 2, length >>= 2;

    for(auto y : range(height)) {
      memory::copy(output + y * length, data + y * pitch, width * sizeof(uint32));
    }

    video->unlock();
    video->refresh();
  }

  /*
  uint32* output = presentation->canvas.data();
  pitch >>= 2;

  bool interlace = pitch == 512;
  if(interlace == false) {
    for(uint y = 0; y < height; y++) {
      const uint32 *sp = data + y * pitch;
      uint32* dp0 = output + y * pitch, *dp1 = dp0 + (pitch >> 1);
      for(uint x = 0; x < width; x++) {
        *dp0++ = palette[*sp];
        *dp1++ = palette[*sp++];
      }
    }
  } else {
    for(uint y = 0; y < height; y++) {
      const uint32* sp = data + y * pitch;
      uint32* dp = output + y * 512; // outputPitch
      for(uint x = 0; x < width; x++) {
        *dp++ = palette[*sp++];
      }
    }
  }

  presentation->canvas.setData();
  */
}

void Interface::audioSample(int16 lsample, int16 rsample) {
  if(settings->audio.mute) lsample = 0, rsample = 0;
  int samples[] = {lsample, rsample};
  dspaudio.sample(samples);
  while(dspaudio.pending()) {
    dspaudio.read(samples);
    audio->sample(samples[0], samples[1]);
  }
}

int16 Interface::inputPoll(uint port, uint device, uint input) {
  if(presentation->focused() == false) return 0;
  auto keyboardState = phoenix::Keyboard::state();

  if(port == SuperFamicom::ID::ControllerPort1) {
    if(device == (uint)SuperFamicom::Device::ID::Gamepad) {
      switch(input) {
      case SuperFamicom::Gamepad::B:      return keyboardState[(uint)phoenix::Keyboard::Scancode::Z];
      case SuperFamicom::Gamepad::Y:      return keyboardState[(uint)phoenix::Keyboard::Scancode::A];
      case SuperFamicom::Gamepad::Select: return keyboardState[(uint)phoenix::Keyboard::Scancode::Apostrophe];
      case SuperFamicom::Gamepad::Start:  return keyboardState[(uint)phoenix::Keyboard::Scancode::Return];
      case SuperFamicom::Gamepad::Up:     return keyboardState[(uint)phoenix::Keyboard::Scancode::Up];
      case SuperFamicom::Gamepad::Down:   return keyboardState[(uint)phoenix::Keyboard::Scancode::Down];
      case SuperFamicom::Gamepad::Left:   return keyboardState[(uint)phoenix::Keyboard::Scancode::Left];
      case SuperFamicom::Gamepad::Right:  return keyboardState[(uint)phoenix::Keyboard::Scancode::Right];
      case SuperFamicom::Gamepad::A:      return keyboardState[(uint)phoenix::Keyboard::Scancode::X];
      case SuperFamicom::Gamepad::X:      return keyboardState[(uint)phoenix::Keyboard::Scancode::S];
      case SuperFamicom::Gamepad::L:      return keyboardState[(uint)phoenix::Keyboard::Scancode::D];
      case SuperFamicom::Gamepad::R:      return keyboardState[(uint)phoenix::Keyboard::Scancode::C];
      }
    }
  }

  return 0;
}

string Interface::path(uint group) {
  return lpath(group);
}

void Interface::message(const string &text) {
  debugger->print(text, "\n");
}
