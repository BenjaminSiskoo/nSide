struct CartPal {
  //core.cpp
  CartPal();

  auto error() const -> string;
  auto success(string location) -> string;
  auto failure(string message) -> string;

  auto manifest(string location) -> string;
  auto import(string location) -> string;

  auto concatenate(vector<uint8_t>& output, string location) -> void;

  //atari-2600.cpp
  auto atari2600Manifest(string location) -> string;
  auto atari2600Manifest(vector<uint8_t>& buffer, string location) -> string;
  auto atari2600Import(vector<uint8_t>& buffer, string location) -> string;

  //famicom.cpp
  auto famicomManifest(string location) -> string;
  auto famicomManifest(vector<uint8_t>& buffer, string location, uint* prgrom = nullptr, uint* chrrom = nullptr) -> string;
  auto famicomManifestScan(vector<Markup::Node>& roms, Markup::Node node) -> void;
  auto famicomImport(vector<uint8_t>& buffer, string location) -> string;

  //vs-system.cpp
  auto vsSystemManifest(string location) -> string;
  auto vsSystemManifest(vector<uint8_t>& buffer, string location, uint* prgrom = nullptr, uint* chrrom = nullptr) -> string;
  auto vsSystemManifestScan(vector<Markup::Node>& roms, Markup::Node node) -> void;
  auto vsSystemImport(vector<uint8_t>& buffer, string location) -> string;

  //playchoice-10.cpp
  auto playchoice10Manifest(string location) -> string;
  auto playchoice10Manifest(vector<uint8_t>& buffer, string location, uint* prgrom = nullptr, uint* chrrom = nullptr, uint* instrom = nullptr, uint* keyrom = nullptr) -> string;
  auto playchoice10ManifestScan(vector<Markup::Node>& roms, Markup::Node node) -> void;
  auto playchoice10Import(vector<uint8_t>& buffer, string location) -> string;

  //super-famicom.cpp
  auto superFamicomManifest(string location) -> string;
  auto superFamicomManifest(vector<uint8_t>& buffer, string location, bool* firmwareAppended = nullptr) -> string;
  auto superFamicomManifestScan(vector<Markup::Node>& roms, Markup::Node node) -> void;
  auto superFamicomImport(vector<uint8_t>& buffer, string location) -> string;

  //sg-1000.cpp
  auto sg1000Manifest(string location) -> string;
  auto sg1000Manifest(vector<uint8_t>& buffer, string location) -> string;
  auto sg1000Import(vector<uint8_t>& buffer, string location) -> string;

  //master-system.cpp
  auto masterSystemManifest(string location) -> string;
  auto masterSystemManifest(vector<uint8_t>& buffer, string location) -> string;
  auto masterSystemImport(vector<uint8_t>& buffer, string location) -> string;

  //mega-drive.cpp
  auto megaDriveManifest(string location) -> string;
  auto megaDriveManifest(vector<uint8_t>& buffer, string location) -> string;
  auto megaDriveImport(vector<uint8_t>& buffer, string location) -> string;

  //game-boy.cpp
  auto gameBoyManifest(string location) -> string;
  auto gameBoyManifest(vector<uint8_t>& buffer, string location) -> string;
  auto gameBoyImport(vector<uint8_t>& buffer, string location) -> string;

  //game-boy-color.cpp
  auto gameBoyColorManifest(string location) -> string;
  auto gameBoyColorManifest(vector<uint8_t>& buffer, string location) -> string;
  auto gameBoyColorImport(vector<uint8_t>& buffer, string location) -> string;

  //game-boy-advance.cpp
  auto gameBoyAdvanceManifest(string location) -> string;
  auto gameBoyAdvanceManifest(vector<uint8_t>& buffer, string location) -> string;
  auto gameBoyAdvanceImport(vector<uint8_t>& buffer, string location) -> string;

  //game-gear.cpp
  auto gameGearManifest(string location) -> string;
  auto gameGearManifest(vector<uint8_t>& buffer, string location) -> string;
  auto gameGearImport(vector<uint8_t>& buffer, string location) -> string;

  //wonderswan.cpp
  auto wonderSwanManifest(string location) -> string;
  auto wonderSwanManifest(vector<uint8_t>& buffer, string location) -> string;
  auto wonderSwanImport(vector<uint8_t>& buffer, string location) -> string;

  //wonderswan-color.cpp
  auto wonderSwanColorManifest(string location) -> string;
  auto wonderSwanColorManifest(vector<uint8_t>& buffer, string location) -> string;
  auto wonderSwanColorImport(vector<uint8_t>& buffer, string location) -> string;

  //bs-memory.cpp
  auto bsMemoryManifest(string location) -> string;
  auto bsMemoryManifest(vector<uint8_t>& buffer, string location) -> string;
  auto bsMemoryImport(vector<uint8_t>& buffer, string location) -> string;

  //sufami-turbo.cpp
  auto sufamiTurboManifest(string location) -> string;
  auto sufamiTurboManifest(vector<uint8_t>& buffer, string location) -> string;
  auto sufamiTurboImport(vector<uint8_t>& buffer, string location) -> string;

private:
  string errorMessage;

  struct {
    Markup::Node atari2600;
    Markup::Node famicom;
    Markup::Node vsSystem;
    Markup::Node playchoice10;
    Markup::Node superFamicom;
    Markup::Node sg1000;
    Markup::Node masterSystem;
    Markup::Node megaDrive;
    Markup::Node gameBoy;
    Markup::Node gameBoyColor;
    Markup::Node gameBoyAdvance;
    Markup::Node gameGear;
    Markup::Node wonderSwan;
    Markup::Node wonderSwanColor;
    Markup::Node bsMemory;
    Markup::Node sufamiTurbo;
  } database;
};
