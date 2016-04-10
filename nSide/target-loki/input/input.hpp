struct AbstractInput {
  void bind();
  int16_t poll();

  string name;
  string mapping;

  shared_pointer<HID::Device> device;
  unsigned group = 0;
  unsigned input = 0;
};

struct InputManager : Configuration::Document {
  InputManager();
  void load();
  void unload();

  void bind();
  void poll();

  vector<shared_pointer<HID::Device>> devices;
  vector<AbstractInput*> inputMap;
};

extern unique_pointer<InputManager> inputManager;
