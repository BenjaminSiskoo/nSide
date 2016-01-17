#include <nall/nall.hpp>
using namespace nall;

#include <hiro/hiro.hpp>
using namespace hiro;

//if file already exists in the same path as the binary; use it (portable mode)
//if not, use default requested path (*nix/user mode)
auto locate(string pathname, string filename) -> string {
  string location{programpath(), filename};
  if(file_system_object::exists(location)) return location;
  return {pathname, filename};
}

#include "settings.cpp"
Settings settings;

#include "heuristics/famicom.hpp"
#include "heuristics/super-famicom.hpp"
#include "heuristics/game-boy.hpp"
#include "heuristics/game-boy-advance.hpp"
#include "heuristics/bs-memory.hpp"
#include "heuristics/sufami-turbo.hpp"

#include "core/core.hpp"
#include "core/core.cpp"
#include "core/famicom.cpp"
#include "core/vs-system.cpp"
#include "core/playchoice-10.cpp"
#include "core/super-famicom.cpp"
#include "core/game-boy.cpp"
#include "core/game-boy-color.cpp"
#include "core/game-boy-advance.cpp"
#include "core/bs-memory.cpp"
#include "core/sufami-turbo.cpp"
CartPal cart_pal;

#include "ui/ui.hpp"
#include "ui/scan-dialog.cpp"
#include "ui/settings-dialog.cpp"
#include "ui/import-dialog.cpp"
#include "ui/error-dialog.cpp"

#include <nall/main.hpp>
auto nall::main(lstring args) -> void {
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
    .setFilters("ROM Files|*.fc:*.vs:*.pc10:*.nes:*.sfc:*.smc:*.gb:*.gbc:*.gba:*.bs:*.st:*.zip")
    .openFile()) {
      if(string target = cart_pal.import(source)) {
        settings["cart-pal/Path"].setValue(pathname(source));
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
