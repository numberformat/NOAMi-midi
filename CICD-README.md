# CI/CD Notes – NOAMi-MIDI

A step-by-step record of how the automated build + release pipeline was set up for NOAMi-MIDI using Docker and GitHub Actions. Use this as a template for future Windows/MinGW projects.

## 1. Dockerized Build Environment

1. **Dockerfile**
   ```dockerfile
   FROM fedora:40

   RUN dnf install -y \
       mingw64-gcc mingw64-gcc-c++ mingw64-winpthreads-static \
       mingw64-headers mingw64-crt mingw64-binutils \
       cmake ninja-build git && \
       dnf clean all

   WORKDIR /work
   ```
   - Fedora’s packages include MinGW-w64 cross compilers and `ninja-build`.
   - `mingw64-binutils` provides `x86_64-w64-mingw32-windres` (no separate `mingw64-windres` package).

2. **Local wrapper (`docker-build.bat`)**
   - Builds the Docker image and runs the container:
   ```bat
   docker build -t NOAMi-MIDI-builder .
   docker run --rm -v "%cd%:/work" NOAMi-MIDI-builder \
       /bin/bash -lc "cmake -S . -B build/cmake -G Ninja -DCMAKE_SYSTEM_NAME=Windows \
       -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
       -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ \
       -DCMAKE_RC_COMPILER=x86_64-w64-mingw32-windres && \
       cmake --build build/cmake --config Release"
   ```
   - Produces `build\cmake\bin\NOAMi-MIDI.exe` on the host.

## 2. CMake Updates for MinGW

- Added `CMakeLists.txt` with a single target for NOAMi-MIDI.
- Added `if (MINGW)` branch with linker options: `-mwindows -static -static-libgcc -static-libstdc++` so runtime DLLs aren’t required.

## 3. GitHub Actions Workflow

`.github/workflows/build.yml`:
```yaml
name: Build NOAMi-MIDI

permissions:
  contents: write

on:
  push:
    branches: [ main, master, cicd ]
    tags: [ 'v*' ]
  pull_request:
    branches: [ main, master ]
  workflow_dispatch:

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Build container image
        run: docker build -t NOAMi-MIDI-builder .
      - name: Build NOAMi-MIDI inside container
        run: >-
          docker run --rm -v "${{ github.workspace }}:/work" NOAMi-MIDI-builder
          /bin/bash -lc "cmake -S . -B build/cmake -G Ninja -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ -DCMAKE_RC_COMPILER=x86_64-w64-mingw32-windres && cmake --build build/cmake --config Release"
      - name: Upload artifact
        uses: actions/upload-artifact@v4
        with:
          name: NOAMi-MIDI-release
          path: build/cmake/bin/NOAMi-MIDI.exe
  release:
    needs: build
    if: startsWith(github.ref, 'refs/tags/')
    runs-on: ubuntu-latest
    steps:
      - uses: actions/download-artifact@v4
        with:
          name: NOAMi-MIDI-release
          path: dist
      - name: Create GitHub Release
        uses: softprops/action-gh-release@v2
        with:
          files: dist/NOAMi-MIDI.exe
        env:
          GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
```

- `permissions: contents: write` is required for `action-gh-release` to publish releases.
- Builds on pushes to `main`, `master`, `cicd` (for testing) — artifacts downloadable from Actions UI.
- On tags `v*`, workflow automatically creates a GitHub Release with `NOAMi-MIDI.exe` attached.

## 4. Release Process

1. Ensure Actions are enabled on the repo.
2. Push branch commits as usual; workflow runs and produces an artifact.
3. When ready to publish:
   ```bash
   git tag v1.0.0
   git push origin v1.0.0
   ```
4. Workflow builds NOAMi-MIDI and `softprops/action-gh-release` publishes the executable as part of the release.
5. For test releases, tag something like `v0.0.1-test`, verify, then delete the release/tag via GitHub UI.

## 5. Why this pattern works

- **Portability**: Docker image ensures identical toolchain locally and in CI.
- **Safety**: No Windows runners required; Linux host + MinGW cross-compiler suffices.
- **Reproducibility**: `docker-build.bat` + workflow share the same commands, so local builds mimic CI exactly.
- **Self-contained binaries**: Static linking removes dependency on MinGW runtime DLLs.

## 6. Reuse checklist for another project

1. Add/adjust `CMakeLists.txt` (ensure case-sensitive includes match filenames).
2. Copy the Dockerfile, update package list for dependencies.
3. Add `docker-build.bat` (or shell equivalent) using the same commands.
4. Copy `.github/workflows/build.yml`, adjust names/paths.
5. Make sure README addresses the Docker workflow and warning prompts.
6. Enable Actions, push, tag, and confirm release upload.
