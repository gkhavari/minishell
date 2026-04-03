#!/usr/bin/env python3
"""Print a human-readable summary of funcheck JSON output files.

Usage:
    funcheck_summary.py <pass1.json> <pass2.json>

Exit 0 if both passes have exit-code 0 and no crashes.
Exit 1 otherwise.
"""
import json
import sys
from collections import Counter
from pathlib import Path


def load_json(path: str) -> dict:
    try:
        return json.loads(Path(path).read_text())
    except FileNotFoundError:
        print(f"  (file not found: {path})")
        return {}
    except json.JSONDecodeError as exc:
        print(f"  (invalid JSON in {path}: {exc})")
        return {}


def src_frame(backtrace: list) -> str:
    """Return 'function (file:line)' for the first app source frame."""
    for frame in backtrace:
        fi = frame.get("file", "")
        if "runner/work/" in fi or fi.startswith("/app/"):
            name = frame.get("function", "?")
            short = fi.split("/")[-1]
            line = frame.get("line", "?")
            return f"{name} ({short}:{line})"
    return "(non-src)"


def summarize_pass1(path: str) -> bool:
    data = load_json(path)
    sites = data.get("malloc-sites", [])
    passed = sum(1 for s in sites if s.get("null-check"))
    total = len(sites)
    ec = data.get("exit-code", "N/A")
    ok = ec == 0
    mark = "PASS" if ok else "FAIL"
    print(f"\n=== Pass 1: malloc site coverage [{mark}] ===")
    print(f"  exit-code  : {ec}")
    print(f"  coverage   : {passed}/{total} sites null-checked")
    if not ok:
        for site in sites:
            if not site.get("null-check"):
                frame = (site.get("backtrace") or [{}])[0]
                fn = frame.get("function", "?")
                fi = frame.get("file", "?").split("/")[-1]
                ln = frame.get("line", "?")
                print(f"  UNCHECKED: {fn} ({fi}:{ln})")
    return ok


def summarize_pass2(path: str) -> bool:
    data = load_json(path)
    nf = data.get("allocations-not-freed", [])
    crashes = data.get("crashes", [])
    ec = data.get("exit-code", "N/A")
    ok = ec == 0 and not crashes
    mark = "PASS" if ok else "FAIL"
    print(f"\n=== Pass 2: allocation tracking [{mark}] ===")
    print(f"  exit-code             : {ec}")
    print(f"  allocations-not-freed : {len(nf)}")
    print(f"  crashes               : {len(crashes)}")
    if crashes:
        print("  CRASH DETAILS:")
        for crash in crashes[:3]:
            bt = crash.get("backtrace", [])
            for frame in bt[:4]:
                fi = frame.get("file", "?").split("/")[-1]
                print(f"    {frame.get('function','?')} ({fi}:{frame.get('line','?')})")
    if nf:
        counts: Counter = Counter()
        for entry in nf:
            counts[src_frame(entry.get("backtrace", []))] += 1
        print("  Allocation call sites (top 10):")
        for site, count in counts.most_common(10):
            print(f"    {count:3}x  {site}")
    return ok


def main() -> int:
    if len(sys.argv) != 3:
        print("Usage: funcheck_summary.py <pass1.json> <pass2.json>", file=sys.stderr)
        return 2
    p1_ok = summarize_pass1(sys.argv[1])
    p2_ok = summarize_pass2(sys.argv[2])
    print()
    if p1_ok and p2_ok:
        print("Overall: PASS")
        return 0
    print("Overall: FAIL")
    return 1


if __name__ == "__main__":
    sys.exit(main())
