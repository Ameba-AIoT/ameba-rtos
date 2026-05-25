"""
Alias-driven serial session manager.

Maps `alias -> ActiveSession`. Each session wraps either a local
pyserial.Serial or a RemoteSerial (when transport=remote). The MCP
serial / flash / reset_device tools take only an alias and resolve
the actual board from board_info.json5 via this manager.

A single global `manager` is exposed; tests can construct their own
`BoardSessionManager()` for isolation.
"""

from __future__ import annotations

import atexit
import logging
import os
import sys
import threading
import time
from dataclasses import dataclass, field
from typing import Dict, List, Optional

import serial
from serial.serialutil import SerialException

from ameba_dev_mcp._paths import REMOTE_SERVICE_DIR, TOOLS_ROOT
from ameba_dev_mcp.models.schemas import ConfigError, ResolvedBoard

# RemoteService imports require both TOOLS_ROOT (so `ameba.*` resolves as a
# package) and REMOTE_SERVICE_DIR (so `remote_serial.py`'s own
# `from version_manager import *` resolves) on sys.path.
for _p in (TOOLS_ROOT, REMOTE_SERVICE_DIR):
    if _p not in sys.path:
        sys.path.insert(0, _p)

from remote_serial import RemoteSerial  # noqa: E402

logger = logging.getLogger(__name__)


class BoardSessionError(Exception):
    """Connection-time failure carrying a structured ConfigError."""
    def __init__(self, error: ConfigError):
        self.error = error
        super().__init__(error.message)


# ---------------------------------------------------------------------------
# Session record
# ---------------------------------------------------------------------------

@dataclass
class ActiveSession:
    alias: str
    board: ResolvedBoard
    connection: object              # serial.Serial or RemoteSerial
    is_remote: bool
    aag_parser: object = None       # populated lazily by serial_read tool
    created_at: float = field(default_factory=time.monotonic)
    last_used_at: float = field(default_factory=time.monotonic)

    def is_open(self) -> bool:
        c = self.connection
        if hasattr(c, "is_open"):
            v = c.is_open
            return bool(v() if callable(v) else v)
        if hasattr(c, "isOpen"):
            return bool(c.isOpen())
        return False

    def close(self) -> None:
        try:
            if self.is_remote:
                self.connection.close(close_tcp=True)
            else:
                self.connection.close()
        except Exception as ex:
            logger.warning("Error closing session %s: %s", self.alias, ex)

    def touch(self) -> None:
        self.last_used_at = time.monotonic()


# ---------------------------------------------------------------------------
# Manager
# ---------------------------------------------------------------------------

_IDLE_TTL_SECONDS = 30 * 60  # 30 min: same as the legacy connection_id reaper


class BoardSessionManager:
    """Process-wide singleton mapping alias -> ActiveSession."""

    def __init__(self):
        self._lock = threading.RLock()
        self._sessions: Dict[str, ActiveSession] = {}
        self._reaper_started = False

    # -- lookup ------------------------------------------------------------

    def get(self, alias: str) -> Optional[ActiveSession]:
        with self._lock:
            return self._sessions.get(alias)

    def aliases(self) -> List[str]:
        with self._lock:
            return list(self._sessions.keys())

    def find_local_holder(self, device: str) -> Optional[str]:
        """Return the alias of an OPEN local session bound to `device`, or None.

        Used by `list_serial_ports_tool` to flag held_by_self for local ports.
        """
        with self._lock:
            for alias, sess in self._sessions.items():
                if not sess.is_remote and sess.board.port == device and sess.is_open():
                    return alias
        return None

    def find_remote_holder(self, host: str, port_name: str) -> Optional[str]:
        """Return the alias of an OPEN remote session bound to (host, port_name)."""
        with self._lock:
            for alias, sess in self._sessions.items():
                if (sess.is_remote
                        and sess.board.remote is not None
                        and sess.board.remote.host == host
                        and sess.board.port == port_name
                        and sess.is_open()):
                    return alias
        return None

    # -- connect / disconnect ---------------------------------------------

    def connect(self, board: ResolvedBoard, *, monitor_mode: bool = False) -> ActiveSession:
        """Open (or reuse) a serial session for `board`.

        `monitor_mode=True` uses board.monitor_baudrate; otherwise the
        flash baudrate. Idempotent: a second call returns the existing
        session if it's still open.
        """
        alias = board.alias
        with self._lock:
            existing = self._sessions.get(alias)
            if existing is not None and existing.is_open():
                existing.touch()
                return existing
            if existing is not None:
                # stale entry; drop it
                self._sessions.pop(alias, None)

            baud = board.monitor_baudrate if monitor_mode else board.baudrate
            if board.transport == "remote":
                conn = self._open_remote(board, baud)
                is_remote = True
            else:
                conn = self._open_local(board, baud)
                is_remote = False

            sess = ActiveSession(
                alias=alias,
                board=board,
                connection=conn,
                is_remote=is_remote,
            )
            self._sessions[alias] = sess
            self._ensure_reaper()
            return sess

    def disconnect(self, alias: str) -> bool:
        with self._lock:
            sess = self._sessions.pop(alias, None)
        if sess is None:
            return False
        sess.close()
        return True

    def disconnect_all(self) -> None:
        with self._lock:
            sessions = list(self._sessions.values())
            self._sessions.clear()
        for s in sessions:
            s.close()

    # -- transport-specific opens -----------------------------------------

    @staticmethod
    def _open_local(board: ResolvedBoard, baudrate: int) -> serial.Serial:
        try:
            return serial.Serial(port=board.port, baudrate=baudrate, timeout=1.0)
        except SerialException as ex:
            msg = str(ex).lower()
            if "no such file" in msg or "could not open port" in msg and "errno 2" in msg:
                code = "PORT_NOT_FOUND"
                hint = (
                    f"Serial port '{board.port}' does not exist. Check the USB cable "
                    f"and update boards.{board.alias}.port in board_info.json5."
                )
            elif "permission denied" in msg or "errno 13" in msg:
                code = "PORT_BUSY"
                hint = (
                    f"Permission denied on '{board.port}'. Either another process holds "
                    "it (close monitors / VS Code serial views) or your user is not in the "
                    "dialout/uucp group."
                )
            elif "device or resource busy" in msg or "errno 16" in msg:
                code = "PORT_BUSY"
                hint = f"'{board.port}' is held by another process. Close it and retry."
            else:
                code = "PORT_OPEN_FAILED"
                hint = f"Failed to open '{board.port}': {ex}"
            raise BoardSessionError(ConfigError(
                code=code,
                field_path=f"boards.{board.alias}.port",
                message=f"Cannot open serial port '{board.port}' for board '{board.alias}': {ex}",
                hint=hint,
            ))
        except Exception as ex:
            raise BoardSessionError(ConfigError(
                code="PORT_OPEN_FAILED",
                field_path=f"boards.{board.alias}.port",
                message=f"Unexpected error opening '{board.port}': {ex}",
            ))

    @staticmethod
    def _open_remote(board: ResolvedBoard, baudrate: int) -> RemoteSerial:
        if board.remote is None:
            raise BoardSessionError(ConfigError(
                code="REMOTE_SPEC_MISSING",
                field_path=f"boards.{board.alias}.remote",
                message=f"transport=remote requires a `remote` block on board '{board.alias}'",
            ))
        host = board.remote.host
        port = board.remote.port
        # Allow env override of password
        password = board.remote.password or os.environ.get("AMEBA_REMOTE_PWD")

        try:
            conn = RemoteSerial(
                logger=logger,
                remote_server=host,
                remote_port=port,
                port=board.port,
                baudrate=baudrate,
            )
        except SerialException as ex:
            msg = str(ex).lower()
            if "refused" in msg or "timed out" in msg or "timeout" in msg or "unreachable" in msg:
                code = "REMOTE_UNREACHABLE"
                hint = (
                    f"Cannot reach AmebaRemoteService at {host}:{port}. Verify the host is "
                    "powered on, AmebaRemoteService.py is running, and "
                    f"boards.{board.alias}.remote.host / .port are correct."
                )
            else:
                code = "REMOTE_TCP_FAILED"
                hint = f"TCP setup to {host}:{port} failed: {ex}"
            raise BoardSessionError(ConfigError(
                code=code,
                field_path=f"boards.{board.alias}.remote",
                message=f"Remote connection to {host}:{port} failed: {ex}",
                hint=hint,
            ))
        except Exception as ex:
            raise BoardSessionError(ConfigError(
                code="REMOTE_TCP_FAILED",
                field_path=f"boards.{board.alias}.remote",
                message=f"Remote connection to {host}:{port} failed: {ex}",
            ))

        # Authenticate if password is set
        if password is not None:
            try:
                conn.validate(password)
            except Exception as ex:
                try:
                    conn.close(close_tcp=True)
                except Exception:
                    pass
                raise BoardSessionError(ConfigError(
                    code="REMOTE_AUTH_FAILED",
                    field_path=f"boards.{board.alias}.remote.password",
                    message=f"Remote auth rejected for {host}:{port}: {ex}",
                    hint=(
                        f"Update boards.{board.alias}.remote.password in board_info.json5 "
                        "or set AMEBA_REMOTE_PWD."
                    ),
                ))

        # Open the actual serial channel on the remote side
        try:
            conn.open()
        except Exception as ex:
            try:
                conn.close(close_tcp=True)
            except Exception:
                pass
            msg = str(ex).lower()
            if "permission" in msg or "auth" in msg:
                code = "REMOTE_AUTH_FAILED"
            elif "no such" in msg or "not found" in msg:
                code = "PORT_NOT_FOUND"
            elif "busy" in msg or "in use" in msg:
                code = "PORT_BUSY"
            else:
                code = "REMOTE_OPEN_FAILED"
            raise BoardSessionError(ConfigError(
                code=code,
                field_path=f"boards.{board.alias}.port",
                message=f"Remote port '{board.port}' open failed: {ex}",
                hint=(
                    f"Verify '{board.port}' exists on remote host {host} and isn't held "
                    "by another process."
                ),
            ))
        return conn

    # -- idle reaper ------------------------------------------------------

    def _ensure_reaper(self) -> None:
        if self._reaper_started:
            return
        self._reaper_started = True
        t = threading.Thread(
            target=self._reaper_loop, daemon=True, name="board-session-reaper",
        )
        t.start()

    def _reaper_loop(self) -> None:
        while True:
            time.sleep(60)
            try:
                self._reap_expired()
            except Exception as ex:
                logger.debug("reaper iteration failed: %s", ex)

    def _reap_expired(self) -> None:
        now = time.monotonic()
        expired: List[str] = []
        with self._lock:
            for a, s in self._sessions.items():
                if now - s.last_used_at > _IDLE_TTL_SECONDS:
                    expired.append(a)
        for a in expired:
            logger.info("Reaping idle board session '%s'", a)
            self.disconnect(a)


# ---------------------------------------------------------------------------
# Process-wide singleton
# ---------------------------------------------------------------------------

manager = BoardSessionManager()


def _atexit_cleanup() -> None:
    try:
        manager.disconnect_all()
    except Exception:
        pass


atexit.register(_atexit_cleanup)
