struct InputMapping {
  auto bind() -> void;
  auto bind(shared_pointer<HID::Device> device, unsigned group, unsigned input, int16 oldValue, int16 newValue) -> bool;
  auto poll() -> int16;
  auto unbind() -> void;

  auto isDigital() const -> bool { return !link || link->type == 0; }
  auto isAnalog() const -> bool { return link && link->type == 1; }
  auto isRumble() const -> bool { return link && link->type == 2; }

  auto assignmentName() -> string;
  auto deviceName() -> string;

  string name;
  string assignment = "None";
  Emulator::Interface::Device::Input* link = nullptr;
  shared_pointer<HID::Device> device;
  unsigned group = 0;
  unsigned input = 0;
  enum class Qualifier : unsigned { None, Lo, Hi, Rumble } qualifier = Qualifier::None;
};

struct InputHotkey : InputMapping {
  function<void ()> action;

  int16 state = 0;
};

struct InputDevice {
  string name;
  vector<InputMapping*> mappings;  //pointers used so that addresses do not change when arrays are resized
};

struct InputPort {
  string name;
  vector<InputDevice> devices;
};

struct InputEmulator {
  string name;
  vector<InputPort> ports;
};

struct InputManager {
  InputManager();
  auto bind() -> void;
  auto poll() -> void;
  auto onChange(shared_pointer<HID::Device> device, unsigned group, unsigned input, int16 oldValue, int16 newValue) -> void;
  auto quit() -> void;

  auto findMouse() -> shared_pointer<HID::Device>;

  //hotkeys.cpp
  auto appendHotkeys() -> void;
  auto pollHotkeys() -> void;

  vector<shared_pointer<HID::Device>> devices;
  vector<InputEmulator> emulators;
  vector<InputHotkey*> hotkeys;
  Configuration::Document config;
};

extern InputManager* inputManager;
