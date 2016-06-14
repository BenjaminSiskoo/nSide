struct pixel_t {
  //bgr555 color data for main/subscreen pixels: 0x0000 = transparent / use palette color # 0
  //needs to be bgr555 instead of palette index for direct color mode ($2130 bit 0) to work
  uint16 src_above, src_below;
  //indicates source of palette # for main/subscreen (BG1-4, OAM, or back)
  uint8  bg_above,  bg_below;
  //color_exemption -- true when bg == OAM && palette index >= 192, disables color add/sub effects
  uint8  ce_above,  ce_below;
  //priority level of src_n. to set src_n,
  //the priority of the pixel must be >pri_n
  uint8  pri_above, pri_below;
} pixelCache[256];

uint8* bg_tiledata[3];
uint8* bg_tiledataState[3];  //0 = valid, 1 = dirty

auto render_bg_tile(uint color_depth, uint16 tile_num) -> void;
inline auto flushPixelCache() -> void;
auto alloc_tiledata_cache() -> void;
auto flushTiledataCache() -> void;
auto freeTiledataCache() -> void;

//windows.cpp
struct window_t {
  uint8 above[256], below[256];
} windowCache[6];

auto buildWindowTable(uint bg_id, bool screen) -> void;
auto buildWindowTables(uint bg_id) -> void;

//bg.cpp
struct {
  uint16 tw,  th;  //tile width, height
  uint16 mx,  my;  //screen mask x, y
  uint16 scx, scy; //sc index offsets
} bg_info[4];
auto updateBGInfo() -> void;

template<uint bg_id> auto renderBGLine(uint mode) -> void;

//mode7.cpp
template<uint bg_id> auto renderMode7Line() -> void;

//oam.cpp
struct SpriteItem {
  uint8  width, height;
  uint16 x;
  uint16 y;
  uint8  character;
  bool   nameSelect;
  bool   vflip;
  bool   hflip;
  uint8  priority;
  uint8  palette;
  bool   size;
} spriteList[128];
bool spriteListValid;
uint activeSprite;

uint8 oam_itemList[32];
struct oam_tileitem {
  uint16 x, y, pri, pal, tile;
  bool   hflip;
} oam_tileList[34];

const uint8 OAM_PRI_NONE = ~1;
uint8 oam_linePalette[256], oam_linePriority[256];

auto updateSpriteList(uint addr, uint8 data) -> void;
auto build_spriteList() -> void;
auto is_sprite_on_scanline() -> bool;
auto load_oam_tiles() -> void;
auto render_oam_tile(int tile_num) -> void;
auto renderLine_oam_rto() -> void;
auto renderLine_oam(uint8 pri0_pos, uint8 pri1_pos, uint8 pri2_pos, uint8 pri3_pos) -> void;

//line.cpp
inline auto getPalette(uint8 index) -> uint16;
inline auto getDirectColor(uint8 p, uint8 t) -> uint16;
inline auto get_pixel_normal(uint32 x) -> uint16;
inline auto get_pixel_swap(uint32 x) -> uint16;
auto renderLine_output() -> void;
auto clearLine() -> void;
