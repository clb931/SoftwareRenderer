@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x64

if not exist .\Build mkdir .\Build

pushd .\Build
cl -FC -Zi ..\Main.cpp user32.lib gdi32.lib
popd