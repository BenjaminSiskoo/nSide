//PPUcounter emulates the H/V latch counters of the S-PPU2.
//
//real hardware has the S-CPU maintain its own copy of these counters that are
//updated based on the state of the S-PPU Vblank and Hblank pins. emulating this
//would require full lock-step synchronization for every clock tick.
//to bypass this and allow the two to run out-of-order, both the CPU and PPU
//classes inherit PPUcounter and keep their own counters.
//the timers are kept in sync, as the only differences occur on V=240 and V=261,
//based on interlace. thus, we need only synchronize and fetch interlace at any
//point before this in the frame, which is handled internally by this class at
//V=128.

struct PPUcounter {
  alwaysinline auto tick() -> void;
  alwaysinline auto tick(uint clocks) -> void;

  alwaysinline auto field() const -> bool;
  alwaysinline auto vcounter() const -> uint16;
  alwaysinline auto hcounter() const -> uint16;
  inline auto hdot() const -> uint16;
  inline auto lineclocks() const -> uint16;

  alwaysinline auto field(uint offset) const -> bool;
  alwaysinline auto vcounter(uint offset) const -> uint16;
  alwaysinline auto hcounter(uint offset) const -> uint16;

  inline auto reset() -> void;
  auto serialize(serializer&) -> void;

  function<auto () -> void> scanline;

private:
  inline auto vcounterTick() -> void;

  struct {
    bool interlace;
    bool field;
    uint16 vcounter;
    uint16 hcounter;
  } status;

  struct {
    bool field[2048];
    uint16 vcounter[2048];
    uint16 hcounter[2048];

    int32 index;
  } history;
};
