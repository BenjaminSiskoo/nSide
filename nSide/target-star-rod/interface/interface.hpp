struct Interface : Emulator::Interface::Bind {
  Interface();

  string pathName;

  bool loadCartridge(const string& foldername);
  bool loadState(uint slot);
  bool saveState(uint slot);

  //void loadRequest(uint id, string name, string type, bool required);
  void loadRequest(uint id, string path, bool required);
  void saveRequest(uint id, string path, bool required);

  void unload();

  uint32 videoColor(uint source, uint16, uint16, uint16, uint16);
  void videoRefresh(const uint32* palette, const uint32* data, uint pitch, uint width, uint height);
  void audioSample(int16 lsample, int16 rsample);
  int16 inputPoll(uint port, uint device, uint index);
  uint dipSettings(const Markup::Node& node) { return 0u; }

  string path(uint group);
  lstring lpath;

  void message(const string& text);
};

extern Interface* interface;
extern SuperFamicom::Interface* emulator;
