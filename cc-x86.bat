@echo off
mkdir "obj"
mkdir "obj\x64"
mkdir "obj\x86"
move "obj\*.o" "obj\x64"
move "obj\x86\*.o" "obj"

rem Balanced-x86 profile
mkdir "obj\balanced-x86"
move "obj\balanced-x86\sfc-*.o" "obj"
move "obj\balanced-x86\gba-*.o" "obj"
move "obj\balanced-x86\ui-ethos.o" "obj"
move "obj\balanced-x86\ui-settings.o" "obj"
mingw32-make -j8 profile=balanced arch=x86 name=nSide-balanced-x86
move "obj\sfc-*.o" "obj\balanced-x86"
move "obj\gba-*.o" "obj\balanced-x86"
move "obj\ui-ethos.o" "obj\balanced-x86"
move "obj\ui-settings.o" "obj\balanced-x86"
if not exist "out\nSide-balanced-x86.exe" (pause)

rem Performance profile
mkdir "obj\performance"
move "obj\performance\sfc-*.o" "obj"
move "obj\balanced-x86\gba-*.o" "obj"
move "obj\performance\ui-ethos.o" "obj"
move "obj\performance\ui-settings.o" "obj"
mingw32-make -j8 profile=performance arch=x86 name=nSide-performance
move "obj\sfc-*.o" "obj\performance"
move "obj\gba-*.o" "obj\balanced-x86"
move "obj\ui-ethos.o" "obj\performance"
move "obj\ui-settings.o" "obj\performance"
if not exist "out\nSide-performance.exe" (pause)

move "obj\*.o" "obj\x86"
move "obj\x64\*.o" "obj"

@echo on