@echo off
setlocal

if "%~1"=="" (
    set "CONFIG=Release"
) else (
    set "CONFIG=%~1"
)

where cmake >nul 2>&1
if errorlevel 1 goto :nocmake

where g++ >nul 2>&1
if errorlevel 1 goto :nomingw

echo [build] Configuring (%CONFIG%)...
cmake -S . -B build/cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=%CONFIG%
if errorlevel 1 goto :cmakefail

echo [build] Building (%CONFIG%)...
cmake --build build/cmake --config %CONFIG%
if errorlevel 1 goto :buildfail

echo [build] Done. Binary is in build\cmake\bin\NOAMi-MIDI.exe
exit /b 0

:nocmake
echo [build] CMake not found on PATH.
echo [build] Install it via: choco install cmake --installargs "ADD_CMAKE_TO_PATH=System" -y
echo [build] Then run: refreshenv
exit /b 1

:nomingw
echo [build] MinGW toolchain not found (g++ is missing).
echo [build] Install it via: choco install mingw -y
echo [build] Ensure C:\tools\mingw64\bin is on PATH and rerun this script.
exit /b 1

:cmakefail
echo [build] CMake configure failed.
exit /b 1

:buildfail
echo [build] Build failed.
exit /b 1
