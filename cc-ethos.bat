rem For use with Windows
@echo off
mkdir "obj"

rem Accuracy profile
mkdir "obj\accuracy"
move "obj\accuracy\sfc-*.o" "obj"
move "obj\accuracy\ethos-ethos.o" "obj"
move "obj\accuracy\ethos-settings.o" "obj"
mingw32-make -j4 target=nSide-e profile=accuracy
move "obj\sfc-*.o" "obj\accuracy"
move "obj\ethos-ethos.o" "obj\accuracy"
move "obj\ethos-settings.o" "obj\accuracy"
if not exist "out\nSide-e.exe" (pause)

rem Balanced profile
mkdir "obj\balanced"
move "obj\balanced\sfc-*.o" "obj"
move "obj\balanced\ethos-ethos.o" "obj"
move "obj\balanced\ethos-settings.o" "obj"
mingw32-make -j4 target=nSide-e profile=balanced name=nSide-e-balanced
move "obj\sfc-*.o" "obj\balanced"
move "obj\ethos-ethos.o" "obj\balanced"
move "obj\ethos-settings.o" "obj\balanced"
if not exist "out\nSide-e-balanced.exe" (pause)

@echo on