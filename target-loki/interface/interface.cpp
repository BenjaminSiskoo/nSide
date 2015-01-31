#include "../loki.hpp"
Interface* interface = nullptr;
SuperFamicom::Interface* emulator = nullptr;

Interface::Interface() {
  interface = this;
  emulator = new SuperFamicom::Interface;
  emulator->bind = this;
}

bool Interface::load(string pathname) {
  pathname.transform("\\", "/").rtrim("/");
  if(!directory::exists(pathname)) return false;

  string type = extension(pathname);

  for(auto& media : emulator->media) {
    if(media.bootable == false) continue;
    if(type != media.type) continue;

    this->pathname = pathname.append("/");
    pathnames.reset();
    pathnames(0) = program->path({media.name, ".sys/"});
    pathnames(media.id) = pathname;
    echo("Loaded ", pathname, "\n");

    emulator->load(media.id);
    emulator->paletteUpdate(Emulator::Interface::PaletteMode::Standard);
    emulator->power();
    presentation->setTitle(emulator->title());

    return true;
  }

  return false;
}

void Interface::unload() {
  emulator->unload();
}

//bindings

void Interface::loadRequest(unsigned id, string name, string type) {
}

void Interface::loadRequest(unsigned id, string path) {
  string pathname = {pathnames(emulator->group(id)), path};
  if(file::exists(pathname) == false) return;
  mmapstream stream(pathname);
  emulator->load(id, stream);
  echo("Loaded ", path, "\n");
}

void Interface::saveRequest(unsigned id, string path) {
  string pathname = {pathnames(emulator->group(id)), path};
  filestream stream(pathname, file::mode::write);
  emulator->save(id, stream);
  echo("Saved ", path, "\n");
}

uint32_t Interface::videoColor(unsigned source, uint16_t alpha, uint16_t red, uint16_t green, uint16_t blue) {
  return ((alpha >> 8) << 24) | ((red >> 8) << 16) | ((green >> 8) << 8) | ((blue >> 8) << 0);
}

void Interface::videoRefresh(const uint32_t* palette, const uint32_t* data, unsigned pitch, unsigned width, unsigned height) {
  uint32_t* output;
  unsigned outputPitch;

  if(video.lock(output, outputPitch, width, height)) {
    pitch >>= 2, outputPitch >>= 2;

    for(unsigned y = 0; y < height; y++) {
      const uint32_t* sp = data + y * pitch;
      uint32_t* dp = output + y * outputPitch;
      for(unsigned x = 0; x < width; x++) {
        *dp++ = palette[*sp++];
      }
    }

    video.unlock();
    video.refresh();
  }
}

void Interface::audioSample(int16_t lsample, int16_t rsample) {
  if(settings->audio.mute) lsample = 0, rsample = 0;
  signed samples[] = {lsample, rsample};
  dspaudio.sample(samples);
  while(dspaudio.pending()) {
    dspaudio.read(samples);
    audio.sample(samples[0], samples[1]);
  }
}

int16_t Interface::inputPoll(unsigned port, unsigned device, unsigned input) {
  unsigned guid = emulator->port[port].device[device].input[input].guid;
  return inputManager->inputMap[guid]->poll();
}

void Interface::inputRumble(unsigned port, unsigned device, unsigned input, bool enable) {
}

unsigned Interface::dipSettings(const Markup::Node& node) {
  return 0;
}

string Interface::path(unsigned group) {
  return pathnames(group);
}

string Interface::server() {
  return "";
}

void Interface::notify(string text) {
}
