#include "../laevateinn.hpp"
unique_pointer<MemoryEditor> memoryEditor;

MemoryEditor::MemoryEditor() {
  memoryEditor = this;
  setTitle("Memory Editor");
  setGeometry({128, 128, 585, 235});

  gotoLabel.setText("Goto:");
  gotoAddress.setFont(Font().setFamily(Font::Mono));
  source.append(ComboButtonItem().setText("CPU-Bus"));
  source.append(ComboButtonItem().setText("APU-Bus"));
  source.append(ComboButtonItem().setText("VRAM"));
  source.append(ComboButtonItem().setText("OAM"));
  source.append(ComboButtonItem().setText("CGRAM"));
  exportMemory.setText("Export");
  autoUpdate.setText("Auto");
  update.setText("Update");
  editor.setFont(Font().setFamily(Font::Mono));
  editor.setColumns(16);
  editor.setRows(16);

  layout.setMargin(5);

  gotoAddress.onChange([&] {
    editor.setAddress(hex(gotoAddress.text()));
    editor.update();
  });

  gotoAddress.onActivate([&] {
    gotoAddress.doChange();
  });

  update.onActivate({ &MemoryEditor::updateView, this });

  source.onChange({ &MemoryEditor::selectSource, this });
  exportMemory.onActivate({ &MemoryEditor::exportMemoryToDisk, this });
  editor.onRead({ &MemoryEditor::read, this });
  editor.onWrite({ &MemoryEditor::write, this });
}

auto MemoryEditor::read(uint addr) -> uint8_t {
  if(!SFC::system.loaded()) return 0x00;
  switch(source.selected().offset()) {
  case CPU:   return cpuDebugger->read(addr);
  case APU:   return smpDebugger->read(addr);
  case VRAM:  return SFC::ppu.vram[(addr & 0xffff) >> 1].byte(addr & 1);
  case OAM:   return SFC::ppu.oam[addr % 544];
  case CGRAM: return SFC::ppu.cgram[(addr & 0xff) >> 1].byte(addr & 1);
  }
  return ~0;
}

auto MemoryEditor::write(uint addr, uint8_t data) -> void {
  if(!SFC::system.loaded()) return;
  switch(source.selected().offset()) {
  case CPU:
    SFC::cartridge.rom.writeProtect(false);
    cpuDebugger->write(addr, data);
    SFC::cartridge.rom.writeProtect(true);
    break;
  case APU:
    smpDebugger->write(addr, data);
    break;
  case VRAM:
    SFC::ppu.vram[(addr & 0xffff) >> 1].byte(addr & 1) = data;
    break;
  case OAM:
    SFC::ppu.oam[addr % 544] = data;
    SFC::ppu.obj.synchronize();  //cache OAM changes internally
    break;
  case CGRAM:
    SFC::ppu.cgram[(addr & 0xff) >> 1].byte(addr & 1) = data;
    break;
  }
}

auto MemoryEditor::selectSource() -> void {
  editor.setAddress(0);
  switch(source.selected().offset()) {
  case CPU:   editor.setLength(16 * 1024 * 1024); break;
  case APU:   editor.setLength(64 * 1024); break;
  case VRAM:  editor.setLength((SFC::ppu.vram.mask + 1) << 1); break;
  case OAM:   editor.setLength(544); break;
  case CGRAM: editor.setLength(512); break;
  }
  updateView();
}

auto MemoryEditor::exportMemoryToDisk() -> void {
  string filename = {program->mediumPaths(1), "debug/"};
  switch(source.selected().offset()) {
  case CPU:   filename.append("work.ram"); break;
  case APU:   filename.append("apu.ram"); break;
  case VRAM:  filename.append("video.ram"); break;
  case OAM:   filename.append("object.ram"); break;
  case CGRAM: filename.append("palette.ram"); break;
  }
  file fp;
  if(fp.open(filename, file::mode::write) == false) return;
  switch(source.selected().offset()) {
  case CPU:   for(uint addr : range(0xffffff)) fp.write(cpuDebugger->read(addr)); break;
  case APU:   for(uint addr : range(0xffff)) fp.write(smpDebugger->read(addr)); break;
  case VRAM:  for(uint addr : range((SFC::ppu.vram.mask + 1) << 1)) fp.write(SFC::ppu.vram[addr]); break;
  case OAM:   for(uint addr : range(0x021f)) fp.write(SFC::ppu.oam[addr]); break;
  case CGRAM: for(uint addr : range(0x01ff)) fp.write(SFC::ppu.cgram[addr]); break;
  }
  debugger->print("Exported memory to ", filename, "\n");
}

auto MemoryEditor::updateView() -> void {
  editor.update();
}
