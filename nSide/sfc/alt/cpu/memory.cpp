auto CPU::pio() -> uint8_t {
  return status.pio;
}

auto CPU::joylatch() -> bool {
  return status.joypad_strobe_latch;
}

auto CPU::interrupt_pending() -> bool {
  return false;
}

auto CPU::port_read(uint8_t port) -> uint8_t {
  return port_data[port & 3];
}

auto CPU::port_write(uint8_t port, uint8_t data) -> void {
  port_data[port & 3] = data;
}

auto CPU::op_io() -> void {
  add_clocks(6);
}

auto CPU::op_read(uint_t addr) -> uint8_t {
  regs.mdr = bus.read(addr, regs.mdr);
  add_clocks(speed(addr));
  return regs.mdr;
}

auto CPU::op_write(uint_t addr, uint8_t data) -> void {
  add_clocks(speed(addr));
  bus.write(addr, regs.mdr = data);
}

auto CPU::speed(uint_t addr) const -> uint_t {
  if(addr & 0x408000) {
    if(addr & 0x800000) return status.rom_speed;
    return 8;
  }
  if((addr + 0x6000) & 0x4000) return 8;
  if((addr - 0x4000) & 0x7e00) return 6;
  return 12;
}
