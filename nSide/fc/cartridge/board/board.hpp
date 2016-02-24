struct Board {
  Board(Markup::Node& board_node);
  virtual ~Board();

  static auto read(MappedRAM& memory, uint addr) -> uint8;
  static auto write(MappedRAM& memory, uint addr, uint8 byte) -> void;
  static auto mirror(uint addr, uint size) -> uint;

  virtual auto main() -> void;
  virtual auto tick() -> void;

  virtual uint8 prg_read(uint addr) = 0;
  virtual void prg_write(uint addr, uint8 data) = 0;

  virtual auto chr_read(uint addr) -> uint8;
  virtual auto chr_write(uint addr, uint8 data) -> void;

  virtual inline auto scanline(uint y) -> void {}

  virtual auto power() -> void;
  virtual auto reset() -> void;

  virtual auto serialize(serializer&) -> void;

  static Board* load(Markup::Node board_node);

  Chip* chip = nullptr;

  MappedRAM prgrom;
  MappedRAM prgram;
  MappedRAM chrrom;
  MappedRAM chrram;
  MappedRAM instrom;
  MappedRAM keyrom;
};
