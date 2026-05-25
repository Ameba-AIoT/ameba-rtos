"""
MCP resources for board_info.json5.

Exposes:
  board://list             — list of configured aliases
  board://{alias}           — single resolved board (with defaults applied,
                              password masked)
"""

from __future__ import annotations

import json
from typing import Any, Dict

from mcp.server.fastmcp import FastMCP

from ameba_dev_mcp._paths import SDK_ROOT
from ameba_dev_mcp.config.loader import (
    ConfigLoadError,
    ensure_board_info_template,
    load_board_info,
    masked_board_info_dict,
    resolve_board,
)


_PASSWORD_MASK = "***"


def _safe_load() -> Dict[str, Any]:
    """Load board_info.json5 or return a structured error envelope."""
    try:
        info = load_board_info(SDK_ROOT)
    except ConfigLoadError as ex:
        # Side effect: ensure template exists so the user has somewhere
        # to start from on first read.
        try:
            template_path = ensure_board_info_template(SDK_ROOT)
        except Exception:
            template_path = None
        return {
            "error": True,
            "errors": [e.model_dump() for e in ex.errors],
            "template_path": template_path,
            "docs_url": "docs/board_info.md",
            "resource_url": "debug://hardware",
            "hint": (
                "board_info.json5 is missing or invalid. A template has "
                "been created if it didn't exist; fill in the `boards` "
                "section per docs/board_info.md, or read MCP resource "
                "`debug://hardware` for board-wiring and serial-driver "
                "requirements."
            ),
        }
    return {"error": False, "info": info}


def register_board_resources(mcp: FastMCP) -> None:
    @mcp.resource("board://list")
    def list_boards() -> str:
        """List configured board aliases (with the SoC + transport for each).

        AI agents read this resource first to discover which alias to pass
        to flash / serial tools.
        """
        env = _safe_load()
        if env["error"]:
            return json.dumps(env, indent=2)
        info = env["info"]
        aliases = sorted(info.boards.keys())
        summary = []
        for a in aliases:
            entry = info.boards[a]
            summary.append({
                "alias": a,
                "soc": entry.soc,
                "transport": entry.transport,
                "port": entry.port,
                "remote_host": entry.remote.host if entry.remote else None,
            })
        return json.dumps({
            "count": len(aliases),
            "aliases": aliases,
            "boards": summary,
        }, indent=2)

    @mcp.resource("board://{alias}")
    def get_board(alias: str) -> str:
        """Get one board's resolved configuration (defaults applied, password masked)."""
        env = _safe_load()
        if env["error"]:
            return json.dumps(env, indent=2)
        info = env["info"]
        if alias not in info.boards:
            return json.dumps({
                "error": True,
                "code": "ALIAS_NOT_FOUND",
                "alias": alias,
                "available_aliases": sorted(info.boards.keys()),
                "hint": "Add this alias under `boards` in board_info.json5, or use one of the available aliases.",
            }, indent=2)
        resolved = resolve_board(info, alias)
        data = resolved.model_dump()
        if data.get("remote") and data["remote"].get("password"):
            data["remote"]["password"] = _PASSWORD_MASK
        return json.dumps(data, indent=2)
