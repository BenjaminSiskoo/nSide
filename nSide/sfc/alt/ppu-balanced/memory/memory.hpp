auto get_vram_address() -> uint16_t;

auto vram_mmio_read(uint16_t addr) -> uint8_t;
auto vram_mmio_write(uint16_t addr, uint8_t data) -> void;

auto oam_mmio_read(uint16_t addr) -> uint8_t;
auto oam_mmio_write(uint16_t addr, uint8_t data) -> void;

auto cgram_mmio_read(uint16_t addr) -> uint8_t;
auto cgram_mmio_write(uint16_t addr, uint8_t data) -> void;
