#include "cache.cpp"
#include "windows.cpp"
#include "bg.cpp"
#include "oam.cpp"
#include "mode7.cpp"
#include "line.cpp"

//Mode 0: ->
//     1,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,   12
//  BG4B, BG3B, OAM0, BG4A, BG3A, OAM1, BG2B, BG1B, OAM2, BG2A, BG1A, OAM3

//Mode 1 (pri=1): ->
//     1,    2,    3,    4,    5,    6,    7,    8,    9,   10
//  BG3B, OAM0, OAM1, BG2B, BG1B, OAM2, BG2A, BG1A, OAM3, BG3A
//
//Mode 1 (pri=0): ->
//     1,    2,    3,    4,    5,    6,    7,    8,    9,   10
//  BG3B, OAM0, BG3A, OAM1, BG2B, BG1B, OAM2, BG2A, BG1A, OAM3

//Mode 2: ->
//     1,    2,    3,    4,    5,    6,    7,    8
//  BG2B, OAM0, BG1B, OAM1, BG2A, OAM2, BG1A, OAM3

//Mode 3: ->
//     1,    2,    3,    4,    5,    6,    7,    8
//  BG2B, OAM0, BG1B, OAM1, BG2A, OAM2, BG1A, OAM3

//Mode 4: ->
//     1,    2,    3,    4,    5,    6,    7,    8
//  BG2B, OAM0, BG1B, OAM1, BG2A, OAM2, BG1A, OAM3

//Mode 5: ->
//     1,    2,    3,    4,    5,    6,    7,    8
//  BG2B, OAM0, BG1B, OAM1, BG2A, OAM2, BG1A, OAM3

//Mode 6: ->
//     1,    2,    3,    4,    5,    6
//  OAM0, BG1B, OAM1, OAM2, BG1A, OAM3

//Mode7: ->
//     1,    2,    3,    4,    5
//  OAM0, BG1n, OAM1, OAM2, OAM3

//Mode 7 EXTBG: ->
//     1,    2,    3,    4,    5,    6,    7
//  BG2B, OAM0, BG1n, OAM1, BG2A, OAM2, OAM3

auto PPU::renderLine() -> void {
  renderLine_oam_rto();
  if(r.displayDisable || line >= vdisp()) return clearLine();

  flushPixelCache();
  buildWindowTables(Window::ID::COL);
  updateBGInfo();

  if(r.bgMode != 7) {
    renderBGLine<Background::ID::BG1>(r.bgMode);
    renderBGLine<Background::ID::BG2>(r.bgMode);
    renderBGLine<Background::ID::BG3>(r.bgMode);
    renderBGLine<Background::ID::BG4>(r.bgMode);
  } else {
    if(!r.extbg) {
      renderMode7Line<Background::ID::BG1>();
    } else {
      renderMode7Line<Background::ID::BG1>();
      renderMode7Line<Background::ID::BG2>();
    }
  }
  renderLine_oam(oam.r.priority[0], oam.r.priority[1], oam.r.priority[2], oam.r.priority[3]);

  renderLine_output();
}
