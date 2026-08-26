#!/usr/bin/env python3
"""
Heuristic validator for codexion log output.

Usage:
    analyze_log.py <logfile> <expect_burnout: yes|no> <n> <burnout> <compile> <debug> <refactor> <required> <cooldown> <scheduler>

Exit code 0 = looks correct, 1 = a check failed (details printed to stdout).

NOTE: this is a heuristic aid, not an official grader. It only sees stdout
(coder id + event), not which physical dongle was touched, so dongle-overlap
checks are derived from the deterministic left/right dongle assignment
(coder i's dongles are i and (i % n) + 1) rather than read from the log.
Always eyeball flagged runs yourself before trusting the verdict.
"""
import re
import sys

LINE_RE = re.compile(
    r"^(\d+) (\d+) (has taken a dongle|is compiling|is debugging|is refactoring|burned out)$"
)

EVENTS = ["has taken a dongle", "has taken a dongle", "is compiling",
          "is debugging", "is refactoring"]


def dongle_of_pair(n, coder_id):
    """Return (left_dongle, right_dongle) ids for a coder, matching fill_coder()."""
    left = coder_id
    right = (coder_id % n) + 1
    return left, right


def owner_pair_for_dongle(n, dongle_id):
    """Which two coder ids share dongle_id, per the left/right formula."""
    # dongle j is coder j's left, and coder i's right where (i % n) + 1 == j
    other = n if dongle_id == 1 else dongle_id - 1
    return {dongle_id, other}


def main():
    args = sys.argv[1:]
    if len(args) < 10:
        print("usage: analyze_log.py <logfile> <expect_burnout yes/no> "
              "<n> <burnout> <compile> <debug> <refactor> <required> <cooldown> <scheduler>")
        return 2

    logfile, expect_burnout = args[0], args[1].lower()
    n, t_burnout, t_compile, t_debug, t_refactor, required, cooldown, scheduler = args[2:10]
    n = int(n)
    t_burnout = int(t_burnout)
    required = int(required)
    cooldown = int(cooldown)

    failures = []

    with open(logfile) as f:
        raw_lines = [l.rstrip("\n") for l in f if l.strip()]

    if not raw_lines:
        failures.append("log file is empty")
        _report(failures)
        return 1 if failures else 0

    parsed = []
    for i, line in enumerate(raw_lines):
        m = LINE_RE.match(line)
        if not m:
            failures.append(f"line {i+1} does not match expected format / possible interleaving: {line!r}")
            continue
        ts, cid, ev = int(m.group(1)), int(m.group(2)), m.group(3)
        parsed.append((ts, cid, ev))

    # timestamps should be non-decreasing overall (single global clock)
    for i in range(1, len(parsed)):
        if parsed[i][0] < parsed[i - 1][0]:
            failures.append(f"timestamp goes backwards at line {i+1}: {parsed[i-1]} -> {parsed[i]}")

    burnout_lines = [p for p in parsed if p[2] == "burned out"]

    if expect_burnout == "yes":
        if not burnout_lines:
            failures.append("expected a burnout but none was logged")
        else:
            if parsed[-1][2] != "burned out":
                failures.append("'burned out' must be the last line printed, but it isn't")
            if len(burnout_lines) > 1:
                failures.append(f"more than one 'burned out' line logged: {burnout_lines}")
            bo_ts = burnout_lines[0][0]
            # soft timing check -- report, don't hard-fail, since the sheet
            # itself allows tolerance on borderline/slow-hardware cases
            if abs(bo_ts - t_burnout) > 100:
                print(f"  [warn] burnout at t={bo_ts}, expected close to t={t_burnout} "
                      f"(off by {bo_ts - t_burnout}ms) -- check manually, tolerance may still be OK")
    else:
        if burnout_lines:
            failures.append(f"unexpected burnout(s) logged: {burnout_lines}")

    # per-coder state machine + compile counts
    per_coder_events = {}
    for ts, cid, ev in parsed:
        per_coder_events.setdefault(cid, []).append((ts, ev))

    compiles_done = {cid: 0 for cid in range(1, n + 1)}
    for cid, events in per_coder_events.items():
        step = 0
        for ts, ev in events:
            if ev == "burned out":
                break
            expected = EVENTS[step % len(EVENTS)]
            if ev != expected:
                failures.append(
                    f"coder {cid}: expected '{expected}' but got '{ev}' at t={ts} "
                    f"(out-of-order state transition)"
                )
                break
            if ev == "is compiling":
                compiles_done[cid] += 1
            step += 1

    if expect_burnout == "no":
        for cid in range(1, n + 1):
            if compiles_done.get(cid, 0) < required:
                failures.append(
                    f"coder {cid} only completed {compiles_done.get(cid, 0)} compiles, "
                    f"needed {required}"
                )

    # dongle non-duplication + cooldown, derived from deterministic left/right ids
    # holding interval approximated as [2nd 'has taken a dongle' ts, 'is debugging' ts]
    intervals_by_dongle = {j: [] for j in range(1, n + 1)}
    for cid, events in per_coder_events.items():
        i = 0
        while i < len(events):
            if events[i][1] == "has taken a dongle" and i + 1 < len(events) and events[i + 1][1] == "has taken a dongle":
                take2_ts = events[i + 1][0]
                # find the following 'is debugging' to mark release
                release_ts = None
                for j in range(i + 2, len(events)):
                    if events[j][1] == "is debugging":
                        release_ts = events[j][0]
                        break
                if release_ts is not None:
                    left, right = dongle_of_pair(n, cid)
                    intervals_by_dongle[left].append((take2_ts, release_ts, cid))
                    intervals_by_dongle[right].append((take2_ts, release_ts, cid))
                i += 2
            else:
                i += 1

    for dongle_id, intervals in intervals_by_dongle.items():
        intervals.sort()
        for k in range(1, len(intervals)):
            prev_start, prev_end, prev_cid = intervals[k - 1]
            cur_start, cur_end, cur_cid = intervals[k]
            if cur_start < prev_end:
                failures.append(
                    f"dongle {dongle_id}: overlap between coder {prev_cid} "
                    f"({prev_start}-{prev_end}) and coder {cur_cid} "
                    f"({cur_start}-{cur_end}) -- possible duplication!"
                )
            elif cooldown > 0 and cur_start < prev_end + cooldown - 5:  # 5ms jitter grace
                failures.append(
                    f"dongle {dongle_id}: coder {cur_cid} took it at t={cur_start}, "
                    f"only {cur_start - prev_end}ms after coder {prev_cid} released at "
                    f"t={prev_end} (cooldown={cooldown}ms)"
                )

    _report(failures)
    return 1 if failures else 0


def _report(failures):
    for f in failures:
        print(f"  [FAIL] {f}")


if __name__ == "__main__":
    sys.exit(main())
