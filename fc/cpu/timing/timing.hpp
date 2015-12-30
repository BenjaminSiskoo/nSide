auto add_clocks(uint clocks) -> void;
auto op_read(uint16 addr) -> uint8;
auto op_write(uint16 addr, uint8 data) -> void;
auto last_cycle() -> void;
auto nmi(uint16 &vector) -> void;

auto oam_dma() -> void;

auto set_nmi_line(bool) -> void;
auto set_irq_line(bool) -> void;
auto set_irq_apu_line(bool) -> void;

auto set_rdy_line(bool) -> void;
auto set_rdy_addr(bool valid, uint16 value = 0) -> void;