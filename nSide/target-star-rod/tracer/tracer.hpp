struct Tracer {
  file fp;
  bool mask;
  uint8* cpuMask;
  uint8* smpMask;

  auto resetMask() -> void;
  auto maskCPU(uint24 addr) -> bool;
  auto maskSMP(uint16 addr) -> bool;

  auto enabled() -> bool;
  auto enable(bool state) -> void;

  Tracer();
  ~Tracer();

  template<typename... Args> void print(Args&&... args) {
    fp.print(std::forward<Args>(args)...);
  }
};

extern unique_pointer<Tracer> tracer;
