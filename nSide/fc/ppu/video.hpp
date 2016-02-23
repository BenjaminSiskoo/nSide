struct Video {
  Video();

  auto reset() -> void;
  auto refresh() -> void;

private:
  auto refreshMain() -> void;
  auto refreshVSDualSystem() -> void;
  auto refreshPC10DualScreen() -> void;
  auto refreshPC10Menu() -> void;

  auto generateColor(uint, double, double, double, double, double) -> uint32;
  auto generatePalettes(const uint9* ppu_pal) -> void;

  auto drawCursor(uint16 color, int x, int y) -> void;
  auto drawCursors() -> void;

  unique_pointer<uint32[]> output;
  unique_pointer<uint32[]> paletteLiteral;
  unique_pointer<uint32[]> paletteStandard;
  unique_pointer<uint32[]> paletteEmulation;
};

extern Video video;
