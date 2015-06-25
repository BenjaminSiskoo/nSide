rem For use with Windows
@echo off
mkdir "obj"

rem Accuracy profile
mkdir "obj\accuracy"
move "obj\accuracy\sfc-*.o" "obj"
move "obj\accuracy\gba-*.o" "obj"
move "obj\accuracy\ui-ethos.o" "obj"
move "obj\accuracy\ui-settings.o" "obj"
mingw32-make -j6 profile=accuracy console=true
move "obj\sfc-*.o" "obj\accuracy"
move "obj\gba-*.o" "obj\accuracy"
move "obj\ui-ethos.o" "obj\accuracy"
move "obj\ui-settings.o" "obj\accuracy"
if not exist "out\nSide-e.exe" (pause)

@echo on