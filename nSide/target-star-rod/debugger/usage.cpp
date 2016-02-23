void Usage::allocate(uint size_) {
  if(data) delete[] data;
  size = size_;
  data = new uint8[size]();
}

Usage::Usage() {
  data = nullptr;
  size = 0;
}

Usage::~Usage() {
  if(data) delete[] data;
}

void Debugger::loadUsage() {
  file fp;

  //if cartridge image was modified after the usage files,
  //then it is possible that the memory map has changed.
  //will print invalidation message when files do not exist as well.

  if(file::timestamp({program->folderPaths(0), "program.rom"    }, file::time::modify) >=
     file::timestamp({program->folderPaths(0), "debug/usage.cpu"}, file::time::modify)
  ) {
    print("CPU usage invalidated\n");
  } else {
    if(fp.open({program->folderPaths(0), "debug/usage.cpu"}, file::mode::read)) {
      fp.read(cpuUsage.data, min(cpuUsage.size, fp.size()));
      fp.close();
    }
  }

  if(file::timestamp({program->folderPaths(0), "program.rom"    }, file::time::modify) >=
     file::timestamp({program->folderPaths(0), "debug/usage.apu"}, file::time::modify)
  ) {
    print("APU usage invalidated\n");
  } else {
    if(fp.open({program->folderPaths(0), "debug/usage.apu"}, file::mode::read)) {
      fp.read(apuUsage.data, min(apuUsage.size, fp.size()));
      fp.close();
    }
  }
}

void Debugger::saveUsage() {
  string filename;
  filename = {program->folderPaths(0), "debug/usage.cpu"};
  file::write(filename, cpuUsage.data, cpuUsage.size);
  filename = {program->folderPaths(0), "debug/usage.apu"};
  file::write(filename, apuUsage.data, apuUsage.size);
}

void Debugger::resetUsage() {
  if(cpuUsage.data) memset(cpuUsage.data, 0, cpuUsage.size);
  if(apuUsage.data) memset(apuUsage.data, 0, apuUsage.size);
}
