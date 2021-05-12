@echo off

echo %VCVARSALL% %1
call "%VCVARSALL%" %1

if not exist .\Build mkdir .\Build

pushd .\Build
cl -nologo -Zi -EHsc -FC -DRESIZE_BUFFERS -Fe:%2.exe ../*.cpp user32.lib gdi32.lib
popd