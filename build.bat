@echo off

rem call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x64

set APP_NAME=bootstrap
set BUILD_OPTIONS= -DPLATFORM_WIN32=1
set COMPILE_FLAGS= /MT /nologo /Gm- /GR- /EHa /Od /Oi /WX /W4 /wd4201 /wd4100 /wd4189 /wd4505 /Z7 /I ../source/
set COMMON_LINKER_FLAGS= -opt:ref -incremental:no /Debug:fastlink
set PLATFORM_LINKER_FLAGAS= gdi32.lib user32.lib winmm.lib %COMMON_LINKER_FLAGS%

if not exist build mkdir build
pushd build
cl %BUILD_OPTIONS% %COMPILE_FLAGS% ../source/win32/win32_main.cpp /link %PLATFORM_LINKER_FLAGAS% /out:%APP_NAME%.exe
popd