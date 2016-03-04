@echo off
rem For use with Windows
mkdir "obj"

rem Performance profile
mkdir "obj\performance"
move "obj\performance\sfc-*.o" "obj"
move "obj\performance\ui-presentation.o" "obj"
mingw32-make.exe -j4 profile=performance name=nSide-performance
move "obj\sfc-*.o" "obj\performance"
move "obj\ui-presentation.o" "obj\performance"
if not exist "out\nSide-performance.exe" (pause)

@echo on