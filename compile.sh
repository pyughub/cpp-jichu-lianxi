#!/usr/bin/env bash
# Compile one C++ file into bin/, and put its .dSYM into dsym/.
set -euo pipefail

root="$(cd "$(dirname "$0")" && pwd)"
src_in="${1:-}"

if [[ -z "$src_in" ]]; then
  echo "用法: compile.sh <file.cpp>" >&2
  exit 1
fi
if [[ ! -f "$src_in" ]]; then
  echo "找不到源文件: $src_in" >&2
  exit 1
fi

filename="$(basename "$src_in")"
ext="${filename##*.}"
case "$ext" in
  c|cc|cpp|cxx|C|CPP) ;;
  *)
    echo "当前打开的不是 C/C++ 源文件: $filename" >&2
    exit 1
    ;;
esac

src="$(cd "$(dirname "$src_in")" && pwd)/$filename"
name="${filename%.*}"
mkdir -p "$root/src" "$root/bin" "$root/dsym"
out="$root/bin/$name"

if command -v clang++ >/dev/null 2>&1; then
  cxx=clang++
else
  cxx=c++
fi

"$cxx" -std=c++17 -g -O0 -Wall -Wextra -o "$out" "$src"

rm -rf "$root/dsym/$name.dSYM"
if [[ -d "$out.dSYM" ]]; then
  mv "$out.dSYM" "$root/dsym/$name.dSYM"
fi
