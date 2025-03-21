#!/bin/bash

# Compiler options
CC=g++
CFLAGS="-O1 -Wall -std=c++17 -Wno-missing-braces"

# Paths
INCLUDE_DIR="include/"
LIB_DIR="lib/"

FILES="src/main.cpp src/Game/Game.cpp src/Entity/Entity.cpp src/Player/Player.cpp"

# Build command
$CC $FILES $LIB_DIR/libraylib.mac.a -o build/mac/app $CFLAGS -I $INCLUDE_DIR -L $LIB_DIR -lraylib.mac -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
./build/mac/app