struct Tracer {
  file fp;
  bool mask;
  uint8* cpuMask;
  uint8* smpMask;

  void resetMask();
  bool maskCPU(uint24 addr);
  bool maskSMP(uint16 addr);

  bool enabled();
  void enable(bool);

  Tracer();
  ~Tracer();

  template<typename... Args> void print(Args&&... args) {
    fp.print(std::forward<Args>(args)...);
  }
};

extern Tracer* tracer;
