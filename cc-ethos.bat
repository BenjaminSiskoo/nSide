rem For use with Windows
@echo off
mkdir "obj"

rem Accuracy profile
mkdir "obj\accuracy"
move "obj\accuracy\sfc-*.o" "obj"
mingw32-make -j4 target=nSide-e profile=accuracy
move "obj\sfc-*.o" "obj\accuracy"
if not exist "out\nSide-e.exe" (pause)

@echo on