AboutWindow::AboutWindow() {
  aboutWindow = this;

  setTitle("About nSide ...");
  setBackgroundColor({255, 255, 240});
  layout.setMargin(10);
  auto logo = image{Resource::Logo::nSide};
  logo.alphaBlend(0xfffff0);
  canvas.setIcon(logo);
  informationLeft.setFont(Font().setBold()).setAlignment(1.0).setText({
    "Version:\n",
    "Based on:\n",
    "Fork Author:\n",
    "License:\n",
    "Website of ", Emulator::OriginalName, ":"
  });
  informationRight.setFont(Font().setBold()).setAlignment(0.0).setText({
    Emulator::Version, "\n",
    Emulator::OriginalName, " ", Emulator::FromVersion, " by ", Emulator::OriginalAuthor, "\n",
    Emulator::Author, "\n",
    Emulator::License, "\n",
    Emulator::Website
  });
  informationBottom.setFont(Font().setBold()).setAlignment(0.0).setText({
    "Contributors to ", Emulator::OriginalName, ":\n",
    Emulator::Contributors.merge("\n")
  });

  setResizable(false);
  setSize(layout.minimumSize());
  setCentered();
}
