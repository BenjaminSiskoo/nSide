struct FamicomBox : Thread {
  enum class Exception : uint {
    Interrupt6_82Hz = 0,
    AttractionTimer = 1,
    ControllerRead  = 2,
    KeyswitchRotate = 3,
    Coin            = 4,
    ResetButton     = 5,
    Watchdog        = 6,
    CATVPin1        = 7,
  };

  static auto Enter() -> void;
  auto main() -> void;

  auto init() -> void;
  auto load() -> void;
  auto unload() -> void;
  auto power() -> void;
  auto reset() -> void;

  auto setDip(uint10 dip) -> void;
  auto trap(Exception exceptionId) -> void;

  auto wramRead(uint16 addr, uint8 data) -> uint8;
  auto portRead(uint16 addr, uint8 data) -> uint8;
  auto sramRead(uint16 addr, uint8 data) -> uint8;
  auto cartridgeRead(uint16 addr, uint8 data) -> uint8;
  auto wramWrite(uint16 addr, uint8 data) -> void;
  auto portWrite(uint16 addr, uint8 data) -> void;
  auto sramWrite(uint16 addr, uint8 data) -> void;
  auto cartridgeWrite(uint16 addr, uint8 data) -> void;

  auto chrRead(uint14 addr) -> uint8;
  auto chrWrite(uint14 addr, uint8 data) -> void;

  auto serialize(serializer& s) -> void;

  //memory map
  //0000-07ff: Game RAM
  //0800-1fff: BIOS RAM
  //2000-3fff: PPU Registers
  //4000-4015: APU/CPU Registers
  //4016-4017: APU/CPU Registers and Watchdog
  //5000-5fff: FamciomBox registers
  //6000-7fff: FamicomBox TEST Mode RAM
  //8000-ffff: cartridge ROM
  uint8 bios_prg[0x8000];
  uint8 bios_chr[0x2000];

  uint8 bios_ram[0x1800]; // actually 0x2000, but 0x800 of it is game RAM
  uint8 test_ram[0x2000];

  uint10 dip;

  uint8 exceptionEnable;
  uint8 exceptionTrap;

  uint3 ramProtect;

  uint counter;
  uint15 attractionTimer;
  uint14 watchdog;

  uint4 cartridgeSelect;
  uint2 cartridgeRowSelect;
  bool registerLock;
};

extern FamicomBox famicombox;
