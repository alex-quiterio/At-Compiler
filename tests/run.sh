#!/usr/bin/env bash
#
# Compile, link and run every program in tests/cases, then compare its output
# with tests/expected. Whitespace is ignored in the comparison: the course
# reference outputs are not exact about it (C-01 expects " 0", A-07 expects
# "X Y Z" where the program prints "X \n Y \n Z \n").
#
# Usage: tests/run.sh [NAME...]     e.g. tests/run.sh A-01-1-N-ok B-03-11-N-ok
#
# Needs the compiler built (make), yasm, and the RTS7 runtime library (librts).

set -u

HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(dirname "$HERE")"
AT="${AT:-$ROOT/at}"
CASES="$HERE/cases"
EXPECTED="$HERE/expected"
INPUT="$HERE/input"   # optional NAME.in, fed to the program on stdin
OUT="$HERE/out"

[ -x "$AT" ] || { echo "compiler not found at $AT (run make first)" >&2; exit 2; }
mkdir -p "$OUT"

if [ $# -gt 0 ]; then
  names=("$@")
else
  names=()
  for f in "$CASES"/*.at; do names+=("$(basename "$f" .at)"); done
fi

pass=0
fail=()
for name in "${names[@]}"; do
  asm="$OUT/$name.asm"
  obj="$OUT/$name.o"
  bin="$OUT/$name"
  if cp "$CASES/$name.at" "$OUT/" &&
     "$AT" "$OUT/$name.at" &&
     yasm -felf -o "$obj" "$asm" &&
     ld -m elf_i386 -o "$bin" "$obj" -lrts &&
     { if [ -f "$INPUT/$name.in" ]; then "$bin" < "$INPUT/$name.in"; else "$bin" < /dev/null; fi; } > "$OUT/$name.out" &&
     [ "$(tr -d '[:space:]' < "$OUT/$name.out")" = "$(tr -d '[:space:]' < "$EXPECTED/$name.out")" ]
  then
    pass=$((pass + 1))
  else
    fail+=("$name")
  fi
done

echo "$pass/${#names[@]} passed"
if [ ${#fail[@]} -gt 0 ]; then
  printf 'FAIL %s\n' "${fail[@]}"
  exit 1
fi
