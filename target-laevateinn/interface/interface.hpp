struct Interface : Emulator::Interface::Bind {
  Interface();

  string pathName;

  bool loadCartridge(const string& foldername);
  bool loadState(unsigned slot);
  bool saveState(unsigned slot);

  //void loadRequest(unsigned id, string name, string type, bool required);
  void loadRequest(unsigned id, string path, bool required);
  void saveRequest(unsigned id, string path, bool required);

  void unload();

  uint32_t videoColor(unsigned source, uint16_t, uint16_t, uint16_t, uint16_t);
  void videoRefresh(const uint32_t* palette, const uint32_t* data, unsigned pitch, unsigned width, unsigned height);
  void audioSample(int16_t lsample, int16_t rsample);
  int16_t inputPoll(unsigned port, unsigned device, unsigned index);
  unsigned dipSettings(const Markup::Node& node) { return 0u; }

  string path(unsigned group);
  lstring lpath;

  void message(const string& text);
};

extern Interface* interface;
extern SuperFamicom::Interface* emulator;
