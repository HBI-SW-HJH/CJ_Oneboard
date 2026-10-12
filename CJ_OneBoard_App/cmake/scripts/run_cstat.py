#!/usr/bin/env python3
"""C-STAT static analysis runner using compile_commands.json.

Wraps each iccarm invocation with icstat to perform static analysis
on all (or selected) compilation units. Results accumulate in a SQLite
database; an HTML report is generated via ireport.

All outputs (log, DB, HTML report, JSON summary) are stored in a
dedicated report directory (default: cstat-reports/). Each run clears
the directory first, so only the latest results are kept.

Usage:
    python cmake/scripts/run_cstat.py [options]

Examples:
    # Full project analysis (default: exclude vendor/extern)
    python cmake/scripts/run_cstat.py

    # Analyze only comsect1 project files (no deps)
    python cmake/scripts/run_cstat.py --include "*/project/*"

    # Analyze a single feature
    python cmake/scripts/run_cstat.py --include "*safety*"

    # Fresh analysis with clean database
    python cmake/scripts/run_cstat.py --clean
"""

import argparse
import json
import os
import shlex
import shutil
import subprocess
import sys
import time
from datetime import datetime
from fnmatch import fnmatch
from pathlib import Path


def find_iar_path():
    """Resolve IAR EWARM installation path (same logic as iar.toolchain.cmake)."""
    env_path = os.environ.get("IAR_EWARM_PATH")
    if env_path and os.path.isdir(env_path):
        return Path(env_path)

    iar_base = Path("C:/iar")
    if iar_base.is_dir():
        candidates = sorted(iar_base.glob("ewarm-*"), reverse=True)
        if candidates:
            return candidates[0]

    return None


def match_any(filepath, patterns):
    """Check if filepath matches any glob pattern in the list."""
    normalized = filepath.replace("\\", "/")
    return any(fnmatch(normalized, p.strip()) for p in patterns)


class TeeWriter:
    """Write to both stdout and a log file simultaneously."""

    def __init__(self, log_path):
        self._log = open(log_path, "w", encoding="utf-8")
        self._stdout = sys.stdout

    def write(self, text):
        self._stdout.write(text)
        self._log.write(text)

    def flush(self):
        self._stdout.flush()
        self._log.flush()

    def close(self):
        self._log.close()


def parse_args():
    p = argparse.ArgumentParser(
        description="C-STAT static analysis via compile_commands.json"
    )
    p.add_argument(
        "--build-dir", default=".cmakeBuild-release",
        help="Build directory containing compile_commands.json (default: .cmakeBuild-release)"
    )
    p.add_argument(
        "--checks", default="cmake/cstat_checks.txt",
        help="Checks manifest file (default: cmake/cstat_checks.txt)"
    )
    p.add_argument(
        "--report-dir", default="documents/analysis-reports/cstat",
        help="Report output directory (default: documents/analysis-reports/cstat/)"
    )
    p.add_argument(
        "--include", default=None,
        help="Only analyze files matching glob (e.g. '*safety*', '*/project/*')"
    )
    p.add_argument(
        "--exclude", default="*/extern/*,*/CJ_4DPLEX*",
        help="Exclude files matching globs, comma-separated "
             "(default: '*/extern/*,*/CJ_4DPLEX*')"
    )
    p.add_argument(
        "--parallel", type=int, default=0,
        help="icstat parallel threads per file (0 = icstat default)"
    )
    p.add_argument(
        "--suppress-headers",
        default="*CJ_4DPLEX*,*packs*,*CMSIS*,*deps/extern*,*deps?extern*",
        help="Suppress findings from headers matching these globs "
             "(default: '*CJ_4DPLEX*,*packs*,*CMSIS*,*deps/extern*')"
    )
    p.add_argument(
        "--clean", action="store_true",
        help="Clear database before analysis"
    )
    p.add_argument(
        "--no-report", action="store_true",
        help="Skip HTML report generation"
    )
    return p.parse_args()


def main():
    args = parse_args()

    # Resolve repo root from script location: cmake/scripts/run_cstat.py -> repo root
    repo_root = Path(__file__).resolve().parent.parent.parent
    os.chdir(repo_root)

    # Resolve IAR tools
    iar_path = find_iar_path()
    if not iar_path:
        print("ERROR: IAR EWARM not found. Set IAR_EWARM_PATH.", file=sys.stderr)
        sys.exit(2)

    icstat = iar_path / "arm" / "bin" / "icstat.exe"
    ireport_tool = iar_path / "arm" / "bin" / "ireport.exe"

    if not icstat.is_file():
        print(f"ERROR: icstat not found at {icstat}", file=sys.stderr)
        sys.exit(2)

    # Resolve input files
    checks_file = Path(args.checks).resolve()
    if not checks_file.is_file():
        print(f"ERROR: Checks manifest not found: {checks_file}", file=sys.stderr)
        sys.exit(2)

    cc_path = Path(args.build_dir) / "compile_commands.json"
    if not cc_path.is_file():
        print(f"ERROR: {cc_path} not found. Run cmake configure first.", file=sys.stderr)
        sys.exit(2)

    with open(cc_path, encoding="utf-8") as f:
        compile_db = json.load(f)

    # Filter compilation units
    exclude_patterns = [p.strip() for p in args.exclude.split(",")] if args.exclude else []

    entries = []
    skipped = 0
    for entry in compile_db:
        fp = entry["file"].replace("\\", "/")
        if exclude_patterns and match_any(fp, exclude_patterns):
            skipped += 1
            continue
        if args.include and not match_any(fp, [args.include]):
            skipped += 1
            continue
        entries.append(entry)

    if not entries:
        print("No files matched the filter criteria.")
        sys.exit(0)

    # --- Report directory setup: clean and recreate ---
    report_dir = Path(args.report_dir).resolve()
    if report_dir.is_dir():
        shutil.rmtree(report_dir)
    report_dir.mkdir(parents=True, exist_ok=True)

    db_file = report_dir / "cstat.db"
    report_html = report_dir / "cstat-report.html"
    log_file = report_dir / "cstat-log.txt"
    summary_file = report_dir / "cstat-summary.json"

    # Tee stdout to log file
    tee = TeeWriter(str(log_file))
    sys.stdout = tee

    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    print(f"C-STAT Analysis  ({timestamp})")
    print(f"  IAR:      {iar_path.name}")
    print(f"  Files:    {len(entries)} selected, {skipped} excluded")
    print(f"  Checks:   {checks_file.name}")
    print(f"  Output:   {report_dir}")
    if args.include:
        print(f"  Filter:   --include {args.include}")
    print()

    # Clear database if requested
    if args.clean and db_file.is_file():
        subprocess.run(
            [str(icstat), "--db", str(db_file), "clear"],
            check=False, capture_output=True
        )

    # Build suppress patterns for vendor/DFP header findings
    suppress_patterns = []
    if args.suppress_headers:
        suppress_patterns = [p.strip() for p in args.suppress_headers.split(",")]

    # Analyze each compilation unit
    total_findings = 0
    suppressed_findings = 0
    severity_counts = {"High": 0, "Medium": 0, "Low": 0}
    files_with_findings = {}
    all_findings = []
    errors = 0
    start_time = time.time()

    for i, entry in enumerate(entries, 1):
        filepath = entry["file"]
        filename = Path(filepath).name

        # Parse compiler command from compile_commands.json
        cmd_str = entry["command"]
        try:
            cmd_parts = shlex.split(cmd_str, posix=False)
        except ValueError:
            cmd_parts = cmd_str.split()

        # Build icstat invocation
        icstat_cmd = [
            str(icstat),
            "--db", str(db_file),
            "--checks", str(checks_file),
        ]
        if args.parallel > 0:
            icstat_cmd.extend(["--parallel", str(args.parallel)])
        icstat_cmd.append("analyze")
        icstat_cmd.append("--")
        icstat_cmd.extend(cmd_parts)

        # Execute
        try:
            result = subprocess.run(
                icstat_cmd,
                cwd=entry.get("directory", "."),
                capture_output=True,
                text=True,
                timeout=300,
            )
        except subprocess.TimeoutExpired:
            print(f"  [{i}/{len(entries)}] {filename}: TIMEOUT")
            errors += 1
            continue
        except OSError as e:
            print(f"  [{i}/{len(entries)}] {filename}: ERROR ({e})")
            errors += 1
            continue

        # Parse findings from stdout, suppress vendor header noise
        findings = []
        for line in result.stdout.splitlines():
            line = line.strip()
            if not line:
                continue
            # icstat output: "file",line Severity-Level[check]:message
            if "Severity-" not in line:
                continue

            # Extract the file path from the finding (between quotes)
            if line.startswith('"'):
                finding_file = line.split('"')[1].replace("\\", "/")
                if suppress_patterns and match_any(finding_file, suppress_patterns):
                    suppressed_findings += 1
                    continue

            findings.append(line)
            all_findings.append({"source": filename, "raw": line})
            total_findings += 1
            for sev in severity_counts:
                if f"Severity-{sev}" in line:
                    severity_counts[sev] += 1
                    break

        # Display progress
        tag = f"[{i}/{len(entries)}]"
        if findings:
            files_with_findings[filename] = findings
            print(f"  {tag} {filename}: {len(findings)} finding(s)")
            for finding in findings:
                print(f"      {finding}")
        else:
            # Compact output for clean files
            print(f"  {tag} {filename}: clean")

        # Report icstat errors (not compiler warnings)
        if result.returncode != 0 and result.stderr:
            for line in result.stderr.splitlines():
                stripped = line.strip()
                if stripped and "Analyzing" not in stripped:
                    print(f"      WARN: {stripped}")

    elapsed = time.time() - start_time

    # Summary
    print()
    print("=" * 65)
    print(f"  Analysis complete: {len(entries)} files in {elapsed:.1f}s")
    print(f"  Total findings:    {total_findings}")
    if suppressed_findings > 0:
        print(f"  Suppressed (vendor headers): {suppressed_findings}")
    if total_findings > 0:
        print(f"    High:   {severity_counts['High']}")
        print(f"    Medium: {severity_counts['Medium']}")
        print(f"    Low:    {severity_counts['Low']}")
        print(f"  Files with findings: {len(files_with_findings)}/{len(entries)}")
    else:
        print("  All files clean!")
    if errors:
        print(f"  Errors/timeouts: {errors}")
    print(f"  Report dir: {report_dir}")
    print("=" * 65)

    # Generate HTML report
    if not args.no_report and ireport_tool.is_file() and db_file.is_file():
        print(f"\nGenerating HTML report: {report_html.name}")
        subprocess.run(
            [
                str(ireport_tool),
                "--db", str(db_file),
                "--project", "app_moodLamp_rgbi",
                "--full",
                "--output", str(report_html),
            ],
            check=False,
            capture_output=True,
        )
        if report_html.is_file():
            print(f"Report: {report_html}")

    # Write machine-readable summary for AIAD consumption
    summary = {
        "timestamp": timestamp,
        "iar_version": iar_path.name,
        "checks_manifest": str(checks_file),
        "files_analyzed": len(entries),
        "files_excluded": skipped,
        "filter": args.include,
        "elapsed_seconds": round(elapsed, 1),
        "total_findings": total_findings,
        "suppressed_findings": suppressed_findings,
        "severity": severity_counts,
        "files_with_findings": {
            k: len(v) for k, v in files_with_findings.items()
        },
        "errors": errors,
        "findings": all_findings,
    }
    with open(summary_file, "w", encoding="utf-8") as f:
        json.dump(summary, f, indent=2, ensure_ascii=False)
    print(f"Summary: {summary_file}")

    # Restore stdout and close log
    sys.stdout = tee._stdout
    tee.close()

    sys.exit(0 if total_findings == 0 else 1)


if __name__ == "__main__":
    main()
