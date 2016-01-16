struct Cheat {
  enum : uint { Unused = ~0u };

  alwaysinline auto enable() const -> bool { return codes.size() > 0; }

  auto reset() -> void;
  auto append(uint addr, uint data, uint mode) -> void;
  auto append(uint addr, uint comp, uint data, uint mode) -> void;
  auto find(uint addr, uint comp, uint mode) -> maybe<uint>;

  struct Code {
    uint addr;
    uint comp;
    uint data;
    uint mode;
  };
  vector<Code> codes;
};

extern Cheat cheat;
