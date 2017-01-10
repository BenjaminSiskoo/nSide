struct Bus : Processor::Z80::Bus {
  auto read(uint16 addr) -> uint8 override;
  auto write(uint16 addr, uint8 data) -> void override;

  auto in(uint8 addr) -> uint8 override;
  auto out(uint8 addr, uint8 data) -> void override;

  auto reset() -> void;

  boolean disableIO;
  boolean disableBIOS;
  boolean disableRAM;
  boolean disableMyCard;
  boolean disableCartridge;
  boolean disableExpansion;

private:
  uint8 ram[0x2000];  //SG-1000 = 1KB, MS/GG = 8KB
  uint16 ramMask;
};

extern Bus bus;
