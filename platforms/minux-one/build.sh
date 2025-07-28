#!/bin/bash

# Minux RTOS Build Script
# Usage: ./build.sh [clean|build|upload|monitor|all]

PROJECT_DIR="$(dirname "$0")"
cd "$PROJECT_DIR"

case "$1" in
    "clean")
        echo "Cleaning build files..."
        pio run -t clean
        ;;
    "build")
        echo "Building Minux RTOS..."
        pio run
        ;;
    "upload")
        echo "Uploading to device..."
        pio run -t upload
        ;;
    "monitor")
        echo "Starting serial monitor..."
        pio device monitor --baud 115200
        ;;
    "all")
        echo "Building and uploading Minux RTOS..."
        pio run -t upload
        echo "Starting monitor..."
        sleep 2
        pio device monitor --baud 115200
        ;;
    *)
        echo "Minux RTOS Build Script"
        echo "Usage: $0 [clean|build|upload|monitor|all]"
        echo ""
        echo "Commands:"
        echo "  clean   - Clean build files"
        echo "  build   - Build the project"
        echo "  upload  - Upload to Arduino"
        echo "  monitor - Start serial monitor"
        echo "  all     - Build, upload, and monitor"
        ;;
esac
