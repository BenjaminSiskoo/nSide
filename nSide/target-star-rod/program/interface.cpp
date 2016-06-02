//request from emulation core to load non-volatile media file
auto Program::loadRequest(uint id, string filename, bool required) -> void {
  string pathname = mediumPaths(emulator->group(id));
  string location = {pathname, filename};

  if(filename == "manifest.bml" && pathname && !pathname.endsWith("sys/")) {
    if(!file::exists(location)) {
      //As a debugger, star-rod will never ignore manifests.
      //Only call cart-pal if the manifest is missing.
      if(auto manifest = execute("cart-pal", "--manifest", pathname)) {
        memorystream stream{manifest.output.data<uint8_t>(), manifest.output.size()};
        return emulator->load(id, stream);
      }
    }
  }

  if(file::exists(location)) {
    mmapstream stream{location};
    debugger->print("Loaded ", location, "\n");
    return emulator->load(id, stream);
  }

  if(required) debugger->print(
    "Missing required file: ", filename, "\n",
    "From location:\n", this->path(emulator->group(id)), "\n"
  );
}

//request from emulation core to save non-volatile media file
auto Program::saveRequest(uint id, string filename) -> void {
  string pathname = mediumPaths(emulator->group(id));
  string location = {pathname, filename};
  filestream stream(location, file::mode::write);
  debugger->print("Saved ", location, "\n");
  return emulator->save(id, stream);
}

//hires is always true for accuracy core
//overscan is ignored for the debugger port
auto Program::videoRefresh(const uint32* data, uint pitch, uint width, uint height) -> void {
  uint32_t* output;
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

auto Program::audioSample(const double* samples, uint channels) -> void {
  if(settings->audio.mute) return audio->sample(0, 0);
  int16 left  = sclamp<16>(samples[0] * 32768.0);
  int16 right = sclamp<16>(samples[1] * 32768.0);
  audio->sample(left, right);
}

auto Program::inputPoll(uint port, uint device, uint input) -> int16 {
  if(presentation->focused() == false) return 0;

  if(port == SuperFamicom::Port::Controller1) {
    if(device == SuperFamicom::Device::Gamepad) {
      switch(input) {
      case SuperFamicom::Gamepad::B:      return hiro::Keyboard::pressed("Z");
      case SuperFamicom::Gamepad::Y:      return hiro::Keyboard::pressed("A");
      case SuperFamicom::Gamepad::Select: return hiro::Keyboard::pressed("Apostrophe");
      case SuperFamicom::Gamepad::Start:  return hiro::Keyboard::pressed("Enter");
      case SuperFamicom::Gamepad::Up:     return hiro::Keyboard::pressed("Up");
      case SuperFamicom::Gamepad::Down:   return hiro::Keyboard::pressed("Down");
      case SuperFamicom::Gamepad::Left:   return hiro::Keyboard::pressed("Left");
      case SuperFamicom::Gamepad::Right:  return hiro::Keyboard::pressed("Right");
      case SuperFamicom::Gamepad::A:      return hiro::Keyboard::pressed("X");
      case SuperFamicom::Gamepad::X:      return hiro::Keyboard::pressed("S");
      case SuperFamicom::Gamepad::L:      return hiro::Keyboard::pressed("D");
      case SuperFamicom::Gamepad::R:      return hiro::Keyboard::pressed("C");
      }
    }
  }

  return 0;
}
