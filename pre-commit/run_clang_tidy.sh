#!/usr/bin/env bash
# -------------------------------------------------------------------------
#  This file is part of the Vision SDK project.
# Copyright (c) 2025 Huawei Technologies Co.,Ltd.
#
# Vision SDK is licensed under Mulan PSL v2.
# -------------------------------------------------------------------------
# Description: clang-tidy pre-commit wrapper.
#   Resolves the problem of compile_commands.json being scattered across
#   4 component build directories while .clang-tidy is at the project root.
#
#   Priority:
#     1. Merged compile_commands.json at project root  -->  -p=.
#     2. Per-component compile_commands.json           -->  -p=<comp>/build_result/<SYSTEM>
#     3. Neither exists                                -->  warn, skip, exit 0
# -------------------------------------------------------------------------

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
CLANG_TIDY_CFG="${ROOT_DIR}/.clang-tidy"
MERGED_DB="${ROOT_DIR}/compile_commands.json"

# Available platforms (try in order)
PLATFORMS=("arm-gcc4" "x86-gcc4")

# ---- helper functions ----

detect_platform() {
    for plat in "${PLATFORMS[@]}"; do
        if [[ -f "${ROOT_DIR}/mxBase/build_result/${plat}/compile_commands.json" ]]; then
            echo "${plat}"
            return 0
        fi
    done
    echo ""
    return 1
}

# Map a source file path to its component name
get_component() {
    local file="$1"
    case "${file}" in
        */mxBase/*)     echo "mxBase"    ;;
        */mxTools/*)    echo "mxTools"   ;;
        */mxStream/*)   echo "mxStream"  ;;
        */mxPlugins/*)  echo "mxPlugins" ;;
        *)              echo ""          ;;
    esac
}

run_clang_tidy() {
    local db_dir="$1"
    shift
    local files=("$@")
    local extra_args=()

    # If db_dir is NOT the root, explicitly point to .clang-tidy
    if [[ "${db_dir}" != "${ROOT_DIR}" ]]; then
        extra_args+=("--config-file=${CLANG_TIDY_CFG}")
    fi

    clang-tidy "${files[@]}" \
        -p="${db_dir}" \
        --quiet \
        --header-filter='^$' \
        "${extra_args[@]}"
}

# ---- filter input to only .cpp/.cc/.cxx/.c/.h/.hpp files ----
FILES=()
for arg in "$@"; do
    if [[ -f "${arg}" ]]; then
        case "${arg}" in
            *.cpp|*.cc|*.cxx|*.c|*.h|*.hpp)
                FILES+=("${arg}")
                ;;
        esac
    fi
done

if [[ ${#FILES[@]} -eq 0 ]]; then
    exit 0
fi

EXIT_CODE=0

# ======================================================================
# Strategy 1: Use merged compile_commands.json at project root
# ======================================================================
if [[ -f "${MERGED_DB}" ]]; then
    echo "[clang-tidy] using merged compile_commands.json at project root"
    run_clang_tidy "${ROOT_DIR}" "${FILES[@]}" || EXIT_CODE=1
    exit ${EXIT_CODE}
fi

# ======================================================================
# Strategy 2: Per-component compile_commands.json
# ======================================================================
PLATFORM=$(detect_platform)
if [[ -z "${PLATFORM}" ]]; then
    echo "[clang-tidy] WARNING: No compile_commands.json found."
    echo "[clang-tidy] Please run 'bash build_all.sh <system> <platform> notest' first to generate it."
    echo "[clang-tidy] Skipping clang-tidy checks."
    exit 0
fi

# Group files by component
declare -A COMP_FILES
UNMATCHED=()

for f in "${FILES[@]}"; do
    comp=$(get_component "${f}")
    if [[ -n "${comp}" ]]; then
        COMP_FILES["${comp}"]="${COMP_FILES[${comp}]:-} ${f}"
    else
        UNMATCHED+=("${f}")
    fi
done

for comp in "${!COMP_FILES[@]}"; do
    db_dir="${ROOT_DIR}/${comp}/build_result/${PLATFORM}"
    if [[ ! -f "${db_dir}/compile_commands.json" ]]; then
        echo "[clang-tidy] WARNING: compile_commands.json not found at ${db_dir}, skipping ${comp}"
        continue
    fi
    echo "[clang-tidy] checking ${comp} files (platform=${PLATFORM})"
    # shellcheck disable=SC2086
    read -ra comp_files <<< "${COMP_FILES[${comp}]}"
    run_clang_tidy "${db_dir}" "${comp_files[@]}" || EXIT_CODE=1
done

if [[ ${#UNMATCHED[@]} -gt 0 ]]; then
    echo "[clang-tidy] WARNING: Could not determine component for: ${UNMATCHED[*]}"
fi

exit ${EXIT_CODE}
