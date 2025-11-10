#!/usr/bin/env bash

set -euo pipefail

if [ -d build ]; then
    echo "[clean] Removing build directory..."
    rm -rf build
else
    echo "[clean] Nothing to clean."
fi
