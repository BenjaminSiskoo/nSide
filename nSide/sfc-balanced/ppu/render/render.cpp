#include "cache.cpp"
#include "windows.cpp"
#include "bg.cpp"
#include "object.cpp"
#include "line.cpp"

//Mode 0: ->
//     1,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,   12
//  BG4B, BG3B, OBJ0, BG4A, BG3A, OBJ1, BG2B, BG1B, OBJ2, BG2A, BG1A, OBJ3

//Mode 1 (pri=1): ->
//     1,    2,    3,    4,    5,    6,    7,    8,    9,   10
//  BG3B, OBJ0, OBJ1, BG2B, BG1B, OBJ2, BG2A, BG1A, OBJ3, BG3A
//
//Mode 1 (pri=0): ->
//     1,    2,    3,    4,    5,    6,    7,    8,    9,   10
//  BG3B, OBJ0, BG3A, OBJ1, BG2B, BG1B, OBJ2, BG2A, BG1A, OBJ3

//Mode 2: ->
//     1,    2,    3,    4,    5,    6,    7,    8
//  BG2B, OBJ0, BG1B, OBJ1, BG2A, OBJ2, BG1A, OBJ3

//Mode 3: ->
//     1,    2,    3,    4,    5,    6,    7,    8
//  BG2B, OBJ0, BG1B, OBJ1, BG2A, OBJ2, BG1A, OBJ3

//Mode 4: ->
//     1,    2,    3,    4,    5,    6,    7,    8
//  BG2B, OBJ0, BG1B, OBJ1, BG2A, OBJ2, BG1A, OBJ3

//Mode 5: ->
//     1,    2,    3,    4,    5,    6,    7,    8
//  BG2B, OBJ0, BG1B, OBJ1, BG2A, OBJ2, BG1A, OBJ3

//Mode 6: ->
//     1,    2,    3,    4,    5,    6
//  OBJ0, BG1B, OBJ1, OBJ2, BG1A, OBJ3

//Mode7: ->
//     1,    2,    3,    4,    5
//  OBJ0, BG1n, OBJ1, OBJ2, OBJ3

//Mode 7 EXTBG: ->
//     1,    2,    3,    4,    5,    6,    7
//  BG2B, OBJ0, BG1n, OBJ1, BG2A, OBJ2, OBJ3

auto PPU::renderLine() -> void {
  obj_renderLine_rto();
  if(r.displayDisable || line >= vdisp()) return clearLine();

  flushPixelCache();
  buildWindowTables(Window::ID::COL);
  updateBGInfo();

  bg_renderLine<Background::ID::BG1>();
  bg_renderLine<Background::ID::BG2>();
  bg_renderLine<Background::ID::BG3>();
  bg_renderLine<Background::ID::BG4>();
  obj_renderLine();

  renderLine_output();
}
