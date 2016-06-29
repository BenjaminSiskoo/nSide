//ARMv3 (ARM60)

struct ArmDSP : Processor::ARM, Cothread {
  #include "registers.hpp"

  ArmDSP();
  ~ArmDSP();

  static auto Enter() -> void;
  auto boot() -> void;
  auto main() -> void;

  auto step(uint clocks) -> void override;
  auto _idle() -> void override;
  auto _read(uint mode, uint32 addr) -> uint32 override;
  auto _write(uint mode, uint32 addr, uint32 word) -> void override;

  auto read(uint24 addr, uint8 data) -> uint8;
  auto write(uint24 addr, uint8 data) -> void;

  auto init() -> void;
  auto load() -> void;
  auto unload() -> void;
  auto power() -> void;
  auto reset() -> void;
  auto resetARM() -> void;

  auto firmware() const -> nall::vector<uint8>;
  auto serialize(serializer&) -> void;

  uint8* programROM;
  uint8* dataROM;
  uint8* programRAM;
};

extern ArmDSP armdsp;
