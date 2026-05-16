#!/usr/bin/env bash
set -euo pipefail

mode="${1:-fix}"
root_dir="$(git rev-parse --show-toplevel 2>/dev/null || pwd)"
cd "${root_dir}"

if ! command -v clang-format >/dev/null 2>&1; then
  echo "error: clang-format is not installed" >&2
  echo "install: sudo apt install clang-format clang-tidy" >&2
  exit 127
fi

mapfile -d '' files < <(
  find apps include src \
    -type f \
    \( -name '*.cpp' -o -name '*.h' -o -name '*.hpp' -o -name '*.tpp' \) \
    -print0
)

if [[ "${#files[@]}" -eq 0 ]]; then
  echo "No C++ files found."
  exit 0
fi

case "${mode}" in
  fix|format)
    clang-format -i "${files[@]}"
    ;;
  check)
    clang-format --dry-run --Werror "${files[@]}"
    ;;
  *)
    echo "usage: $0 [fix|check]" >&2
    exit 2
    ;;
esac
