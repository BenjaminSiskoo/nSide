//Zilog Z80

struct CPU : Processor::Z80, Thread {
  static auto Enter() -> void;
  auto main() -> void;
  auto step(uint clocks) -> void;

  auto power() -> void;
  auto reset() -> void;

  auto idle() -> void;
  auto read(uint16 addr) -> uint8;
  auto write(uint16 addr, uint8 data) -> void;
  auto portRead(uint8 port) -> uint8;
  auto portWrite(uint8 port, uint8 data) -> void;
  auto stop() -> bool;

  vector<Thread*> peripherals;

private:
  uint8 ram[8 * 1024];  //SG-1000 = 1024, MS/GG = 8192
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
