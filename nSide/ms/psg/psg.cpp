#include <ms/ms.hpp>

namespace MasterSystem {

PSG psg;
#include "io.cpp"
#include "tone.cpp"
#include "noise.cpp"
#include "serialization.cpp"

auto PSG::Enter() -> void {
  while(true) scheduler.synchronize(), psg.main();
}

auto PSG::main() -> void {
  tone0.run();
  tone1.run();
  tone2.run();
  noise.run();

  int left = 0;
  if(tone0.output && tone0.left) left += levels[tone0.volume];
  if(tone1.output && tone1.left) left += levels[tone1.volume];
  if(tone2.output && tone2.left) left += levels[tone2.volume];
  if(noise.output && noise.left) left += levels[noise.volume];

  lowpassLeft += (left - lowpassLeft) * 20 / 256;
  left = left * 2 / 6 + lowpassLeft * 3 / 4;
  left = sclamp<16>(left - 32768);

  int right = 0;
  if(tone0.output && tone0.right) right += levels[tone0.volume];
  if(tone1.output && tone1.right) right += levels[tone1.volume];
  if(tone2.output && tone2.right) right += levels[tone2.volume];
  if(noise.output && noise.right) right += levels[noise.volume];

  lowpassRight += (right - lowpassRight) * 20 / 256;
  right = right * 2 / 6 + lowpassRight * 3 / 4;
  right = sclamp<16>(right - 32768);

  stream->sample(left / 32768.0, right / 32768.0);
  step(1);
}

auto PSG::step(uint clocks) -> void {
  Thread::step(clocks);
  synchronize(cpu);
}

auto PSG::power() -> void {
  //Master System is monaural; Game Gear is stereo
  //use stereo mode for both; output same sample to both channels for Master System
  create(PSG::Enter, system.colorburst() / 16.0);
  stream = Emulator::audio.createStream(2, frequency());

  select = 0;
  lowpassLeft = 0;
  lowpassRight = 0;
  for(auto n : range(15)) {
    levels[n] = 0x2000 * pow(2, n * -2.0 / 6.0) + 0.5;
  }
  levels[15] = 0;

  tone0.power();
  tone1.power();
  tone2.power();
  noise.power();
}

}
