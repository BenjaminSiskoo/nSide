@echo off
rem For use with Windows

rem Accuracy profile
mingw32-make -j4 profile=accuracy
if not exist "out\nSide.exe" (pause)

rem Balanced profile
mingw32-make -j4 profile=balanced name=nSide-balanced
if not exist "out\nSide-balanced.exe" (pause)

@echo on