SettingsDialog::SettingsDialog() {
  settingsDialog = this;

  layout.setMargin(5);
  locationLabel.setText("Library Location:");
  locationEdit.setEditable(false).setText(settings["Library/Location"].text());
  changeLocationButton.setText("Change ...").onActivate([&] {
    if(auto location = BrowserDialog().setParent(*this).setTitle("Select Library Location").selectFolder()) {
      settings["Library/Location"].setValue(location);
      locationEdit.setText(location);
    }
  });
  createManifestsOption.setText("Create Manifests (not recommended; breaks backward-compatibility)")
  .setChecked(settings["cart-pal/CreateManifests"].boolean()).onToggle([&] {
    settings["cart-pal/CreateManifests"].setValue(createManifestsOption.checked());
  });
  useDatabaseOption.setText("Use Database (highly recommended; provides bit-perfect memory mapping)")
  .setChecked(settings["cart-pal/UseDatabase"].boolean()).onToggle([&] {
    settings["cart-pal/UseDatabase"].setValue(useDatabaseOption.checked());
  });

  setTitle("cart-pal Settings");
  setSize({480, layout.minimumSize().height()});
}
