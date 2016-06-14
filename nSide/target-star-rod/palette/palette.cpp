#include "../laevateinn.hpp"
unique_pointer<PaletteViewer> paletteViewer;

PaletteViewer::PaletteViewer() {
  paletteViewer = this;
  setTitle("Palette Viewer");
  statusBar.setFont(Font().setBold());
  statusBar.setVisible();

  layout.setMargin(5);
  autoUpdate.setText("Auto");
  update.setText("Update");
  canvas.setSize({256, 256});

  update.onActivate({ &PaletteViewer::updateColors, this });

  canvas.onMouseLeave([&] { statusBar.setText(""); });
  canvas.onMouseMove([&](Position position) {
    uint x = position.x() / 16, y = position.y() / 16;
    string output = { x, ", ", y, ", " };
    uint color = y << 4 | x;
    color = SuperFamicom::ppu.memory.cgram[color << 1] | SuperFamicom::ppu.memory.cgram[color << 1 | 1] << 8;
    output.append("Color: 0x", hex(color, 4L), ", RGB: ",
      image::normalize(color >>  0 & 31, 5, 8), ",",
      image::normalize(color >>  5 & 31, 5, 8), ",",
      image::normalize(color >> 10 & 31, 5, 8)
    );
    statusBar.setText(output);
  });

  updateColors();

  setGeometry({{128, 128}, layout.minimumSize()});
  windowManager->append(this, "PaletteViewer");
}

auto PaletteViewer::updateColors() -> void {
  uint32_t* dp = canvas.data();
  const uint8* sp = SuperFamicom::ppu.memory.cgram;

  for(uint paletteId : range(16)) {
    for(uint colorId : range(16)) {
      uint color = paletteId << 4 | colorId;
      color = SuperFamicom::ppu.memory.cgram[color << 1] | SuperFamicom::ppu.memory.cgram[color << 1 | 1] << 8;
      color = (255u << 24) |
        (image::normalize(color >>  0 & 31, 5, 8) << 16) |
        (image::normalize(color >>  5 & 31, 5, 8) <<  8) |
        (image::normalize(color >> 10 & 31, 5, 8) <<  0);
      for(uint y = 0; y < 16; y++) {
        for(uint x = 0; x < 16; x++) {
          dp[(paletteId * 16 + y) * 256 + (colorId * 16 + x)] = color;
        }
      }
    }
  }

  canvas.update();
}
