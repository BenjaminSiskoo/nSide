struct Video {
  Video();
  ~Video();

  auto reset() -> void;
  auto refresh() -> void;

  uint32* output = nullptr;
  uint32* paletteStandard = nullptr;
  uint32* paletteEmulation = nullptr;

private:
  auto refreshMain() -> void;
  auto refreshVSDualSystem() -> void;
  auto refreshPC10DualScreen() -> void;
  auto refreshPC10Menu() -> void;

  auto generateColor(uint, double, double, double, double, double) -> uint32;
  auto generatePalettes(const uint9* ppu_pal) -> void;

  auto drawCursor(uint16 color, int x, int y) -> void;
  auto drawCursors() -> void;

  static const uint8 gammaRamp[8];
  static const uint8 cursor[15 * 15];

  friend class System;
};

extern Video video;
