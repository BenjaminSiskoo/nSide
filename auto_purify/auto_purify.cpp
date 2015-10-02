#include <nall/nall.hpp>
#include <nall/beat/patch.hpp>
#include "heuristics/famicom.hpp"
#include "heuristics/super-famicom.hpp"
#include "heuristics/game-boy.hpp"
#include "heuristics/game-boy-advance.hpp"
using namespace nall;

#include <phoenix/phoenix.hpp>
using namespace phoenix;

namespace Database {
  #include "../cart-pal/database/famicom.hpp"
  #include "../cart-pal/database/playchoice-10.hpp"
  #include "../cart-pal/database/super-famicom.hpp"
  #include "../cart-pal/database/sufami-turbo.hpp"
  #include "../cart-pal/database/bsx-satellaview.hpp"
};

struct AutoPurify {
  #include "configuration.cpp"
  string libraryPath;

  AutoPurify();

  struct Information {
    string path;      //path to selected file
    string name;      //name of selected file (inside of archive if .zip)
    string archive;   //pathname of archive
    string manifest;  //manifest from successfully applied patch
  } information;

  //archive.cpp
  vector<uint8_t> extractROM();
  vector<uint8_t> extractFile(const string &filename);

  //patch.cpp
  void applyBeatPatch(vector<uint8_t> &buffer);

  //famicom.cpp
  void copyFamicomSaves(const string &pathname);
  string createFamicomDatabase(vector<uint8_t> &buffer, Markup::Node &document, const string &manifest);
  string createFamicomHeuristic(vector<uint8_t> &buffer);
  string openFamicom(vector<uint8_t> &buffer);
  string syncFamicom(const string &pathname);

  //super-famicom.cpp
  void copySuperFamicomSaves(const string &pathname);
  string createSuperFamicomDatabase(vector<uint8_t> &buffer, Markup::Node &document, const string &manifest);
  string createSuperFamicomHeuristic(vector<uint8_t> &buffer);
  void createSuperFamicomHeuristicFirmware(vector<uint8_t> &buffer, const string &pathname, bool firmware_appended);
  string openSuperFamicom(vector<uint8_t> &buffer);
  string syncSuperFamicom(const string &pathname);

  //sufami-turbo.cpp
  void copySufamiTurboSaves(const string &pathname);
  string createSufamiTurboDatabase(vector<uint8_t> &buffer, Markup::Node &document, const string &manifest);
  string createSufamiTurboHeuristic(vector<uint8_t> &buffer);
  string openSufamiTurbo(vector<uint8_t> &buffer);
  string syncSufamiTurbo(const string &pathname);

  //bsx-satellaview.cpp
  string createBsxSatellaviewDatabase(vector<uint8_t> &buffer, Markup::Node &document, const string &manifest);
  string createBsxSatellaviewHeuristic(vector<uint8_t> &buffer);
  string openBsxSatellaview(vector<uint8_t> &buffer);
  string syncBsxSatellaview(const string &pathname);

  //game-boy.cpp
  void copyGameBoySaves(const string &pathname);
  string createGameBoyHeuristic(vector<uint8_t> &buffer);
  string openGameBoy(vector<uint8_t> &buffer);
  string syncGameBoy(const string &pathname);

  //game-boy-advance.cpp
  void copyGameBoyAdvanceSaves(const string &pathname);
  string createGameBoyAdvanceHeuristic(vector<uint8_t> &buffer);
  string openGameBoyAdvance(vector<uint8_t> &buffer);
  string syncGameBoyAdvance(const string &pathname);

  static bool supported(const string &filename);
  string open(string filename = "");
  string sync(string pathname);
};

#include "resource/resource.cpp"
#include "file-dialog.cpp"
#include "archive.cpp"
#include "patch.cpp"
#include "famicom.cpp"
#include "super-famicom.cpp"
#include "sufami-turbo.cpp"
#include "bsx-satellaview.cpp"
#include "game-boy.cpp"
#include "game-boy-advance.cpp"

FileDialog *fileDialog = nullptr;

AutoPurify::AutoPurify() {
  libraryPath = string::read({configpath(), "nSide-e/library.bml"}).strip().ltrim("Path: ", 1L).replace("\\", "/");
  if(libraryPath.empty()) libraryPath = {userpath(), "Emulation/"};
  if(libraryPath.endsWith("/") == false) libraryPath.append("/");
}

bool AutoPurify::supported(const string &filename) {
  string extension = suffixname(filename).ltrim(".", 1L);

  if(extension == "fc" ) return true;
  if(extension == "nes") return true;
  if(extension == "sfc") return true;
  if(extension == "smc") return true;
  if(extension == "st" ) return true;
  if(extension == "bs" ) return true;
  if(extension == "gb" ) return true;
  if(extension == "gbc") return true;
  if(extension == "gba") return true;
  if(extension == "zip") return true;

  return false;
}

string AutoPurify::open(string filename) {
  if(filename.empty()) {
    if(!fileDialog) {
      fileDialog = new FileDialog;
      fileDialog->setGeometry(config.geometry);
    }
    fileDialog->setPath(config.path);
    filename = fileDialog->open();
    config.geometry = fileDialog->geometry().text();
  }

  if(filename.empty()) return "";

  information.path = dirname(filename);
  information.name = basename(filename);
  config.path = information.path;  //remember last used directory

  vector<uint8_t> buffer;
  if(filename.endsWith(".zip")) {
    information.archive = filename;
    buffer = extractROM();
  } else {
    buffer = file::read(filename);
  }
  if(buffer.size() == 0) return "";  //failed to read file

  applyBeatPatch(buffer);

  if(information.name.endsWith(".fc") || information.name.endsWith(".nes")) return openFamicom(buffer);
  if(information.name.endsWith(".sfc") || information.name.endsWith(".smc")) return openSuperFamicom(buffer);
  if(information.name.endsWith(".st")) return openSufamiTurbo(buffer);
  if(information.name.endsWith(".bs")) return openBsxSatellaview(buffer);
  if(information.name.endsWith(".gb") || information.name.endsWith(".gbc")) return openGameBoy(buffer);
  if(information.name.endsWith(".gba")) return openGameBoyAdvance(buffer);
  return "";
}

string AutoPurify::sync(string pathname) {
  if(pathname.endsWith(".fc/")) return syncFamicom(pathname);
  if(pathname.endsWith(".sfc/")) return syncSuperFamicom(pathname);
  if(pathname.endsWith(".st/")) return syncSufamiTurbo(pathname);
  if(pathname.endsWith(".bs/")) return syncBsxSatellaview(pathname);
  if(pathname.endsWith(".gb/")) return syncGameBoy(pathname);
  if(pathname.endsWith(".gbc/")) return syncGameBoy(pathname);
  if(pathname.endsWith(".gba/")) return syncGameBoyAdvance(pathname);
  return "";
}

extern "C" string auto_purify_browse(const string &filename) {
  AutoPurify auto_purify;
  return auto_purify.open();
}

extern "C" string auto_purify_open(const string &filename) {
  AutoPurify auto_purify;
  return auto_purify.open(filename);
}

extern "C" string auto_purify_sync(const string &pathname) {
  AutoPurify auto_purify;
  return auto_purify.sync(pathname);
}
