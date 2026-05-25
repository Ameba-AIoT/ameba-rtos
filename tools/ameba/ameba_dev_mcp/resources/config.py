"""
MCP resources for project_info.json5 and board_info.json5 raw contents
(post-validation, defaults merged, passwords masked).

URIs:
  config://project_info     — full ProjectInfo (auto / manual entries)
  config://board_info       — full BoardInfo (passwords masked)
"""

from __future__ import annotations

import json

from mcp.server.fastmcp import FastMCP

from ameba_dev_mcp._paths import SDK_ROOT
from ameba_dev_mcp.config.loader import (
    ConfigLoadError,
    load_board_info,
    load_project_info,
    masked_board_info_dict,
    masked_project_info_dict,
)


def _err_envelope(ex: ConfigLoadError) -> str:
    return json.dumps({
        "error": True,
        "errors": [e.model_dump() for e in ex.errors],
    }, indent=2)


def register_config_resources(mcp: FastMCP) -> None:
    @mcp.resource("config://project_info")
    def get_project_info() -> str:
        """Full project_info.json5 contents, validated and merged."""
        try:
            info = load_project_info(SDK_ROOT)
        except ConfigLoadError as ex:
            return _err_envelope(ex)
        return json.dumps(masked_project_info_dict(info), indent=2)

    @mcp.resource("config://board_info")
    def get_board_info() -> str:
        """Full board_info.json5 contents (passwords masked as '***')."""
        try:
            info = load_board_info(SDK_ROOT)
        except ConfigLoadError as ex:
            return _err_envelope(ex)
        return json.dumps(masked_board_info_dict(info), indent=2)
