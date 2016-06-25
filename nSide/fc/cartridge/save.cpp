auto Cartridge::saveCartridge(Markup::Node node) -> void {
  auto boardNode = node["board"];

  if(auto node = boardNode["prg/ram"]) saveMemory(board->prgram, node);
  if(auto node = boardNode["chr/ram"]) saveMemory(board->chrram, node);
  if(board->chip) {
    if(auto node = boardNode["chip/ram"]) saveMemory(board->chip->ram, node);
  }
}

//

auto Cartridge::saveMemory(MappedRAM& memory, Markup::Node node, maybe<uint> id) -> void {
  if(!id) id = pathID();
  if(!node || node["volatile"]) return;
  auto name = node["name"].text();
  auto size = node["size"].natural();
  if(auto fp = interface->open(id(), name, File::Write)) {
    fp->write(memory.data(), memory.size());
  }
}
