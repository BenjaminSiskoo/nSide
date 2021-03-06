#include <nall/nall.hpp>
using namespace nall;

#include <hiro/hiro.hpp>
using namespace hiro;

using uint8 = Natural<8>;

auto locate(string name) -> string {
  string location = {Path::program(), name};
  if(inode::exists(location)) return location;

  location = {Path::config(), "cart-pal/", name};
  if(inode::exists(location)) return location;

  directory::create({Path::local(), "cart-pal/"});
  return {Path::local(), "cart-pal/", name};
}

#include "settings.cpp"
Settings settings;

#include "heuristics/atari-2600.cpp"
#include "heuristics/famicom.cpp"
#include "heuristics/super-famicom.cpp"
#include "heuristics/master-system.cpp"
#include "heuristics/mega-drive.cpp"
#include "heuristics/pc-engine.cpp"
#include "heuristics/supergrafx.cpp"
#include "heuristics/game-boy.cpp"
#include "heuristics/game-boy-advance.cpp"
#include "heuristics/game-gear.cpp"
#include "heuristics/wonderswan.cpp"
#include "heuristics/vs-system.cpp"
#include "heuristics/bs-memory.cpp"
#include "heuristics/sufami-turbo.cpp"

#include "core/core.hpp"
#include "core/core.cpp"
#include "core/atari-2600.cpp"
#include "core/famicom.cpp"
#include "core/super-famicom.cpp"
#include "core/sg-1000.cpp"
#include "core/master-system.cpp"
#include "core/mega-drive.cpp"
#include "core/pc-engine.cpp"
#include "core/supergrafx.cpp"
#include "core/game-boy.cpp"
#include "core/game-boy-color.cpp"
#include "core/game-boy-advance.cpp"
#include "core/game-gear.cpp"
#include "core/wonderswan.cpp"
#include "core/wonderswan-color.cpp"
#include "core/vs-system.cpp"
#include "core/playchoice-10.cpp"
#include "core/famicombox.cpp"
#include "core/bs-memory.cpp"
#include "core/sufami-turbo.cpp"
CartPal cart_pal;

#include "ui/ui.hpp"
#include "ui/scan-dialog.cpp"
#include "ui/settings-dialog.cpp"
#include "ui/import-dialog.cpp"
#include "ui/error-dialog.cpp"

#include <nall/main.hpp>
auto nall::main(string_vector args) -> void {
  Application::setName("cart-pal");

  if(args.size() == 2 && args[1] == "--name") {
    return print("cart-pal");
  }

  if(args.size() == 3 && args[1] == "--manifest" && directory::exists(args[2])) {
    return print(cart_pal.manifest(args[2]));
  }

  if(args.size() == 3 && args[1] == "--import" && file::exists(args[2])) {
    if(string target = cart_pal.import(args[2])) {
      return print(target, "\n");
    }
    return;
  }

  if(args.size() == 2 && args[1] == "--import") {
    if(string source = BrowserDialog()
    .setTitle("Load ROM Image")
    .setPath(settings["cart-pal/Path"].text())
    .setFilters("ROM Files|"
      "*.a26:"
      "*.fc:*.nes:"
      "*.sfc:*.smc:"
      "*.sg1000:"
      "*.ms:*.sms:"
      "*.md:*.smd:*.gen:"
      "*.pce:"
      "*.sg:"
      "*.gb:"
      "*.gbc:"
      "*.gba:"
      "*.gg:"
      "*.ws:"
      "*.wsc:"
      "*.vs:"
      "*.pc10:"
      "*.fcb:"
      "*.bs:"
      "*.st:"
      "*.zip"
    ).openFile()) {
      if(string target = cart_pal.import(source)) {
        settings["cart-pal/Path"].setValue(Location::path(source));
        return print(target, "\n");
      }
    }
    return;
  }

  new ScanDialog;
  new SettingsDialog;
  new ImportDialog;
  new ErrorDialog;
  #if defined(PLATFORM_MACOSX)
  Application::Cocoa::onAbout([&] {
    MessageDialog().setTitle("About cart-pal").setText({
      "cart-pal\n\n"
      "Based on icarus by byuu\n"
      "Famicom Additions Author: hex_usr\n"
      "License: GPLv3\n"
      "icarus Website: http://byuu.org/\n"
    }).information();
  });
  Application::Cocoa::onPreferences([&] {
    scanDialog->settingsButton.doActivate();
  });
  Application::Cocoa::onQuit([&] {
    Application::quit();
  });
  #endif
  scanDialog->show();
  Application::run();
}
