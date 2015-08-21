CheatDatabase::CheatDatabase() {
  cheatDatabase = this;

  layout.setMargin(5);
  cheatList.setCheckable();
  selectAllButton.setText("Select All").onActivate([&] { cheatList.checkAll(); });
  unselectAllButton.setText("Unselect All").onActivate([&] { cheatList.uncheckAll(); });
  addCodesButton.setText("Add Codes").onActivate([&] { addCodes(); });

  setSize({800, 400});
  setPlacement(0.5, 1.0);

  onSize([&] { cheatList.resizeColumns(); });
}

auto CheatDatabase::findCodes() -> void {
  if(!emulator) return;
  auto sha256 = emulator->sha256();

  auto contents = string::read(locate({localpath(), "tomoko/"}, "cheats.bml"));
  auto document = BML::unserialize(contents);

  for(auto cartridge : document.find("cartridge")) {
    if(cartridge["sha256"].text() != sha256) continue;

    codes.reset();
    cheatList.reset();
    cheatList.append(ListViewColumn().setExpandable());
    for(auto cheat : cartridge.find("cheat")) {
      codes.append(cheat["code"].text());
      cheatList.append(ListViewItem()
        .append(ListViewCell().setText(cheat["description"].text()))
      );
    }

    setTitle(cartridge["name"].text());
    setVisible();
    cheatList.resizeColumns();
    return;
  }

  MessageDialog().setParent(*toolsManager).setText("Sorry, no cheats were found for this game.").information();
}

auto CheatDatabase::addCodes() -> void {
  for(auto item : cheatList.checked()) {
    string code = codes(item->offset(), "");
    string description = item->cell(0)->text();
    if(toolsManager->cheatEditor.addCode(code, description) == false) {
      MessageDialog().setParent(*this).setText("Free slots exhausted. Not all codes could be added.").warning();
      break;
    }
  }
  setVisible(false);
  toolsManager->cheatEditor.doRefresh();
}
