#!/usr/bin/env bash
# Codexion correction-sheet test runner.
# Run this from the root of your codexion repo (where the Makefile lives).
# Requires python3 in PATH. Put analyze_log.py in the same folder as this script.
set -uo pipefail

BIN=./codexion
OUTDIR=test_logs
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PYTHON=${PYTHON:-python3}

mkdir -p "$OUTDIR"
pass=0
fail=0

check_bin() {
    if [ ! -x "$BIN" ]; then
        echo "Binary not found, building..."
        make -s || { echo "BUILD FAILED"; exit 1; }
    fi
}

# run_case NAME EXPECT_BURNOUT(yes/no) REPEATS ARG1..ARG8
run_case() {
    local name="$1"; shift
    local expect_burnout="$1"; shift
    local repeats="$1"; shift
    local args=("$@")

    echo "=== $name ==="
    echo "    ./codexion ${args[*]}   (repeats=$repeats, expect_burnout=$expect_burnout)"
    for i in $(seq 1 "$repeats"); do
        local base="$OUTDIR/${name}_run${i}"
        timeout 30 "$BIN" "${args[@]}" > "${base}.log" 2> "${base}.err"
        local rc=$?

        if [ $rc -ge 128 ]; then
            echo "  run$i: CRASH (signal $((rc - 128))) -- see ${base}.err"
            fail=$((fail + 1))
            continue
        fi
        if [ $rc -eq 124 ]; then
            echo "  run$i: TIMEOUT / hung after 30s"
            fail=$((fail + 1))
            continue
        fi

        if "$PYTHON" "$SCRIPT_DIR/analyze_log.py" "${base}.log" "$expect_burnout" "${args[@]}"; then
            echo "  run$i: PASS"
            pass=$((pass + 1))
        else
            echo "  run$i: FAIL -- full log at ${base}.log"
            fail=$((fail + 1))
        fi
    done
    echo
}

check_bin

# ---------------- Easy ----------------
run_case "easy_single_coder_must_burnout"      yes 5 1    800  200 200 200 10 0   fifo
run_case "easy_fifo_no_burnout_10_compiles"    no  5 5    2000 200 200 200 10 0   fifo
run_case "easy_edf_no_burnout_7_compiles"      no  5 5    2000 200 200 200 7  0   edf

# ---------------- Less easy ----------------
run_case "lesseasy_infeasible_must_burnout"    yes 5 5    500  200 200 200 10 0   fifo

# ---------------- Medium ----------------
run_case "medium_cooldown_check_fifo"          no  5 3    3000 200 200 200 10 400 fifo
run_case "medium_contention_fifo"              no  5 3    3000 200 200 200 10 800 fifo
run_case "medium_contention_edf"               no  5 3    3000 200 200 200 10 800 edf

echo "========================================"
echo "TOTAL: $pass passed, $fail failed"
echo "Raw logs saved under $OUTDIR/"
echo
echo "Reminder -- this script only covers the Easy/Less-easy/Medium timing"
echo "and invariant checks. Still run separately, and share output for:"
echo "  make                                    # -Wall -Wextra -Werror -pthread, no warnings"
echo "  valgrind --leak-check=full --show-leak-kinds=all ./codexion 5 2000 200 200 200 10 0 fifo"
echo "  valgrind --tool=helgrind ./codexion 5 3000 200 200 200 10 800 edf"
echo "  valgrind --tool=drd ./codexion 5 3000 200 200 200 10 800 edf"
echo "  (rebuild with -fsanitize=thread and rerun the contention cases)"
