# NOAMi-MIDI Changelog

## 2025-11 – CMake Modernization & Tooling Cleanup - Neeraj Verma

- Converted the build system to a single CMake-based workflow that targets both MinGW and Visual Studio generators; added `CMakeLists.txt` at repo root.
- Removed the legacy Visual Studio project files after migrating their settings into CMake; introduced `build/` as the sole output tree.
- Added convenience scripts for Windows contributors:
  - `build.bat` (configurable Release/Debug builds with toolchain checks)
  - `run.bat` (builds on demand and launches `NOAMi-MIDI.exe`)
  - `clean.bat` (purges the `build` directory)
  - `docker-build.bat` + `Dockerfile` (Docker Desktop workflow that cross-builds NOAMi-MIDI via MinGW without installing local toolchains)
- Documented the new workflow in `README.md`, including Chocolatey commands for installing MinGW, CMake, optional IDEs, and the Docker build path.
- Restructured the repository layout so source assets stay under `src/` and transient outputs live only under `build/`.
- Added a portable `winres.h` shim and refreshed resource includes so MinGW's `windres` can compile the Windows resources without MSVC headers.
- Cleared compiler warnings by:
  - Guarding MSVC-only pragmas, tightening string handling (`strncpy` + null termination), and making lookup tables `const`.
  - Cleaning up unsigned/signed comparisons, replacing raw counts with `*_count` constants, and fixing fallthroughs and indentation.
  - Removing unused variables/parameters or marking them with `(void)` to silence noise while keeping intent clear.
- Added a GitHub Actions workflow that builds NOAMi-MIDI via Docker/Ninja on every push and automatically publishes `NOAMi-MIDI.exe` to GitHub Releases whenever a `v*` tag is pushed; non-tag builds still expose the artifact for testing.
- Switched MinGW builds to static linking (`-static -static-libgcc -static-libstdc++`) so released executables no longer depend on `libstdc++-6.dll` or other runtime DLLs.

