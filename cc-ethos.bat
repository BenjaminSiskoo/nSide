rem For use with Windows
@echo off
mkdir "obj"
mkdir "obj\x64"
mkdir "obj\x86"

rem Accuracy profile
mkdir "obj\accuracy"
move "obj\accuracy\sfc-*.o" "obj"
move "obj\accuracy\gba-*.o" "obj"
move "obj\accuracy\ui-ethos.o" "obj"
move "obj\accuracy\ui-settings.o" "obj"
mingw32-make -j4 target=nSide-e profile=accuracy
move "obj\sfc-*.o" "obj\accuracy"
move "obj\gba-*.o" "obj\accuracy"
move "obj\ui-ethos.o" "obj\accuracy"
move "obj\ui-settings.o" "obj\accuracy"
if not exist "out\nSide-e.exe" (pause)

rem Balanced profile
mkdir "obj\balanced"
move "obj\balanced\sfc-*.o" "obj"
move "obj\balanced\gba-*.o" "obj"
move "obj\balanced\ui-ethos.o" "obj"
move "obj\balanced\ui-settings.o" "obj"
mingw32-make -j4 target=nSide-e profile=balanced name=nSide-e-balanced
move "obj\sfc-*.o" "obj\balanced"
move "obj\gba-*.o" "obj\balanced"
move "obj\ui-ethos.o" "obj\balanced"
move "obj\ui-settings.o" "obj\balanced"
if not exist "out\nSide-e-balanced.exe" (pause)

@echo on