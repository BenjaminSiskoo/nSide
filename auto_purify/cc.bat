rem For use with Windows
mkdir "obj"
mkdir "obj\x86"
mkdir "obj\x64"
move "obj\*.o" "obj\x64"
move "obj\x86\*.o" "obj"
mingw32-make -j8 arch=x86
if not exist "auto_purify.dll" (pause)
rename "auto_purify.dll" "auto_purify-x86.dll"
move "obj\*.o" "obj\x86"
move "obj\x64\*.o" "obj"
mingw32-make -j8
if not exist "auto_purify.dll" (pause)
rem Applications using phoenix will use their own versions of the DLL
del "phoenix.dll"