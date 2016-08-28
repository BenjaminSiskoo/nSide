struct Bus {
  auto read(uint16 addr) -> uint8;
  auto write(uint16 addr, uint8 data) -> void;

  auto in(uint8 addr) -> uint8;
  auto out(uint8 addr, uint8 data) -> void;

  uint ramMask;

  bool disableIO;
  bool disableBIOS;
  bool disableRAM;
  bool disableMyCard;
  bool disableCartridge;
  bool disableExpansion;

  struct IO {
    struct Port {
      bool trDirection;
      bool thDirection;
      bool trOutput;
      bool thOutput;
    } port[2];
  } io;

private:
  uint8 ram[0x2000];  //SG-1000 = 1KB, MS/GG = 8KB
};

extern Bus bus;
