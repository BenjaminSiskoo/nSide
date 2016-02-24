//render.cpp
inline auto render_line_mode0() -> void;
inline auto render_line_mode1() -> void;
inline auto render_line_mode2() -> void;
inline auto render_line_mode3() -> void;
inline auto render_line_mode4() -> void;
inline auto render_line_mode5() -> void;
inline auto render_line_mode6() -> void;
inline auto render_line_mode7() -> void;

//cache.cpp
enum : uint_t { COLORDEPTH_4 = 0, COLORDEPTH_16 = 1, COLORDEPTH_256 = 2 };
enum : uint_t { TILE_2BIT = 0, TILE_4BIT = 1, TILE_8BIT = 2 };

struct pixel_t {
  //bgr555 color data for main/subscreen pixels: 0x0000 = transparent / use palette color # 0
  //needs to be bgr555 instead of palette index for direct color mode ($2130 bit 0) to work
  uint16_t src_main, src_sub;
  //indicates source of palette # for main/subscreen (BG1-4, OAM, or back)
  uint8_t  bg_main,  bg_sub;
  //color_exemption -- true when bg == OAM && palette index >= 192, disables color add/sub effects
  uint8_t  ce_main,  ce_sub;
  //priority level of src_n. to set src_n,
  //the priority of the pixel must be >pri_n
  uint8_t  pri_main, pri_sub;
} pixel_cache[256];

uint8_t* bg_tiledata[3];
uint8_t* bg_tiledata_state[3];  //0 = valid, 1 = dirty

template<uint_t color_depth> auto render_bg_tile(uint16_t tile_num) -> void;
inline auto flush_pixel_cache() -> void;
auto alloc_tiledata_cache() -> void;
auto flush_tiledata_cache() -> void;
auto free_tiledata_cache() -> void;

//windows.cpp
struct window_t {
  uint8_t main[256], sub[256];
} window[6];

auto build_window_table(uint8_t bg, bool mainscreen) -> void;
auto build_window_tables(uint8_t bg) -> void;

//bg.cpp
struct {
  uint16_t tw,  th;  //tile width, height
  uint16_t mx,  my;  //screen mask x, y
  uint16_t scx, scy; //sc index offsets
} bg_info[4];
auto update_bg_info() -> void;

template<uint_t bg> auto bg_get_tile(uint16_t x, uint16_t y) -> uint16_t;
template<uint_t mode, uint_t bg, uint_t color_depth> auto render_line_bg(uint8_t pri0_pos, uint8_t pri1_pos) -> void;

//oam.cpp
struct sprite_item {
  uint8_t  width, height;
  uint16_t x, y;
  uint8_t  character;
  bool   use_nameselect;
  bool   vflip, hflip;
  uint8_t  palette;
  uint8_t  priority;
  bool   size;
} sprite_list[128];
bool sprite_list_valid;
uint_t active_sprite;

uint8_t oam_itemlist[32];
struct oam_tileitem {
  uint16_t x, y, pri, pal, tile;
  bool   hflip;
} oam_tilelist[34];

enum : uint_t { OAM_PRI_NONE = 4 };
uint8_t oam_line_pal[256], oam_line_pri[256];

auto update_sprite_list(unsigned addr, uint8_t data) -> void;
auto build_sprite_list() -> void;
auto is_sprite_on_scanline() -> bool;
auto load_oam_tiles() -> void;
auto render_oam_tile(int_t tile_num) -> void;
auto render_line_oam_rto() -> void;
auto render_line_oam(uint8_t pri0_pos, uint8_t pri1_pos, uint8_t pri2_pos, uint8_t pri3_pos) -> void;

//mode7.cpp
template<uint_t bg> auto render_line_mode7(uint8_t pri0_pos, uint8_t pri1_pos) -> void;

//addsub.cpp
inline auto addsub(uint32_t x, uint32_t y, bool halve) -> uint16_t;

//line.cpp
inline auto get_palette(uint8_t index) -> uint16_t;
inline auto get_direct_color(uint8_t p, uint8_t t) -> uint16_t;
inline auto get_pixel_normal(uint32_t x) -> uint16_t;
inline auto get_pixel_swap(uint32_t x) -> uint16_t;
auto render_line_output() -> void;
auto render_line_clear() -> void;
