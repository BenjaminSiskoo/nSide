SuperGrafxInterface::SuperGrafxInterface() {
  information.devState     = DevState::Alpha;
  information.manufacturer = "NEC";
  information.name         = "SuperGrafx";

  media.append({ID::SuperGrafx, "SuperGrafx", "sg"});
}

auto SuperGrafxInterface::load(uint id) -> bool {
  if(id == ID::SuperGrafx) return system.load(this, System::Model::SuperGrafx);
  return false;
}
