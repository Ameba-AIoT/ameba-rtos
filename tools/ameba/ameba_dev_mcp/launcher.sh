#!/usr/bin/env bash
# Wrapper for `claude mcp add ameba-dev`. Injects prebuilts toolchain PATH
# so MCP server has cmake/ninja regardless of Claude Code's launch env.

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SDK_ROOT="$(cd "$SCRIPT_DIR/../../.." && pwd)"

# Parse (and CONSUME) --project-root: it must NOT be forwarded to ameba-mcp,
# whose argparse only knows --transport/--port/--host/--debug. When present we
# export AMEBA_PROJECT_ROOT (absolute), which _paths.py reads to point the
# build/flash/config layer at an out-of-tree project. Everything else is
# passed through to the server unchanged.
PROJECT_ROOT_ARG=""
REST_ARGS=()
while [ $# -gt 0 ]; do
    case "$1" in
        --project-root)
            if [ -z "${2:-}" ]; then
                echo "[ameba-mcp launcher] --project-root requires a value" >&2
                exit 1
            fi
            PROJECT_ROOT_ARG="$2"; shift 2 ;;
        --project-root=*)
            PROJECT_ROOT_ARG="${1#*=}"; shift ;;
        *)
            REST_ARGS+=("$1"); shift ;;
    esac
done
if [ -n "$PROJECT_ROOT_ARG" ]; then
    if [[ "${PROJECT_ROOT_ARG:0:1}" == "~" ]]; then
        PROJECT_ROOT_ARG="$HOME${PROJECT_ROOT_ARG:1}"
    fi
    # Resolve to an absolute path; the server's cwd is not guaranteed to be
    # the workspace root, so a relative value would be ambiguous. Force absolute
    # first so even the realpath-less fallback can never export a relative path
    # (it would otherwise echo the original value verbatim for a not-yet-created
    # project dir).
    case "$PROJECT_ROOT_ARG" in
        /*) ;;
        *) PROJECT_ROOT_ARG="$(pwd)/$PROJECT_ROOT_ARG" ;;
    esac
    if command -v realpath >/dev/null 2>&1; then
        PROJECT_ROOT_ARG="$(realpath -m "$PROJECT_ROOT_ARG")"
    else
        PROJECT_ROOT_ARG="$(cd "$PROJECT_ROOT_ARG" 2>/dev/null && pwd || echo "$PROJECT_ROOT_ARG")"
    fi
    export AMEBA_PROJECT_ROOT="$PROJECT_ROOT_ARG"
    echo "[ameba-mcp launcher] AMEBA_PROJECT_ROOT=$AMEBA_PROJECT_ROOT" >&2
fi

PREBUILTS_VERSION="1.0.3"
if [ -n "$RTK_TOOLCHAIN_DIR" ]; then
    if [[ "${RTK_TOOLCHAIN_DIR:0:1}" == "~" ]]; then
        RTK_TOOLCHAIN_DIR="$HOME${RTK_TOOLCHAIN_DIR:1}"
    fi
else
    RTK_TOOLCHAIN_DIR="$HOME/rtk-toolchain"
fi
PREBUILTS_DIR="$RTK_TOOLCHAIN_DIR/prebuilts-linux-$PREBUILTS_VERSION"
MCP_BIN="$SDK_ROOT/.venv/bin/ameba-mcp"

if [ ! -f "$PREBUILTS_DIR/setenv.sh" ]; then
    echo "[ameba-mcp launcher] prebuilts not found at $PREBUILTS_DIR" >&2
    echo "[ameba-mcp launcher] run 'source $SDK_ROOT/env.sh' once to install it" >&2
    exit 1
fi
if [ ! -x "$MCP_BIN" ]; then
    echo "[ameba-mcp launcher] $MCP_BIN missing — run 'source $SDK_ROOT/env.sh' to set up venv" >&2
    exit 1
fi

# shellcheck disable=SC1091
source "$PREBUILTS_DIR/setenv.sh" >&2
# shellcheck disable=SC1091
source "$SDK_ROOT/.venv/bin/activate" >&2
exec "$MCP_BIN" "${REST_ARGS[@]}"
