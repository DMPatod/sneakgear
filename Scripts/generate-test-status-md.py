#!/usr/bin/env python3
from __future__ import annotations

import json
import sys
from pathlib import Path


def load_report(path: Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8-sig"))


def short_state(state: str) -> str:
    if state == "Success":
        return "Passed"
    if state == "Fail":
        return "Failed"
    if state == "SuccessWithWarnings":
        return "Passed with warnings"
    return state


def collect_messages(entries: list[dict], event_type: str) -> list[str]:
    messages: list[str] = []
    for entry in entries:
        event = entry.get("event", {})
        if event.get("type") == event_type:
            message = str(event.get("message", "")).strip()
            if message:
                messages.append(message)
    return messages


def build_markdown(report: dict) -> str:
    lines: list[str] = []
    lines.append("# Unreal Automation Test Status")
    lines.append("")
    lines.append("## Summary")
    lines.append("")
    lines.append(f"- Failed: {report.get('failed', 0)}")
    lines.append(f"- Succeeded: {report.get('succeeded', 0)}")
    lines.append(f"- Succeeded with warnings: {report.get('succeededWithWarnings', 0)}")
    lines.append(f"- Not run: {report.get('notRun', 0)}")
    lines.append("")
    lines.append("## Tests")
    lines.append("")
    lines.append("| Test | Status | Duration (s) | Warnings | Errors |")
    lines.append("| --- | --- | ---: | ---: | ---: |")

    for test in sorted(report.get("tests", []), key=lambda item: item.get("fullTestPath", "")):
        lines.append(
            f"| `{test.get('fullTestPath', '')}` | {short_state(test.get('state', 'Unknown'))} | "
            f"{float(test.get('duration', 0.0)):.3f} | {int(test.get('warnings', 0))} | {int(test.get('errors', 0))} |"
        )

    detailed_tests = [
        test for test in report.get("tests", [])
        if collect_messages(test.get("entries", []), "Warning") or collect_messages(test.get("entries", []), "Error")
    ]
    if detailed_tests:
        lines.append("")
        lines.append("## Details")
        lines.append("")
        for test in detailed_tests:
            lines.append(f"### `{test.get('fullTestPath', '')}`")
            error_messages = collect_messages(test.get("entries", []), "Error")
            warning_messages = collect_messages(test.get("entries", []), "Warning")
            if warning_messages:
                lines.append("")
                lines.append("Warnings:")
                for message in warning_messages:
                    lines.append(f"- {message}")
            if error_messages:
                lines.append("")
                lines.append("Errors:")
                for message in error_messages:
                    lines.append(f"- {message}")
            lines.append("")

    return "\n".join(lines).rstrip() + "\n"


def main() -> int:
    if len(sys.argv) != 3:
        print("Usage: generate-test-status-md.py <index.json> <output.md>", file=sys.stderr)
        return 1

    report_path = Path(sys.argv[1])
    output_path = Path(sys.argv[2])

    if not report_path.is_file():
        print(f"Report file not found: {report_path}", file=sys.stderr)
        return 1

    report = load_report(report_path)
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(build_markdown(report), encoding="utf-8")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
