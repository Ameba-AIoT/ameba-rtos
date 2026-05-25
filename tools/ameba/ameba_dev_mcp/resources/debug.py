"""
MCP resource for hardware-side troubleshooting.

URI:
  debug://hardware    — markdown reference for serial driver versions,
                        auto-download circuit, RTS/DTR signal timing,
                        common failure patterns, and an error-code →
                        section index.

This resource is reference-only. It is NOT loaded into the MCP system
prompt by default; callers fetch it explicitly when an error envelope
points here (look for `resource_url: "debug://hardware"`).
"""

from __future__ import annotations

from pathlib import Path

from mcp.server.fastmcp import FastMCP

from ameba_dev_mcp import _paths

_HARDWARE_DEBUG_PATH = (
    Path(_paths._MCPSCRIPTS_ROOT) / "docs" / "hardware_debug.md"
)


def register_debug_resources(mcp: FastMCP) -> None:
    @mcp.resource("debug://hardware")
    def hardware_debug() -> str:
        """Hardware-side troubleshooting (PL2303 driver, auto-download
        circuit, RTS/DTR timing, failure patterns, error-code index).
        """
        try:
            return _HARDWARE_DEBUG_PATH.read_text(encoding="utf-8")
        except OSError as ex:
            return (
                f"# debug://hardware\n\n"
                f"Failed to read {_HARDWARE_DEBUG_PATH}: {ex}\n"
                f"This is an MCP packaging issue; the docs file should "
                f"be shipped alongside the server.\n"
            )
