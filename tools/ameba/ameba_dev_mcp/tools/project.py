"""
Project-level tools for Ameba SoC: build, (future: kconfig).

Wraps `ameba.py build -q` and post-processes the output before returning
to the MCP caller:
  - Full raw log saved to build_<SOC>/build.log for AI reference.
  - Compiler command lines inside FAILED blocks are replaced with just the
    portion starting at `-c` (the source file path); full command in log_path.
"""

import json
import os
import re
import subprocess
import sys
from typing import Optional

from mcp.server.fastmcp import FastMCP
from ameba_dev_mcp._paths import SDK_ROOT

AMEBA_PY = os.path.join(SDK_ROOT, "ameba.py")

_ANSI_RE = re.compile(r"\x1b\[[0-9;]*[mK]")
_NINJA_FAIL_RE = re.compile(r"^FAILED:")
_NINJA_PROGRESS_RE = re.compile(r"^\[\d+/\d+\]")
# Matches ccache-prefixed or absolute-path compiler invocations
_COMPILER_CMD_RE = re.compile(r"^(ccache\s|/\S+-(?:gcc|g\+\+|cc)\s)")


def _trim_compiler_cmd(line: str, log_path: str) -> str:
    plain = _ANSI_RE.sub("", line)
    idx = plain.rfind(" -c ")  # last -c is the source file arg
    if idx == -1:
        return line
    return f"{plain[idx + 1:].strip()}\n[full compiler command in {log_path}]"


def _postprocess(raw_output: str, log_path: str) -> str:
    """Replace compiler command lines in FAILED blocks with just the -c <src> portion."""
    lines = raw_output.splitlines()
    result = []
    in_failed = False
    next_is_cmd = False

    for line in lines:
        plain = _ANSI_RE.sub("", line)

        if _NINJA_PROGRESS_RE.match(plain):
            in_failed = False
            next_is_cmd = False
            result.append(line)
            continue

        if _NINJA_FAIL_RE.match(plain):
            in_failed = True
            next_is_cmd = True
            result.append(line)
            continue

        if in_failed and next_is_cmd and _COMPILER_CMD_RE.match(plain):
            next_is_cmd = False
            result.append(_trim_compiler_cmd(line, log_path))
            continue

        result.append(line)

    return "\n".join(result)


def _read_current_soc() -> Optional[str]:
    info_file = os.path.join(SDK_ROOT, "soc_info.json")
    if not os.path.exists(info_file):
        return None
    try:
        with open(info_file) as f:
            return json.load(f).get("soc", {}).get("name")
    except Exception:
        return None


def run_build_quiet(
    soc: Optional[str] = None,
    example: Optional[str] = None,
    clean: bool = False,
    pristine: bool = False,
) -> dict:
    """Run ``ameba.py build -q`` and return a result dict with stripped output.

    Keys returned:
      success   bool
      summary   short human-readable status
      output    filtered build log with -I paths removed from failed steps
      log_path  path to full unstripped log (pass to Read tool if needed)
      soc       the SoC that was built
    """
    effective_soc = soc or _read_current_soc()
    if not effective_soc:
        return {
            "success": False,
            "summary": "No SoC selected. Pass soc= or run `ameba.py soc <name>` first.",
            "output": "",
            "log_path": None,
            "soc": None,
        }

    soc_workdir = os.path.join(SDK_ROOT, f"build_{effective_soc}")
    log_path = os.path.join(soc_workdir, "build.log")

    # Base build command (no -q yet; added per sub-command below)
    base_cmd = [sys.executable, "-u", AMEBA_PY, "build"]
    if soc:
        base_cmd.append(soc)

    # 1800s (30 min) covers first-time toolchain extract + a clean full build.
    # Pure code-change rebuilds finish in well under a minute.
    build_timeout = int(os.environ.get("AMEBA_BUILD_TIMEOUT", "1800"))

    def _run(cmd: list) -> Optional[subprocess.CompletedProcess]:
        return subprocess.run(
            cmd,
            cwd=SDK_ROOT,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            stdin=subprocess.DEVNULL,
            timeout=build_timeout,
        )

    try:
        # `ameba.py build -c` only cleans; it does NOT rebuild.
        # Run it first, then follow up with a normal build.
        if clean:
            _run(base_cmd + ["-c"])

        build_cmd = base_cmd + ["-q"]
        if example:
            build_cmd += ["-a", example]
        if pristine:
            build_cmd.append("-p")

        proc = _run(build_cmd)
    except subprocess.TimeoutExpired:
        return {
            "success": False,
            "summary": f"Build timed out after {build_timeout} s "
                       "(set AMEBA_BUILD_TIMEOUT to override)",
            "output": "",
            "log_path": log_path,
            "soc": effective_soc,
        }
    except Exception as exc:
        return {
            "success": False,
            "summary": str(exc),
            "output": "",
            "log_path": None,
            "soc": effective_soc,
        }

    raw = proc.stdout.decode("utf-8", errors="replace")
    stripped = _ANSI_RE.sub("", raw)

    os.makedirs(soc_workdir, exist_ok=True)
    with open(log_path, "w", encoding="utf-8") as f:
        f.write(stripped)

    success = proc.returncode == 0
    processed = _postprocess(stripped, log_path)

    if success:
        has_warnings = ": warning:" in raw
        summary = "Build done (with warnings)" if has_warnings else "Build done"
    else:
        summary = "Build failed"

    project_info_status: Optional[dict] = None
    if success:
        project_info_status = _sync_project_info(effective_soc)

    result = {
        "success": success,
        "summary": summary,
        "output": processed,
        "log_path": log_path,
        "soc": effective_soc,
    }
    if project_info_status is not None:
        result["project_info"] = project_info_status
    return result


def _sync_project_info(soc: str) -> dict:
    """After a successful build, refresh project_info.json5 for `soc`.

    Auto-mode entries get `build_dir` + `images` overwritten with values
    re-parsed from the SDK; manual-mode entries are left alone.
    """
    try:
        from ameba_dev_mcp.config.flashcfg_parser import parse_project, FlashCfgParseError
        from ameba_dev_mcp.config.loader import (
            ConfigLoadError, update_project_for_soc,
        )
    except Exception as ex:
        return {"updated": False, "error": f"import failed: {ex}"}

    try:
        parsed = parse_project(SDK_ROOT, soc)
    except FlashCfgParseError as ex:
        return {"updated": False, "soc": soc, "error": f"parse failed: {ex}"}
    except Exception as ex:
        return {"updated": False, "soc": soc, "error": str(ex)}

    try:
        info = update_project_for_soc(SDK_ROOT, parsed)
    except ConfigLoadError as ex:
        return {"updated": False, "soc": soc,
                "error": "; ".join(f"[{e.code}] {e.message}" for e in ex.errors)}
    except Exception as ex:
        return {"updated": False, "soc": soc, "error": str(ex)}

    entry = info.projects.get(soc)
    return {
        "updated": True,
        "soc": soc,
        "mode": entry.flash_layout_setting_mode if entry else None,
        "image_count": len(entry.images) if entry else 0,
    }


def _write_soc_info(soc_name: str) -> None:
    info_file = os.path.join(SDK_ROOT, "soc_info.json")
    with open(info_file, "w", encoding="utf-8") as f:
        json.dump({"soc": {"name": soc_name}}, f, indent=4)


def register_project_tools(mcp: FastMCP) -> None:
    """Register project-level MCP tools."""

    @mcp.tool()
    async def set_target(soc_name: str) -> dict:
        """Set the build target SoC. Must be called before build_firmware when
        switching targets. Subsequent build_firmware calls will use this target
        automatically without needing to pass any parameters.

        Writes to a shared file (`<SDK_ROOT>/soc_info.json`); do not call
        this tool (or build_firmware) concurrently with another set_target
        / build_firmware for a different SoC — they race on the same file.

        Args:
            soc_name: SoC name to target (e.g. RTL8721F, RTL8720E, RTL8730E).

        Returns:
            success   Whether the target was set successfully.
            soc       The SoC name that was set.
        """
        try:
            _write_soc_info(soc_name)
            return {"success": True, "soc": soc_name}
        except Exception as exc:
            return {"success": False, "error": str(exc)}

    @mcp.tool()
    async def build_firmware(
        app: Optional[str] = None,
        clean: bool = False,
        pristine: bool = False,
        summary_only: bool = True,
        alias: Optional[str] = None,
    ) -> dict:
        """Build firmware for the selected Ameba SoC using quiet mode.

        Runs `ameba.py build -q` and returns filtered output. The compiler
        command in error blocks is trimmed to just the source file path;
        read the file at `log_path` for the full compiler command.

        IMPORTANT — DO NOT call this tool in parallel for different SoCs.

        Args:
            app:          Application folder name to build (e.g. "raw_flash_otp").
                          Pass only the last path component, not the full path.
                          Omit to build the currently configured application.
            clean:        Clean intermediate object files first (-c), then rebuild.
                          `ameba.py build -c` only cleans without rebuilding, so
                          this tool runs clean followed by a normal build automatically.
            pristine:     Remove the entire build directory, then rebuild (-p).
                          WARNING: This resets ALL Kconfig values back to their
                          defaults — any custom kconfig_set changes will be lost.
                          ALWAYS confirm with the user before passing pristine=True.
            summary_only: Default True. On SUCCESSFUL builds, drop the
                          `output` field (10+ INFOR/cmake lines) — the
                          agent only needs success + log_path. On FAILED
                          builds, `output` is always returned so the
                          agent can read errors. Pass False to always
                          include `output`.
            alias:        Optional. Board alias from board_info.json5; the
                          alias's SoC is used as the build target,
                          equivalent to running set_target before this
                          call. Use this to keep build & flash on the
                          same coordinate system in multi-board setups —
                          e.g. build_firmware(alias="RTL8720E_COM20")
                          followed by flash_firmware_tool(alias=...).
                          Omit to use the SoC selected by the most
                          recent set_target. When passed, the value of
                          `set_target` for subsequent calls IS updated
                          (so a follow-up build without alias keeps
                          building the same SoC).

        Returns:
            success         Whether the build succeeded.
            summary         One-line status ("Build done" / "Build failed" / …).
            output          Filtered build log (only included on failure when
                            summary_only=True).
            log_path        Absolute path to the full unstripped build log.
            soc             The SoC that was built.
            retargeted_from When `alias` triggered a SoC switch, the
                            previously-active SoC; otherwise omitted.
        """
        retargeted_from = None
        if alias is not None:
            try:
                from ameba_dev_mcp._paths import SDK_ROOT as _SDK_ROOT
                from ameba_dev_mcp.config.loader import (
                    ConfigLoadError,
                    load_board_info,
                    resolve_alias_or_error,
                    resolve_board,
                )
                binfo = load_board_info(_SDK_ROOT)
            except ConfigLoadError as ex:
                return {
                    "success": False,
                    "summary": "Failed to load board_info.json5",
                    "errors": [e.model_dump() for e in ex.errors],
                    "soc": None,
                    "log_path": None,
                }
            resolved, alias_errs = resolve_alias_or_error(binfo, alias)
            if alias_errs is not None:
                return {
                    "success": False,
                    "summary": f"alias '{alias}' resolution failed",
                    "errors": [e.model_dump() for e in alias_errs],
                    "soc": None,
                    "log_path": None,
                }
            target_soc = resolve_board(binfo, resolved).soc
            current_soc = _read_current_soc()
            if current_soc != target_soc:
                try:
                    _write_soc_info(target_soc)
                    retargeted_from = current_soc
                except Exception as ex:
                    return {
                        "success": False,
                        "summary": f"set_target({target_soc}) failed: {ex}",
                        "soc": current_soc,
                        "log_path": None,
                    }
        result = run_build_quiet(example=app, clean=clean, pristine=pristine)
        if retargeted_from is not None:
            result["retargeted_from"] = retargeted_from
        if summary_only and result.get("success"):
            result.pop("output", None)
        return result
