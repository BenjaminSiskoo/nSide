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
      "  Display Brightness = ", SuperFamicom::ppu.r.displayBrightness, "\n",
      "  Display Disable = ", SuperFamicom::ppu.r.displayDisable, "\n",
      "\n"
    );

    output.append("$2101  OBSEL\n"
      "  OAM Tiledata Address = $", hex(SuperFamicom::ppu.oam.r.tiledataAddress, 4L), "\n",
      "  OAM Name Select = $", hex(SuperFamicom::ppu.oam.r.nameSelect << 13, 4L), "\n",
      "  OAM Base Size = ", SuperFamicom::ppu.oam.r.baseSize, "\n",
      "\n"
    );

    output.append("$2102  OAMADDL\n"
                  "$2103  OAMADDH\n"
      "  OAM Priority = ", SuperFamicom::ppu.r.oamPriority, "\n",
      "  OAM Base Address = $", hex(SuperFamicom::ppu.r.oamBaseAddress, 4L), "\n",
      "\n"
    );

    output.append("$2105  BGMODE\n"
      "  BG Mode = ", SuperFamicom::ppu.r.bgMode, "\n",
      "  BG Priority = ", SuperFamicom::ppu.r.bgPriority, "\n",
      "  BG1 Tile Size = ", SuperFamicom::ppu.bg1.r.tileSize ? "16×16" : "8×8", "\n",
      "  BG2 Tile Size = ", SuperFamicom::ppu.bg2.r.tileSize ? "16×16" : "8×8", "\n",
      "  BG3 Tile Size = ", SuperFamicom::ppu.bg3.r.tileSize ? "16×16" : "8×8", "\n",
      "  BG4 Tile Size = ", SuperFamicom::ppu.bg4.r.tileSize ? "16×16" : "8×8", "\n",
      "\n"
    );

    output.append("$2106  MOSAIC\n"
      "  BG1 Mosaic = ", 1u + SuperFamicom::ppu.bg1.r.mosaic, "×", 1u + SuperFamicom::ppu.bg1.r.mosaic, "\n",
      "  BG2 Mosaic = ", 1u + SuperFamicom::ppu.bg2.r.mosaic, "×", 1u + SuperFamicom::ppu.bg2.r.mosaic, "\n",
      "  BG3 Mosaic = ", 1u + SuperFamicom::ppu.bg3.r.mosaic, "×", 1u + SuperFamicom::ppu.bg3.r.mosaic, "\n",
      "  BG4 Mosaic = ", 1u + SuperFamicom::ppu.bg4.r.mosaic, "×", 1u + SuperFamicom::ppu.bg4.r.mosaic, "\n",
      "\n"
    );

    static lstring screenSizes = {"32x×32y", "64x×32y", "32x×64y", "64x×64y"};

    output.append("$2107  BG1SC\n"
      "  BG1 Screen Size = ", screenSizes[SuperFamicom::ppu.bg1.r.screenSize], "\n",
      "  BG1 Screen Address = $", hex(SuperFamicom::ppu.bg1.r.screenAddress, 4L), "\n",
      "\n"
    );

    output.append("$2108  BG2SC\n"
      "  BG2 Screen Size = ", screenSizes[SuperFamicom::ppu.bg2.r.screenSize], "\n",
      "  BG2 Screen Address = $", hex(SuperFamicom::ppu.bg2.r.screenAddress, 4L), "\n",
      "\n"
    );

    output.append("$2109  BG3SC\n"
      "  BG3 Screen Size = ", screenSizes[SuperFamicom::ppu.bg3.r.screenSize], "\n",
      "  BG3 Screen Address = $", hex(SuperFamicom::ppu.bg3.r.screenAddress, 4L), "\n",
      "\n"
    );

    output.append("$210a  BG4SC\n"
      "  BG4 Screen Size = ", screenSizes[SuperFamicom::ppu.bg4.r.screenSize], "\n",
      "  BG4 Screen Address = $", hex(SuperFamicom::ppu.bg4.r.screenAddress, 4L), "\n",
      "\n"
    );

    output.append("$210b  BG12NBA\n"
      "  BG1 Tiledata Address = $", hex(SuperFamicom::ppu.bg1.r.tiledataAddress, 4L), "\n",
      "  BG2 Tiledata Address = $", hex(SuperFamicom::ppu.bg2.r.tiledataAddress, 4L), "\n",
      "\n"
    );

    output.append("$210c  BG34NBA\n"
      "  BG3 Tiledata Address = $", hex(SuperFamicom::ppu.bg3.r.tiledataAddress, 4L), "\n",
      "  BG4 Tiledata Address = $", hex(SuperFamicom::ppu.bg4.r.tiledataAddress, 4L), "\n",
      "\n"
    );

    static lstring vramMappings = {"Direct", "2-bit", "4-bit", "8-bit"};
    output.append("$2115  VMAINC\n"
      "  VRAM Increment Mode = ", SuperFamicom::ppu.r.vramIncrementMode ? "high" : "low", "\n",
      "  VRAM Mapping        = ", vramMappings[SuperFamicom::ppu.r.vramMapping], "\n",
      "  VRAM Increment Size = ", SuperFamicom::ppu.r.vramIncrementSize, "\n",
      "\n"
    );

    output.append("$2116  VMADDL\n"
                  "$2117  VMADDH\n"
      "  VRAM Address = $", hex(SuperFamicom::ppu.r.vramAddress << 1, 4L),
      " ($", hex(SuperFamicom::ppu.r.vramAddress, 4L), ")\n",
      "\n"
    );

    output.append("$212c  TM\n"
      "  BG1 Above = ", SuperFamicom::ppu.bg1.r.aboveEnable ? "Enabled" : "Disabled", "\n",
      "  BG2 Above = ", SuperFamicom::ppu.bg2.r.aboveEnable ? "Enabled" : "Disabled", "\n",
      "  BG3 Above = ", SuperFamicom::ppu.bg3.r.aboveEnable ? "Enabled" : "Disabled", "\n",
      "  BG4 Above = ", SuperFamicom::ppu.bg4.r.aboveEnable ? "Enabled" : "Disabled", "\n",
      "  OAM Above = ", SuperFamicom::ppu.oam.r.aboveEnable ? "Enabled" : "Disabled", "\n",
      "\n"
    );

    output.append("$212d  TS\n"
      "  BG1 Below = ", SuperFamicom::ppu.bg1.r.belowEnable ? "Enabled" : "Disabled", "\n",
      "  BG2 Below = ", SuperFamicom::ppu.bg2.r.belowEnable ? "Enabled" : "Disabled", "\n",
      "  BG3 Below = ", SuperFamicom::ppu.bg3.r.belowEnable ? "Enabled" : "Disabled", "\n",
      "  BG4 Below = ", SuperFamicom::ppu.bg4.r.belowEnable ? "Enabled" : "Disabled", "\n",
      "  OAM Below = ", SuperFamicom::ppu.oam.r.belowEnable ? "Enabled" : "Disabled", "\n",
      "\n"
    );

    output.append("$212e  TMW\n"
      "  BG1 Above Window = ", SuperFamicom::ppu.window.r.bg1.aboveEnable ? "Enabled" : "Disabled", "\n",
      "  BG2 Above Window = ", SuperFamicom::ppu.window.r.bg2.aboveEnable ? "Enabled" : "Disabled", "\n",
      "  BG3 Above Window = ", SuperFamicom::ppu.window.r.bg3.aboveEnable ? "Enabled" : "Disabled", "\n",
      "  BG4 Above Window = ", SuperFamicom::ppu.window.r.bg4.aboveEnable ? "Enabled" : "Disabled", "\n",
      "  OAM Above Window = ", SuperFamicom::ppu.window.r.oam.aboveEnable ? "Enabled" : "Disabled", "\n",
      "\n"
    );

    output.append("$212f  TSW\n"
      "  BG1 Below Window = ", SuperFamicom::ppu.window.r.bg1.belowEnable ? "Enabled" : "Disabled", "\n",
      "  BG2 Below Window = ", SuperFamicom::ppu.window.r.bg2.belowEnable ? "Enabled" : "Disabled", "\n",
      "  BG3 Below Window = ", SuperFamicom::ppu.window.r.bg3.belowEnable ? "Enabled" : "Disabled", "\n",
      "  BG4 Below Window = ", SuperFamicom::ppu.window.r.bg4.belowEnable ? "Enabled" : "Disabled", "\n",
      "  OAM Below Window = ", SuperFamicom::ppu.window.r.oam.belowEnable ? "Enabled" : "Disabled", "\n",
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
