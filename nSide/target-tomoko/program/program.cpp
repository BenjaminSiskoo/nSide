#include "../tomoko.hpp"
#include <fc/interface/interface.hpp>
#include <sfc/interface/interface.hpp>
#include <ms/interface/interface.hpp>
#include <md/interface/interface.hpp>
#include <pce/interface/interface.hpp>
#include <gb/interface/interface.hpp>
#include <gba/interface/interface.hpp>
#include <ws/interface/interface.hpp>
#include "interface.cpp"
#include "medium.cpp"
#include "state.cpp"
#include "utility.cpp"
unique_pointer<Program> program;

Program::Program(string_vector args) {
  program = this;

  Emulator::platform = this;
  emulators.append(new Famicom::Interface);
  emulators.append(new SuperFamicom::Interface);
  emulators.append(new MasterSystem::MasterSystemInterface);
  emulators.append(new MegaDrive::Interface);
  emulators.append(new PCEngine::PCEngineInterface);
  emulators.append(new PCEngine::SuperGrafxInterface);
  emulators.append(new GameBoy::GameBoyInterface);
  emulators.append(new GameBoy::GameBoyColorInterface);
  emulators.append(new GameBoyAdvance::Interface);
  emulators.append(new MasterSystem::GameGearInterface);
  emulators.append(new WonderSwan::WonderSwanInterface);
  emulators.append(new WonderSwan::WonderSwanColorInterface);

  new Presentation;
  presentation->setVisible();

  if(settings["Video/Driver/Crashed"].boolean()) {
    settings["Video/Driver"].setValue("None");
    MessageDialog().setText("Video driver crash detected. Driver has been reset to 'None'").information();
  }
  settings["Video/Driver/Crashed"].setValue(true);
  settings.save();
  video = Video::create(settings["Video/Driver"].text());
  video->setContext(presentation->viewport.handle());
  video->setBlocking(settings["Video/Synchronize"].boolean());
  if(!video->ready()) MessageDialog().setText("Failed to initialize video driver").warning();
  settings["Video/Driver/Crashed"].setValue(false);
  settings.save();
  presentation->clearViewport();

  if(settings["Audio/Driver/Crashed"].boolean()) {
    settings["Audio/Driver"].setValue("None");
    MessageDialog().setText("Audio driver crash detected. Driver has been reset to 'None'").information();
  }
  settings["Audio/Driver/Crashed"].setValue(true);
  settings.save();
  audio = Audio::create(settings["Audio/Driver"].text());
  audio->setExclusive(settings["Audio/Exclusive"].boolean());
  audio->setContext(presentation->viewport.handle());
  audio->setDevice(settings["Audio/Device"].text());
  audio->setBlocking(settings["Audio/Synchronize"].boolean());
  audio->setChannels(2);
  if(!audio->ready()) MessageDialog().setText("Failed to initialize audio driver").warning();
  settings["Audio/Driver/Crashed"].setValue(false);
  settings.save();

  if(settings["Input/Driver/Crashed"].boolean()) {
    settings["Input/Driver"].setValue("None");
    MessageDialog().setText("Input driver crash detected. Driver has been reset to 'None'").information();
  }
  settings["Input/Driver/Crashed"].setValue(true);
  settings.save();
  input = Input::create(settings["Input/Driver"].text());
  input->setContext(presentation->viewport.handle());
  input->onChange({&InputManager::onChange, &inputManager()});
  if(!input->ready()) MessageDialog().setText("Failed to initialize input driver").warning();
  settings["Input/Driver/Crashed"].setValue(false);
  settings.save();

  new InputManager;
  new SettingsManager;
  new CheatDatabase;
  new ToolsManager;
  new AboutWindow;

  updateVideoShader();
  updateAudioDriver();
  updateAudioEffects();

  args.takeLeft();  //ignore program location in argument parsing
  for(auto& argument : args) {
    if(argument == "--fullscreen") {
      presentation->toggleFullScreen();
    } else if(directory::exists(argument.split("|", 1L).right())) {
      if(!argument.transform("\\", "/").endsWith("/")) argument.append("/");
      mediumQueue.append(argument);
    } else if(file::exists(argument)) {
      if(auto result = execute("icarus", "--import", argument)) {
        mediumQueue.append(result.output.strip());
      }
    }
  }
  loadMedium();

  Application::onMain({&Program::main, this});
}

auto Program::main() -> void {
  updateStatusText();
  inputManager->poll();
  inputManager->pollHotkeys();

  if(!emulator || !emulator->loaded() || pause || (!focused() && settings["Input/FocusLoss/Pause"].boolean())) {
    audio->clear();
    usleep(20 * 1000);
    return;
  }

  emulator->run();
}

auto Program::quit() -> void {
  hasQuit = true;
  unloadMedium();
  settings.save();
  inputManager->quit();
  video.reset();
  audio.reset();
  input.reset();
  Application::quit();
}
