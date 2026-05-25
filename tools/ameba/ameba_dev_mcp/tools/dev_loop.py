"""
High-level dev-loop helper for Ameba SoCs.

Provides `quick_test()`: a Python helper that composes existing
primitives (build_firmware → flash_firmware → serial_connect with reset
+ wait → optional auto-disconnect). It is NOT a black box: every stage
calls into the same code paths the lower-level tools use, so a failure
at any stage produces the same structured error as if you had called
that tool directly. Stage attribution is added on top.

NOTE: As of 2026-05-19 quick_test is no longer exposed as an MCP tool
(see docs/tool_design_review.md §3 — high-level "magic button" tools
don't fit agent reasoning patterns). The function below remains
importable for shell scripts / CI use.
"""

from __future__ import annotations

import logging
from typing import Any, Dict, List, Optional

from mcp.server.fastmcp import FastMCP

from ameba_dev_mcp._paths import SDK_ROOT
from ameba_dev_mcp.config.loader import (
    ConfigLoadError,
    load_board_info,
    resolve_alias_or_error,
    resolve_board,
)
from ameba_dev_mcp.tools.flash import flash_firmware
from ameba_dev_mcp.tools.project import (
    _read_current_soc,
    _write_soc_info,
    run_build_quiet,
)
from ameba_dev_mcp.tools.serial import (
    serial_connect,
    serial_disconnect,
)

logger = logging.getLogger(__name__)


def _resolve_alias_soc(alias):
    """Best-effort: alias → (resolved_alias, board.soc).

    Returns ((resolved_alias, soc), None) on success or
    (None, error_envelope) on failure. envelope is staged as "validate"
    so the caller can short-circuit before build. `alias=None` is
    accepted only when board_info has exactly one board.
    """
    try:
        binfo = load_board_info(SDK_ROOT)
    except ConfigLoadError as ex:
        env: Dict[str, Any] = {
            "success": False,
            "stage": "validate",
            "alias": alias,
            "errors": [e.model_dump() for e in ex.errors],
            "closed_after": False,
        }
        # First-time-missing path: create the empty template so the user
        # has a concrete file to edit, and surface where to learn what
        # to put in it. Mirrors flash.py / serial.py behavior.
        if any(e.code == "BOARD_CONFIG_MISSING" for e in ex.errors):
            from ameba_dev_mcp.config.loader import ensure_board_info_template
            try:
                env["template_path"] = ensure_board_info_template(SDK_ROOT)
                env["docs_url"] = "docs/board_info.md"
                env["resource_url"] = "debug://hardware"
            except Exception:
                pass
        return None, env
    resolved_alias, alias_errors = resolve_alias_or_error(binfo, alias)
    if alias_errors is not None:
        return None, {
            "success": False,
            "stage": "validate",
            "alias": alias,
            "errors": [e.model_dump() for e in alias_errors],
            "closed_after": False,
        }
    return (resolved_alias, resolve_board(binfo, resolved_alias).soc), None


def _failed(stage: str, alias: str, detail: Dict[str, Any],
            *, closed_after: bool = False) -> Dict[str, Any]:
    """Wrap a primitive's failure return into the staged envelope."""
    out: Dict[str, Any] = {
        "success": False,
        "alias": alias,
        "stage": stage,
        "closed_after": closed_after,
    }
    # Carry over whichever fields the underlying primitive populated.
    for key in ("errors", "summary", "output", "log_path", "soc",
                "info", "transport"):
        if key in detail:
            out[key] = detail[key]
    return out


def quick_test(alias: Optional[str] = None, *,
               build: bool = True,
               wait_for: Optional[List[str]] = None,
               timeout: float = 10.0,
               idle: Optional[float] = None,
               auto_close: bool = True) -> Dict[str, Any]:
    if wait_for is not None and not isinstance(wait_for, list):
        return {
            "success": False,
            "alias": alias,
            "stage": "validate",
            "errors": [{"code": "ARGUMENT_INVALID",
                        "field_path": "wait_for",
                        "message": "wait_for must be a list of regex strings or null"}],
            "closed_after": False,
        }

    out: Dict[str, Any] = {
        "success": False,
        "alias": alias,
        "soc": None,
        "build": None,
        "flash": None,
        "wait": None,
        "closed_after": False,
    }

    # Stage 0: alias resolution (also handles single-board auto-pick
    # when alias is None). Done unconditionally so subsequent stages
    # always operate on a concrete alias.
    resolved, err_env = _resolve_alias_soc(alias)
    if err_env is not None:
        return err_env
    alias, target_soc = resolved
    out["alias"] = alias
    out["soc"] = target_soc

    # Stage 1: build (optional). Before building, reconcile the active
    # set_target with the alias's SoC so we never build SoC-A's firmware
    # and accidentally flash it onto SoC-B (real brick risk on mismatched
    # flash layouts / boot ROMs).
    if build:
        current_soc = _read_current_soc()
        retargeted = False
        if current_soc != target_soc:
            try:
                _write_soc_info(target_soc)
                retargeted = True
            except Exception as ex:
                return {
                    "success": False,
                    "stage": "build",
                    "alias": alias,
                    "soc": target_soc,
                    "errors": [{"code": "SET_TARGET_FAILED",
                                "field_path": "soc_info.json",
                                "message": f"set_target({target_soc}) failed: {ex}",
                                "hint": "Check that <SDK_ROOT> is writable."}],
                    "closed_after": False,
                }

        b = run_build_quiet()
        out["build"] = {
            "summary": b.get("summary"),
            "log_path": b.get("log_path"),
            "image_count": (b.get("project_info") or {}).get("image_count"),
            "retargeted_from": current_soc if retargeted else None,
        }
        if not b.get("success"):
            return _failed("build", alias, b)

    # Stage 2: flash
    f = flash_firmware(alias)
    if not f.get("success"):
        return _failed("flash", alias, f)
    out["soc"] = f.get("soc") or out["soc"]
    out["flash"] = {
        "images_flashed": f.get("images_flashed"),
        "info": f.get("info") or [],
    }

    # Stage 3+4: connect with reset + drain + wait. serial_connect rolls
    # all three into one primitive, so a failure could legitimately be
    # any of "connect" / "reset" / "wait" — we infer from the result.
    # wait_for=None or [] → no wait; just open + reset + return.
    c = serial_connect(alias, reset=True, drain_first=True,
                       wait_for=wait_for if wait_for else None,
                       wait_timeout=timeout,
                       idle=idle)
    if not c.get("success"):
        # Distinguish reset failure from connect/validation failure by
        # peeking at the first error code (best-effort).
        stage = "connect"
        errs = c.get("errors") or []
        if errs and errs[0].get("code") == "RESET_FAILED":
            stage = "reset"
        return _failed(stage, alias, c)
    out["wait"] = c.get("wait")

    # Stage 5: optional auto-close. If close fails we still report the
    # successful boot, but flag closed_after=False and surface the error.
    if auto_close:
        d = serial_disconnect(alias)
        if d.get("success"):
            out["closed_after"] = True
        else:
            return _failed("close", alias, d, closed_after=False)

    out["success"] = True
    # Treat a Crash Dump match as a hard failure for higher-level
    # automation (the wait completed, but the board didn't reach a
    # healthy boot point). The caller can still see the dump in
    # wait.data and decide otherwise via wait.matched.
    matched = (out.get("wait") or {}).get("matched") or ""
    if "Crash Dump" in matched:
        out["success"] = False
        out["stage"] = "wait"
        out.setdefault("errors", []).append({
            "code": "BOOT_CRASHED",
            "field_path": f"boards.{alias}",
            "message": f"matched crash pattern: {matched!r}",
            "hint": "See wait.data for the full register dump.",
        })
    return out


def register_dev_loop_tools(mcp: FastMCP) -> None:
    """Register dev-loop tools with FastMCP.

    quick_test_tool was removed in 2026-05-19 (see docs/tool_design_review.md
    §3): high-level "magic button" tools don't fit agent reasoning patterns.
    Agents prefer atomic build → flash → connect chains. The quick_test()
    function below is kept for shell scripts / CI use, but is no longer
    exposed as an MCP tool.
    """
    # No tools currently registered. Function quick_test() above remains
    # available for direct Python imports.
    return
