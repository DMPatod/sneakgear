#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

rm -rf \
  "${ROOT_DIR}/Binaries" \
  "${ROOT_DIR}/Build" \
  "${ROOT_DIR}/DerivedDataCache" \
  "${ROOT_DIR}/Intermediate" \
  "${ROOT_DIR}/Saved"

echo "Cleaned Unreal generated folders in ${ROOT_DIR}"
