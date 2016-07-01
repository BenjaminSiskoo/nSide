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
    color = SFC::ppu.screen.cgram[color];
    output.append("Color: 0x", hex(color, 4L), ", RGB: ",
      image::normalize(color >>  0 & 31, 5, 8), ",",
      image::normalize(color >>  5 & 31, 5, 8), ",",
      image::normalize(color >> 10 & 31, 5, 8)
    );
    statusBar.setText(output);
  });

  updateColors();

  setGeometry({{128, 128}, layout.minimumSize()});
}

auto PaletteViewer::updateColors() -> void {
  uint32_t* dp = canvas.data();

  uint color;

  for(uint paletteID : range(16)) {
    for(uint colorID : range(16)) {
      color = SFC::ppu.screen.cgram[paletteID << 4 | colorID];
      color = (255u << 24) |
        (image::normalize(color >>  0 & 31, 5, 8) << 16) |
        (image::normalize(color >>  5 & 31, 5, 8) <<  8) |
        (image::normalize(color >> 10 & 31, 5, 8) <<  0);
      for(uint y : range(16)) {
        for(uint x : range(16)) {
          dp[(paletteID * 16 + y) * 256 + (colorID * 16 + x)] = color;
        }
      }
    }
  }

  canvas.update();
}
