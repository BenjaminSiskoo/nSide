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
}

void CPURegisterEditor::loadRegisters() {
  regAValue.setText(hex(SFC::cpu.r.a, 4L));
  regXValue.setText(hex(SFC::cpu.r.x, 4L));
  regYValue.setText(hex(SFC::cpu.r.y, 4L));
  regSValue.setText(hex(SFC::cpu.r.s, 4L));
  regDValue.setText(hex(SFC::cpu.r.d, 4L));
  regDBValue.setText(hex(SFC::cpu.r.db, 2L));
  flagN.setChecked(SFC::cpu.r.p.n);
  flagV.setChecked(SFC::cpu.r.p.v);
  flagM.setChecked(SFC::cpu.r.p.m);
  flagX.setChecked(SFC::cpu.r.p.x);
  flagD.setChecked(SFC::cpu.r.p.d);
  flagI.setChecked(SFC::cpu.r.p.i);
  flagZ.setChecked(SFC::cpu.r.p.z);
  flagC.setChecked(SFC::cpu.r.p.c);
  flagE.setChecked(SFC::cpu.r.e);
}

void CPURegisterEditor::saveRegisters() {
  SFC::cpu.r.a = regAValue.text().hex();
  SFC::cpu.r.x = regXValue.text().hex();
  SFC::cpu.r.y = regYValue.text().hex();
  SFC::cpu.r.s = regSValue.text().hex();
  SFC::cpu.r.d = regDValue.text().hex();
  SFC::cpu.r.db = regDBValue.text().hex();
  SFC::cpu.r.p.n = flagN.checked();
  SFC::cpu.r.p.v = flagV.checked();
  SFC::cpu.r.p.m = flagM.checked();
  SFC::cpu.r.p.x = flagX.checked();
  SFC::cpu.r.p.d = flagD.checked();
  SFC::cpu.r.p.i = flagI.checked();
  SFC::cpu.r.p.z = flagZ.checked();
  SFC::cpu.r.p.c = flagC.checked();
  SFC::cpu.r.e = flagE.checked();
  //SFC::cpu.updateTable();  //cache E/M/X flags
}
