ImportDialog::ImportDialog() {
  importDialog = this;

  onClose([&] {
    stopButton.setEnabled(false).setText("Stopping ...");
    abort = true;
  });
  layout.setMargin(5);
  stopButton.setText("Stop").onActivate([&] { doClose(); });

  setTitle("cart-pal - Importing ...");
  setSize({480, layout.minimumSize().height()});
  setCentered();
}

auto ImportDialog::run(string_vector locations) -> void {
  abort = false;
  errors.reset();
  unsigned position = 0;

  setVisible(true);
  for(auto& location : locations) {
    auto name = Location::base(location);

    if(abort) {
      errors.append(string{"[", name, "] aborted"});
      continue;
    }

    statusLabel.setText(name);
    double progress = 100.0 * (double)position++ / (double)locations.size() + 0.5;
    progressBar.setPosition((unsigned)progress);
    Application::processEvents();

    if(!cart_pal.import(location)) {
      errors.append(string{"[", name, "] ", cart_pal.error()});
    }
  }
  setVisible(false);

  if(errors) {
    string message{"Import completed, but with ", errors.size(), " error", errors.size() ? "s" : "", ". View log?"};
    if(MessageDialog().setTitle("cart-pal").setText(message).question() == "Yes") {
      errorDialog->show(errors.merge("\n"));
    } else {
      scanDialog->show();
    }
  } else {
    MessageDialog().setTitle("cart-pal").setText("Import completed successfully.").information();
    scanDialog->show();
  }
}
