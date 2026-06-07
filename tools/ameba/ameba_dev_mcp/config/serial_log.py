"""
Background serial-log capture for board sessions.

When a board has `serial_log_record.enable = true`, opening its serial
session attaches two cooperating objects to the ActiveSession:

  * `SerialLogger`  — owns the .log file. Decodes the raw byte stream with
    its OWN (never-reset) AAGParser, prefixes every line with a
    `HH:MM:SS.mmm` timestamp, and writes to disk. Handles per-day file
    rotation and board_info.json5 write-back of the active file name.

  * `LoggingReader` — a background thread that becomes the SOLE consumer
    of the underlying connection's RX. It continuously reads bytes, feeds
    them to the SerialLogger (the full, un-drained stream) AND re-buffers
    them for the agent-facing read tools. It exposes a pyserial-compatible
    read API (inWaiting / read / reset_input_buffer / flushInput) so the
    serial tools can point at it transparently. `drain_first` clears only
    THIS buffer — the log file keeps every byte.

Logging is fully opt-in: when disabled, no thread is started and the
serial tools read the real connection directly (behaviour unchanged).

File-name rules (see SerialLogRecord): the generated pattern is
`<alias>_<YYYYMMDD>_<HHMMSS>.log`. A name matching this pattern is
auto-managed (new file per day, written back); any other (user-custom)
name is used verbatim and never rotated.
"""

from __future__ import annotations

import logging
import os
import re
import threading
from datetime import datetime
from typing import Optional, Tuple

from ameba_dev_mcp._paths import PROJECT_ROOT
from ameba_dev_mcp.config.aag import AAGParser

logger = logging.getLogger(__name__)

DEFAULT_LOG_SUBDIR = "mcp_serial_log"


# ---------------------------------------------------------------------------
# File-name resolution + write-back
# ---------------------------------------------------------------------------

def _auto_name_re(alias: str) -> "re.Pattern[str]":
    return re.compile(rf"^{re.escape(alias)}_(\d{{8}})_(\d{{6}})\.log$")


def _gen_filename(alias: str, now: datetime) -> str:
    return f"{alias}_{now:%Y%m%d_%H%M%S}.log"


def _resolve_log_dir(rec, project_root: str) -> str:
    raw = rec.log_dir
    if not raw:
        return os.path.join(project_root, DEFAULT_LOG_SUBDIR)
    if os.path.isabs(raw):
        return raw
    return os.path.join(project_root, raw)


def resolve_log_target(alias: str, rec, project_root: str,
                       now: datetime) -> Tuple[str, str, bool, bool]:
    """Decide where to write and whether the name must be regenerated.

    Returns (log_dir, file_name, auto_rotate, need_rewrite).

    auto_rotate  — True when the name is MCP-generated (and so should roll
                   over per day); False for a user-supplied custom name.
    need_rewrite — True when file_name differs from what's in `rec` and so
                   must be persisted back to board_info.json5.
    """
    log_dir = _resolve_log_dir(rec, project_root)
    cur = (rec.file_name or "").strip()
    today = now.strftime("%Y%m%d")

    if not cur:
        return log_dir, _gen_filename(alias, now), True, True

    m = _auto_name_re(alias).match(cur)
    if m:
        if m.group(1) == today:
            return log_dir, cur, True, False          # reuse today's file
        return log_dir, _gen_filename(alias, now), True, True  # new day
    # User-custom name: keep as-is, never rotate, never rewrite.
    return log_dir, cur, False, False


def persist_filename(alias: str, file_name: str, project_root: str) -> bool:
    """Best-effort write-back of `file_name` into
    board_info.json5 .boards[alias].serial_log_record. Never raises:
    a broken/locked config must not break live logging.
    """
    try:
        from ameba_dev_mcp.config.loader import (
            ConfigLoadError,
            load_board_info,
            save_board_info,
        )
        try:
            info = load_board_info(project_root)
        except ConfigLoadError:
            return False
        entry = info.boards.get(alias)
        if entry is None or entry.serial_log_record is None:
            return False
        if entry.serial_log_record.file_name == file_name:
            return False
        entry.serial_log_record.file_name = file_name
        save_board_info(project_root, info)
        return True
    except Exception as ex:  # pragma: no cover - defensive
        logger.warning("serial-log filename write-back failed for %s: %s", alias, ex)
        return False


# ---------------------------------------------------------------------------
# SerialLogger — owns the .log file
# ---------------------------------------------------------------------------

class SerialLogger:
    """Timestamped, AAG-decoded log writer for one alias."""

    def __init__(self, alias: str, log_dir: str, file_name: str, *,
                 auto_rotate: bool, project_root: str, now: datetime):
        self._alias = alias
        self._log_dir = log_dir
        self._file_name = file_name
        self._auto_rotate = auto_rotate
        self._project_root = project_root
        self._parser = AAGParser()           # independent; never reset by drain
        self._lock = threading.Lock()
        self._fh = None
        self._closed = False
        self._at_line_start = True
        self._eol_lf = 0                     # LF/CR counts of the pending
        self._eol_cr = 0                     #   EOL run (may span chunks)
        self._date = now.strftime("%Y%m%d")
        os.makedirs(self._log_dir, exist_ok=True)
        self._open_fh(now, reason="opened")

    @property
    def path(self) -> str:
        return os.path.join(self._log_dir, self._file_name)

    # -- file handle -------------------------------------------------------

    def _open_fh(self, now: datetime, *, reason: str) -> None:
        self._fh = open(self.path, "a", encoding="utf-8", buffering=1)
        self._at_line_start = True
        self._eol_lf = 0
        self._eol_cr = 0
        self._fh.write(
            f"==== serial log [{self._alias}] {reason} "
            f"{now:%Y-%m-%d %H:%M:%S} ====\n"
        )
        self._fh.flush()

    def _close_fh(self) -> None:
        if self._fh is not None:
            try:
                self._fh.flush()
                self._fh.close()
            except Exception:
                pass
            self._fh = None

    def _rotate(self, now: datetime) -> None:
        self._close_fh()
        self._file_name = _gen_filename(self._alias, now)
        self._date = now.strftime("%Y%m%d")
        persist_filename(self._alias, self._file_name, self._project_root)
        self._open_fh(now, reason="rotated")

    # -- writing -----------------------------------------------------------

    def feed(self, raw: bytes) -> None:
        if not raw:
            return
        with self._lock:
            if self._closed or self._fh is None:
                return
            now = datetime.now()
            if self._auto_rotate and now.strftime("%Y%m%d") != self._date:
                self._rotate(now)
            text = self._parser.parse(raw)
            if text:
                self._emit(text, now)

    def _emit(self, text: str, now: datetime) -> None:
        # Two jobs, line by line:
        #  1. EOL normalisation. The device mixes EOL conventions and at some
        #     boundaries appends a STRAY extra CR, so one logical newline can
        #     arrive as "\n", "\n\r", "\r\n", "\n\r\r", "\r\n\r", ... A naive
        #     "each CR/LF is a break" (or even pairwise) rule turns that stray
        #     CR into a blank line between every record. Instead we collapse
        #     each maximal run of CR/LF into N "\n": N = the LF count in the
        #     run, or — for a pure-CR run, e.g. a lone "\r" the MCU emits for
        #     an in-place refresh — the CR count. So a CR adjacent to an LF is
        #     absorbed (decoration), a CR standing alone (its own run) is a
        #     real break, and a doubled terminator ("\n\n") still yields a
        #     genuine blank line. The run counts carry across feed() chunks.
        #  2. Lazy timestamping: a "HH:MM:SS.mmm " stamp is written only right
        #     before the first content char of a line — never on a blank line
        #     and never stolen by a CR/LF.
        # Non-printable C0 control noise (a stray NUL from ROM baud switching,
        # etc.) and DEL are dropped FIRST; a dropped byte does not break an
        # EOL run (so "\n\x00\r" is still one ending). Tab is preserved.
        stamp = f"{now:%H:%M:%S}.{now.microsecond // 1000:03d} "
        out = []
        for ch in text:
            o = ord(ch)
            if (o < 0x20 and ch not in "\t\r\n") or o == 0x7F:
                continue
            if ch == "\n":
                self._eol_lf += 1
                continue
            if ch == "\r":
                self._eol_cr += 1
                continue
            if self._eol_lf or self._eol_cr:        # close the pending run
                out.append("\n" * (self._eol_lf or self._eol_cr))
                self._at_line_start = True
                self._eol_lf = self._eol_cr = 0
            if self._at_line_start:
                out.append(stamp)
                self._at_line_start = False
            out.append(ch)
        try:
            self._fh.write("".join(out))
            self._fh.flush()
        except Exception as ex:  # pragma: no cover - disk error
            logger.warning("serial-log write failed for %s: %s", self._alias, ex)

    def close(self) -> None:
        with self._lock:
            if self._closed:
                return
            self._closed = True
            # Flush any AAG partial-frame residual before closing.
            try:
                residual = self._parser.flush()
                if residual and self._fh is not None:
                    self._emit(residual, datetime.now())
                # Flush a dangling EOL run so the final line is terminated.
                if (self._eol_lf or self._eol_cr) and self._fh is not None:
                    self._fh.write("\n" * (self._eol_lf or self._eol_cr))
                    self._eol_lf = self._eol_cr = 0
                    self._at_line_start = True
                # Footer mirroring the "opened"/"rotated" header, on its own
                # line (end a dangling mid-line first so it never appends to
                # captured content).
                if self._fh is not None:
                    now = datetime.now()
                    prefix = "" if self._at_line_start else "\n"
                    self._fh.write(
                        f"{prefix}==== serial log [{self._alias}] closed "
                        f"{now:%Y-%m-%d %H:%M:%S} ====\n"
                    )
            except Exception:
                pass
            self._close_fh()


# ---------------------------------------------------------------------------
# LoggingReader — background pump + tool-facing buffer
# ---------------------------------------------------------------------------

_POLL_IDLE_S = 0.01  # 10ms when no bytes are pending


class LoggingReader:
    """Sole RX consumer of `conn`; logs everything, re-buffers for tools.

    Presents the subset of the pyserial read API the serial tools use:
    inWaiting() / in_waiting / read(n) / reset_input_buffer() / flushInput().
    Writes still go directly to the underlying connection by the tools.
    """

    def __init__(self, conn, serial_logger: SerialLogger):
        self._conn = conn
        self._logger = serial_logger
        self._buf = bytearray()
        self._lock = threading.Lock()
        self._stop = threading.Event()
        self._thread = threading.Thread(
            target=self._loop, daemon=True,
            name=f"serial-log-pump-{serial_logger._alias}",
        )
        self._thread.start()

    # -- pump loop ---------------------------------------------------------

    def _conn_waiting(self) -> int:
        try:
            if hasattr(self._conn, "inWaiting"):
                return int(self._conn.inWaiting() or 0)
            return int(getattr(self._conn, "in_waiting", 0) or 0)
        except Exception:
            return 0

    def _loop(self) -> None:
        while not self._stop.is_set():
            waiting = self._conn_waiting()
            data = b""
            if waiting:
                try:
                    data = self._conn.read(waiting)
                except Exception:
                    data = b""
            if data:
                try:
                    self._logger.feed(data)
                except Exception:
                    pass
                with self._lock:
                    self._buf.extend(data)
            else:
                self._stop.wait(_POLL_IDLE_S)

    # -- pyserial-compatible read API (tool-facing buffer) -----------------

    def inWaiting(self) -> int:  # noqa: N802 (pyserial legacy name)
        with self._lock:
            return len(self._buf)

    @property
    def in_waiting(self) -> int:
        with self._lock:
            return len(self._buf)

    def read(self, size: int = 1) -> bytes:
        with self._lock:
            chunk = bytes(self._buf[:size])
            del self._buf[:size]
            return chunk

    def reset_input_buffer(self) -> None:
        with self._lock:
            self._buf.clear()

    def flushInput(self) -> None:  # noqa: N802 (pyserial legacy name)
        self.reset_input_buffer()

    # -- lifecycle ---------------------------------------------------------

    def stop(self) -> None:
        self._stop.set()
        try:
            self._thread.join(timeout=2.0)
        except Exception:
            pass


# ---------------------------------------------------------------------------
# Factory
# ---------------------------------------------------------------------------

def attach(conn, board, project_root: str = PROJECT_ROOT
           ) -> Optional[Tuple[LoggingReader, SerialLogger]]:
    """Build (LoggingReader, SerialLogger) for `board` if logging is enabled.

    Returns None when logging is disabled or setup fails (logging must
    never block opening the serial port). On failure the caller proceeds
    with direct connection reads.
    """
    rec = getattr(board, "serial_log_record", None)
    if rec is None or not rec.enable:
        return None
    try:
        now = datetime.now()
        log_dir, file_name, auto_rotate, need_rewrite = resolve_log_target(
            board.alias, rec, project_root, now,
        )
        slog = SerialLogger(
            board.alias, log_dir, file_name,
            auto_rotate=auto_rotate, project_root=project_root, now=now,
        )
        if need_rewrite:
            persist_filename(board.alias, file_name, project_root)
        reader = LoggingReader(conn, slog)
        logger.info("serial-log capture started for %s -> %s", board.alias, slog.path)
        return reader, slog
    except Exception as ex:
        logger.warning("serial-log capture setup failed for %s: %s", board.alias, ex)
        return None
