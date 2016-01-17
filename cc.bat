@echo off
rem For use with Windows
cd "nSide"

mkdir "obj"

rem Accuracy profile
mkdir "obj\accuracy"
move "obj\accuracy\sfc-*.o" "obj"
move "obj\accuracy\ui-settings.o" "obj"
mingw32-make -j4 profile=accuracy
move "obj\sfc-*.o" "obj\accuracy"
move "obj\ui-settings.o" "obj\accuracy"
if not exist "out\nSide.exe" (pause)

rem Balanced profile
mkdir "obj\balanced"
move "obj\balanced\sfc-*.o" "obj"
move "obj\balanced\ui-settings.o" "obj"
mingw32-make -j4 profile=balanced name=nSide-balanced
move "obj\sfc-*.o" "obj\balanced"
move "obj\ui-settings.o" "obj\balanced"
if not exist "out\nSide-balanced.exe" (pause)

move "out\*.exe" ".."

cd ".."
cd "cart-pal"

mingw32-make -j4
if not exist "out\cart-pal.exe" (pause)
move "out\*.exe" ".."

@echo on