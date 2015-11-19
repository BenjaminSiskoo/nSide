#include "../base.hpp"
Interface* interface = nullptr;
SuperFamicom::Interface* emulator = nullptr;

Interface::Interface() {
  interface = this;
  emulator = new SuperFamicom::Interface;
  emulator->bind = this;
}

bool Interface::loadCartridge(const string& foldername) {
  lpath(0) = program->path("Super Famicom.sys/");
  lpath(1) = foldername;

  if(SuperFamicom::cartridge.loaded()) {
    SuperFamicom::cartridge.unload();
    debugger->print("Cartridge unloaded\n");
  }

  pathName = foldername;
  directory::create({pathName, "debug/"});

  emulator->load(SuperFamicom::ID::SuperFamicom);
  emulator->power();

  string name = pathName;
  name = basename(name);
  name.rtrim("/");

  presentation->setTitle(name);
  emulator->paletteUpdate(Emulator::Interface::PaletteMode::Standard);
  debugger->print(SuperFamicom::cartridge.information.markup.cartridge, "\n");
  debugger->suspend();
  return true;
}

//request from emulation core to load non-volatile media file
void Interface::loadRequest(unsigned id, string path, bool required) {
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
void Interface::saveRequest(unsigned id, string path, bool required) {
  string pathname = {this->path(emulator->group(id)), path};
  filestream stream(pathname, file::mode::write);
  debugger->print("Saved ", pathname, "\n");
  return emulator->save(id, stream);
}

void Interface::unload() {
  emulator->unload();
  lpath.reset();
}

bool Interface::loadState(unsigned slot) {
  string filename = {pathName, "state-", slot, ".bst"};
  auto memory = file::read(filename);
  if(memory.empty()) return false;
  serializer s(memory.data(), memory.size());
  bool result = SuperFamicom::system.unserialize(s);
  if(result) debugger->print("Loaded state from ", filename, "\n");
  return result;
}

bool Interface::saveState(unsigned slot) {
  SuperFamicom::system.runtosave();
  serializer s = SuperFamicom::system.serialize();
  string filename = {pathName, "state-", slot, ".bst"};
  bool result = file::write(filename, s.data(), s.size());
  if(result) debugger->print("Saved state to ", filename, "\n");
  return result;
}

uint32_t Interface::videoColor(unsigned source, uint16_t a, uint16_t r, uint16_t g, uint16_t b) {
  a >>= 8, r >>= 8, g >>= 8, b >>= 8;
  return 255u << 24 | r << 16 | g << 8 | b << 0;
}

//hires is always true for accuracy core
//overscan is ignored for the debugger port
void Interface::videoRefresh(const uint32_t* palette, const uint32_t* data, unsigned pitch, unsigned width, unsigned height) {
  uint32_t* output;
  unsigned outputPitch;

  if(video->lock(output, outputPitch, width, height)) {
    pitch >>= 2, outputPitch >>= 2;

    for(unsigned y = 0; y < height; y++) {
      const uint32_t* sp = data + y * pitch;
      uint32_t* dp = output + y * outputPitch;
      for(unsigned x = 0; x < width; x++) {
        *dp++ = palette[*sp++];
      }
    }

    video->unlock();
    video->refresh();
  }

  /*
  uint32_t* output = presentation->canvas.data();
  pitch >>= 2;

  bool interlace = pitch == 512;
  if(interlace == false) {
    for(unsigned y = 0; y < height; y++) {
      const uint32_t *sp = data + y * pitch;
      uint32_t* dp0 = output + y * pitch, *dp1 = dp0 + (pitch >> 1);
      for(unsigned x = 0; x < width; x++) {
        *dp0++ = palette[*sp];
        *dp1++ = palette[*sp++];
      }
    }
  } else {
    for(unsigned y = 0; y < height; y++) {
      const uint32_t* sp = data + y * pitch;
      uint32_t* dp = output + y * 512; // outputPitch
      for(unsigned x = 0; x < width; x++) {
        *dp++ = palette[*sp++];
      }
    }
  }

  presentation->canvas.setData();
  */
}

void Interface::audioSample(int16_t lsample, int16_t rsample) {
  if(settings->audio.mute) lsample = 0, rsample = 0;
  signed samples[] = {lsample, rsample};
  dspaudio.sample(samples);
  while(dspaudio.pending()) {
    dspaudio.read(samples);
    audio->sample(samples[0], samples[1]);
  }
}

int16_t Interface::inputPoll(unsigned port, unsigned device, unsigned input) {
  if(presentation->focused() == false) return 0;
  auto keyboardState = phoenix::Keyboard::state();

  if(port == 0) {
    if(device == (unsigned)SuperFamicom::Input::Device::Joypad) {
      switch((SuperFamicom::Input::JoypadID)input) {
      case SuperFamicom::Input::JoypadID::B:      return keyboardState[(unsigned)phoenix::Keyboard::Scancode::Z];
      case SuperFamicom::Input::JoypadID::Y:      return keyboardState[(unsigned)phoenix::Keyboard::Scancode::A];
      case SuperFamicom::Input::JoypadID::Select: return keyboardState[(unsigned)phoenix::Keyboard::Scancode::Apostrophe];
      case SuperFamicom::Input::JoypadID::Start:  return keyboardState[(unsigned)phoenix::Keyboard::Scancode::Return];
      case SuperFamicom::Input::JoypadID::Up:     return keyboardState[(unsigned)phoenix::Keyboard::Scancode::Up];
      case SuperFamicom::Input::JoypadID::Down:   return keyboardState[(unsigned)phoenix::Keyboard::Scancode::Down];
      case SuperFamicom::Input::JoypadID::Left:   return keyboardState[(unsigned)phoenix::Keyboard::Scancode::Left];
      case SuperFamicom::Input::JoypadID::Right:  return keyboardState[(unsigned)phoenix::Keyboard::Scancode::Right];
      case SuperFamicom::Input::JoypadID::A:      return keyboardState[(unsigned)phoenix::Keyboard::Scancode::X];
      case SuperFamicom::Input::JoypadID::X:      return keyboardState[(unsigned)phoenix::Keyboard::Scancode::S];
      case SuperFamicom::Input::JoypadID::L:      return keyboardState[(unsigned)phoenix::Keyboard::Scancode::D];
      case SuperFamicom::Input::JoypadID::R:      return keyboardState[(unsigned)phoenix::Keyboard::Scancode::C];
      }
    }
  }

  return 0;
}

string Interface::path(unsigned group) {
  return lpath(group);
}

void Interface::message(const string &text) {
  debugger->print(text, "\n");
}
