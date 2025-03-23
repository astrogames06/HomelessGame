#!/bin/bash

# Compiler options
CC=em++
CFLAGS="-std=c++17 -Os -Wall"
EMFLAGS="-s USE_GLFW=3 --shell-file build/web/shell.html -DPLATFORM_WEB"

# Paths
INCLUDE_DIR="include/"
LIB_DIR="lib/"
OUTPUT="build/web/index.html"
FILES="src/main.cpp src/Game/Game.cpp src/Entity/Entity.cpp src/Player/Player.cpp"

# Build command
$CC -o $OUTPUT $FILES $LIB_DIR/libraylib.web.a $CFLAGS -I. -I $INCLUDE_DIR -L. -L $LIB_DIR -s EXPORTED_FUNCTIONS='["_main", "_SetHighScore", "_GetHighScore"]' -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","getValue","setValue"]' $EMFLAGS