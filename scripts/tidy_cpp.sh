#!/usr/bin/env bash
set -euo pipefail

root_dir="$(git rev-parse --show-toplevel 2>/dev/null || pwd)"
cd "${root_dir}"

if ! command -v clang-tidy >/dev/null 2>&1; then
  echo "error: clang-tidy is not installed" >&2
  echo "install: sudo apt install clang-format clang-tidy" >&2
  exit 127
fi

cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

mapfile -d '' files < <(
  find apps src \
    -type f \
    -name '*.cpp' \
    -print0
)

if [[ "${#files[@]}" -eq 0 ]]; then
  echo "No C++ source files found."
  exit 0
fi

clang-tidy -p build "${files[@]}"
