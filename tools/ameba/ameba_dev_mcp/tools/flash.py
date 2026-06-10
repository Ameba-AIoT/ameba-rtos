"""
Alias-driven firmware flashing tool for Ameba SoCs.

The single MCP entry point is `flash_firmware_tool(alias)`. Everything
else (device profile, port, baudrate, transport, images, addresses,
chip_erase) is read from board_info.json5 + project_info.json5.

For projects in `auto` mode the flash layout is *re-parsed* from the
SDK at flash time; this catches the case where the user built via the
CLI (./ameba.py build) but flashes via MCP — keeping json5 in sync
with the actual headers.
"""

from __future__ import annotations

import base64
import json
import logging
import os
import subprocess
import sys
from datetime import datetime
from typing import Any, Dict, List, Optional, Tuple

from ameba_dev_mcp._paths import PROJECT_ROOT, REMOTE_SERVICE_DIR, SDK_ROOT, TOOLS_ROOT

for _p in (TOOLS_ROOT, REMOTE_SERVICE_DIR):
    if _p not in sys.path:
        sys.path.insert(0, _p)

from mcp.server.fastmcp import FastMCP

from ameba_dev_mcp.config.board_session import manager as session_manager
from ameba_dev_mcp.config.flashcfg_parser import (
    FlashCfgParseError,
    parse_project,
)
from ameba_dev_mcp.config.loader import (
    ConfigLoadError,
    attach_selected_via_default,
    ensure_board_info_template,
    load_board_info,
    load_project_info,
    resolve_alias_or_error,
    resolve_board,
    validate_board_config,
    validate_project_config,
)
from ameba_dev_mcp.models.schemas import ConfigError

logger = logging.getLogger(__name__)

# Flash tool resources
FLASH_BASE_DIR = os.path.join(
    os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))),
    "Flash"
)
PROFILE_DIR = os.path.join(FLASH_BASE_DIR, "Devices", "Profiles")
AMEBA_FLASH_SCRIPT = os.path.join(FLASH_BASE_DIR, "AmebaFlash.py")

DEBUG_LOG_DIR = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "log")
DEBUG_LOG_FILE = os.path.join(DEBUG_LOG_DIR, "flash.log")
_log_initialized = False


def _init_log_file():
    global _log_initialized
    if not _log_initialized:
        try:
            os.makedirs(DEBUG_LOG_DIR, exist_ok=True)
            with open(DEBUG_LOG_FILE, "w") as f:
                f.write(f"=== Log cleared at {datetime.now().isoformat()} ===\n")
            _log_initialized = True
        except Exception:
            pass


def _log_to_file(msg: str) -> None:
    _init_log_file()
    try:
        with open(DEBUG_LOG_FILE, "a") as f:
            f.write(f"{datetime.now().isoformat()} - {msg}\n")
    except Exception:
        pass


def _err_envelope(errors: List[ConfigError], *, alias: str) -> Dict[str, Any]:
    return {
        "success": False,
        "alias": alias,
        "errors": [e.model_dump() for e in errors],
    }


# ---------------------------------------------------------------------------
# Profile lookup
# ---------------------------------------------------------------------------

def _find_profile(device_name: str, memory_type: str = "nor") -> Optional[str]:
    """Find an .rdev profile by device name + memory type."""
    mt = memory_type.upper()
    for pattern in (f"{device_name}.rdev", f"{device_name}_{mt}.rdev"):
        path = os.path.join(PROFILE_DIR, pattern)
        if os.path.exists(path):
            return path
    return None


# ---------------------------------------------------------------------------
# Subprocess driver
# ---------------------------------------------------------------------------

def _run_flash_subprocess(args: List[str], timeout: int = 300) -> Dict[str, Any]:
    cmd = [sys.executable, AMEBA_FLASH_SCRIPT] + args
    _log_to_file(f"CMD: {' '.join(cmd)}")
    try:
        result = subprocess.run(cmd, capture_output=True, text=True,
                                stdin=subprocess.DEVNULL, timeout=timeout)
        stdout = result.stdout or ""
        stderr = result.stderr or ""
        _log_to_file(f"Return code: {result.returncode}")
        # Write the full stdout — we used to keep only the last 2000 chars,
        # which silently dropped early phases (prepare / check_supported_flash_size)
        # whenever the run reached the image-download phase (lots of progress lines).
        _log_to_file("STDOUT:\n" + stdout)
        if stderr:
            _log_to_file("STDERR:\n" + stderr)

        found_pass = "Finished PASS" in stdout
        found_fail = "Finished FAIL" in stdout
        if found_pass and not found_fail and result.returncode == 0:
            return {"success": True, "message": "Firmware flashed successfully",
                    "stdout": stdout, "stderr": stderr, "returncode": 0}
        if found_fail:
            msg = next((l.strip() for l in stdout.splitlines() if "Finished FAIL" in l),
                       "Flash failed")
            return {"success": False, "message": msg,
                    "stdout": stdout, "stderr": stderr, "returncode": result.returncode}
        if result.returncode != 0:
            last = stderr.strip().splitlines()[-1] if stderr.strip() else ""
            msg = last or f"Flash exited with code {result.returncode}"
            return {"success": False, "message": msg,
                    "stdout": stdout, "stderr": stderr, "returncode": result.returncode}
        return {"success": False,
                "message": "Flash ended without 'Finished PASS' marker; verify manually",
                "stdout": stdout, "stderr": stderr, "returncode": result.returncode}
    except subprocess.TimeoutExpired:
        _log_to_file(f"Subprocess timed out after {timeout}s")
        return {"success": False, "message": f"Flash timed out after {timeout}s",
                "stdout": "", "stderr": "", "returncode": -1}
    except Exception as ex:
        _log_to_file(f"Subprocess exception: {ex}")
        return {"success": False, "message": f"Failed to run flash tool: {ex}",
                "stdout": "", "stderr": str(ex), "returncode": -1}


# ---------------------------------------------------------------------------
# Live-reparse vs. json5 reconciliation (auto mode only)
# ---------------------------------------------------------------------------

_MEM_TYPE_CODE = {"ram": 0, "nor": 1, "nand": 2}


def _images_equal(a, b) -> bool:
    """Compare image lists by (type, start_addr, end_addr) — ignore path/optional."""
    if len(a) != len(b):
        return False
    for x, y in zip(a, b):
        if (x.get("type") != y.get("type")
                or x.get("start_addr", "").lower() != y.get("start_addr", "").lower()
                or (x.get("end_addr") or "").lower() != (y.get("end_addr") or "").lower()):
            return False
    return True


def _images_from_parsed(parsed) -> List[Dict[str, Any]]:
    return [{
        "type": img.type,
        "path": img.path,
        "start_addr": img.start_addr,
        "end_addr": img.end_addr,
        "optional": img.type == "VFS1",
    } for img in parsed.images]


def _images_from_entry(entry) -> List[Dict[str, Any]]:
    return [{
        "type": img.type,
        "path": img.path,
        "start_addr": img.start_addr,
        "end_addr": img.end_addr,
        "optional": img.optional,
    } for img in entry.images]


def _resolve_images_for_flash(soc: str, entry, *, alias: str) -> Tuple[Optional[List[Dict[str, Any]]], Optional[Dict[str, Any]], List[str]]:
    """
    Return (images_to_flash, error_envelope, info_messages).

    Auto mode: re-parses live SDK, then reconciles with json5:
      - parse fails              → FLASH_CFG_PARSE_FAILED
      - parse OK, json5 empty    → use parsed (info: build_firmware to sync json5)
      - parse OK, equal          → use json5
      - parse OK, differ         → FLASH_LAYOUT_OUT_OF_SYNC
    Manual mode: use json5 directly (validators already ran).
    """
    info_msgs: List[str] = []
    if entry.flash_layout_setting_mode == "manual":
        return _images_from_entry(entry), None, info_msgs

    # auto mode → re-parse. Same dual-root split as the post-build sync in
    # project.py: layout/addresses from SDK headers, build_dir + .config + bins
    # from PROJECT_ROOT. Keeping both parses on the same (sdk_root, build_base)
    # pair is what guarantees the post-build and pre-flash layouts agree.
    try:
        parsed = parse_project(SDK_ROOT, soc, build_base=PROJECT_ROOT)
    except FlashCfgParseError as ex:
        return None, _err_envelope([ConfigError(
            code="FLASH_CFG_PARSE_FAILED",
            field_path=f"projects.{soc}",
            message=f"Live flash-config parse failed for SoC '{soc}': {ex}",
            hint="Run `ameba.py build` for this SoC and retry; or fix the SDK headers.",
        )], alias=alias), info_msgs

    parsed_images = _images_from_parsed(parsed)
    json_images = _images_from_entry(entry)

    if not json_images:
        info_msgs.append(
            f"json5 has no images for '{soc}'; flashing with live-parsed layout. "
            "Run build_firmware to persist this layout to project_info.json5."
        )
        return parsed_images, None, info_msgs

    if _images_equal(parsed_images, json_images):
        return json_images, None, info_msgs

    return None, _err_envelope([ConfigError(
        code="FLASH_LAYOUT_OUT_OF_SYNC",
        field_path=f"projects.{soc}.images",
        message=(
            f"Flash layout in project_info.json5 differs from current SDK headers for SoC '{soc}'."
        ),
        hint=(
            "The project was likely rebuilt outside MCP (e.g., via `ameba.py build`). "
            "Run `build_firmware` once via MCP to refresh project_info.json5, then retry."
        ),
    )], alias=alias), info_msgs


# ---------------------------------------------------------------------------
# Main flash entry
# ---------------------------------------------------------------------------

def flash_firmware(alias: Optional[str] = None) -> Dict[str, Any]:
    """Flash firmware. `alias=None` is accepted only when board_info has
    exactly one board configured; otherwise ALIAS_REQUIRED is returned.
    """
    # Step 1: load + resolve board
    try:
        binfo = load_board_info(PROJECT_ROOT)
    except ConfigLoadError as ex:
        missing = any(e.code == "BOARD_CONFIG_MISSING" for e in ex.errors)
        env = _err_envelope(ex.errors, alias=alias)
        if missing:
            try:
                env["template_path"] = ensure_board_info_template(PROJECT_ROOT)
                env["docs_url"] = "docs/board_info.md"
                env["resource_url"] = "debug://hardware"
            except Exception:
                pass
        return env

    resolved_alias, alias_errors = resolve_alias_or_error(binfo, alias)
    if alias_errors is not None:
        env = _err_envelope(alias_errors, alias=alias)
        env["configured_aliases"] = _alias_summary(binfo)
        env["available_local_ports"] = available_local_port_names()
        return env
    alias = resolved_alias

    board_errs = validate_board_config(PROJECT_ROOT, alias=alias)
    if board_errs:
        return _err_envelope(board_errs, alias=alias)
    board = resolve_board(binfo, alias)

    # Step 2: load project entry for this SoC
    try:
        pinfo = load_project_info(PROJECT_ROOT)
    except ConfigLoadError as ex:
        return _err_envelope(ex.errors, alias=alias)

    entry = pinfo.projects.get(board.soc)
    if entry is None:
        return _err_envelope([ConfigError(
            code="PROJECT_NOT_CONFIGURED",
            field_path=f"projects.{board.soc}",
            message=f"SoC '{board.soc}' has no entry in project_info.json5",
            hint="Run build_firmware to auto-create an `auto` mode entry, or add manually.",
        )], alias=alias)

    proj_errs = validate_project_config(PROJECT_ROOT, soc=board.soc)
    # Allow AUTO_IMAGES_MISSING through if we can re-parse; report others.
    fatal = [e for e in proj_errs if e.code != "AUTO_IMAGES_MISSING"]
    if fatal:
        return _err_envelope(fatal, alias=alias)

    # Step 3: resolve images (live re-parse + reconcile)
    images, err_env, info_msgs = _resolve_images_for_flash(board.soc, entry, alias=alias)
    if err_env is not None:
        return err_env

    # Step 4: split present / missing. Optional images that don't exist
    # are silently skipped (matches amebaFlash.ts:763-774 for VFS1) — we
    # never hand a missing path to AmebaFlash.py because it stat()s the
    # file before honouring the Mandatory flag and aborts on ENOENT.
    flashable: List[Dict[str, Any]] = []
    missing_mandatory: List[str] = []
    skipped_optional: List[str] = []
    for img in images:
        if os.path.isfile(img["path"]):
            flashable.append(img)
        elif img.get("optional"):
            skipped_optional.append(img["path"])
        else:
            missing_mandatory.append(img["path"])

    if missing_mandatory:
        return _err_envelope([ConfigError(
            code="IMAGE_FILE_NOT_FOUND",
            field_path=f"projects.{board.soc}.images",
            message=f"Image files missing: {missing_mandatory}",
            hint="Run build_firmware to (re)generate the binaries.",
        )], alias=alias)

    if not flashable:
        return _err_envelope([ConfigError(
            code="IMAGE_FILE_NOT_FOUND",
            field_path=f"projects.{board.soc}.images",
            message="No flashable images: all entries are optional and missing.",
            hint="Run build_firmware to (re)generate the binaries.",
        )], alias=alias)

    if skipped_optional:
        info_msgs.append(
            f"Optional image(s) not found, skipped: "
            f"{[os.path.basename(p) for p in skipped_optional]}"
        )

    # Step 5: locate device profile
    memory_type = (entry.memory_type or pinfo.defaults.memory_type or "nor")
    profile_path = _find_profile(board.soc, memory_type)
    if profile_path is None:
        return _err_envelope([ConfigError(
            code="PROFILE_NOT_FOUND",
            field_path=f"projects.{board.soc}",
            message=f"No .rdev profile for device '{board.soc}' (memory={memory_type})",
            hint=f"Expected one of {board.soc}.rdev / {board.soc}_{memory_type.upper()}.rdev under {PROFILE_DIR}",
        )], alias=alias)

    # Step 6: free any monitor session holding the port (subprocess will reopen it).
    # Also drop the AAG parser — its `_line_start` per-core state is stale
    # after a reflash; otherwise the next read can mis-tag boot output.
    if session_manager.get(alias) is not None:
        logger.info("Releasing monitor session on '%s' before flashing", alias)
        session_manager.disconnect(alias)
    try:
        from ameba_dev_mcp.tools.serial import _drop_aag_parser
        _drop_aag_parser(alias)
    except Exception:
        pass

    # Step 7: build AmebaFlash.py CLI args
    mem_code = _MEM_TYPE_CODE.get(memory_type, 1)
    partition = []
    flashed: List[str] = []
    for img in flashable:
        # AmebaFlash needs end_addr; for manual entries that omit it we
        # synthesise from filesize.
        start = int(img["start_addr"], 16)
        if img.get("end_addr"):
            end = int(img["end_addr"], 16)
        else:
            end = start + max(os.path.getsize(img["path"]) - 1, 0)
        partition.append({
            "ImageName": img["path"],
            "StartAddress": start,
            "EndAddress": end,
            "FullErase": False,
            "MemoryType": mem_code,
            "Mandatory": not img.get("optional", False),
            "Description": os.path.basename(img["path"]),
        })
        flashed.append(os.path.basename(img["path"]))

    args = [
        "--download", "--profile", profile_path,
        "--port", board.port,
        "--baudrate", str(board.baudrate),
        "--memory-type", memory_type,
        "--log-level", "info",
    ]
    if board.transport == "remote" and board.remote is not None:
        args += ["--remote-server", board.remote.host]
        password = board.remote.password or os.environ.get("AMEBA_REMOTE_PWD")
        if password:
            args += ["--remote-password", password]
    if board.chip_erase:
        args.append("--chip-erase")

    pt_b64 = base64.b64encode(json.dumps(partition).encode("utf-8")).decode("utf-8")
    args += ["--partition-table", pt_b64]

    # Step 8: run
    res = _run_flash_subprocess(args)

    if not res["success"]:
        # Best-effort hardware-error code; the underlying tool message is preserved.
        return {
            "success": False,
            "alias": alias,
            "soc": board.soc,
            "transport": board.transport,
            "errors": [ConfigError(
                code="FLASH_HW_ERROR",
                field_path=f"boards.{alias}",
                message=res["message"],
                hint=(
                    "Check USB cable / board power / boot mode; verify "
                    f"boards.{alias}.port matches the actual device. "
                    f"Full log at {DEBUG_LOG_FILE}."
                ),
            ).model_dump()],
            "log_path": DEBUG_LOG_FILE,
            "info": info_msgs,
        }

    return {
        "success": True,
        "alias": alias,
        "soc": board.soc,
        "transport": board.transport,
        "memory_type": memory_type,
        "chip_erase": board.chip_erase,
        "images_flashed": flashed,
        "log_path": DEBUG_LOG_FILE,
        "info": info_msgs,
        "message": res["message"],
    }


# ---------------------------------------------------------------------------
# list_serial_ports
# ---------------------------------------------------------------------------

def _lsof_holder(device: str) -> Tuple[Optional[bool], Optional[str]]:
    """Best-effort: return (busy, holder) for a local serial device using lsof.

    busy:    True if any process holds it; False if explicitly empty;
             None if lsof is unavailable/errored (don't trust the answer).
    holder:  "pid=<N> cmd=<comm>[, pid=...]" if busy and lookup worked,
             else None.

    The own-process check (`held_by_self`) is layered on top by the caller
    so that a single lsof call can be reused for both fields.
    """
    import shutil
    import subprocess

    if shutil.which("lsof") is None:
        return None, None
    try:
        proc = subprocess.run(
            ["lsof", "-t", device],
            capture_output=True, text=True, timeout=2.0,
        )
    except Exception:
        return None, None
    pids = [p.strip() for p in (proc.stdout or "").splitlines() if p.strip()]
    if not pids:
        return False, None

    parts: List[str] = []
    for pid in pids:
        cmd = ""
        try:
            with open(f"/proc/{pid}/comm") as f:
                cmd = f.read().strip()
        except Exception:
            pass
        parts.append(f"pid={pid} cmd={cmd}" if cmd else f"pid={pid}")
    return True, ", ".join(parts)


def _list_local_ports() -> List[Dict[str, Any]]:
    try:
        from serial.tools import list_ports
    except Exception as ex:
        return [{"error": f"pyserial list_ports unavailable: {ex}"}]
    from ameba_dev_mcp.config.board_session import manager as session_manager

    out = []
    for p in list_ports.comports():
        busy, holder = _lsof_holder(p.device)
        held_by_self_alias = session_manager.find_local_holder(p.device)
        out.append({
            "device": p.device,
            "name": p.name,
            "description": p.description,
            "hwid": p.hwid,
            "busy": busy,
            "held_by_self": held_by_self_alias is not None,
            "held_by_self_alias": held_by_self_alias,
            "holder": holder,
        })
    return out


def available_local_port_names() -> List[str]:
    """Best-effort local port device names. Used to enrich ALIAS_NOT_FOUND
    errors so the AI/user can see what's actually plugged in."""
    try:
        from serial.tools import list_ports
        return [p.device for p in list_ports.comports()]
    except Exception:
        return []


def _alias_summary(binfo) -> Dict[str, Dict[str, Any]]:
    """Compact alias → {soc, transport, port, host} map for error hints."""
    out: Dict[str, Dict[str, Any]] = {}
    for name in sorted(binfo.boards.keys()):
        b = binfo.boards[name]
        entry: Dict[str, Any] = {
            "soc": b.soc,
            "transport": b.transport,
            "port": b.port,
        }
        if b.remote is not None:
            entry["host"] = b.remote.host
        out[name] = entry
    return out


def _query_remote_ports(host: str, tcp_port: int = 58916,
                        password: Optional[str] = None,
                        host_for_holder_lookup: Optional[str] = None,
                        ) -> List[Dict[str, Any]]:
    """Core: query AmebaRemoteService at (host, tcp_port) for its COM port
    list. Reused by _list_remote_ports (board-driven) and the
    scan_remote_ports tool (ad-hoc, no board entry needed).

    `host_for_holder_lookup` is used to look up own-process holders in the
    BoardSession registry; defaults to `host` itself.
    """
    import socket
    import time

    holder_host = host_for_holder_lookup or host
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.settimeout(5.0)
    try:
        s.connect((host, tcp_port))
        password = password or os.environ.get("AMEBA_REMOTE_PWD")
        if password:
            s.sendall(json.dumps({"type": "validate", "password": password,
                                  "id": str(int(time.time() * 1000))}).encode("utf-8") + b"\n")
            _ = s.recv(4096)
        s.sendall(json.dumps({"type": "list_com_ports",
                              "id": str(int(time.time() * 1000))}).encode("utf-8") + b"\n")
        # The server may stream multiple JSON messages; collect a single
        # `com_ports_update` and stop.
        s.settimeout(5.0)
        buf = b""
        deadline = time.time() + 5.0
        while time.time() < deadline:
            try:
                chunk = s.recv(8192)
            except socket.timeout:
                break
            if not chunk:
                break
            buf += chunk
            for line in buf.split(b"\n"):
                if not line.strip():
                    continue
                try:
                    msg = json.loads(line.decode("utf-8"))
                except Exception:
                    continue
                if msg.get("type") == "com_ports_update":
                    from ameba_dev_mcp.config.board_session import manager as session_manager
                    out: List[Dict[str, Any]] = []
                    for p in msg.get("ports", []):
                        if isinstance(p, (list, tuple)):
                            device = p[0] if len(p) > 0 else ""
                            description = p[1] if len(p) > 1 else ""
                        else:
                            device, description = p, ""
                        held_alias = session_manager.find_remote_holder(
                            holder_host, device,
                        )
                        out.append({
                            "device": device,
                            "name": device,
                            "description": description,
                            "hwid": "",
                            # busy: AmebaRemoteService doesn't expose holder
                            # info today, so we can't tell whether an external
                            # process on the remote machine holds the port.
                            # We DO know if THIS MCP holds it.
                            "busy": True if held_alias is not None else None,
                            "held_by_self": held_alias is not None,
                            "held_by_self_alias": held_alias,
                            "holder": None,
                        })
                    return out
        return [{"error": "no com_ports_update response from remote service"}]
    except Exception as ex:
        return [{"error": f"remote list failed: {ex}"}]
    finally:
        try:
            s.close()
        except Exception:
            pass


def _list_remote_ports(board) -> List[Dict[str, Any]]:
    """board-driven wrapper around _query_remote_ports."""
    if board.remote is None:
        return [{"error": "transport=remote but no remote spec"}]
    return _query_remote_ports(
        board.remote.host, board.remote.port, board.remote.password,
    )


def list_serial_ports(alias: Optional[str] = None) -> Dict[str, Any]:
    """Enumerate serial ports.

    - alias=None: enumerate ports on the local machine (no remote query).
    - alias given: resolve the board and enumerate its scope (local /
      remote). Unlike other tools, alias=None here is NOT auto-picked
      to a single board: the historical "list local ports" behavior is
      preserved so AI can still discover the local environment without
      board_info.json5 configured at all.
    """
    if alias is None:
        return {"success": True, "scope": "local", "ports": _list_local_ports()}

    try:
        binfo = load_board_info(PROJECT_ROOT)
    except ConfigLoadError as ex:
        env = _err_envelope(ex.errors, alias=alias)
        if any(e.code == "BOARD_CONFIG_MISSING" for e in ex.errors):
            try:
                env["template_path"] = ensure_board_info_template(PROJECT_ROOT)
                env["docs_url"] = "docs/board_info.md"
                env["resource_url"] = "debug://hardware"
            except Exception:
                pass
        return env

    resolved_alias, alias_errors = resolve_alias_or_error(binfo, alias)
    if alias_errors is not None:
        env = _err_envelope(alias_errors, alias=alias)
        env["configured_aliases"] = _alias_summary(binfo)
        env["available_local_ports"] = available_local_port_names()
        return env
    alias = resolved_alias
    board = resolve_board(binfo, alias)
    if board.transport == "remote":
        return {"success": True, "scope": "remote", "alias": alias,
                "host": board.remote.host, "port": board.remote.port,
                "ports": _list_remote_ports(board)}
    return {"success": True, "scope": "local", "alias": alias,
            "ports": _list_local_ports()}


# ---------------------------------------------------------------------------
# MCP registration
# ---------------------------------------------------------------------------

def register_flash_tools(mcp: FastMCP) -> None:
    @mcp.tool()
    async def flash_firmware_tool(alias: Optional[str] = None) -> Dict[str, Any]:
        """
        Flash firmware to the board identified by `alias`.

        All connection / image / address / chip_erase parameters come
        from board_info.json5 + project_info.json5. For projects in
        `auto` mode the flash layout is re-parsed from the SDK at flash
        time and reconciled with the values cached in project_info.json5;
        if they diverge the flash is rejected with FLASH_LAYOUT_OUT_OF_SYNC
        (run build_firmware first to refresh).

        Args:
            alias: Board alias from board_info.json5 (e.g. "RTL8721F_ttyUSB0").
                   May be omitted ONLY when board_info.json5 has exactly
                   one board configured — that single alias is then auto-
                   selected. With 2+ boards configured, alias is required
                   and ALIAS_REQUIRED is returned (the response lists all
                   configured aliases for you to pick from).

        Returns:
            On success: {success: true, alias, soc, images_flashed, ...}
            On failure: {success: false, alias, errors: [{code, message, hint}, ...]}

        IMPORTANT: If flashing fails, do NOT retry automatically. Report
        the error codes / hints to the user and ask for guidance. If the
        error mentions reset / boot mode / RTS/DTR / driver, read the
        MCP resource `debug://hardware` for the auto-download circuit and
        PL2303 driver requirements.
        """
        result = flash_firmware(alias)
        attach_selected_via_default(result, alias, PROJECT_ROOT)
        return result

    @mcp.tool()
    async def list_serial_ports_tool(alias: Optional[str] = None) -> Dict[str, Any]:
        """
        List available serial ports.

        - No `alias`: enumerate the local machine. If the result is empty
          and the user expects boards, ASK whether the bench is remote
          before falling back, then use scan_remote_ports_tool.
        - With `alias`: enumerate the alias's scope (local or remote).
        """
        return list_serial_ports(alias)

    @mcp.tool()
    async def scan_remote_ports_tool(
        host: str = "127.0.0.1",
        password: str = "",
        port: int = 58916,
    ) -> Dict[str, Any]:
        """
        Query AmebaRemoteService for its COM port list WITHOUT requiring a
        board_info.json5 entry. Use during initial setup to discover what
        ports are reachable on the remote machine before writing config.

        Args:
            host:     Remote service host. Default 127.0.0.1.
            password: Remote service password (empty string = none).
            port:     TCP port. Default 58916 (rarely changed).

        Returns:
            {success, host, port, ports: [{device, description, ...}, ...]}
        """
        ports = _query_remote_ports(host, port, password or None)
        if ports and isinstance(ports[0], dict) and ports[0].get("error"):
            return {"success": False, "host": host, "port": port,
                    "error": ports[0]["error"]}
        return {"success": True, "host": host, "port": port, "ports": ports}
