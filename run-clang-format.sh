#!/usr/bin/env bash

mapfile -t FILES < <(find include src \( -name "*.hpp" -o -name "*.cpp" \))
echo "DEBUG: ${FILES[@]}"
clang-format -i "${FILES[@]}"
