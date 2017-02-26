struct Bus : Processor::Z80::Bus {
  auto read(uint16 addr) -> uint8 override;
  auto read_(uint16 addr) -> uint8;
  auto write(uint16 addr, uint8 data) -> void override;

  auto in(uint8 addr) -> uint8 override;
  auto out(uint8 addr, uint8 data) -> void override;

  auto power() -> void;

  //serialization.cpp
  auto serialize(serializer&) -> void;

private:
  uint8 ram[0x2000];  //SG-1000 = 1KB, MS/GG = 8KB
  uint16 ramMask;

  bool disableIO;
  bool disableBIOS;
  bool disableRAM;
  bool disableMyCard;
  bool disableCartridge;
  bool disableExpansion;
};

extern Bus bus;
