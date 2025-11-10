@echo off
REM release.bat
REM Usage:
REM   release.bat 1.0.0
REM   release.bat v1.0.0

if "%~1"=="" (
    echo.
    echo Usage:
    echo   %~nx0 version_number
    echo.
    echo Examples:
    echo   %~nx0 1.2.3
    echo   %~nx0 v1.2.3
    echo.
    exit /b 1
)

set "ARG=%~1"

REM Remove leading 'v' or 'V' if present
if /i "%ARG:~0,1%"=="v" (
    set "ARG=%ARG:~1%"
)

set "TAG=v%ARG%"

echo Creating tag %TAG%...
git tag %TAG%
if errorlevel 1 (
    echo Failed to create tag.
    exit /b 1
)

echo Pushing tag %TAG% to origin...
git push origin %TAG%
if errorlevel 1 (
    echo Failed to push tag.
    exit /b 1
)

echo.
echo ✅ Tag %TAG% created and pushed successfully!
