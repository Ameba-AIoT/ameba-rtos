"""
Kconfig read/write MCP tools for Ameba SoC.

Wraps `ameba.py menuconfig --get/--set/--search` and returns parsed JSON.
"""

import json
import os
import re
import subprocess
import sys
from typing import Optional

from mcp.server.fastmcp import FastMCP
from ameba_dev_mcp._paths import PROJECT_ROOT, SDK_ROOT

# ameba.py lives in the SDK; menuconfig is cwd-relative and edits
# <cwd>/build_<soc>/build/.config, so we read soc + run it under PROJECT_ROOT
# to keep kconfig in sync with the project that build_firmware actually builds.
AMEBA_PY = os.path.join(SDK_ROOT, "ameba.py")
_ANSI_RE = re.compile(r"\x1b\[[0-9;]*[mK]")


def _read_current_soc() -> Optional[str]:
    info_file = os.path.join(PROJECT_ROOT, "soc_info.json")
    if not os.path.exists(info_file):
        return None
    try:
        with open(info_file) as f:
            return json.load(f).get("soc", {}).get("name")
    except Exception:
        return None


def _run_menuconfig(extra_args: list, soc: Optional[str]) -> dict:
    effective_soc = soc or _read_current_soc()
    if not effective_soc:
        return {"success": False, "error": "No SoC selected. Pass soc= or run `ameba.py soc <name>` first."}

    cmd = [sys.executable, AMEBA_PY, "menuconfig"]
    if soc:
        cmd.append(soc)
    cmd.extend(extra_args)

    try:
        proc = subprocess.run(
            cmd,
            cwd=PROJECT_ROOT,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            stdin=subprocess.DEVNULL,
            timeout=60,
        )
    except subprocess.TimeoutExpired:
        return {"success": False, "error": "menuconfig timed out after 60 s"}
    except Exception as exc:
        return {"success": False, "error": str(exc)}

    stdout = _ANSI_RE.sub("", proc.stdout.decode("utf-8", errors="replace"))
    stderr = _ANSI_RE.sub("", proc.stderr.decode("utf-8", errors="replace")).strip()

    # menuconfig.py prints a JSON line; other lines (e.g. "Start to menuconfig…")
    # come from ameba_manager.py and should be ignored.
    for line in stdout.splitlines():
        line = line.strip()
        if line.startswith('{') or line.startswith('['):
            try:
                return json.loads(line)
            except json.JSONDecodeError:
                pass

    return {
        "success": False,
        "error": f"no JSON in output (exit {proc.returncode})",
        "output": stdout.strip(),
        "stderr": stderr,
    }


def register_kconfig_tools(mcp: FastMCP) -> None:
    """Register Kconfig read/write MCP tools."""

    @mcp.tool()
    async def kconfig_get(symbols: list[str], soc: Optional[str] = None) -> dict:
        """Get the current value of one or more Kconfig symbols.

        Args:
            symbols: Symbol names to query (CONFIG_ prefix optional, e.g. "CONFIG_WIFI" or "WIFI").
            soc:     SoC name (e.g. RTL8721F). Omit to use the currently selected SoC.

        Returns:
            success  Whether the query succeeded.
            values   Dict mapping "CONFIG_<NAME>" to its current string value, or null if unknown.
        """
        return _run_menuconfig(["--get"] + symbols, soc)

    @mcp.tool()
    async def kconfig_set(assignments: list[str], soc: Optional[str] = None) -> dict:
        """Set one or more Kconfig symbol values and regenerate per-core headers.

        Strict: if a dependency is not satisfied the entire operation is rejected
        and .config is left unchanged. On dependency-not-satisfied errors this
        wrapper probes for a `<NAME>_MENU` companion symbol — many features in
        this SDK are gated by a user-facing `*_MENU` toggle that selects the
        bare symbol (e.g. CONFIG_WLAN ← CONFIG_WLAN_MENU). The hint is attached
        to the failing error.

        Args:
            assignments: List of "SYMBOL=value" strings, e.g. ["CONFIG_SHELL=n", "CONFIG_BT=y"].
                         CONFIG_ prefix is optional.
            soc:         SoC name. Omit to use the currently selected SoC.

        Returns:
            success      Whether all assignments were applied.
            applied      Dict of symbols that were successfully set.
            errors       List of per-symbol error objects (on failure), each
                         possibly augmented with `hint` and `menu_symbol`.
            config_path  Absolute path to the .config file that was (or would
                         have been) modified.
        """
        result = _run_menuconfig(["--set"] + assignments, soc)
        errs = result.get("errors") if isinstance(result, dict) else None
        if errs:
            menu_candidates: list[str] = []
            for err in errs:
                if "dependency not satisfied" not in (err.get("error") or ""):
                    continue
                bare = (err.get("symbol") or "").removeprefix("CONFIG_")
                if bare and not bare.endswith("_MENU"):
                    menu_candidates.append(f"{bare}_MENU")
            if menu_candidates:
                probe = _run_menuconfig(["--get"] + menu_candidates, soc)
                values = probe.get("values", {}) if isinstance(probe, dict) else {}
                for err in errs:
                    bare = (err.get("symbol") or "").removeprefix("CONFIG_")
                    menu_name = f"CONFIG_{bare}_MENU"
                    if values.get(menu_name) is not None:
                        err["menu_symbol"] = menu_name
                        err["hint"] = (
                            f"Try {menu_name}=y — it's the user-facing toggle "
                            f"that selects CONFIG_{bare}."
                        )
        return result

    @mcp.tool()
    async def kconfig_apply_file(files: list[str], soc: Optional[str] = None) -> dict:
        """Apply Kconfig config file(s) such as a project's prj.conf (mirrors `menuconfig.py --apply-file`).

        Reset + overlay: .config is REGENERATED from the SDK's default.conf plus
        your file(s), so prior kconfig_set tweaks are wiped. Use kconfig_set for
        incremental edits. NOTE: this updates the source .config only — the change
        does NOT reach firmware until a rebuild (prefer build_firmware(config_files=[...])).

        Args:
            files: Paths to files of CONFIG_* lines (absolute, or relative to the
                   project root). Later files override earlier ones.
            soc:   SoC name (e.g. RTL8721F). Omit to use the currently selected SoC.

        Returns:
            success/applied_files/config_path, plus a `hint`; error/missing on failure.
        """
        if not files:
            return {"success": False, "error": "files list must not be empty"}
        # Resolve relative paths against PROJECT_ROOT (where the project's config
        # lives) here in the server, not in the subprocess — its cwd is an
        # implementation detail of the SDK/project dual-root split. Absolute
        # paths (e.g. an external project's prj.conf) pass through untouched.
        resolved = [f if os.path.isabs(f) else os.path.join(PROJECT_ROOT, f) for f in files]
        result = _run_menuconfig(["--apply-file"] + resolved, soc)
        if isinstance(result, dict) and result.get("success"):
            result["hint"] = ("Source .config updated, NOT in firmware yet — run "
                               "build_firmware to apply (rebuild required).")
        return result

    @mcp.tool()
    async def kconfig_search(pattern: str, soc: Optional[str] = None) -> dict:
        """Search Kconfig symbols by name or prompt text (case-insensitive regex).

        Args:
            pattern: Regular expression to match against symbol names and prompt strings.
            soc:     SoC name. Omit to use the currently selected SoC.

        Returns:
            success  Whether the search succeeded.
            matches  List of {"name", "type", "prompt", "value"} dicts for matching symbols.
        """
        return _run_menuconfig(["--search", pattern], soc)
