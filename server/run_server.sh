#!/usr/bin/env bash
set -euo pipefail

# Ensure we run from the script location so relative paths resolve correctly.
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

make

cd "$SCRIPT_DIR/src"

if [ -x "./tndhms.exe" ]; then
    # Pass the config file explicitly to avoid missing-file errors.
    ./tndhms.exe -c "$(pwd)/tndhms.cfg" &
    echo "tndhms.exe started (PID: $!)"
else
    echo "ERROR: ./server/src/tndhms.exe not found or not executable"
    exit 1
fi
