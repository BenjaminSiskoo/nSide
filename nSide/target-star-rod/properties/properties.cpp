#include "../laevateinn.hpp"
unique_pointer<PropertiesViewer> propertiesViewer;

PropertiesViewer::PropertiesViewer() {
  propertiesViewer = this;
  setTitle("Properties Viewer");
  setGeometry({128, 128, 300, 400});

  layout.setMargin(5);
  sourceLabel.setText("Source:");
  sourceSelection.append(ComboButtonItem().setText("PPU"));
  sourceSelection.append(ComboButtonItem().setText("DMA"));
  autoUpdate.setText("Auto");
  update.setText("Update");
  properties.setFont(Font().setFamily(Font::Mono));

  sourceSelection.onChange({ &PropertiesViewer::updateProperties, this });
  update.onActivate({ &PropertiesViewer::updateProperties, this });

  windowManager->append(this, "PropertiesViewer");
}

auto PropertiesViewer::updateProperties() -> void {
  string output;

  switch(sourceSelection.selected().offset()) {
  case 0: { //PPU
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
      "  BG1 Tile Size = ", SuperFamicom::ppu.bg1.regs.tile_size ? "16×16" : "8×8", "\n",
      "  BG2 Tile Size = ", SuperFamicom::ppu.bg2.regs.tile_size ? "16×16" : "8×8", "\n",
      "  BG3 Tile Size = ", SuperFamicom::ppu.bg3.regs.tile_size ? "16×16" : "8×8", "\n",
      "  BG4 Tile Size = ", SuperFamicom::ppu.bg4.regs.tile_size ? "16×16" : "8×8", "\n",
      "\n"
    );

    output.append("$2106  MOSAIC\n"
      "  BG1 Mosaic = ", 1u + SuperFamicom::ppu.bg1.regs.mosaic, "×", 1u + SuperFamicom::ppu.bg1.regs.mosaic, "\n",
      "  BG2 Mosaic = ", 1u + SuperFamicom::ppu.bg2.regs.mosaic, "×", 1u + SuperFamicom::ppu.bg2.regs.mosaic, "\n",
      "  BG3 Mosaic = ", 1u + SuperFamicom::ppu.bg3.regs.mosaic, "×", 1u + SuperFamicom::ppu.bg3.regs.mosaic, "\n",
      "  BG4 Mosaic = ", 1u + SuperFamicom::ppu.bg4.regs.mosaic, "×", 1u + SuperFamicom::ppu.bg4.regs.mosaic, "\n",
      "\n"
    );

    static lstring screenSizes = {"32x×32y", "64x×32y", "32x×64y", "64x×64y"};

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

    static lstring vramMappings = {"Direct", "2-bit", "4-bit", "8-bit"};
    output.append("$2115  VMAINC\n"
      "  VRAM Increment Mode = ", SuperFamicom::ppu.regs.vram_incmode ? "high" : "low", "\n",
      "  VRAM Increment Mapping = ", vramMappings[SuperFamicom::ppu.regs.vram_mapping], "\n",
      "  VRAM Increment Size = ", SuperFamicom::ppu.regs.vram_incsize, "\n",
      "\n"
    );

    output.append("$2116  VMADDL\n"
                  "$2117  VMADDH\n"
      "  VRAM Address = $", hex(SuperFamicom::ppu.regs.vram_addr << 1, 4L),
      ", $", hex(SuperFamicom::ppu.regs.vram_addr, 4L), "\n",
      "\n"
    );

    output.append("$212c  TM\n"
      "  BG1 Main = ", SuperFamicom::ppu.bg1.regs.main_enable ? "Enabled" : "Disabled", "\n",
      "  BG2 Main = ", SuperFamicom::ppu.bg2.regs.main_enable ? "Enabled" : "Disabled", "\n",
      "  BG3 Main = ", SuperFamicom::ppu.bg3.regs.main_enable ? "Enabled" : "Disabled", "\n",
      "  BG4 Main = ", SuperFamicom::ppu.bg4.regs.main_enable ? "Enabled" : "Disabled", "\n",
      "  OBJ Main = ", SuperFamicom::ppu.sprite.regs.main_enable ? "Enabled" : "Disabled", "\n",
      "\n"
    );

    output.append("$212d  TS\n"
      "  BG1 Sub = ", SuperFamicom::ppu.bg1.regs.sub_enable ? "Enabled" : "Disabled", "\n",
      "  BG2 Sub = ", SuperFamicom::ppu.bg2.regs.sub_enable ? "Enabled" : "Disabled", "\n",
      "  BG3 Sub = ", SuperFamicom::ppu.bg3.regs.sub_enable ? "Enabled" : "Disabled", "\n",
      "  BG4 Sub = ", SuperFamicom::ppu.bg4.regs.sub_enable ? "Enabled" : "Disabled", "\n",
      "  OBJ Sub = ", SuperFamicom::ppu.sprite.regs.main_enable ? "Enabled" : "Disabled", "\n",
      "\n"
    );

    output.append("$212e  TMW\n"
      "  BG1 Main Window = ", SuperFamicom::ppu.window.regs.bg1_main_enable ? "Enabled" : "Disabled", "\n",
      "  BG2 Main Window = ", SuperFamicom::ppu.window.regs.bg2_main_enable ? "Enabled" : "Disabled", "\n",
      "  BG3 Main Window = ", SuperFamicom::ppu.window.regs.bg3_main_enable ? "Enabled" : "Disabled", "\n",
      "  BG4 Main Window = ", SuperFamicom::ppu.window.regs.bg4_main_enable ? "Enabled" : "Disabled", "\n",
      "  OBJ Main Window = ", SuperFamicom::ppu.window.regs.oam_main_enable ? "Enabled" : "Disabled", "\n",
      "\n"
    );

    output.append("$212f  TSW\n"
      "  BG1 Sub Window = ", SuperFamicom::ppu.window.regs.bg1_sub_enable ? "Enabled" : "Disabled", "\n",
      "  BG2 Sub Window = ", SuperFamicom::ppu.window.regs.bg2_sub_enable ? "Enabled" : "Disabled", "\n",
      "  BG3 Sub Window = ", SuperFamicom::ppu.window.regs.bg3_sub_enable ? "Enabled" : "Disabled", "\n",
      "  BG4 Sub Window = ", SuperFamicom::ppu.window.regs.bg4_sub_enable ? "Enabled" : "Disabled", "\n",
      "  OBJ Sub Window = ", SuperFamicom::ppu.window.regs.oam_sub_enable ? "Enabled" : "Disabled", "\n",
      "\n"
    );

    break;
  }
  
  case 1: { //DMA
    for(uint c : range(8)) {
      output.append("$43", c, "0  DMAP", c, "\n"
        "  Direction        = ", SuperFamicom::cpu.channel[c].direction ? "decrement" : "increment", "\n",
        "  (HDMA) Indirect  = ", SuperFamicom::cpu.channel[c].indirect,         "\n",
        "  Reverse Transfer = ", SuperFamicom::cpu.channel[c].reverse_transfer, "\n",
        "  Fixed Transfer   = ", SuperFamicom::cpu.channel[c].fixed_transfer,   "\n",
        "  Transfer Mode    = ", SuperFamicom::cpu.channel[c].transfer_mode,    "\n",
        "\n"
      );

      output.append("$43", c, "1  DDBAD", c, "\n"
        "  Bus B Address = $", hex(SuperFamicom::cpu.channel[c].dest_addr, 2L), "\n",
        "\n"
      );

      output.append("$43", c, "2  A1T", c, "L\n"
                    "$43", c, "3  A1T", c, "H\n"
        "  Bus A Address = $", hex(SuperFamicom::cpu.channel[c].source_addr, 4L), "\n",
        "\n"
      );

      output.append("$43", c, "4  A1B", c, "\n"
        "  Bus A Bank = $", hex(SuperFamicom::cpu.channel[c].source_bank, 2L), "\n",
        "\n"
      );

      output.append("$43", c, "5  DAS", c, "L\n"
                    "$43", c, "6  DAS", c, "H\n"
        "  (DMA) Transfer Size     = $", hex((SuperFamicom::cpu.channel[c].transfer_size - 1) % 65536 + 1, 4L), "\n",
        "  (HDMA) Indirect Address = $", hex( SuperFamicom::cpu.channel[c].indirect_addr,                  4L), "\n",
        "\n"
      );

      output.append("$43", c, "7  DASB", c, "\n"
        "  (HDMA) Indirect Bank = $", hex(SuperFamicom::cpu.channel[c].indirect_bank, 2L), "\n",
        "\n"
      );
    }

    break;
  }

  }

  properties.setText(output);
}
