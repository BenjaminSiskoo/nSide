#include <gba/gba.hpp>

#define CHEAT_CPP
namespace GameBoyAdvance {

Cheat cheat;

auto Cheat::reset() -> void {
  codes.reset();
}

auto Cheat::append(unsigned addr, unsigned data, unsigned mode) -> void {
  codes.append({addr, Unused, data, mode});
}

auto Cheat::append(unsigned addr, unsigned comp, unsigned data, unsigned mode) -> void {
  codes.append({addr, comp, data, mode});
}

auto Cheat::find(unsigned addr, unsigned comp, unsigned mode) -> maybe<unsigned> {
  for(auto& code : codes) {
    if(code.addr == addr && (code.comp == Unused || code.comp == comp) && (code.mode & mode)) {
      return code.data;
    }
  }
  return nothing;
}

}
