//#include "../base.hpp"
AboutWindow* aboutWindow = nullptr;

AboutWindow::AboutWindow() {
  aboutWindow = this;
  setTitle("About Laevateinn");
  setResizable(false);

  layout.setMargin(10);
  layout.setAlignment(0.5);
  canvas.setSize({288, 360});
  title.setFont(Font::sans(16, "Bold"));
  title.setText("Laevateinn");
  version.setFont(Font::sans(8, "Bold"));
  version.setText({"bsnes/debugger v", Emulator::Version});
  website.setFont(Font::sans(8, "Bold"));
  website.setText("http://byuu.org/");

  layout.append(canvas, {288, 360});
  layout.append(title, {0, 0});
  layout.append(version, {0, 0});
  layout.append(website, {0, 0});
  append(layout);

  image logo(laevateinnLogo, sizeof laevateinnLogo);
  logo.alphaBlend(backgroundColor().rgb());
  canvas.setImage(logo);

  setGeometry({128, 128, layout.minimumSize().width, layout.minimumSize().height});
  windowManager->append(this, "AboutWindow");
}
