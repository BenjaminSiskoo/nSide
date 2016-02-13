AboutWindow* aboutWindow = nullptr;

AboutWindow::AboutWindow() {
  aboutWindow = this;
  setTitle("About star-rod");
  setResizable(false);

  layout.setMargin(10);
  layout.setAlignment(0.5);
  canvas.setSize({224, 360});
  title.setFont(Font().setBold());
  title.setText("star-rod");
  version.setFont(Font().setBold());
  version.setText({"Based on higan/laevateinn v", Emulator::FromVersion});
  website.setFont(Font().setBold());
  website.setText("http://byuu.org/");

  layout.append(canvas, {224, 360});
  layout.append(title, {0, 0});
  layout.append(version, {0, 0});
  layout.append(website, {0, 0});

  image logo(resource::star_rod);
  logo.alphaBlend(backgroundColor().value());
  canvas.setIcon(logo);

  setGeometry({128, 128, layout.minimumSize().width(), layout.minimumSize().height()});
  windowManager->append(this, "AboutWindow");
}
