"""
Alias-driven serial communication tools for Ameba SoCs.

Tools take a single `alias` argument that resolves through
board_info.json5 → ResolvedBoard → BoardSessionManager.
The connection_id model from the previous version is gone:
the alias *is* the connection identity.
"""

from __future__ import annotations

import logging
import sys
import threading
import time
from typing import Any, Dict, List, Optional

from ameba_dev_mcp._paths import PROJECT_ROOT, REMOTE_SERVICE_DIR, TOOLS_ROOT

for _p in (TOOLS_ROOT, REMOTE_SERVICE_DIR):
    if _p not in sys.path:
        sys.path.insert(0, _p)

from mcp.server.fastmcp import FastMCP

from ameba_dev_mcp.config.board_session import (
    BoardSessionError,
    manager as session_manager,
)
from ameba_dev_mcp.config.loader import (
    ConfigLoadError,
    attach_selected_via_default,
    ensure_board_info_template,
    load_board_info,
    resolve_alias_or_error,
    resolve_board,
    validate_board_config,
)
from ameba_dev_mcp.models.schemas import ConfigError

logger = logging.getLogger(__name__)


# ===========================================================================
# AAG (Aggregate log) protocol parser
# ===========================================================================
# The parser now lives in config/aag.py so it can be shared verbatim with
# the background serial-log capture (config/serial_log.py). Re-exported here
# to keep the historical `tools.serial.AAGParser` import path working.

from ameba_dev_mcp.config.aag import AAGParser  # noqa: E402,F401


# ===========================================================================
# Resolution helper
# ===========================================================================

def _err_envelope(errors, *, alias: Optional[str] = None) -> Dict[str, Any]:
    return {
        "success": False,
        "alias": alias,
        "errors": [e.model_dump() for e in errors],
    }


def _resolve_or_error(alias: Optional[str]):
    """Load board_info, resolve alias (single-board auto-pick), run validators.

    Returns (resolved_board, None) on success or (None, error_envelope) on
    failure. Auto-creates an empty board_info.json5 template if missing.
    `alias=None` is allowed: when board_info has exactly 1 board, that
    alias is selected automatically; otherwise ALIAS_REQUIRED is returned.
    """
    # Load (auto-create template on first miss)
    try:
        info = load_board_info(PROJECT_ROOT)
    except ConfigLoadError as ex:
        missing = any(e.code == "BOARD_CONFIG_MISSING" for e in ex.errors)
        template_path = None
        if missing:
            try:
                template_path = ensure_board_info_template(PROJECT_ROOT)
            except Exception:
                pass
        env = _err_envelope(ex.errors, alias=alias)
        if template_path:
            env["template_path"] = template_path
            env["docs_url"] = "docs/board_info.md"
            env["resource_url"] = "debug://hardware"
        return None, env

    resolved_alias, alias_errors = resolve_alias_or_error(info, alias)
    if alias_errors is not None:
        from ameba_dev_mcp.tools.flash import _alias_summary, available_local_port_names
        env = _err_envelope(alias_errors, alias=alias)
        env["configured_aliases"] = _alias_summary(info)
        env["available_local_ports"] = available_local_port_names()
        return None, env

    val_errors = validate_board_config(PROJECT_ROOT, alias=resolved_alias)
    if val_errors:
        return None, _err_envelope(val_errors, alias=resolved_alias)

    return resolve_board(info, resolved_alias), None


# ===========================================================================
# Core operations (alias-driven)
# ===========================================================================

# Per-alias AAG parser cache (the read tool needs sticky state across calls)
_aag_parsers: Dict[str, AAGParser] = {}
_aag_lock = threading.Lock()


def _get_aag_parser(alias: str) -> AAGParser:
    with _aag_lock:
        p = _aag_parsers.get(alias)
        if p is None:
            p = AAGParser()
            _aag_parsers[alias] = p
        return p


def _drop_aag_parser(alias: str) -> None:
    with _aag_lock:
        _aag_parsers.pop(alias, None)


# ===========================================================================
# Shared helpers: drain / reset / wait-loop for expect / connect / write
# ===========================================================================

import re as _re

_EXPECT_POLL_INTERVAL_S = 0.05  # 50ms — bounded latency on pattern detection


def _compile_patterns(patterns):
    """Compile a list of regex strings; return (compiled_pairs, err_or_None).

    compiled_pairs is [(compiled_regex, original_string), ...]. err_or_None
    is a ConfigError on the first pattern that fails to compile.
    """
    if patterns is None:
        return [], None
    out = []
    for p in patterns:
        try:
            out.append((_re.compile(p, _re.MULTILINE), p))
        except _re.error as ex:
            return [], ConfigError(
                code="INVALID_PATTERN",
                field_path="patterns",
                message=f"regex {p!r} failed to compile: {ex}",
                hint="Patterns are Python re syntax; escape special chars with \\.",
            )
    return out, None


def _read_conn(sess):
    """Return the object the read/drain path should pull RX bytes from.

    When background serial-log capture is active, a LoggingReader is the
    sole consumer of the real connection; the tools must read from its
    re-buffered queue (so `drain_first` clears only that queue and never
    the log). When logging is off — and for the fake sessions used in
    tests, which have no `reader` attribute — this falls back to the raw
    connection, preserving the original behaviour byte-for-byte.

    WRITES and DTR/RTS reset always use `sess.connection` directly.
    """
    reader = getattr(sess, "reader", None)
    return reader if reader is not None else sess.connection


def _drain_serial(sess, alias: str) -> int:
    """Discard everything currently buffered for the tools AND reset the
    agent-facing AAG parser line-state. With logging on this clears only
    the LoggingReader's tool buffer — the .log file is untouched. Returns
    bytes dropped (best-effort estimate).
    """
    conn = _read_conn(sess)
    dropped = 0
    try:
        if hasattr(conn, "inWaiting"):
            dropped = conn.inWaiting()
        elif hasattr(conn, "in_waiting"):
            dropped = conn.in_waiting
    except Exception:
        dropped = 0
    try:
        if hasattr(conn, "flushInput"):
            conn.flushInput()
        elif hasattr(conn, "reset_input_buffer"):
            conn.reset_input_buffer()
    except Exception:
        pass
    parser = _get_aag_parser(alias)
    parser.reset()
    return int(dropped or 0)


def _apply_reset(sess) -> None:
    """Pulse DTR/RTS (local) or call RemoteSerial.reset_device() (remote)."""
    conn = sess.connection
    if sess.is_remote:
        ok = conn.reset_device(timeout=10.0)
        if not ok:
            raise RuntimeError("RemoteSerial.reset_device returned falsy")
        return
    conn.setDTR(True); conn.setRTS(True)
    time.sleep(0.1)
    conn.setDTR(False); conn.setRTS(False)
    time.sleep(0.1)


def _wait_loop(sess, alias: str, compiled_pairs, *,
               timeout: float, idle, max_bytes: int) -> Dict[str, Any]:
    """Poll the serial port, accumulating decoded text, until one of the
    stop conditions fires. Returns the `wait`/`expect` sub-object with
    keys reason, matched, match_index, data, elapsed_ms, buffer_left.

    Stop priority: pattern → max_bytes → idle → timeout.
    `idle` only fires after at least one byte has been received (so a
    silent board cannot masquerade as "settled").
    """
    start = time.monotonic()
    last_byte = start
    accum = ""
    raw_total = 0
    parser = _get_aag_parser(alias)
    conn = _read_conn(sess)

    while True:
        now = time.monotonic()
        elapsed = now - start

        try:
            waiting = (conn.inWaiting() if hasattr(conn, "inWaiting")
                       else getattr(conn, "in_waiting", 0))
        except Exception:
            waiting = 0

        if waiting:
            try:
                chunk = conn.read(waiting)
            except Exception:
                chunk = b""
            if chunk:
                accum += parser.parse(chunk)
                raw_total += len(chunk)
                last_byte = time.monotonic()

                # max_bytes wins over pattern only if we already overshot
                if raw_total >= max_bytes:
                    accum += parser.take_partial()
                    return _wait_result("max_bytes", None, accum, start, conn, alias)

                # Match against complete lines PLUS any held partial line, so a
                # pattern that lands before its terminating newline still fires
                # (AGG de-interleaving holds a core's line until newline).
                hay = accum + parser.peek_partial()
                for cp, orig in compiled_pairs:
                    m = cp.search(hay)
                    if m:
                        accum += parser.take_partial()
                        return _wait_result("pattern", orig, accum, start, conn, alias,
                                            match_index=m.start())

        if elapsed >= timeout:
            accum += parser.take_partial()
            return _wait_result("timeout", None, accum, start, conn, alias)

        if idle is not None and raw_total > 0 and (time.monotonic() - last_byte) >= idle:
            accum += parser.take_partial()
            return _wait_result("idle", None, accum, start, conn, alias)

        time.sleep(_EXPECT_POLL_INTERVAL_S)


def _wait_result(reason: str, matched, data: str, start_mono: float,
                 conn, alias: str, *, match_index: int = -1) -> Dict[str, Any]:
    try:
        buffer_left = (conn.inWaiting() if hasattr(conn, "inWaiting")
                       else getattr(conn, "in_waiting", 0))
    except Exception:
        buffer_left = 0
    tail_lines = [l for l in data.splitlines() if l.strip()][-15:]
    return {
        "reason": reason,
        "matched": matched,
        "match_index": match_index,
        "data": data,
        "tail": "\n".join(tail_lines),
        "elapsed_ms": int((time.monotonic() - start_mono) * 1000),
        "buffer_left": int(buffer_left or 0),
    }


def _session_hint(alias: str, sess=None) -> str:
    """Standard 'session open' hint. When serial-log recording is active for
    this alias, also point at the .log file — it holds the COMPLETE session
    output (including bytes dropped by `drain_first`), independent of what any
    read returns."""
    hint = f"Session open — call serial_disconnect_tool('{alias}') when done."
    slog = getattr(sess, "serial_logger", None) if sess is not None else None
    path = getattr(slog, "path", None)
    if path:
        hint += (f" Full session log (incl. drained bytes) is recorded to {path} "
                 f"(serial_log_record in board_info.json5).")
    return hint


def serial_connect(alias: Optional[str] = None, *,
                   reset: bool = True,
                   drain_first: bool = True,
                   wait_for=None,
                   wait_timeout: float = 15.0,
                   idle=None) -> Dict[str, Any]:
    """Open or reuse the serial session for `alias`. If `reset=True`
    (default), pulses DTR/RTS (local) or calls `RemoteSerial.reset_device`
    (remote) right after the port is open. If `wait_for` is non-empty,
    blocks until any pattern matches OR a stop condition fires.
    `alias=None` is accepted only when board_info contains exactly one board.
    """
    if wait_for is not None and not isinstance(wait_for, list):
        return _err_envelope([ConfigError(
            code="ARGUMENT_INVALID",
            field_path="wait_for",
            message="wait_for must be a list of regex strings or null",
        )], alias=alias)

    compiled, perr = _compile_patterns(wait_for)
    if perr is not None:
        return _err_envelope([perr], alias=alias)

    board, err = _resolve_or_error(alias)
    if err is not None:
        return err
    alias = board.alias
    try:
        sess = session_manager.connect(board, monitor_mode=True)
    except BoardSessionError as ex:
        return _err_envelope([ex.error], alias=alias)

    dropped = 0
    if drain_first and sess.is_open():
        try:
            dropped = _drain_serial(sess, alias)
        except Exception:
            dropped = 0

    if reset and sess.is_open():
        try:
            _apply_reset(sess)
        except Exception as ex:
            return _err_envelope([ConfigError(
                code="RESET_FAILED",
                field_path=f"boards.{alias}",
                message=f"reset failed: {ex}",
            )], alias=alias)

    wait = None
    if wait_for:
        wait = _wait_loop(sess, alias, compiled,
                          timeout=wait_timeout, idle=idle, max_bytes=65536)

    return {
        "success": True,
        "alias": alias,
        "soc": sess.board.soc,
        "transport": sess.board.transport,
        "port": sess.board.port,
        "baudrate": sess.board.monitor_baudrate,
        "is_open": sess.is_open(),
        "reset_applied": bool(reset),
        "dropped_bytes": dropped,
        "wait": wait,
        "_hint": _session_hint(alias, sess),
    }


def _trim_wait_for_token_budget(wait_obj: Optional[Dict[str, Any]],
                                verbose: bool) -> Optional[Dict[str, Any]]:
    """When verbose=False (default), drop the full `data` blob and keep
    only the tail + match metadata. Server-side full log is preserved
    via the AAG parser cache; agents rarely re-read the boot log.
    """
    if not wait_obj or verbose:
        return wait_obj
    trimmed = dict(wait_obj)
    if "data" in trimmed:
        trimmed["data_truncated"] = True
        del trimmed["data"]
    return trimmed


def serial_disconnect(alias: Optional[str] = None) -> Dict[str, Any]:
    """Close the serial connection bound to `alias` (if any). `alias=None`
    is accepted only when board_info contains exactly one board.
    """
    if alias is None:
        try:
            info = load_board_info(PROJECT_ROOT)
        except ConfigLoadError as ex:
            return _err_envelope(ex.errors, alias=None)
        resolved, alias_errs = resolve_alias_or_error(info, None)
        if alias_errs is not None:
            return _err_envelope(alias_errs, alias=None)
        alias = resolved
    ok = session_manager.disconnect(alias)
    _drop_aag_parser(alias)
    return {"success": ok, "alias": alias}


def serial_write(alias: Optional[str] = None, data: str = "",
                 encoding: str = "utf-8",
                 *, expect=None, timeout: float = 5.0,
                 idle=None) -> Dict[str, Any]:
    """Write `data` to alias's serial port. If `expect` is provided,
    block after the write until any pattern matches OR a stop condition
    fires (timeout / idle). `alias=None` is accepted only when board_info
    contains exactly one board.
    """
    if expect is not None and not isinstance(expect, list):
        return _err_envelope([ConfigError(
            code="ARGUMENT_INVALID",
            field_path="expect",
            message="expect must be a list of regex strings or null",
        )], alias=alias)

    compiled, perr = _compile_patterns(expect)
    if perr is not None:
        return _err_envelope([perr], alias=alias)

    board, err = _resolve_or_error(alias)
    if err is not None:
        return err
    alias = board.alias
    try:
        sess = session_manager.connect(board, monitor_mode=True)
    except BoardSessionError as ex:
        return _err_envelope([ex.error], alias=alias)

    if not sess.is_open():
        return _err_envelope([ConfigError(
            code="PORT_CLOSED",
            field_path=f"boards.{alias}",
            message=f"connection for '{alias}' is not open",
        )], alias=alias)

    sess.touch()
    if board.add_crlf and not data.endswith("\r\n"):
        data = data.rstrip("\n").rstrip("\r") + "\r\n"

    raw = data.encode(encoding)
    try:
        n = sess.connection.write(raw)
    except Exception as ex:
        return _err_envelope([ConfigError(
            code="WRITE_FAILED",
            field_path=f"boards.{alias}",
            message=f"serial write failed: {ex}",
        )], alias=alias)
    if n is None:  # RemoteSerial.write returns None
        n = len(raw)

    expect_result = None
    if expect:
        expect_result = _wait_loop(sess, alias, compiled,
                                   timeout=timeout, idle=idle, max_bytes=65536)

    return {"success": True, "alias": alias, "bytes_written": n,
            "expect": expect_result,
            "_hint": _session_hint(alias, sess)}


def serial_read(alias: Optional[str] = None, size: int = 1024,
                timeout: float = 1.0) -> Dict[str, Any]:
    """Non-blocking read. `alias=None` is accepted only when board_info
    contains exactly one board.
    """
    board, err = _resolve_or_error(alias)
    if err is not None:
        return err
    alias = board.alias
    try:
        sess = session_manager.connect(board, monitor_mode=True)
    except BoardSessionError as ex:
        return _err_envelope([ex.error], alias=alias)

    if not sess.is_open():
        return _err_envelope([ConfigError(
            code="PORT_CLOSED",
            field_path=f"boards.{alias}",
            message=f"connection for '{alias}' is not open",
        )], alias=alias)

    sess.touch()
    conn = _read_conn(sess)
    try:
        waiting = conn.inWaiting() if hasattr(conn, "inWaiting") else conn.in_waiting
    except Exception as ex:
        return _err_envelope([ConfigError(
            code="READ_FAILED",
            field_path=f"boards.{alias}",
            message=f"in_waiting query failed: {ex}",
        )], alias=alias)

    if waiting == 0:
        return {"success": True, "alias": alias, "data": "", "bytes_read": 0,
                "message": "No data available",
                "_hint": _session_hint(alias, sess)}

    n = min(size, waiting)
    try:
        raw = conn.read(n)
    except Exception as ex:
        return _err_envelope([ConfigError(
            code="READ_FAILED",
            field_path=f"boards.{alias}",
            message=f"serial read failed: {ex}",
        )], alias=alias)

    parser = _get_aag_parser(alias)
    # parse() de-interleaves within this burst; take_partial() finalizes any
    # trailing line so this non-blocking peek returns everything received.
    text = (parser.parse(raw) if raw else "") + parser.take_partial()
    return {
        "success": True,
        "alias": alias,
        "data": text,
        "bytes_read": len(raw),
        "is_binary": False,
        "_hint": _session_hint(alias, sess),
    }


def serial_drain(alias: Optional[str] = None) -> Dict[str, Any]:
    """Discard buffered bytes for `alias` and reset AAG line state.
    `alias=None` is accepted only when board_info contains exactly one board.
    """
    board, err = _resolve_or_error(alias)
    if err is not None:
        return err
    alias = board.alias
    try:
        sess = session_manager.connect(board, monitor_mode=True)
    except BoardSessionError as ex:
        return _err_envelope([ex.error], alias=alias)
    if not sess.is_open():
        return _err_envelope([ConfigError(
            code="PORT_CLOSED",
            field_path=f"boards.{alias}",
            message=f"connection for '{alias}' is not open",
        )], alias=alias)
    sess.touch()
    dropped = _drain_serial(sess, alias)
    return {"success": True, "alias": alias, "dropped_bytes": dropped,
            "_hint": _session_hint(alias, sess)}


def serial_command(alias: Optional[str] = None, data: str = "",
                   *, expect=None, timeout: float = 5.0,
                   idle: Optional[float] = None,
                   drain_first: bool = True,
                   encoding: str = "utf-8") -> Dict[str, Any]:
    """drain (optional) + write + expect, in one call.

    Replaces the common 3-tool sequence
    (serial_drain → serial_write → serial_expect). `expect=None` makes
    this a pure write (matches old serial_write semantics).
    """
    if expect is not None and not isinstance(expect, list):
        return _err_envelope([ConfigError(
            code="ARGUMENT_INVALID",
            field_path="expect",
            message="expect must be a list of regex strings or null",
        )], alias=alias)

    compiled, perr = _compile_patterns(expect)
    if perr is not None:
        return _err_envelope([perr], alias=alias)

    board, err = _resolve_or_error(alias)
    if err is not None:
        return err
    alias = board.alias
    try:
        sess = session_manager.connect(board, monitor_mode=True)
    except BoardSessionError as ex:
        return _err_envelope([ex.error], alias=alias)

    if not sess.is_open():
        return _err_envelope([ConfigError(
            code="PORT_CLOSED",
            field_path=f"boards.{alias}",
            message=f"connection for '{alias}' is not open",
        )], alias=alias)

    sess.touch()

    dropped = 0
    if drain_first:
        try:
            dropped = _drain_serial(sess, alias)
        except Exception:
            dropped = 0

    if board.add_crlf and not data.endswith("\r\n"):
        data = data.rstrip("\n").rstrip("\r") + "\r\n"

    raw = data.encode(encoding)
    try:
        n = sess.connection.write(raw)
    except Exception as ex:
        return _err_envelope([ConfigError(
            code="WRITE_FAILED",
            field_path=f"boards.{alias}",
            message=f"serial write failed: {ex}",
        )], alias=alias)
    if n is None:  # RemoteSerial.write returns None
        n = len(raw)

    expect_result = None
    if expect:
        expect_result = _wait_loop(sess, alias, compiled,
                                   timeout=timeout, idle=idle, max_bytes=65536)

    return {"success": True, "alias": alias,
            "dropped_bytes": dropped, "bytes_written": n,
            "expect": expect_result,
            "_hint": _session_hint(alias, sess)}


def serial_expect(alias: Optional[str] = None, patterns=None,
                  *, timeout: float, idle=None,
                  max_bytes: int = 65536) -> Dict[str, Any]:
    """Block until any pattern matches OR a stop condition fires.
    `alias=None` is accepted only when board_info contains exactly one board.
    """
    if patterns is None:
        patterns = []
    if not isinstance(patterns, list):
        return _err_envelope([ConfigError(
            code="ARGUMENT_INVALID",
            field_path="patterns",
            message="patterns must be a list of regex strings",
        )], alias=alias)
    if not patterns and idle is None:
        return _err_envelope([ConfigError(
            code="ARGUMENT_INVALID",
            field_path="patterns",
            message=("patterns is empty and idle is None — would just sleep "
                     "for `timeout`. Provide patterns or set idle."),
            hint="Pass at least one regex, OR set idle=<seconds> for time-based stop.",
        )], alias=alias)

    compiled, perr = _compile_patterns(patterns)
    if perr is not None:
        return _err_envelope([perr], alias=alias)

    board, err = _resolve_or_error(alias)
    if err is not None:
        return err
    alias = board.alias
    try:
        sess = session_manager.connect(board, monitor_mode=True)
    except BoardSessionError as ex:
        return _err_envelope([ex.error], alias=alias)
    if not sess.is_open():
        return _err_envelope([ConfigError(
            code="PORT_CLOSED",
            field_path=f"boards.{alias}",
            message=f"connection for '{alias}' is not open",
        )], alias=alias)

    sess.touch()
    wait = _wait_loop(sess, alias, compiled,
                      timeout=timeout, idle=idle, max_bytes=max_bytes)
    return {"success": True, "alias": alias, **wait,
            "_hint": _session_hint(alias, sess)}




# ===========================================================================
# Lifecycle hook for server.py
# ===========================================================================

def cleanup_all_connections() -> None:
    """Close every alias-bound session. Called on server shutdown."""
    session_manager.disconnect_all()
    with _aag_lock:
        _aag_parsers.clear()


# ===========================================================================
# MCP tool registration
# ===========================================================================


def register_serial_tools(mcp: FastMCP) -> None:
    """Register serial communication tools (alias-driven) with FastMCP."""

    @mcp.tool()
    async def serial_connect_tool(
        alias: Optional[str] = None,
        reset: bool = True,
        drain_first: bool = True,
        wait_for: Optional[List[str]] = None,
        wait_timeout: float = 15.0,
        idle: Optional[float] = None,
        verbose: bool = False,
    ) -> Dict[str, Any]:
        """
        Open the serial connection bound to `alias`. Idempotent: if a
        session is already open, the existing one is reused (no TCP
        reconnect for remote boards).

        Args:
            alias:        Board alias from board_info.json5
                          (e.g., "RTL8721F_ttyUSB0"). May be omitted ONLY
                          when board_info.json5 has exactly one board
                          configured — that single alias is then auto-
                          selected. With 2+ boards configured, alias is
                          required and ALIAS_REQUIRED is returned.
            reset:        Pulse DTR/RTS (local) or call
                          RemoteSerial.reset_device (remote) right after
                          the port is open. Default True — matches the
                          common "open monitor + reboot to capture fresh
                          boot log" flow. Set to False when attaching to
                          an already-running board to read ongoing logs
                          without rebooting.
            drain_first:  Discard any bytes currently in the receive
                          buffer AND reset the AAG parser line state,
                          BEFORE the reset is triggered. Default True.
                          Set False only if you must preserve already-
                          buffered bytes (rare).
            wait_for:     Optional list[str] of regex patterns. If
                          non-empty, BLOCK after reset until any pattern
                          matches OR a stop condition fires. Patterns
                          are compiled with re.MULTILINE (so `^` / `$`
                          anchor to line boundaries). Pass None or [] to
                          skip waiting. Invalid regex returns
                          INVALID_PATTERN.

                          Boot-done marker (any AP/WiFi/BT config):
                          [r"START SCHEDULER"]. Boards print NO shell
                          prompt, so `\\$` / `^# ` never match — match a
                          response token instead.
            wait_timeout: Hard upper bound (seconds) on the wait. Only
                          used when wait_for is non-empty. Default 15.0.
                          Always finite — will not wait indefinitely.
            idle:         Seconds of "no new bytes" that ALSO stops the
                          wait (in addition to wait_timeout). None
                          disables idle stop. Idle only fires after at
                          least one byte has been received, so a silent
                          board cannot masquerade as "settled". When
                          `wait_for` is non-empty AND `idle` is None,
                          this defaults to 2.0 — covers the common case
                          where the boot marker lands but log keeps
                          flowing for another second.
            verbose:      Default False. When False, the `wait.data`
                          field (full captured boot log, often 1+ KB)
                          is dropped from the return — only `wait.tail`
                          (last 15 non-empty lines) and match metadata
                          are kept. Pass True when you actually want
                          the full log in the response.

        Stop priority for wait_for (whichever fires first):
            1. one of the patterns matches → reason="pattern"
            2. max bytes (64 KiB)          → reason="max_bytes"
            3. idle seconds elapse         → reason="idle"
            4. wait_timeout seconds        → reason="timeout"

        Returns:
            {success, alias, soc, transport, port, baudrate, is_open,
             reset_applied, dropped_bytes,
             wait: {reason, matched, match_index, tail,
                    elapsed_ms, buffer_left,
                    data?, data_truncated?} | null,
             _hint}
        """
        if wait_for and idle is None:
            idle = 2.0
        result = serial_connect(alias, reset=reset, drain_first=drain_first,
                                wait_for=wait_for, wait_timeout=wait_timeout,
                                idle=idle)
        if result.get("success") and result.get("wait") is not None:
            result["wait"] = _trim_wait_for_token_budget(
                result["wait"], verbose,
            )
        attach_selected_via_default(result, alias, PROJECT_ROOT)
        return result

    @mcp.tool()
    async def serial_disconnect_tool(alias: Optional[str] = None) -> Dict[str, Any]:
        """Close the serial connection bound to `alias` (if any), and
        drop the AAG parser state for that alias. Pass `alias=None` (the
        default) only when board_info.json5 has exactly one board
        configured OR when `default_alias` is set; otherwise
        ALIAS_REQUIRED is returned.
        """
        result = serial_disconnect(alias)
        attach_selected_via_default(result, alias, PROJECT_ROOT)
        return result

    @mcp.tool()
    async def serial_command_tool(
        alias: Optional[str] = None,
        *,
        data: str,
        expect: Optional[List[str]] = None,
        timeout: float = 5.0,
        idle: Optional[float] = None,
        drain_first: bool = True,
        verbose: bool = False,
    ) -> Dict[str, Any]:
        """
        Send a command and wait for response. Use this for monitor
        commands, AT commands, and any send-then-expect interaction.

        Combines drain (clear stale buffer) + write + expect (wait for
        regex match) in a single call — replaces the older 3-tool
        sequence (serial_drain → serial_write → serial_expect).

        For pure-listen scenarios (waiting for async events without
        sending) use serial_expect_tool. For non-blocking peeks at the
        receive buffer use serial_read_tool.

        Args:
            alias:       Board alias.
            data:        Text payload to send (utf-8). CRLF is appended
                         per board_info.json5's add_crlf (default true).
            expect:      Optional list[str] of regex patterns OR-matched
                         against post-write output. Empty/None → pure
                         write (no wait). Compiled with re.MULTILINE.

                         No shell prompt is echoed, so `\\$` / `^# `
                         won't match — match a response token (for `?`
                         use `[r"\\[MONITOR-I\\]"]`) or rely on `idle`.
            timeout:     Hard upper bound (seconds) for `expect`.
                         Default 5.0. Always finite.
            idle:        Seconds of silence that ALSO stops the wait.
                         None disables. When `expect` is non-empty AND
                         `idle` is None, defaults to 1.0 — covers the
                         common "command output ends with quiet
                         period" pattern.
            drain_first: Discard buffered bytes before writing. Default
                         True — prevents stale output from contaminating
                         the response. Set False only when you must
                         preserve already-buffered bytes.
            verbose:     Default False. When False, drops `expect.data`
                         (the full captured response) from the return
                         and keeps only `expect.tail` + match metadata.
                         Pass True for the full response.

        Example:
            # `?` dumps the monitor command list on every Ameba SoC; use
            # the [MONITOR-I] tag to confirm the response landed instead
            # of waiting for a (non-existent) shell prompt.
            serial_command_tool(alias="...", data="?",
                                expect=[r"\\[MONITOR-I\\]"],
                                timeout=5, idle=1)

        Returns:
            {success, alias, dropped_bytes, bytes_written,
             expect: {reason, matched, match_index, tail,
                      elapsed_ms, buffer_left,
                      data?, data_truncated?} | null,
             _hint}
        """
        if expect and idle is None:
            idle = 1.0
        result = serial_command(alias, data=data, expect=expect,
                                timeout=timeout, idle=idle,
                                drain_first=drain_first)
        if result.get("success") and result.get("expect") is not None:
            result["expect"] = _trim_wait_for_token_budget(
                result["expect"], verbose,
            )
        attach_selected_via_default(result, alias, PROJECT_ROOT)
        return result

    @mcp.tool()
    async def serial_expect_tool(
        alias: Optional[str] = None,
        *,
        patterns: Optional[List[str]] = None,
        timeout: float,
        idle: Optional[float] = None,
        max_bytes: int = 65536,
    ) -> Dict[str, Any]:
        """
        Block until one of `patterns` matches on alias's serial buffer,
        OR a stop condition fires. This is the deterministic-read
        primitive; serial_read_tool remains the non-blocking peek.

        Args:
            alias:     Board alias.
            patterns:  list[str] of regex strings, OR-matched. Compiled
                       with re.MULTILINE (`^` / `$` anchor to line
                       boundaries). Empty list is rejected unless
                       `idle` is set (which makes this a pure time-based
                       monitor).

                       Boot-done marker: [r"START SCHEDULER"]. No shell
                       prompt is printed, so `\\$` / `^# ` won't match.
            timeout:   MANDATORY hard upper bound in seconds. Always
                       finite — will NOT wait indefinitely.
            idle:      Optional. Seconds of "no new bytes" that ALSO
                       stops the wait. Only fires after at least one
                       byte has been received.
            max_bytes: Stop when this many bytes have been accumulated
                       since the call started. Defends against runaway
                       logs. Default 64 KiB.

        Stop priority (whichever fires first):
            pattern match → reason="pattern"
            max_bytes     → reason="max_bytes"
            idle elapsed  → reason="idle"
            timeout       → reason="timeout"

        Returns:
            {success, alias, reason, matched, match_index, data, tail,
             elapsed_ms, buffer_left, _hint}
        """
        result = serial_expect(alias, patterns, timeout=timeout,
                               idle=idle, max_bytes=max_bytes)
        attach_selected_via_default(result, alias, PROJECT_ROOT)
        return result

    @mcp.tool()
    async def serial_read_tool(alias: Optional[str] = None,
                               size: int = 1024) -> Dict[str, Any]:
        """
        Non-blocking peek: return whatever is currently in the receive
        buffer for `alias`, up to `size` bytes. For deterministic
        wait-for-pattern reads use serial_expect_tool.

        Multi-core AAG output is auto-detected and tagged
        ([HP]/[LP]/[AP]).

        Args:
            alias: Board alias.
            size:  Maximum bytes to read (default 1024).

        Returns:
            {success, alias, data, bytes_read, _hint}
        """
        result = serial_read(alias, size=size)
        attach_selected_via_default(result, alias, PROJECT_ROOT)
        return result
