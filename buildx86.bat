@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86

if not exist .\Build mkdir .\Build

pushd .\Build
cl -FC -Zi ..\SoftwareRenderer.cpp user32.lib gdi32.lib
popd