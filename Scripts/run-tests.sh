#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PROJECT_PATH="${ROOT_DIR}/SneakGear.uproject"
REPORT_DIR="${ROOT_DIR}/Docs/AutomationReports"
REPORT_JSON="${REPORT_DIR}/index.json"
REPORT_MD="${REPORT_DIR}/TestStatus.md"
TEST_FILTER="SneakGear."
SKIP_BUILD=0

usage() {
  cat <<'EOF'
Usage:
  Scripts/run-tests.sh [TestFilter] [--no-build]

Examples:
  Scripts/run-tests.sh
  Scripts/run-tests.sh SneakGear.Inventory.
  Scripts/run-tests.sh SneakGear.Inventory.PlayerInventoryComponent.CannotSelectEmptyWeaponSlot --no-build

Environment overrides:
  UE_EDITOR_BIN  Full path to the UnrealEditor binary
  UE_BUILD_SH    Full path to Unreal's Build.sh
EOF
}

generate_markdown_report() {
  if [[ -f "${REPORT_JSON}" ]]; then
    python3 "${ROOT_DIR}/Scripts/generate-test-status-md.py" "${REPORT_JSON}" "${REPORT_MD}" || true
  fi
}

resolve_editor_bin() {
  if [[ -n "${UE_EDITOR_BIN:-}" ]]; then
    printf '%s\n' "${UE_EDITOR_BIN}"
    return 0
  fi

  local candidates=(
    "/Users/Shared/Epic Games/UE_5.7/Engine/Binaries/Mac/UnrealEditor.app/Contents/MacOS/UnrealEditor"
    "/Users/Shared/Epic Games/UE_5.6/Engine/Binaries/Mac/UnrealEditor.app/Contents/MacOS/UnrealEditor"
    "/Users/Shared/Epic Games/UE_5.5/Engine/Binaries/Mac/UnrealEditor.app/Contents/MacOS/UnrealEditor"
  )

  local candidate
  for candidate in "${candidates[@]}"; do
    if [[ -x "${candidate}" ]]; then
      printf '%s\n' "${candidate}"
      return 0
    fi
  done

  return 1
}

resolve_build_sh() {
  if [[ -n "${UE_BUILD_SH:-}" ]]; then
    printf '%s\n' "${UE_BUILD_SH}"
    return 0
  fi

  local editor_bin
  editor_bin="$(resolve_editor_bin)" || return 1
  local engine_root
  engine_root="$(cd "$(dirname "${editor_bin}")/../../../../.." && pwd)"
  printf '%s\n' "${engine_root}/Build/BatchFiles/Mac/Build.sh"
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --no-build)
      SKIP_BUILD=1
      shift
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      TEST_FILTER="$1"
      shift
      ;;
  esac
done

EDITOR_BIN="$(resolve_editor_bin)" || {
  echo "Unable to find UnrealEditor. Set UE_EDITOR_BIN to the full binary path." >&2
  exit 1
}

mkdir -p "${REPORT_DIR}"

if [[ "${SKIP_BUILD}" -eq 0 ]]; then
  BUILD_SH="$(resolve_build_sh)" || {
    echo "Unable to find Unreal Build.sh. Set UE_BUILD_SH to the full script path." >&2
    exit 1
  }

  if [[ ! -x "${BUILD_SH}" ]]; then
    echo "Build script is not executable: ${BUILD_SH}" >&2
    exit 1
  fi

  echo "Building SneakGearEditor..."
  "${BUILD_SH}" SneakGearEditor Mac Development "${PROJECT_PATH}" -NoHotReload
fi

echo "Running automation tests: ${TEST_FILTER}"
set +e
"${EDITOR_BIN}" \
  "${PROJECT_PATH}" \
  -ExecCmds="Automation RunTests ${TEST_FILTER}" \
  -TestExit="Automation Test Queue Empty" \
  -Unattended \
  -NullRHI \
  -NoSplash \
  -NoSound \
  -ReportExportPath="${REPORT_DIR}" \
  -Log
TEST_EXIT_CODE=$?
set -e

generate_markdown_report

if [[ -f "${REPORT_MD}" ]]; then
  echo "Markdown report written to ${REPORT_MD}"
fi

exit "${TEST_EXIT_CODE}"
