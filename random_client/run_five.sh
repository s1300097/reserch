#!/usr/bin/env bash

make

if [ -x "./client.exe" ]; then
    ./client.exe -n random-based&
    ./client.exe -n random-based&
    ./client.exe -n random-based&
    ./client.exe -n random-based&
    ./client.exe -n random-based&
    echo " → client.exe started (PID: $!)"
else
    echo "ERROR: ./client/client.exe not found or not executable"
    exit 1
fi
echo "Done."