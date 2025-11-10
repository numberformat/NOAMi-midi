#!/usr/bin/env bash

set -euo pipefail

cmake -S . -B build/macos -DCMAKE_BUILD_TYPE=Release
cmake --build build/macos --target MidiBackendHarness
