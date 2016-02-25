struct LayerWindow {
  auto render(bool screen) -> void;
  auto serialize(serializer&) -> void;

  bool one_enable;
  bool one_invert;
  bool two_enable;
  bool two_invert;

  uint mask;

  bool main_enable;
  bool sub_enable;

  uint8_t main[256];
  uint8_t sub[256];
};

struct ColorWindow {
  auto render(bool screen) -> void;
  auto serialize(serializer&) -> void;

  bool one_enable;
  bool one_invert;
  bool two_enable;
  bool two_invert;

  uint mask;

  uint main_mask;
  uint sub_mask;

  uint8_t main[256];
  uint8_t sub[256];
};
