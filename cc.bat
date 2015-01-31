@echo off
mkdir "obj"

rem Accuracy profile
mkdir "obj\accuracy"
move "obj\accuracy\sfc-*.o" "obj"
move "obj\accuracy\gba-*.o" "obj"
move "obj\accuracy\ui-ethos.o" "obj"
move "obj\accuracy\ui-settings.o" "obj"
mingw32-make -j8 profile=accuracy
move "obj\sfc-*.o" "obj\accuracy"
move "obj\gba-*.o" "obj\accuracy"
move "obj\ui-ethos.o" "obj\accuracy"
move "obj\ui-settings.o" "obj\accuracy"
if not exist "out\nSide.exe" (pause)

rem Balanced profile
mkdir "obj\balanced"
move "obj\balanced\sfc-*.o" "obj"
move "obj\balanced\gba-*.o" "obj"
move "obj\balanced\ui-ethos.o" "obj"
move "obj\balanced\ui-settings.o" "obj"
mingw32-make -j8 profile=balanced name=nSide-balanced
move "obj\sfc-*.o" "obj\balanced"
move "obj\gba-*.o" "obj\balanced"
move "obj\ui-ethos.o" "obj\balanced"
move "obj\ui-settings.o" "obj\balanced"
if not exist "out\nSide-balanced.exe" (pause)

@echo on