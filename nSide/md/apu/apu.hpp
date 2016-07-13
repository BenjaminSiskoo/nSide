//Zilog Z80

struct APU : Processor::Z80, Thread {
  static auto Enter() -> void;
  auto main() -> void;
  auto step(uint clocks) -> void;

  auto power() -> void;
  auto reset() -> void;

  auto io() -> void;
  auto read(uint16 addr) -> uint8;
  auto write(uint16 addr, uint8 data) -> void;
  auto portRead(uint8 port) -> uint8;
  auto portWrite(uint8 port, uint8 data) -> void;
  auto stop() -> bool;
};

extern APU apu;
