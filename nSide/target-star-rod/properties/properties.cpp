#include "../laevateinn.hpp"
unique_pointer<PropertiesViewer> propertiesViewer;

PropertiesViewer::PropertiesViewer() {
  propertiesViewer = this;
  setTitle("Properties Viewer");
  setGeometry({128, 128, 300, 400});

  layout.setMargin(5);
  sourceLabel.setText("Source:");
  sourceSelection.append(ComboButtonItem().setText("PPU"));
  sourceSelection.setEnabled(false);
  autoUpdate.setText("Auto");
  update.setText("Update");
  properties.setFont(Font().setFamily(Font::Mono));

  update.onActivate([&] {
    updateProperties();
  });

  windowManager->append(this, "PropertiesViewer");
}

auto PropertiesViewer::updateProperties() -> void {
  string output;

  output.append("$2100  INIDISP\n"
    "  Display Disable = ", SuperFamicom::ppu.regs.display_disable, "\n",
    "  Display Brightness = ", SuperFamicom::ppu.regs.display_brightness, "\n",
    "\n"
  );

  output.append("$2101  OBSEL\n"
    "  Base Size = ", SuperFamicom::ppu.sprite.regs.base_size, "\n",
    "  Name Select = $", hex(SuperFamicom::ppu.sprite.regs.nameselect << 13, 4L), "\n",
    "  Tiledata Address = $", hex(SuperFamicom::ppu.sprite.regs.tiledata_addr, 4L), "\n",
    "\n"
  );

  output.append("$2102  OAMADDL\n"
                "$2103  OAMADDH\n"
    "  Base Address = $", hex(SuperFamicom::ppu.regs.oam_baseaddr, 4L), "\n",
    "  Priority = ", SuperFamicom::ppu.regs.oam_priority, "\n",
    "\n"
  );

  output.append("$2105  BGMODE\n"
    "  Mode = ", SuperFamicom::ppu.regs.bgmode, "\n",
    "  BG3 Priority = ", SuperFamicom::ppu.regs.bg3_priority, "\n",
    "  BG1 Tile Size = ", SuperFamicom::ppu.bg1.regs.tile_size ? "16x16" : "8x8", "\n",
    "  BG2 Tile Size = ", SuperFamicom::ppu.bg2.regs.tile_size ? "16x16" : "8x8", "\n",
    "  BG3 Tile Size = ", SuperFamicom::ppu.bg3.regs.tile_size ? "16x16" : "8x8", "\n",
    "  BG4 Tile Size = ", SuperFamicom::ppu.bg4.regs.tile_size ? "16x16" : "8x8", "\n",
    "\n"
  );

  output.append("$2106  MOSAIC\n"
    "  BG1 Mosaic = ", 1u + SuperFamicom::ppu.bg1.regs.mosaic, "x", 1u + SuperFamicom::ppu.bg1.regs.mosaic, "\n",
    "  BG2 Mosaic = ", 1u + SuperFamicom::ppu.bg2.regs.mosaic, "x", 1u + SuperFamicom::ppu.bg2.regs.mosaic, "\n",
    "  BG3 Mosaic = ", 1u + SuperFamicom::ppu.bg3.regs.mosaic, "x", 1u + SuperFamicom::ppu.bg3.regs.mosaic, "\n",
    "  BG4 Mosaic = ", 1u + SuperFamicom::ppu.bg4.regs.mosaic, "x", 1u + SuperFamicom::ppu.bg4.regs.mosaic, "\n",
    "\n"
  );

  lstring screenSizes = { "32x×32y", "64x×32y", "32x×64y", "64x×64y" };

  output.append("$2107  BG1SC\n"
    "  BG1 Screen Address = $", hex(SuperFamicom::ppu.bg1.regs.screen_addr, 4L), "\n",
    "  BG1 Screen Size = ", screenSizes[SuperFamicom::ppu.bg1.regs.screen_size], "\n",
    "\n"
  );

  output.append("$2108  BG2SC\n"
    "  BG2 Screen Address = $", hex(SuperFamicom::ppu.bg2.regs.screen_addr, 4L), "\n",
    "  BG2 Screen Size = ", screenSizes[SuperFamicom::ppu.bg2.regs.screen_size], "\n",
    "\n"
  );

  output.append("$2109  BG3SC\n"
    "  BG3 Screen Address = $", hex(SuperFamicom::ppu.bg3.regs.screen_addr, 4L), "\n",
    "  BG3 Screen Size = ", screenSizes[SuperFamicom::ppu.bg3.regs.screen_size], "\n",
    "\n"
  );

  output.append("$210a  BG4SC\n"
    "  BG4 Screen Address = $", hex(SuperFamicom::ppu.bg4.regs.screen_addr, 4L), "\n",
    "  BG4 Screen Size = ", screenSizes[SuperFamicom::ppu.bg4.regs.screen_size], "\n",
    "\n"
  );

  output.append("$210b  BG12NBA\n"
    "  BG1 Tiledata Address = $", hex(SuperFamicom::ppu.bg1.regs.tiledata_addr, 4L), "\n",
    "  BG2 Tiledata Address = $", hex(SuperFamicom::ppu.bg2.regs.tiledata_addr, 4L), "\n",
    "\n"
  );

  output.append("$210c  BG34NBA\n"
    "  BG3 Tiledata Address = $", hex(SuperFamicom::ppu.bg3.regs.tiledata_addr, 4L), "\n",
    "  BG4 Tiledata Address = $", hex(SuperFamicom::ppu.bg4.regs.tiledata_addr, 4L), "\n",
    "\n"
  );

  properties.setText(output);
}
