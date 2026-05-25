#!/usr/bin/env bash
# Wrapper for `claude mcp add ameba-dev`. Injects prebuilts toolchain PATH
# so MCP server has cmake/ninja regardless of Claude Code's launch env.

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SDK_ROOT="$(cd "$SCRIPT_DIR/../../.." && pwd)"

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
exec "$MCP_BIN" "$@"
