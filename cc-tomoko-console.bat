rem For use with Windows
@echo off
mkdir "obj"

rem Accuracy profile
mkdir "obj\accuracy"
move "obj\accuracy\sfc-*.o" "obj"
move "obj\accuracy\gba-*.o" "obj"
move "obj\accuracy\ui-tomoko.o" "obj"
move "obj\accuracy\ui-settings.o" "obj"
mingw32-make -j4 target=nSide-t profile=accuracy console=true
move "obj\sfc-*.o" "obj\accuracy"
move "obj\gba-*.o" "obj\accuracy"
move "obj\ui-tomoko.o" "obj\accuracy"
move "obj\ui-settings.o" "obj\accuracy"
if not exist "out\nSide-t.exe" (pause)

@echo on