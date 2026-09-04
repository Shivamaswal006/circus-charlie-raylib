#!/bin/bash

echo "=========================================="
echo "  Compiling Circus Charlie for Linux...  "
echo "=========================================="

PROJECT_DIR="$(pwd)"

# 1. Check if Raylib is already installed on the system
if ! [ -f /usr/include/raylib.h ] && ! [ -f /usr/local/include/raylib.h ] && ! pkg-config --exists raylib 2>/dev/null; then
    echo "Raylib not detected. Starting automatic setup..."

    if command -v apt &> /dev/null; then
        # Try apt install first
        echo "Checking package manager..."
        sudo add-apt-repository -y universe 2>/dev/null
        sudo apt update
        sudo apt install -y libraylib-dev 2>/dev/null

        # If apt couldn't find the package, download and compile Raylib from source
        if ! [ -f /usr/include/raylib.h ] && ! [ -f /usr/local/include/raylib.h ]; then
            echo "Building Raylib from source (takes ~30 seconds)..."
            sudo apt install -y build-essential git libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev
            
            rm -rf /tmp/raylib
            git clone --depth 1 https://github.com/raysan5/raylib.git /tmp/raylib
            cd /tmp/raylib/src || exit 1
            make PLATFORM=PLATFORM_DESKTOP -j$(nproc)
            sudo make install
            cd "$PROJECT_DIR" || exit 1
        fi
    else
        echo "Please install Raylib manually using your system package manager."
        exit 1
    fi
fi

# 2. Compile the Circus Charlie game
echo "Compiling game binary..."
gcc main.c -o main -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

if [ $? -eq 0 ]; then
    chmod +x main
    echo ""
    echo " Build Successful!"
    echo " Launch the game by typing: ./main"
    echo "=========================================="
else
    echo ""
    echo "❌ Build Failed."
    echo "=========================================="
fi