#ifndef FC_HPP
namespace Famicom {
#endif

struct ID {
  enum : unsigned {
    //cartridges (folders)
    System,
    Famicom,
    PlayChoice10,
    VSSystem,

    //memory (files)
    PC10BIOS,

    Manifest,
    ProgramROM,
    ProgramRAM,
    CharacterROM,
    CharacterRAM,

    InstructionROM,
    Key,

    //controller ports
    Port1 = 1,
    Port2 = 2,
    ExpansionPort = 4,
  };
};

struct Interface : Emulator::Interface {
  string title();
  double videoFrequency();
  double audioFrequency();

  bool loaded();
  string sha256();
  unsigned group(unsigned id);
  void load(unsigned id);
  void save();
  void load(unsigned id, const stream& stream);
  void save(unsigned id, const stream& stream);
  void unload();

  void connect(unsigned port, unsigned device);
  void power();
  void reset();
  void run();

  serializer serialize();
  bool unserialize(serializer&);

  void cheatSet(const lstring&);

  void paletteUpdate(PaletteMode mode);

  //debugger functions
  void exportMemory();

  Interface();

  struct DeviceRef {
    unsigned id;
    unsigned port[3];
  };
  vector<DeviceRef> device_ref;

private:
  void addDevice(Device device);
};

extern Interface* interface;

#ifndef FC_HPP
}
#endif
