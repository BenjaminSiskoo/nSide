struct CachePixel {
  //bgr555 color data for main/subscreen pixels: 0x0000 = transparent / use palette color # 0
  //needs to be bgr555 instead of palette index for direct color mode ($2130 bit 0) to work
  uint16 aboveColor, belowColor;
  //indicates source of palette # for main/subscreen (BG1-4, OBJ, or back)
  uint8  aboveLayer, belowLayer;
  //color_exemption -- true when bg == OBJ && palette index >= 192, disables color blend effects
  uint8  aboveColorExemption, belowColorExemption;
  //priority level of src_n. to set src_n,
  //the priority of the pixel must be >pri_n
  uint8  abovePriority, belowPriority;
} pixelCache[256];

struct TiledataCache {
  uint8* tiledata[3];
  uint8* tiledataState[3];  //0 = valid, 1 = dirty
  auto allocate() -> void;
  auto flush() -> void;
  auto free() -> void;
} tiledataCache;

auto renderBGTile(uint colorDepth, uint16 tile_num) -> void;
inline auto flushPixelCache() -> void;

//windows.cpp
struct CacheWindow {
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

template<uint bg_id> auto bg_renderLine() -> void;
template<uint bg_id> auto bg_renderLineMode7() -> void;

//object.cpp
bool spriteListValid;
uint activeSprite;

uint8 obj_itemList[32];
struct obj_tileItem {
  uint16 x;
  uint16 y;
  uint16 priority;
  uint16 palette;
  bool   hflip;
  uint16 tile;
} obj_tileList[34];

const uint8 OBJ_PRI_NONE = ~1;
uint8 obj_linePalette[256], obj_linePriority[256];

auto updateSpriteList(uint addr, uint8 data) -> void;
auto buildSpriteList() -> void;
auto isSpriteOnScanline() -> bool;
auto obj_loadTiles() -> void;
auto obj_renderTile(int n) -> void;
auto obj_renderLine_rto() -> void;
auto obj_renderLine() -> void;

//line.cpp
inline auto getPalette(uint8 index) -> uint16;
inline auto getDirectColor(uint8 p, uint8 t) -> uint16;
inline auto get_pixel_normal(uint32 x) -> uint16;
inline auto get_pixel_swap(uint32 x) -> uint16;
auto renderLine_output() -> void;
auto clearLine() -> void;
