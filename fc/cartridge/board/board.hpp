struct Board {
  static uint8 read(MappedRAM& memory, unsigned addr);
  static void write(MappedRAM& memory, unsigned addr, uint8 byte);
  static unsigned mirror(unsigned addr, unsigned size);

  virtual void enter();
  virtual void tick();

  virtual uint8 prg_read(unsigned addr) = 0;
  virtual void prg_write(unsigned addr, uint8 data) = 0;

  virtual uint8 chr_read(unsigned addr);
  virtual void chr_write(unsigned addr, uint8 data);

  virtual inline void scanline(unsigned y) {}

  virtual void power();
  virtual void reset();

  virtual void serialize(serializer&);
  Board(Markup::Node& cartridge);
  virtual ~Board();

  static Board* load(Markup::Node cartridge);

  Chip* chip = nullptr;

  MappedRAM prgrom;
  MappedRAM prgram;
  MappedRAM chrrom;
  MappedRAM chrram;
  MappedRAM instrom;
  MappedRAM keyrom;
};
