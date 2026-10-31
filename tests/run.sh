#!/usr/bin/env bash
#
# Compile, link and run every program in tests/cases, then compare its output
# with tests/expected. Whitespace is ignored in the comparison: the course
# reference outputs are not exact about it (C-01 expects " 0", A-07 expects
# "X Y Z" where the program prints "X \n Y \n Z \n").
#
# Usage: tests/run.sh [NAME...]     e.g. tests/run.sh A-01-1-N-ok B-03-11-N-ok
#
# Set REPORT=file.md to also write a Markdown report (CI appends it to the
# GitHub Actions job summary).
#
# Needs the compiler built (make), yasm, and the RTS7 runtime library (librts).

set -u

HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(dirname "$HERE")"
AT="${AT:-$ROOT/build/at}"
CASES="$HERE/cases"
EXPECTED="$HERE/expected"
INPUT="$HERE/input"   # optional NAME.in, fed to the program on stdin
OUT="$HERE/out"
REPORT="${REPORT:-}"

[ -x "$AT" ] || { echo "compiler not found at $AT (run make first)" >&2; exit 2; }
mkdir -p "$OUT"

if [ $# -gt 0 ]; then
  names=("$@")
else
  names=()
  for f in "$CASES"/*.at; do names+=("$(basename "$f" .at)"); done
fi

# Run one test; prints the stage it failed at (nothing when it passes).
run_one() {
  local name=$1 bin="$OUT/$1"
  cp "$CASES/$name.at" "$OUT/" || { echo missing; return; }
  "$AT" "$OUT/$name.at" 2> "$OUT/$name.err" || { echo compile; return; }
  yasm -felf -o "$bin.o" "$bin.asm" 2>> "$OUT/$name.err" || { echo assemble; return; }
  ld -m elf_i386 -o "$bin" "$bin.o" -lrts 2>> "$OUT/$name.err" || { echo link; return; }
  local input=/dev/null
  [ -f "$INPUT/$name.in" ] && input="$INPUT/$name.in"
  timeout 10 "$bin" < "$input" > "$OUT/$name.out" 2>> "$OUT/$name.err" || { echo run; return; }
  [ "$(tr -d '[:space:]' < "$OUT/$name.out")" = "$(tr -d '[:space:]' < "$EXPECTED/$name.out")" ] ||
    echo output
}

# a program's output on one line, for the report
oneline() { [ -f "$1" ] && tr '\n' ' ' < "$1" | cut -c1-80 | sed 's/|/\\|/g; s/`/'"'"'/g'; }

pass=0
fail=()
rows=()
for name in "${names[@]}"; do
  stage=$(run_one "$name")
  if [ -z "$stage" ]; then
    pass=$((pass + 1))
    rows+=("| ✅ | \`$name\` | \`$(oneline "$OUT/$name.out")\` |")
  else
    fail+=("$name ($stage)")
    rows+=("| ❌ | \`$name\` | failed at **$stage** |")
  fi
done

echo "$pass/${#names[@]} passed"
[ ${#fail[@]} -gt 0 ] && printf 'FAIL %s\n' "${fail[@]}"

if [ -n "$REPORT" ]; then
  {
    if [ ${#fail[@]} -eq 0 ]; then
      echo "## ✅ Tests: $pass/${#names[@]} passed"
    else
      echo "## ❌ Tests: $pass/${#names[@]} passed, ${#fail[@]} failed"
      echo
      echo "| Test | Stage | Expected | Got |"
      echo "|------|-------|----------|-----|"
      for f in "${fail[@]}"; do
        name=${f%% *}
        stage=${f#* (}; stage=${stage%)}
        got=$(oneline "$OUT/$name.out")
        [ "$stage" != output ] && got=$(oneline "$OUT/$name.err")
        echo "| \`$name\` | $stage | \`$(oneline "$EXPECTED/$name.out")\` | \`$got\` |"
      done
    fi
    echo
    echo "Output is compared ignoring whitespace."
    echo
    echo "<details><summary>All ${#names[@]} tests</summary>"
    echo
    echo "| | Test | Output |"
    echo "|-|------|--------|"
    printf '%s\n' "${rows[@]}"
    echo
    echo "</details>"
  } > "$REPORT"
fi

[ ${#fail[@]} -eq 0 ]
