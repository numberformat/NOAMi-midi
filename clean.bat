@echo off
setlocal
echo [clean] Removing build artifacts...
if exist build (
    rmdir /S /Q build
    if errorlevel 1 (
        echo [clean] Failed to remove build directory.
        exit /b 1
    )
    echo [clean] build directory removed.
) else (
    echo [clean] Nothing to clean.
)
exit /b 0
