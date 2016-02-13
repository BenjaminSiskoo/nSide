#include "../laevateinn.hpp"
Presentation* presentation = nullptr;

Presentation::Presentation() {
  presentation = this;
  setTitle("Video");
  setBackgroundColor({0, 0, 0});
  setResizable(false);
  setGeometry({64, 64, 512, 480});
  statusBar.setFont(Font().setBold());
  statusBar.setVisible();

  splash.allocate(512, 480);
  splash.verticalGradient(0xff00005f, 0xff000000, 512, 480, 256, 0);
  nall::image floor;
  floor.allocate(512, 480);
  floor.radialGradient(0xffff0000, 0x00000000, 384, 240, 256, 415);
  splash.impose(image::blend::sourceAlpha, 0, 0, floor, 0, 0, floor.width(), floor.height());
  nall::image logo(resource::star_rod);
  splash.impose(image::blend::sourceAlpha, (512 - logo.width()) / 2, (480 - logo.height()) / 2, logo, 0, 0, logo.width(), logo.height());

  windowManager->append(this, "Presentation");
}

void Presentation::showSplash() {
  uint32* data;
  uint pitch;
  if(video->lock(data, pitch, 512, 480)) {
    for(uint y = 0; y < 480; y++) {
      memcpy((uint8*)data + y * pitch, splash.data() + y * splash.pitch(), 512 * sizeof(uint32));
    }
    video->unlock();
    video->refresh();
  }
}
