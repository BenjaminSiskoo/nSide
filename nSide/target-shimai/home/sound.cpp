auto Home::Sound::reset() -> void {
  string path = {"Themes/", home->theme.name, "/bgm.pcm"};
  if(audioFile = vfs::fs::file::open(locate(path), vfs::file::mode::read)) {
    if(audioFile->size() >= 8) {
      uint32 header = audioFile->readm(4);
      if(header == 0x4d535531) {  //"MSU1"
        loopOffset = 8 + audioFile->readl(4) * 4;
        if(loopOffset > audioFile->size()) loopOffset = 8;
        audioFile->seek(playOffset);
      }
    }
  }

  Emulator::audio.reset(2, audio->get(Audio::Frequency).get<uint>(44100));
  stream = Emulator::audio.createStream(2, 44'100.0);
}

auto Home::Sound::run() -> void {
  double left  = 0;
  double right = 0;

  if(audioFile) {
    if(audioFile->end()) {
      audioFile->seek(playOffset = loopOffset);
    } else {
      playOffset += 4;
      left  = (double)(int16)audioFile->readl(2) / 32768.0;
      right = (double)(int16)audioFile->readl(2) / 32768.0;
    }
  }
  stream->sample(left, right);
}
