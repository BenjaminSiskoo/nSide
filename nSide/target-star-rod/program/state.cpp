auto Program::loadState(uint slot) -> bool {
  string filename = {mediumPaths(1), "state-", slot, ".bst"};
  auto memory = file::read(filename);
  if(memory.size() == 0) return debugger->print("Slot ", slot, " does not exist\n"), false;
  serializer s(memory.data(), memory.size());
  bool result = SFC::system.unserialize(s);
  if(result) debugger->print("Loaded state from ", filename, "\n");
  return result;
}

auto Program::saveState(uint slot) -> bool {
  SFC::system.runToSave();
  serializer s = SFC::system.serialize();
  string filename = {mediumPaths(1), "state-", slot, ".bst"};
  bool result = file::write(filename, s.data(), s.size());
  if(result) debugger->print("Saved state to ", filename, "\n");
  return result;
}
