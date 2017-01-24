struct Board {
  Board(Markup::Node& boardNode);
  virtual ~Board();

  static auto read(MappedRAM& memory, uint addr) -> uint8;
  static auto write(MappedRAM& memory, uint addr, uint8 byte) -> void;
  static auto mirror(uint addr, uint size) -> uint;

  virtual auto main() -> void;
  virtual auto tick() -> void;

  virtual uint8 readPRG(uint addr, uint8 data) = 0;
  virtual void writePRG(uint addr, uint8 data) = 0;

  virtual auto readCHR(uint addr) -> uint8;
  virtual auto writeCHR(uint addr, uint8 data) -> void;

  virtual inline auto scanline(uint y) -> void {}

  virtual auto power() -> void;
  virtual auto reset() -> void;

  virtual auto serialize(serializer&) -> void;

  static Board* load(Markup::Node boardNode);

  Chip* chip = nullptr;

  MappedRAM prgrom;
  MappedRAM prgram;
  MappedRAM chrrom;
  MappedRAM chrram;
  MappedRAM instrom;
  MappedRAM keyrom;
};
