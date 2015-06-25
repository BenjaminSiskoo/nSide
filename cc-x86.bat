rem For use with Windows
@echo off
mkdir "obj"

rem The below code is set to only run on 64-bit Windows.
if "%ProgramW6432%" == "%ProgramFiles%" (
  mkdir "obj\x64"
  mkdir "obj\x86"
  move "obj\*.o" "obj\x64"
  move "obj\x86\*.o" "obj"
  setlocal
  PATH=C:\MinGW32\bin
)

rem Balanced-x86 profile
mkdir "obj\balanced-x86"
move "obj\balanced-x86\sfc-*.o" "obj"
move "obj\balanced-x86\gba-*.o" "obj"
move "obj\balanced-x86\ui-ethos.o" "obj"
move "obj\balanced-x86\ui-settings.o" "obj"
mingw32-make.exe -j6 profile=balanced arch=x86 name=nSide-e-balanced-x86
move "obj\sfc-*.o" "obj\balanced-x86"
move "obj\gba-*.o" "obj\balanced-x86"
move "obj\ui-ethos.o" "obj\balanced-x86"
move "obj\ui-settings.o" "obj\balanced-x86"
if not exist "out\nSide-e-balanced-x86.exe" (pause)

rem Performance profile
mkdir "obj\performance"
move "obj\performance\sfc-*.o" "obj"
move "obj\balanced-x86\gba-*.o" "obj"
move "obj\performance\ui-ethos.o" "obj"
move "obj\performance\ui-settings.o" "obj"
mingw32-make.exe -j6 profile=performance arch=x86 name=nSide-e-performance
move "obj\sfc-*.o" "obj\performance"
move "obj\gba-*.o" "obj\balanced-x86"
move "obj\ui-ethos.o" "obj\performance"
move "obj\ui-settings.o" "obj\performance"
if not exist "out\nSide-e-performance.exe" (pause)

if "%ProgramW6432%" == "%ProgramFiles%" (
  endlocal
  move "obj\*.o" "obj\x86"
  move "obj\x64\*.o" "obj"
)

@echo on