@echo off
setlocal
echo [run] Launching NOAMi MIDI Player...
set CONFIG=%1
if "%CONFIG%"=="" set CONFIG=Release
set BINARY=build\cmake\bin\NOAMi-MIDI.exe

if not exist "%BINARY%" (
    echo [run] Binary not found. Building %CONFIG% first...
    call "%~dp0build.bat" %CONFIG%
    if errorlevel 1 exit /b 1
)

if not exist "%BINARY%" (
    echo [run] Unable to locate %BINARY%.
    exit /b 1
)

start "NOAMi MIDI Player" "%BINARY%"
exit /b 0
