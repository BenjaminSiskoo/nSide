rem For use with Windows
mkdir "obj"
mingw32-make -j8
if not exist "auto_purify.dll" (pause)
rem Applications using phoenix will use their own versions of the DLL
del "phoenix.dll"