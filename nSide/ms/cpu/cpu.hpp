//Zilog Z80

struct CPU : Processor::Z80, Thread {
  static auto Enter() -> void;
  auto main() -> void;
  auto step(uint clocks) -> void;

  auto wait() -> void override;
  auto read(uint16 addr) -> uint8 override;
  auto write(uint16 addr, uint8 data) -> void override;
  auto portRead(uint8 port) -> uint8 override;
  auto portWrite(uint8 port, uint8 data) -> void override;
  auto stop() -> bool override;

  auto power() -> void;
  auto reset() -> void;

  vector<Thread*> peripherals;

private:
  uint8 ram[0x2000];  //SG-1000 = 1KB, MS/GG = 8KB
  uint ramMask;

  bool ioDisable;
  bool biosDisable;
  bool ramDisable;
  bool cardDisable;
  bool cartridgeDisable;
  bool expansionDisable;

  struct IO {
    struct Port {
      bool trDirection;
      bool thDirection;
      bool trOutput;
      bool thOutput;
    } port[2];
  } io;
};

extern CPU cpu;
