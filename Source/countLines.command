#!/bin/sh
cd /Users/alexander/Desktop/Olypsum/
find ./Source/Engine \
./Source/Engine/GUI ./Source/Engine/Scripting ./Resources/Shaders \
-depth 1 \( -name "*.h" -or -name "*.c" -or -name "*.cpp" -or -name "*.glsl" \) | xargs wc -l


