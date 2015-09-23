rem For use with Windows
@echo off
mkdir "obj"

rem Accuracy profile
mkdir "obj\accuracy"
move "obj\accuracy\sfc-*.o" "obj"
mingw32-make -j4 profile=accuracy console=true
move "obj\sfc-*.o" "obj\accuracy"
if not exist "out\nSide-t.exe" (pause)

@echo on