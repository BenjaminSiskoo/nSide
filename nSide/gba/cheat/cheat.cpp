#include <gba/gba.hpp>

namespace GameBoyAdvance {

Cheat cheat;

auto Cheat::reset() -> void {
  codes.reset();
}

auto Cheat::append(uint addr, uint data, uint mode) -> void {
  codes.append({addr, Unused, data, mode});
}

auto Cheat::append(uint addr, uint comp, uint data, uint mode) -> void {
  codes.append({addr, comp, data, mode});
}

auto Cheat::find(uint addr, uint comp, uint mode) -> maybe<uint> {
  for(auto& code : codes) {
    if(code.addr == addr && (code.comp == Unused || code.comp == comp) && (code.mode & mode)) {
      return code.data;
    }
  }
  return nothing;
}

}
