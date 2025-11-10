@echo off
setlocal

if not exist Dockerfile (
    echo [docker] Dockerfile not found in %cd%
    exit /b 1
)

echo [docker] Building NOAMi-MIDI-builder image...
docker build -t tmidi-builder .
if errorlevel 1 (
    echo [docker] Image build failed.
    exit /b 1
)

echo [docker] Running build inside container...
docker run --rm -v "%cd%:/work" tmidi-builder ^
    /bin/bash -lc "cmake -S . -B build/cmake -G Ninja -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ -DCMAKE_RC_COMPILER=x86_64-w64-mingw32-windres && cmake --build build/cmake"
if errorlevel 1 (
    echo [docker] Container build failed.
    exit /b 1
)

echo [docker] Done. Binary is in build\cmake\bin\NOAMi-MIDI.exe
exit /b 0
