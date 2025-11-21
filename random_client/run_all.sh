#!/usr/bin/env bash
# run_all.sh -- run make, then start ./client/client.exe in background
# Place this script inside ./random_client and run: ./run_all.sh

set -eu  # -e: stop on error, -u: undefined variable = error

# script directory = ./random_client
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

cd "$PROJECT_ROOT"
echo "Working directory: $PROJECT_ROOT"

echo "1) Running make in ./random_client"
(cd ./random_client && make)

echo "2) Starting ./client/client.exe in background"
if [ -x "./client/client.exe" ]; then
    ./client/client.exe &
    ./client/client.exe &
    ./client/client.exe &
    ./client/client.exe &
    echo " → client.exe started (PID: $!)"
else
    echo "ERROR: ./client/client.exe not found or not executable"
    exit 1
fi

echo "3) Running ./random_client/client.exe"
if [ -x "./random_client/client.exe" ]; then
    ./random_client/client.exe -n random &
else
    echo "ERROR: ./random_client/client.exe not found or not executable"
    exit 1
fi

echo "Done."
