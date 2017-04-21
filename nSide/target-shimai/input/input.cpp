#include "../shimai.hpp"
#include "hotkeys.cpp"
unique_pointer<InputManager> inputManager;

auto InputMapping::bind() -> void {
  inputList.reset();
  auto list = assignment.split(logic() == 0 ? "|" : "&");

  for(auto& mapping : list) {
    auto token = mapping.split("/");
    if(token.size() == 1) continue;

    uint64 id = token[0].natural();
    uint group = token[1].natural();
    uint input = token[2].natural();
    string qualifier = token(3, "None");

    Input item;
    for(auto& device : inputManager->devices) {
      if(id != device->id()) continue;

      item.device = device;
      item.group = group;
      item.input = input;
      item.qualifier = Qualifier::None;
      if(qualifier == "Lo") item.qualifier = Qualifier::Lo;
      if(qualifier == "Hi") item.qualifier = Qualifier::Hi;
      if(qualifier == "Rumble") item.qualifier = Qualifier::Rumble;
      break;
    }
    if(!item.device) continue;

    inputList.append(item);
  }

  settings[path].setValue(assignment);
}

auto InputMapping::append(string encoding) -> bool {
  const string logicOperator = logic() == 0 ? "|" : "&";
  auto mappings = assignment.split(logicOperator);
  if(mappings.find(encoding)) return true;  //mapping already bound
  if(!assignment || assignment == "None") assignment = encoding;  //remove "None"
  else assignment.append(logicOperator, encoding);  //add to existing mapping list
  bind();
  return true;
}

auto InputMapping::bind(shared_pointer<HID::Device> device, uint group, uint input, int16 oldValue, int16 newValue) -> bool {
  if(device->isNull() || (device->isKeyboard() && device->group(group).input(input).name() == "Escape")) {
    return unbind(), true;
  }

  string encoding = {"0x", hex(device->id()), "/", group, "/", input};

  if(isDigital()) {
    if((device->isKeyboard() && group == HID::Keyboard::GroupID::Button)
    || (device->isMouse() && group == HID::Mouse::GroupID::Button)
    || (device->isJoypad() && group == HID::Joypad::GroupID::Button)) {
      if(newValue) {
        return append(encoding);
      }
    }

    if((device->isJoypad() && group == HID::Joypad::GroupID::Axis)
    || (device->isJoypad() && group == HID::Joypad::GroupID::Hat)
    || (device->isJoypad() && group == HID::Joypad::GroupID::Trigger)) {
      if(newValue < -16384 && group != HID::Joypad::GroupID::Trigger) {  //triggers are always hi
        return append({encoding, "/Lo"});
      }

      if(newValue > +16384) {
        return append({encoding, "/Hi"});
      }
    }
  }

  if(isAnalog()) {
    if((device->isMouse() && group == HID::Mouse::GroupID::Axis)
    || (device->isJoypad() && group == HID::Joypad::GroupID::Axis)
    || (device->isJoypad() && group == HID::Joypad::GroupID::Hat)) {
      if(newValue < -16384 || newValue > +16384) {
        return append(encoding);
      }
    }
  }

  if(isRumble()) {
    if(device->isJoypad() && group == HID::Joypad::GroupID::Button) {
      if(newValue) {
        return append({encoding, "/Rumble"});
      }
    }
  }

  return false;
}

auto InputMapping::poll() -> int16 {
  if(inputList.size() == 0) return 0;

  if(isDigital()) {
    bool result = logic();
    for(auto& item : inputList) {
      auto device = item.device;
      auto value = device->group(item.group).input(item.input).value();
      bool output = logic();
      if(device->isKeyboard() && item.group == HID::Keyboard::GroupID::Button) output = value != 0;
      if(device->isMouse() && item.group == HID::Mouse::GroupID::Button) output = value != 0;
      if(device->isJoypad() && item.group == HID::Joypad::GroupID::Button) output = value != 0;
      if((device->isJoypad() && item.group == HID::Joypad::GroupID::Axis)
      || (device->isJoypad() && item.group == HID::Joypad::GroupID::Hat)
      || (device->isJoypad() && item.group == HID::Joypad::GroupID::Trigger)) {
        if(item.qualifier == Qualifier::Lo) output = value < -16384;
        if(item.qualifier == Qualifier::Hi) output = value > +16384;
      }
      if(logic() == 0) result |= output;
      if(logic() == 1) result &= output;
    }
    return result;
  }

  if(isAnalog()) {
    int16 result = 0;
    for(auto& item : inputList) {
      auto device = item.device;
      auto value = device->group(item.group).input(item.input).value();
      if(device->isMouse() && item.group == HID::Mouse::GroupID::Axis) result += value;
      if(device->isJoypad() && item.group == HID::Joypad::GroupID::Axis) result += value >> 8;
      if(device->isJoypad() && item.group == HID::Joypad::GroupID::Hat) result += value < 0 ? -1 : value > 0 ? +1 : 0;
    }
    return result;
  }

  return 0;
}

auto InputMapping::rumble(bool enable) -> void {
  if(inputList.size() == 0) return;
  for(auto& item : inputList) {
    ::input->rumble(item.device->id(), enable);
  }
}

auto InputMapping::unbind() -> void {
  inputList.reset();
  assignment = "None";
  settings[path].setValue(assignment);
}

auto InputMapping::assignmentName() -> string {
  if(inputList.size() == 0) return "None";
  string path;
  string logicOperator = logic() == 0 ? " | " : " & ";
  for(auto& item : inputList) {
    path.append(item.device->name());
    if(item.device->name() != "Keyboard") {
      path.append("(", hex(item.device->id()), ")");
      //keyboards only have one group; no need to append group name
      path.append(".", item.device->group(item.group).name());
    }
    path.append(".", item.device->group(item.group).input(item.input).name());
    if(item.qualifier == Qualifier::Lo) path.append(".Lo");
    if(item.qualifier == Qualifier::Hi) path.append(".Hi");
    if(item.qualifier == Qualifier::Rumble) path.append(".Rumble");
    path.append(logicOperator);
  }
  return path.trimRight(logicOperator);
}

//

InputManager::InputManager() {
  inputManager = this;
  frequency = max(1u, settings["Input/Frequency"].natural());

  for(auto& emulator : program->emulators) {
    auto& inputEmulator = emulators(emulators.size());
    inputEmulator.interface = emulator;
    inputEmulator.name = emulator->information.name;

    for(auto& port : emulator->ports) {
      auto& inputPort = inputEmulator.ports(port.id);
      inputPort.name = port.name;
      for(auto& device : port.devices) {
        auto& inputDevice = inputPort.devices(device.id);
        inputDevice.name = device.name;
        for(auto& input : device.inputs) {
          auto& inputMapping = inputDevice.mappings(inputDevice.mappings.size());
          inputMapping.name = input.name;
          inputMapping.type = input.type;

          inputMapping.path = string{inputEmulator.name, "/", inputPort.name, "/", inputDevice.name, "/", inputMapping.name}.replace(" ", "");
          inputMapping.assignment = settings(inputMapping.path).text();
          inputMapping.bind();
        }
      }
    }
  }

  appendHotkeys();
}

//Emulator::Interface::inputPoll() needs to call into InputManager::InputEmulator
//this function is calling during Program::loadMedium() to link the two together
auto InputManager::bind(Emulator::Interface* interface) -> void {
  this->emulator = nullptr;
  for(auto& emulator : emulators) {
    if(emulator.interface == interface) {
      this->emulator = &emulator;
    }
  }
  assert(this->emulator != nullptr);
}

auto InputManager::bind() -> void {
  for(auto& emulator : emulators) {
    for(auto& port : emulator.ports) {
      for(auto& device : port.devices) {
        for(auto& mapping : device.mappings) {
          mapping.bind();
        }
      }
    }
  }

  for(auto& hotkey : hotkeys) {
    hotkey->bind();
  }
}

auto InputManager::poll() -> void {
  //polling actual hardware is very time-consuming: skip call if poll was called too recently
  auto thisPoll = chrono::millisecond();
  if(thisPoll - lastPoll < frequency) return;
  lastPoll = thisPoll;

  auto devices = input->poll();
  bool changed = devices.size() != this->devices.size();
  if(!changed) {
    for(auto n : range(devices)) {
      changed = devices[n] != this->devices[n];
      if(changed) break;
    }
  }
  if(changed) {
    this->devices = devices;
    bind();
  }
}

auto InputManager::onChange(shared_pointer<HID::Device> device, uint group, uint input, int16_t oldValue, int16_t newValue) -> void {
  //if(settingsManager->focused()) {
  //  settingsManager->input.inputEvent(device, group, input, oldValue, newValue);
  //  settingsManager->hotkeys.inputEvent(device, group, input, oldValue, newValue);
  //}
}

auto InputManager::quit() -> void {
  emulators.reset();
  hotkeys.reset();
}

auto InputManager::findMouse() -> shared_pointer<HID::Device> {
  for(auto& device : devices) {
    if(device->isMouse()) return device;
  }
  return {};
}
