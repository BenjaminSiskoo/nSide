//Programmable Sound Generator

struct PSG : Thread {
  shared_pointer<Emulator::Stream> stream;

  static auto Enter() -> void;
  auto main() -> void;
  auto step(uint clocks) -> void;

  auto power() -> void;

  //io.cpp
  auto write(uint4 addr, uint8 data) -> void;

private:
  struct IO {
    uint3 channel;
    uint4 volumeLeft;
    uint4 volumeRight;
    uint8 lfoFrequency;
    uint2 lfoControl;
    uint1 lfoEnable;
  } io;

  struct Channel {
    //channel.cpp
    auto power() -> void;
    auto run() -> void;
    auto sample(uint5 sample) -> void;

    struct IO {
      uint12 frequency;
      uint4  volume;
      uint1  direct;
      uint1  enable;
      uint4  volumeLeft;
      uint4  volumeRight;
      uint5  waveData[32];
      uint5  waveOffset;
      uint5  waveDirect;
      uint5  noiseFrequency;  //channels 4 and 5 only
      uint1  noiseEnable;     //channels 4 and 5 only

      uint12 period;
      uint12 noisePeriod;
      uint5  noiseSample;
    } io;

    struct Output {
      uint left;
      uint right;
    } output;
  } channel[6];
};

extern PSG psg;
