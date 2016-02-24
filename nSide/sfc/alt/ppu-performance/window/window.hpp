struct LayerWindow {
  auto render(bool screen) -> void;
  auto serialize(serializer&) -> void;

  bool one_enable;
  bool one_invert;
  bool two_enable;
  bool two_invert;

  uint_t mask;

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

  uint_t mask;

  uint_t main_mask;
  uint_t sub_mask;

  uint8_t main[256];
  uint8_t sub[256];
};
