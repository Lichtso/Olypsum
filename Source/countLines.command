#!/bin/sh
cd ${0%/*}/
find Engine \
Engine/GUI Engine/Scripting ../Resources/Shaders \
-depth 1 \( -name "*.h" -or -name "*.c" -or -name "*.cpp" -or -name "*.glsl" \) | xargs wc -l