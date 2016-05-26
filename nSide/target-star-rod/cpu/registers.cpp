unique_pointer<CPURegisterEditor> cpuRegisterEditor;

CPURegisterEditor::CPURegisterEditor() {
  cpuRegisterEditor = this;
  setTitle("CPU Register Editor");

  layout.setMargin(5);
  regALabel.setText("A:");
  regAValue.setFont(Font().setFamily(Font::Mono));
  regXLabel.setText("X:");
  regXValue.setFont(Font().setFamily(Font::Mono));
  regYLabel.setText("Y:");
  regYValue.setFont(Font().setFamily(Font::Mono));
  regSLabel.setText("S:");
  regSValue.setFont(Font().setFamily(Font::Mono));
  regDLabel.setText("D:");
  regDValue.setFont(Font().setFamily(Font::Mono));
  regDBLabel.setText("DB:");
  regDBValue.setFont(Font().setFamily(Font::Mono));
  flagN.setText("N");
  flagV.setText("V");
  flagM.setText("M");
  flagX.setText("X");
  flagD.setText("D");
  flagI.setText("I");
  flagZ.setText("Z");
  flagC.setText("C");
  flagE.setText("E");
  update.setText("Update");

  loadRegisters();

  layout.append(primaryLayout, {~0, 0}, 5);
    primaryLayout.append(regALabel, {0, 0}, 5);
    primaryLayout.append(regAValue, {0, 0}, 5);
    primaryLayout.append(regXLabel, {0, 0}, 5);
    primaryLayout.append(regXValue, {0, 0}, 5);
    primaryLayout.append(regYLabel, {0, 0}, 5);
    primaryLayout.append(regYValue, {0, 0}, 5);
    primaryLayout.append(regSLabel, {0, 0}, 5);
    primaryLayout.append(regSValue, {0, 0}, 5);
    primaryLayout.append(regDLabel, {0, 0}, 5);
    primaryLayout.append(regDValue, {0, 0}, 5);
    primaryLayout.append(regDBLabel, {0, 0}, 5);
    primaryLayout.append(regDBValue, {0, 0});
  layout.append(secondaryLayout, {~0, 0}, 5);
    secondaryLayout.append(flagN, {0, 0}, 5);
    secondaryLayout.append(flagV, {0, 0}, 5);
    secondaryLayout.append(flagM, {0, 0}, 5);
    secondaryLayout.append(flagX, {0, 0}, 5);
    secondaryLayout.append(flagD, {0, 0}, 5);
    secondaryLayout.append(flagI, {0, 0}, 5);
    secondaryLayout.append(flagZ, {0, 0}, 5);
    secondaryLayout.append(flagC, {0, 0});
  layout.append(tertiaryLayout, {~0, 0});
    tertiaryLayout.append(flagE, {0, 0}, 5);
    tertiaryLayout.append(spacer, {~0, 0});
    tertiaryLayout.append(update, {80, 0});
  append(layout);

  update.onActivate([&] {
    saveRegisters();
    cpuDebugger->updateDisassembly();
    setVisible(false);
  });

  setGeometry({{128, 128}, layout.minimumSize()});
  windowManager->append(this, "CPURegisterEditor");
}

void CPURegisterEditor::loadRegisters() {
  regAValue.setText(hex(SuperFamicom::cpu.r.a, 4L));
  regXValue.setText(hex(SuperFamicom::cpu.r.x, 4L));
  regYValue.setText(hex(SuperFamicom::cpu.r.y, 4L));
  regSValue.setText(hex(SuperFamicom::cpu.r.s, 4L));
  regDValue.setText(hex(SuperFamicom::cpu.r.d, 4L));
  regDBValue.setText(hex(SuperFamicom::cpu.r.db, 2L));
  flagN.setChecked(SuperFamicom::cpu.r.p.n);
  flagV.setChecked(SuperFamicom::cpu.r.p.v);
  flagM.setChecked(SuperFamicom::cpu.r.p.m);
  flagX.setChecked(SuperFamicom::cpu.r.p.x);
  flagD.setChecked(SuperFamicom::cpu.r.p.d);
  flagI.setChecked(SuperFamicom::cpu.r.p.i);
  flagZ.setChecked(SuperFamicom::cpu.r.p.z);
  flagC.setChecked(SuperFamicom::cpu.r.p.c);
  flagE.setChecked(SuperFamicom::cpu.r.e);
}

void CPURegisterEditor::saveRegisters() {
  SuperFamicom::cpu.r.a = hex(regAValue.text());
  SuperFamicom::cpu.r.x = hex(regXValue.text());
  SuperFamicom::cpu.r.y = hex(regYValue.text());
  SuperFamicom::cpu.r.s = hex(regSValue.text());
  SuperFamicom::cpu.r.d = hex(regDValue.text());
  SuperFamicom::cpu.r.db = hex(regDBValue.text());
  SuperFamicom::cpu.r.p.n = flagN.checked();
  SuperFamicom::cpu.r.p.v = flagV.checked();
  SuperFamicom::cpu.r.p.m = flagM.checked();
  SuperFamicom::cpu.r.p.x = flagX.checked();
  SuperFamicom::cpu.r.p.d = flagD.checked();
  SuperFamicom::cpu.r.p.i = flagI.checked();
  SuperFamicom::cpu.r.p.z = flagZ.checked();
  SuperFamicom::cpu.r.p.c = flagC.checked();
  SuperFamicom::cpu.r.e = flagE.checked();
  //SuperFamicom::cpu.update_table();  //cache E/M/X flags
}
