#include <fc/fc.hpp>

#define CHEAT_CPP
namespace Famicom {

Cheat cheat;

auto Cheat::reset() -> void {
  codes.reset();
}

auto Cheat::append(unsigned addr, unsigned data) -> void {
  codes.append({addr, Unused, data});
}

auto Cheat::append(unsigned addr, unsigned comp, unsigned data) -> void {
  codes.append({addr, comp, data});
}

auto Cheat::find(unsigned addr, unsigned comp) -> maybe<unsigned> {
  for(auto& code : codes) {
    if(code.addr == addr && (code.comp == Unused || code.comp == comp)) {
      return code.data;
    }
  }
  return nothing;
}

}
