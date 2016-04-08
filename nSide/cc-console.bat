@echo off
rem For use with Windows
mkdir "obj"

rem Accuracy profile
mingw32-make -j4 console=true
if not exist "out\nSide.exe" (pause)

@echo on