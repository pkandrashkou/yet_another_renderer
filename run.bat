@echo off
call build.bat
pushd build
renderer.exe
popd