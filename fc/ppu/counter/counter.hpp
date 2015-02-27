class PPUcounter {
public:
  alwaysinline void tick(unsigned clocks);

  alwaysinline bool   field   () const;
  alwaysinline uint16 vcounter() const;
  alwaysinline uint16 hcounter() const;
  inline uint16 lineclocks() const;

  inline void reset();
  function<void ()> scanline;
  void serialize(serializer&);

//private:
  inline void vcounter_tick();

  struct {
    bool field;
    uint16 vcounter;
    uint16 hcounter;
  } status;
};
