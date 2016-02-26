auto add_clocks(uint clocks) -> void;
auto last_cycle() -> void;
auto nmi(uint16 &vector) -> void;

auto oam_dma() -> void;

auto set_nmi_line(bool) -> void;
auto set_irq_line(bool) -> void;
auto set_irq_apu_line(bool) -> void;

auto set_rdy_line(bool) -> void;
auto set_rdy_addr(bool valid, uint16 value = 0) -> void;