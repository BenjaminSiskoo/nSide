struct Program : Emulator::Interface::Bind {
  //program.cpp
  Program(lstring args);
  auto main() -> void;
  auto quit() -> void;

  //interface.cpp
  //auto loadRequest(uint id, string name, string type, bool required) -> void override;
  auto loadRequest(uint id, string path, bool required) -> void override;
  auto saveRequest(uint id, string path) -> void override;
  auto videoRefresh(const uint32* data, uint pitch, uint width, uint height) -> void override;
  auto audioSample(int16 lsample, int16 rsample) -> void override;
  auto inputPoll(uint port, uint device, uint index) -> int16 override;
  auto dipSettings(const Markup::Node& node) -> uint override { return 0u; }

  //media.cpp
  auto loadMedia(string foldername) -> bool;
  auto unloadMedia() -> void;

  //state.cpp
  auto loadState(uint slot) -> bool;
  auto saveState(uint slot) -> bool;

  //utility.cpp
  auto message(const string& text) -> void;

  DSP dspaudio;

  vector<string> mediaPaths;
  vector<string> folderPaths;

  Markup::Node higan_settings;
};

extern unique_pointer<Program> program;
