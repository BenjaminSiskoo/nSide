rem For use with Windows
@echo off
mkdir "obj"

rem Accuracy profile
mkdir "obj\accuracy"
move "obj\accuracy\sfc-*.o" "obj"
mingw32-make -j4 target=nSide-t profile=accuracy
move "obj\sfc-*.o" "obj\accuracy"
if not exist "out\nSide-t.exe" (pause)

rem Balanced profile
mkdir "obj\balanced"
move "obj\balanced\sfc-*.o" "obj"
mingw32-make -j4 target=nSide-t profile=balanced name=nSide-t-balanced
move "obj\sfc-*.o" "obj\balanced"
if not exist "out\nSide-t-balanced.exe" (pause)

@echo on