#pragma once

namespace Emulator {

struct Cheat {
  struct Code {
    uint addr;
    uint dataWidth;
    uint data;
    maybe<uint> comp;
  };

  explicit operator bool() const {
    return codes.size() > 0;
  }

  auto reset() -> void {
    codes.reset();
  }

  auto append(uint addr, uint dataWidth, uint data, maybe<uint> comp = nothing) -> void {
    codes.append({addr, dataWidth, data, comp});
  }

  auto assign(const string_vector& list) -> void {
    reset();
    for(auto& entry : list) {
      for(auto code : entry.split("+")) {
        auto part = code.split("/");
        //reject cheats where the data is an odd number of digits long
        if(part[1].size() & 1) continue;
        //reject cheats with mismatched data and comp sizes
        if(part.size() == 3 && (part[1].size() != part[2].size())) continue;

        uint dataWidth = part[1].size() >> 1;
        if(part.size() == 2) append(part[0].hex(), dataWidth, part[1].hex());
        if(part.size() == 3) append(part[0].hex(), dataWidth, part[2].hex(), part[1].hex());
      }
    }
  }

  template<uint dataWidth> auto find(uint addr, uint comp) -> maybe<uint> {
    for(auto& code : codes) {
      if(code.addr == addr && code.dataWidth == dataWidth && (!code.comp || code.comp() == comp)) {
        return code.data;
      }
    }
    return nothing;
  }

private:
  vector<Code> codes;
};

}
