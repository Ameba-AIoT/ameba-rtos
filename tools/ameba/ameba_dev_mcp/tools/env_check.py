"""
Environment pre-check: a single read-only diagnostic that surfaces every
condition that could later cause a flash / serial tool to fail.

Returns a structured report covering:
  - JSON config files (board_info.json5 / project_info.json5) presence + validity
  - Per-alias port visibility (local lsof / remote TCP probe)
  - Per-alias remote service reachability (TCP connect to AmebaRemoteService)
  - Optional: per-alias reset smoke test (with_reset=True; reboots the board)

This tool does NOT flash, write any persistent state, or modify the
board. It opens local serial ports briefly only when `with_reset=True`,
in which case it triggers a DTR/RTS pulse (or RemoteSerial.reset_device
for remote boards) to verify the auto-download circuit is wired. With
`with_reset=False` (default) the report is purely passive.
"""

from __future__ import annotations

import logging
import os
import socket
from typing import Any, Dict, List, Optional

from mcp.server.fastmcp import FastMCP

from ameba_dev_mcp._paths import PROJECT_ROOT
from ameba_dev_mcp.config.board_session import (
    BoardSessionError,
    manager as session_manager,
)
from ameba_dev_mcp.config.loader import (
    ConfigLoadError,
    board_info_path,
    ensure_board_info_template,
    ensure_project_info_template,
    load_board_info,
    load_project_info,
    project_info_path,
    resolve_board,
    save_board_info,
)
from ameba_dev_mcp.models.schemas import (
    BoardEntry,
    BoardInfo,
    ConfigError,
    RemoteSpec,
)

logger = logging.getLogger(__name__)


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

def _config_section() -> Dict[str, Any]:
    """Probe both JSON config files; auto-create empty templates on miss
    (matches the existing flash/serial tool behavior so subsequent tool
    calls don't have to re-create them)."""
    bpath = board_info_path(PROJECT_ROOT)
    ppath = project_info_path(PROJECT_ROOT)

    out: Dict[str, Any] = {
        "board_info": {
            "present": os.path.isfile(bpath),
            "path": bpath,
            "valid": False,
            "errors": [],
            "alias_count": 0,
        },
        "project_info": {
            "present": os.path.isfile(ppath),
            "path": ppath,
            "valid": False,
            "errors": [],
        },
        "docs_url": None,
        "resource_url": None,
    }

    # Auto-create templates so the first run leaves the user a starting point.
    if not out["board_info"]["present"]:
        try:
            ensure_board_info_template(PROJECT_ROOT)
            out["board_info"]["template_created"] = True
        except Exception as ex:
            out["board_info"]["errors"].append({
                "code": "TEMPLATE_CREATE_FAILED",
                "message": f"failed to create empty board_info template: {ex}",
            })
    if not out["project_info"]["present"]:
        try:
            ensure_project_info_template(PROJECT_ROOT)
            out["project_info"]["template_created"] = True
        except Exception as ex:
            out["project_info"]["errors"].append({
                "code": "TEMPLATE_CREATE_FAILED",
                "message": f"failed to create empty project_info template: {ex}",
            })

    # Now (re-)load and validate.
    try:
        binfo = load_board_info(PROJECT_ROOT)
        out["board_info"]["valid"] = True
        out["board_info"]["alias_count"] = len(binfo.boards)
    except ConfigLoadError as ex:
        out["board_info"]["errors"].extend(e.model_dump() for e in ex.errors)
        binfo = None

    try:
        load_project_info(PROJECT_ROOT)
        out["project_info"]["valid"] = True
    except ConfigLoadError as ex:
        out["project_info"]["errors"].extend(e.model_dump() for e in ex.errors)

    # Surface docs_url whenever the user is likely to need them: parse
    # errors, missing files, OR a freshly-created empty template (which
    # parses cleanly but has zero aliases — exactly the moment they
    # need to know where the schema is documented).
    if (out["board_info"]["errors"] or out["project_info"]["errors"]
            or not out["board_info"]["valid"]
            or not out["project_info"]["valid"]
            or out["board_info"]["alias_count"] == 0):
        out["docs_url"] = "docs/board_info.md"
        out["resource_url"] = "debug://hardware"

    return out, binfo


def _local_port_visibility(port: str) -> Dict[str, Any]:
    """Look up a local serial port in pyserial's enumeration. Returns
    {visible, busy, holder}.
    """
    from ameba_dev_mcp.tools.flash import _list_local_ports

    for entry in _list_local_ports():
        if entry.get("device") == port or entry.get("name") == port:
            return {
                "visible": True,
                "busy": entry.get("busy"),
                "holder": entry.get("holder"),
                "held_by_self": entry.get("held_by_self", False),
                "held_by_self_alias": entry.get("held_by_self_alias"),
            }
    return {"visible": False, "busy": None, "holder": None,
            "held_by_self": False, "held_by_self_alias": None}


def _remote_service_reachable(host: str, tcp_port: int,
                              timeout: float = 2.0) -> Dict[str, Any]:
    """TCP connect probe — does NOT issue any business command.
    Returns {reachable, error}.
    """
    s = None
    try:
        s = socket.create_connection((host, tcp_port), timeout=timeout)
        return {"reachable": True, "error": None}
    except OSError as ex:
        return {"reachable": False, "error": f"{ex.__class__.__name__}: {ex}"}
    finally:
        if s is not None:
            try:
                s.close()
            except Exception:
                pass


def _remote_port_visibility(board) -> Dict[str, Any]:
    """For a remote alias, ask AmebaRemoteService whether the configured
    COM port appears in its list. Reuses _list_remote_ports.
    """
    from ameba_dev_mcp.tools.flash import _list_remote_ports
    ports = _list_remote_ports(board)
    # ports is either [{...}, ...] or [{"error": "..."}]
    if ports and isinstance(ports[0], dict) and ports[0].get("error"):
        return {"visible": None, "error": ports[0]["error"],
                "holder": None, "held_by_self": False,
                "held_by_self_alias": None}
    target = board.port
    for entry in ports:
        if entry.get("device") == target or entry.get("name") == target:
            return {
                "visible": True,
                "busy": entry.get("busy"),
                "holder": entry.get("holder"),
                "held_by_self": entry.get("held_by_self", False),
                "held_by_self_alias": entry.get("held_by_self_alias"),
                "error": None,
            }
    return {"visible": False, "busy": None, "holder": None,
            "held_by_self": False, "held_by_self_alias": None,
            "error": None}


def _reset_smoke_test(board) -> Dict[str, Any]:
    """Open the port briefly, pulse reset, close. Reuses serial helpers.
    Returns {attempted, success, error}.
    """
    from ameba_dev_mcp.tools.serial import _apply_reset
    attempted: Dict[str, Any] = {"attempted": True, "success": None, "error": None}
    try:
        sess = session_manager.connect(board, monitor_mode=True)
    except BoardSessionError as ex:
        attempted["success"] = False
        attempted["error"] = ex.error.message
        return attempted
    try:
        if not sess.is_open():
            attempted["success"] = False
            attempted["error"] = "session opened but is_open() reports False"
            return attempted
        try:
            _apply_reset(sess)
            attempted["success"] = True
        except Exception as ex:
            attempted["success"] = False
            attempted["error"] = f"reset failed: {ex}"
    finally:
        # Always close — env_pre_check is meant to leave no state behind.
        try:
            session_manager.disconnect(board.alias)
        except Exception:
            pass
    return attempted


# ---------------------------------------------------------------------------
# Main entry
# ---------------------------------------------------------------------------

def env_pre_check(*, with_reset: bool = False,
                  soc_filter: Optional[str] = None,
                  terse: bool = False) -> Dict[str, Any]:
    """Build a complete environment readiness report. See docstring on
    env_pre_check_tool() for the full schema.

    When terse=True, drop the per-alias `aliases` array and the full
    `config` block, keeping only success / summary / next_steps so the
    return is small enough for routine pre-flight checks across many
    boards.
    """
    config, binfo = _config_section()

    aliases: List[Dict[str, Any]] = []
    blocking_codes: List[str] = []
    ok_count = 0

    if binfo is not None:
        for alias_name in sorted(binfo.boards.keys()):
            entry = binfo.boards[alias_name]
            if soc_filter and entry.soc != soc_filter:
                continue
            try:
                board = resolve_board(binfo, alias_name)
            except Exception as ex:
                aliases.append({
                    "alias": alias_name, "soc": entry.soc,
                    "transport": entry.transport, "port": entry.port,
                    "errors": [{"code": "BOARD_RESOLVE_FAILED",
                                "message": str(ex)}],
                })
                blocking_codes.append("BOARD_RESOLVE_FAILED")
                continue

            row: Dict[str, Any] = {
                "alias": alias_name,
                "soc": board.soc,
                "transport": board.transport,
                "port": board.port,
                "errors": [],
                "reset_test": None,
            }

            if board.transport == "remote":
                if board.remote is None:
                    row["errors"].append({
                        "code": "REMOTE_SPEC_MISSING",
                        "message": "transport=remote but no remote spec",
                    })
                    blocking_codes.append("REMOTE_SPEC_MISSING")
                    aliases.append(row); continue
                row["remote_host"] = board.remote.host
                row["remote_port"] = board.remote.port
                reach = _remote_service_reachable(
                    board.remote.host, board.remote.port,
                )
                row["remote_reachable"] = reach["reachable"]
                if not reach["reachable"]:
                    row["errors"].append({
                        "code": "REMOTE_UNREACHABLE",
                        "message": (
                            f"AmebaRemoteService at "
                            f"{board.remote.host}:{board.remote.port} "
                            f"not reachable: {reach['error']}"
                        ),
                        "hint": (
                            "Start AmebaRemoteService on the host, or "
                            "check firewall / port forwarding. See "
                            "debug://hardware §3 for remote service setup."
                        ),
                    })
                    blocking_codes.append("REMOTE_UNREACHABLE")
                    aliases.append(row); continue
                vis = _remote_port_visibility(board)
                row["port_visible"] = vis.get("visible")
                row["port_busy"] = vis.get("busy")
                row["port_holder"] = vis.get("holder")
                row["held_by_self"] = vis.get("held_by_self")
                row["held_by_self_alias"] = vis.get("held_by_self_alias")
                if vis.get("visible") is False:
                    row["errors"].append({
                        "code": "PORT_NOT_VISIBLE",
                        "message": (
                            f"port '{board.port}' not in remote port list "
                            f"on {board.remote.host}"
                        ),
                        "hint": (
                            "Check the cable on the remote machine, or "
                            "fix the `port` field for this alias."
                        ),
                    })
                    blocking_codes.append("PORT_NOT_VISIBLE")
                elif vis.get("error"):
                    row["errors"].append({
                        "code": "REMOTE_PORTLIST_FAILED",
                        "message": vis["error"],
                    })
                    blocking_codes.append("REMOTE_PORTLIST_FAILED")
            else:  # local
                row["remote_reachable"] = None
                vis = _local_port_visibility(board.port)
                row["port_visible"] = vis["visible"]
                row["port_busy"] = vis["busy"]
                row["port_holder"] = vis["holder"]
                row["held_by_self"] = vis["held_by_self"]
                row["held_by_self_alias"] = vis["held_by_self_alias"]
                if not vis["visible"]:
                    row["errors"].append({
                        "code": "PORT_NOT_VISIBLE",
                        "message": (
                            f"port '{board.port}' not present in local "
                            f"`pyserial.list_ports`"
                        ),
                        "hint": (
                            "Re-plug the USB cable, check the PL2303 "
                            "driver per debug://hardware §1, or fix the "
                            "`port` field for this alias."
                        ),
                    })
                    blocking_codes.append("PORT_NOT_VISIBLE")

            # Reset smoke test runs only when explicitly requested AND
            # the port is visible (otherwise it would just re-fail).
            if with_reset and row.get("port_visible"):
                if row.get("port_busy") and not row.get("held_by_self"):
                    row["reset_test"] = {
                        "attempted": False,
                        "success": None,
                        "error": (
                            "skipped: port is held by another process "
                            f"({row.get('port_holder')!r})"
                        ),
                    }
                else:
                    row["reset_test"] = _reset_smoke_test(board)
                    if row["reset_test"].get("success") is False:
                        row["errors"].append({
                            "code": "RESET_TEST_FAILED",
                            "message": row["reset_test"]["error"] or "reset returned failure",
                            "hint": (
                                "See debug://hardware §1 (PL2303 driver) "
                                "and §2 (auto-download circuit) before "
                                "concluding it's an SDK issue."
                            ),
                        })
                        blocking_codes.append("RESET_TEST_FAILED")

            if not row["errors"]:
                ok_count += 1
            aliases.append(row)

    summary = {
        "ok_aliases": ok_count,
        "blocked_aliases": len(aliases) - ok_count,
        "blocking_codes": sorted(set(blocking_codes)),
    }

    next_steps: List[str] = []
    if not config["board_info"]["valid"] or config["board_info"]["alias_count"] == 0:
        next_steps.append(
            f"Edit {config['board_info']['path']} per docs/board_info.md "
            "(see debug://hardware for board wiring)."
        )
    if not config["project_info"]["valid"]:
        next_steps.append(
            f"Edit {config['project_info']['path']} per docs/project_info.md, "
            "or run build_firmware to auto-populate it."
        )
    if "PORT_NOT_VISIBLE" in summary["blocking_codes"]:
        next_steps.append(
            "Some alias ports are not visible — re-plug the USB cable "
            "and/or check PL2303 driver (debug://hardware §1)."
        )
    if "REMOTE_UNREACHABLE" in summary["blocking_codes"]:
        next_steps.append(
            "Start AmebaRemoteService on remote host(s), or fix host/port."
        )
    if "RESET_TEST_FAILED" in summary["blocking_codes"]:
        next_steps.append(
            "Reset smoke test failed — read debug://hardware §1-§3 to "
            "decide whether this is the driver, the auto-download "
            "circuit, or a custom-board limitation."
        )

    success = (
        config["board_info"]["valid"]
        and config["project_info"]["valid"]
        and config["board_info"]["alias_count"] > 0
        and summary["blocked_aliases"] == 0
    )

    if terse:
        result: Dict[str, Any] = {
            "success": success,
            "summary": summary,
            "next_steps": next_steps,
        }
    else:
        result = {
            "success": success,
            "config": config,
            "aliases": aliases,
            "summary": summary,
            "next_steps": next_steps,
        }

    # When the bench has not been described yet, hand the agent a ready-to-use
    # two-stage AskUserQuestion schema so it does not have to invent the wording.
    # Stage 1 is asked of every user; stage 2 is conditional on transport=remote
    # so local users never see remote-only fields. Always included (even in
    # terse mode) because the init flow depends on it.
    if config["board_info"]["valid"] and config["board_info"]["alias_count"] == 0:
        result["agent_should_ask"] = {
            "via": "Two AskUserQuestion calls + a free-text question for SoCs",
            "after": "call apply_board_config_tool with the answers, then re-run env_pre_check_tool",
            "step1_first_ask": {
                "via": "AskUserQuestion (every user)",
                "questions": [
                    {"key": "board_count", "prompt": "How many boards on the bench?",
                     "type": "int>=1"},
                    {"key": "transport", "prompt": "Are the boards local or remote?",
                     "type": "enum",
                     "options": ["local", "remote"],
                     "note": "'remote' covers any port forwarded via AmebaRemoteService, including inside WSL."},
                ],
            },
            "step2_second_ask_if_remote": {
                "via": "AskUserQuestion (ONLY when transport==remote)",
                "questions": [
                    {"key": "remote_host", "prompt": "Remote host", "type": "str",
                     "default": "127.0.0.1"},
                    {"key": "remote_password", "prompt": "Remote password",
                     "type": "str", "default": ""},
                ],
                "fixed": {"remote_port": 58916},
            },
            "step3_free_text": {
                "prompt": "Which SoCs are on the bench? (free-text list, one per board)",
                "examples": ["RTL8721F", "RTL8720E", "RTL8730E", "RTL8726E", "RTL8720F"],
                "note": "Open-ended — any SoC name supported by this SDK is fine.",
            },
            "step4_pair_soc_with_port": {
                "required_for_every_board": True,
                "note": (
                    "apply_board_config_tool needs (soc, port) for every "
                    "entry. With board_count >= 2 you MUST explicitly ask "
                    "the user which port corresponds to which SoC — never "
                    "infer the mapping from step 3's order."
                ),
                "when_user_already_knows_ports": "use them directly",
                "when_user_does_not_know_ports": {
                    "local": "call list_serial_ports_tool(), show results, ask user to map each port to a SoC",
                    "remote": "call scan_remote_ports_tool(host, password), show results, ask user to map each port to a SoC",
                },
                "goal_shape": [
                    {"soc": "RTL8721F", "port": "COM23"},
                    {"soc": "RTL8720E", "port": "ttyUSB0"},
                ],
            },
        }

    return result


# ---------------------------------------------------------------------------
# MCP registration
# ---------------------------------------------------------------------------

def _sanitize_alias_part(s: str) -> str:
    out = []
    for ch in s:
        out.append(ch if ch.isalnum() or ch in ("_", "-") else "_")
    return "".join(out).strip("_") or "x"


# Sentinel for "default_alias parameter was not provided".
# We can't use None for that because None is a valid request: clear the field.
_UNCHANGED_DEFAULT_ALIAS = "<<unchanged>>"


def apply_board_config(boards: List[Dict[str, Any]],
                       merge: bool = True,
                       default_alias: Optional[str] = _UNCHANGED_DEFAULT_ALIAS,
                       ) -> Dict[str, Any]:
    """Atomic write of board entries to board_info.json5.

    Each item: {alias?, soc, port, transport?, remote?{host,port?,password?},
                baudrate?, monitor_baudrate?, chip_erase?}.
    Alias defaults to "<SOC>_<PORT>" (sanitized).

    `default_alias` semantics:
      - sentinel (default) → preserve existing default_alias if it still
        points to a surviving alias; otherwise clear (current behavior)
      - None or empty string → clear the default_alias field
      - non-empty string → set as the new default_alias; must resolve to
        a surviving alias in the merged result
    """
    ensure_board_info_template(PROJECT_ROOT)
    try:
        existing = load_board_info(PROJECT_ROOT)
    except ConfigLoadError as ex:
        return {"success": False, "errors": [e.model_dump() for e in ex.errors]}

    new_entries: Dict[str, BoardEntry] = {}
    errors: List[Dict[str, Any]] = []
    for i, raw in enumerate(boards):
        try:
            soc = raw["soc"]
            port = raw["port"]
        except KeyError as k:
            errors.append({"index": i, "error": f"missing required field: {k}"})
            continue
        alias = raw.get("alias") or f"{_sanitize_alias_part(soc)}_{_sanitize_alias_part(port)}"
        transport = raw.get("transport") or ("remote" if raw.get("remote") else "local")
        entry_kwargs: Dict[str, Any] = {
            "soc": soc, "transport": transport, "port": port,
        }
        if transport == "remote":
            r = raw.get("remote") or {}
            entry_kwargs["remote"] = RemoteSpec(
                host=r.get("host", "127.0.0.1"),
                port=r.get("port", 58916),
                password=r.get("password") or None,
            )
        for k in ("baudrate", "monitor_baudrate", "chip_erase", "add_crlf"):
            if raw.get(k) is not None:
                entry_kwargs[k] = raw[k]
        try:
            new_entries[alias] = BoardEntry(**entry_kwargs)
        except Exception as ex:
            errors.append({"index": i, "alias": alias, "error": str(ex)})

    if errors:
        return {"success": False, "errors": errors, "applied": []}

    if merge:
        merged = dict(existing.boards)
        merged.update(new_entries)
    else:
        merged = new_entries
    # Resolve final default_alias based on caller intent.
    if default_alias == _UNCHANGED_DEFAULT_ALIAS:
        # Preserve only when the existing value still points to a surviving alias.
        final_default = existing.default_alias if (
            existing.default_alias is not None
            and existing.default_alias in merged
        ) else None
    elif default_alias is None or default_alias == "":
        final_default = None
    else:
        if default_alias not in merged:
            return {
                "success": False,
                "errors": [{
                    "code": "DEFAULT_ALIAS_NOT_FOUND",
                    "field_path": "default_alias",
                    "message": (
                        f"default_alias '{default_alias}' is not among the "
                        f"merged aliases ({sorted(merged.keys())})."
                    ),
                    "hint": (
                        "Either pass an alias listed in `boards` (after this "
                        "merge) or omit `default_alias` to leave the field as is."
                    ),
                }],
                "applied": [],
            }
        final_default = default_alias

    final = BoardInfo(schema_version=existing.schema_version,
                      defaults=existing.defaults,
                      default_alias=final_default,
                      boards=merged)
    path = save_board_info(PROJECT_ROOT, final)
    return {
        "success": True,
        "path": path,
        "applied": sorted(new_entries.keys()),
        "total_aliases": len(merged),
        "default_alias": final_default,
        "config_paths": {
            "board_info": board_info_path(PROJECT_ROOT),
            "project_info": project_info_path(PROJECT_ROOT),
        },
        "remind_user": (
            "Bench config saved. Two files own this state:\n"
            f"  - {board_info_path(PROJECT_ROOT)}  (boards / ports / remote)\n"
            f"  - {project_info_path(PROJECT_ROOT)}  (per-SoC flash images, auto-filled by build)\n"
            "When boards change later: either edit board_info.json5 directly "
            "(add/remove/rename entries under \"boards\"), or re-run the "
            "/ameba-setup-boards slash command to walk through the setup again. "
            "project_info.json5 is regenerated by build_firmware on next build, "
            "no manual edit needed."
        ),
    }


def register_env_check_tools(mcp: FastMCP) -> None:
    @mcp.tool()
    async def apply_board_config_tool(
        boards: List[Dict[str, Any]],
        merge: bool = True,
        default_alias: Optional[str] = None,
        set_default_alias: bool = False,
    ) -> Dict[str, Any]:
        """Atomically write board entries to board_info.json5.

        Args:
            boards:            list of dicts. Required: soc, port. Optional:
                               alias (auto = "<SOC>_<PORT>"), transport
                               ("local"/"remote"; auto = "remote" iff
                               `remote` provided), remote
                               {host=127.0.0.1, port=58916, password=""},
                               baudrate, monitor_baudrate, chip_erase.
            merge:             True (default) keeps existing aliases;
                               same-alias entries are updated in place.
                               False replaces the entire boards list.
            default_alias:     Alias to mark as the bench's default —
                               picked automatically when flash/serial
                               tools are called without an alias and 2+
                               boards are configured. Pass empty string ""
                               to clear an existing default. Ignored
                               unless `set_default_alias=True` (so the
                               default behavior on a 1-arg call is to
                               leave the field untouched).
            set_default_alias: When False (default), the existing
                               default_alias in board_info.json5 is
                               preserved (or cleared if its target was
                               removed by the merge). Set True together
                               with `default_alias` to actually update
                               the field. This two-flag pattern avoids
                               accidentally clearing the default on
                               every routine apply call.

        Returns: {success, path, applied: [alias...], total_aliases,
                  default_alias} or {success: false, errors: [...]}.
        """
        if set_default_alias:
            return apply_board_config(boards, merge=merge,
                                      default_alias=default_alias)
        return apply_board_config(boards, merge=merge)

    @mcp.tool()
    async def env_pre_check_tool(
        with_reset: bool = False,
        soc_filter: Optional[str] = None,
        terse: bool = False,
    ) -> Dict[str, Any]:
        """
        Read-only environment readiness check. Run this FIRST when the
        user reports "nothing works" or before kicking off a long
        flash/test session, to surface every JSON / port / hardware
        precondition that would otherwise fail later with cryptic errors.

        Args:
            with_reset:  If True, additionally pulse DTR/RTS (local) or
                         call RemoteSerial.reset_device (remote) for each
                         alias whose port is visible — verifies the
                         auto-download circuit is wired correctly.
                         **WILL REBOOT THE BOARD** — pass True only when
                         you actually intend to verify reset, NOT for a
                         passive precheck. Default False.
            soc_filter:  If set (e.g. "RTL8721F"), only aliases whose SoC
                         matches are included in the report. Useful when
                         board_info.json5 has many aliases across SoCs
                         and the user is only working on one.
            terse:       Default False. When True, drop the per-alias
                         `aliases` array and the full `config` block;
                         keep only `success`, `summary`, `next_steps`
                         (and `agent_should_ask` when alias_count==0).
                         Use for routine pre-flight on benches with many
                         boards. Pass False (default) for full report
                         when investigating a specific failure.

        Does NOT flash, write any persistent file, or modify hardware
        beyond an optional reset pulse. Returns:

          {
            "success": bool,                       # all green if true
            "config": {
              "board_info":   {present, path, valid, errors, alias_count},
              "project_info": {present, path, valid, errors},
              "docs_url":     str | null,
              "resource_url": str | null,          # debug://hardware
            },
            "aliases": [                           # one per configured alias
              {
                "alias", "soc", "transport", "port",
                "remote_reachable",                # remote only
                "port_visible", "port_busy", "port_holder",
                "held_by_self", "held_by_self_alias",
                "reset_test": {attempted, success, error} | null,
                "errors": [{code, message, hint}, ...],
              }, ...
            ],
            "summary": {ok_aliases, blocked_aliases, blocking_codes: [...]},
            "next_steps": [str, ...],              # human-readable hints
            "agent_should_ask": {           # ONLY present when alias_count==0
              "questions": [...],           # setup questions to ask the user
              "fixed": {...}               # answers already known (e.g. from scan)
            }                              # use this to drive /ameba-setup-boards
          }

        Common blocking codes:
          BOARD_CONFIG_MISSING / BOARD_CONFIG_INVALID — fix board_info.json5
          PROJECT_CONFIG_MISSING / PROJECT_CONFIG_INVALID — fix project_info.json5
          PORT_NOT_VISIBLE       — cable, driver (debug://hardware §1), wrong port
          REMOTE_UNREACHABLE     — AmebaRemoteService not running
          RESET_TEST_FAILED      — driver / circuit / custom-board (debug://hardware §1-§3)
        """
        return env_pre_check(with_reset=with_reset, soc_filter=soc_filter,
                             terse=terse)
