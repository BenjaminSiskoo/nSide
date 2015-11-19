CPURegisterEditor* cpuRegisterEditor = nullptr;

CPURegisterEditor::CPURegisterEditor() {
  cpuRegisterEditor = this;
  setTitle("CPU Register Editor");

  layout.setMargin(5);
  regALabel.setText("A:");
  regAValue.setFont(Font::monospace(8));
  regXLabel.setText("X:");
  regXValue.setFont(Font::monospace(8));
  regYLabel.setText("Y:");
  regYValue.setFont(Font::monospace(8));
  regSLabel.setText("S:");
  regSValue.setFont(Font::monospace(8));
  regDLabel.setText("D:");
  regDValue.setFont(Font::monospace(8));
  regDBLabel.setText("DB:");
  regDBValue.setFont(Font::monospace(8));
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

  update.onActivate = [&] {
    saveRegisters();
    cpuDebugger->updateDisassembly();
    setVisible(false);
  };

  setGeometry({{128, 128}, layout.minimumGeometry().size()});
  windowManager->append(this, "CPURegisterEditor");
}

void CPURegisterEditor::loadRegisters() {
  regAValue.setText(hex<4>(SuperFamicom::cpu.regs.a));
  regXValue.setText(hex<4>(SuperFamicom::cpu.regs.x));
  regYValue.setText(hex<4>(SuperFamicom::cpu.regs.y));
  regSValue.setText(hex<4>(SuperFamicom::cpu.regs.s));
  regDValue.setText(hex<4>(SuperFamicom::cpu.regs.d));
  regDBValue.setText(hex<2>(SuperFamicom::cpu.regs.db));
  flagN.setChecked(SuperFamicom::cpu.regs.p.n);
  flagV.setChecked(SuperFamicom::cpu.regs.p.v);
  flagM.setChecked(SuperFamicom::cpu.regs.p.m);
  flagX.setChecked(SuperFamicom::cpu.regs.p.x);
  flagD.setChecked(SuperFamicom::cpu.regs.p.d);
  flagI.setChecked(SuperFamicom::cpu.regs.p.i);
  flagZ.setChecked(SuperFamicom::cpu.regs.p.z);
  flagC.setChecked(SuperFamicom::cpu.regs.p.c);
  flagE.setChecked(SuperFamicom::cpu.regs.e);
}

void CPURegisterEditor::saveRegisters() {
  SuperFamicom::cpu.regs.a = hex(regAValue.text());
  SuperFamicom::cpu.regs.x = hex(regXValue.text());
  SuperFamicom::cpu.regs.y = hex(regYValue.text());
  SuperFamicom::cpu.regs.s = hex(regSValue.text());
  SuperFamicom::cpu.regs.d = hex(regDValue.text());
  SuperFamicom::cpu.regs.db = hex(regDBValue.text());
  SuperFamicom::cpu.regs.p.n = flagN.checked();
  SuperFamicom::cpu.regs.p.v = flagV.checked();
  SuperFamicom::cpu.regs.p.m = flagM.checked();
  SuperFamicom::cpu.regs.p.x = flagX.checked();
  SuperFamicom::cpu.regs.p.d = flagD.checked();
  SuperFamicom::cpu.regs.p.i = flagI.checked();
  SuperFamicom::cpu.regs.p.z = flagZ.checked();
  SuperFamicom::cpu.regs.p.c = flagC.checked();
  SuperFamicom::cpu.regs.e = flagE.checked();
  SuperFamicom::cpu.update_table();  //cache E/M/X flags
}
