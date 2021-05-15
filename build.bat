@echo off

echo %VCVARSALL% %1
call "%VCVARSALL%" %1

if not exist .\out mkdir .\out

pushd .\out
del %2.exe
cl -nologo -Zi -EHsc -FC -DRESIZE_BUFFERS -Fe:%2.exe ../src/*.cpp user32.lib gdi32.lib
popd