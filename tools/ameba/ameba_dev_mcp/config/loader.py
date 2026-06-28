"""
Read / write / validate / incrementally update project_info.json5 and
board_info.json5.

All public functions are stateless: they take an `sdk_root` and return
either a parsed Pydantic model or a list of structured ConfigError.

Public surface:
    load_project_info(sdk_root) -> ProjectInfo            (raises on parse error)
    load_board_info(sdk_root)   -> BoardInfo
    save_project_info(sdk_root, model)
    save_board_info(sdk_root, model)
    ensure_project_info_template(sdk_root) -> path        (creates if missing)
    ensure_board_info_template(sdk_root)   -> path
    update_project_for_soc(sdk_root, parsed_project)      (auto-mode incremental)
    resolve_alias_or_error(board_info, alias) -> (alias, errors)  (single-board auto-pick)
    resolve_board(board_info, alias) -> ResolvedBoard     (defaults filled in)
    masked_board_info_dict(board_info) -> dict            (for MCP resource)
    validate_project_config(sdk_root, soc=None) -> List[ConfigError]
    validate_board_config(sdk_root, alias=None) -> List[ConfigError]
"""

from __future__ import annotations

import os
from pathlib import Path
from typing import List, Optional, Tuple

import json5
from pydantic import ValidationError

from ameba_dev_mcp.models.schemas import (
    BoardEntry,
    BoardInfo,
    BoardInfoDefaults,
    ConfigError,
    ProjectEntry,
    ProjectImageEntry,
    ProjectInfo,
    RemoteSpec,
    ResolvedBoard,
)
from ameba_dev_mcp.config.flashcfg_parser import ParsedProject
from ameba_dev_mcp.config.templates import (
    BOARD_INFO_FILENAME,
    EMPTY_BOARD_INFO,
    EMPTY_PROJECT_INFO,
    PROJECT_INFO_FILENAME,
    render_board_info,
    render_project_info,
)


class ConfigLoadError(Exception):
    """Raised when a json5 file is malformed or schema-invalid.

    Carries a list of ConfigError describing each issue, so callers can
    return them straight back to the AI without further translation.
    """
    def __init__(self, errors: List[ConfigError]):
        self.errors = errors
        super().__init__("; ".join(f"[{e.code}] {e.message}" for e in errors))


# ---------------------------------------------------------------------------
# Path helpers
# ---------------------------------------------------------------------------

def project_info_path(sdk_root: str) -> str:
    return os.path.join(sdk_root, PROJECT_INFO_FILENAME)


def board_info_path(sdk_root: str) -> str:
    return os.path.join(sdk_root, BOARD_INFO_FILENAME)


# ---------------------------------------------------------------------------
# Loading
# ---------------------------------------------------------------------------

def _read_json5(path: str, code_missing: str, code_parse: str) -> dict:
    if not os.path.isfile(path):
        raise ConfigLoadError([ConfigError(
            code=code_missing,
            message=f"{os.path.basename(path)} not found at {path}",
            hint=(
                "MCP will create an empty template on next flash/serial "
                "call; fill it per docs/board_info.md (or docs/project_info.md), "
                "and read MCP resource `debug://hardware` for board wiring "
                "and serial driver requirements."
            ),
        )])
    try:
        with open(path, "r", encoding="utf-8") as f:
            data = json5.load(f)
    except Exception as ex:
        raise ConfigLoadError([ConfigError(
            code=code_parse,
            field_path=path,
            message=f"Failed to parse {os.path.basename(path)}: {ex}",
            hint="Use /* ... */ comments only; check for trailing commas or unbalanced braces.",
        )])
    if not isinstance(data, dict):
        raise ConfigLoadError([ConfigError(
            code=code_parse,
            field_path=path,
            message=f"{os.path.basename(path)} top-level must be an object, got {type(data).__name__}",
        )])
    return data


def _validation_errors_to_config_errors(ex: ValidationError, code: str) -> List[ConfigError]:
    out: List[ConfigError] = []
    for err in ex.errors():
        loc = ".".join(str(x) for x in err.get("loc", ()))
        out.append(ConfigError(
            code=code,
            field_path=loc,
            message=err.get("msg", "validation error"),
        ))
    return out


def load_project_info(sdk_root: str) -> ProjectInfo:
    raw = _read_json5(project_info_path(sdk_root),
                      code_missing="PROJECT_CONFIG_MISSING",
                      code_parse="PROJECT_CONFIG_PARSE_ERROR")
    try:
        return ProjectInfo.model_validate(raw)
    except ValidationError as ex:
        raise ConfigLoadError(_validation_errors_to_config_errors(ex, "PROJECT_CONFIG_INVALID"))


def load_board_info(sdk_root: str) -> BoardInfo:
    raw = _read_json5(board_info_path(sdk_root),
                      code_missing="BOARD_CONFIG_MISSING",
                      code_parse="BOARD_CONFIG_PARSE_ERROR")
    try:
        return BoardInfo.model_validate(raw)
    except ValidationError as ex:
        raise ConfigLoadError(_validation_errors_to_config_errors(ex, "BOARD_CONFIG_INVALID"))


# ---------------------------------------------------------------------------
# Saving / templating
# ---------------------------------------------------------------------------

def save_project_info(sdk_root: str, model: ProjectInfo) -> str:
    path = project_info_path(sdk_root)
    text = render_project_info(model)
    Path(path).write_text(text, encoding="utf-8")
    return path


def save_board_info(sdk_root: str, model: BoardInfo) -> str:
    path = board_info_path(sdk_root)
    text = render_board_info(model)
    Path(path).write_text(text, encoding="utf-8")
    return path


def ensure_project_info_template(sdk_root: str) -> str:
    path = project_info_path(sdk_root)
    if not os.path.isfile(path):
        Path(path).write_text(EMPTY_PROJECT_INFO, encoding="utf-8")
    return path


def ensure_board_info_template(sdk_root: str) -> str:
    path = board_info_path(sdk_root)
    if not os.path.isfile(path):
        Path(path).write_text(EMPTY_BOARD_INFO, encoding="utf-8")
    return path


# ---------------------------------------------------------------------------
# Incremental update from build_firmware
# ---------------------------------------------------------------------------

def update_project_for_soc(sdk_root: str, parsed: ParsedProject) -> ProjectInfo:
    """Called by build_firmware on success.

    Behaviour:
      - File missing → create from EMPTY_PROJECT_INFO, then add this SoC
        as `auto`.
      - SoC entry missing → add as `auto` with parsed images.
      - SoC entry exists and is `auto` → overwrite build_dir + images
        (the "generated" region).
      - SoC entry exists and is `manual` → leave untouched (user owns it).

    Returns the updated ProjectInfo (post-write).
    """
    ensure_project_info_template(sdk_root)
    try:
        info = load_project_info(sdk_root)
    except ConfigLoadError:
        # Re-template if user's edits broke parsing — never happens on
        # the empty template, so this only fires after manual breakage.
        # We do NOT silently overwrite a corrupt user file.
        raise

    images: List[ProjectImageEntry] = [
        ProjectImageEntry(
            type=img.type,
            path=img.path,
            start_addr=img.start_addr,
            end_addr=img.end_addr,
            optional=(img.type == "VFS1"),
        )
        for img in parsed.images
    ]

    existing = info.projects.get(parsed.soc)
    if existing is None or existing.flash_layout_setting_mode == "auto":
        info.projects[parsed.soc] = ProjectEntry(
            flash_layout_setting_mode="auto",
            build_dir=parsed.build_dir,
            images=images,
        )
    # manual: leave as is
    save_project_info(sdk_root, info)
    return info


# ---------------------------------------------------------------------------
# Validation (called internally by flash / serial tools)
# ---------------------------------------------------------------------------

def _addr_int(s: str) -> int:
    return int(s, 16)


def validate_project_config(sdk_root: str, soc: Optional[str] = None) -> List[ConfigError]:
    """Hidden tool: full check of project_info.json5.

    If `soc` is provided, only that project is checked (other entries
    are still parsed for schema correctness).
    """
    try:
        info = load_project_info(sdk_root)
    except ConfigLoadError as ex:
        return ex.errors

    errors: List[ConfigError] = []
    targets = {soc: info.projects[soc]} if soc and soc in info.projects else dict(info.projects)
    if soc and soc not in info.projects:
        errors.append(ConfigError(
            code="PROJECT_NOT_CONFIGURED",
            field_path=f"projects.{soc}",
            message=f"SoC '{soc}' has no entry in project_info.json5",
            hint="Run build_firmware to auto-create an `auto` mode entry, or add manually.",
        ))
        return errors

    for s, entry in targets.items():
        path_prefix = f"projects.{s}"
        if not os.path.isabs(entry.build_dir):
            errors.append(ConfigError(
                code="PROJECT_PATH_NOT_ABSOLUTE",
                field_path=f"{path_prefix}.build_dir",
                message=f"build_dir must be absolute, got '{entry.build_dir}'",
            ))
        if entry.flash_layout_setting_mode == "auto":
            if not os.path.isdir(entry.build_dir):
                errors.append(ConfigError(
                    code="BUILD_DIR_MISSING",
                    field_path=f"{path_prefix}.build_dir",
                    message=f"build_dir does not exist: {entry.build_dir}",
                    hint=f"Run build_firmware for SoC '{s}' first.",
                ))
            if not entry.images:
                errors.append(ConfigError(
                    code="AUTO_IMAGES_MISSING",
                    field_path=f"{path_prefix}.images",
                    message=f"auto mode entry has no generated images for SoC '{s}'",
                    hint="Run build_firmware to populate the generated images[] block.",
                ))
        else:  # manual
            if not entry.images:
                errors.append(ConfigError(
                    code="MANUAL_IMAGES_EMPTY",
                    field_path=f"{path_prefix}.images",
                    message=f"manual mode entry must declare at least one image for SoC '{s}'",
                ))

        # Per-image checks
        for i, img in enumerate(entry.images):
            ipath = f"{path_prefix}.images[{i}]"
            if not os.path.isabs(img.path):
                errors.append(ConfigError(
                    code="IMAGE_PATH_NOT_ABSOLUTE",
                    field_path=f"{ipath}.path",
                    message=f"image path must be absolute, got '{img.path}'",
                ))
            if not os.path.isfile(img.path) and not img.optional:
                errors.append(ConfigError(
                    code="IMAGE_FILE_NOT_FOUND",
                    field_path=f"{ipath}.path",
                    message=f"image not found: {img.path}",
                    hint="Run build_firmware, or set `optional: true` if this image is allowed to be missing.",
                ))

        # Overlap check (manual only — auto comes from validated layout)
        if entry.flash_layout_setting_mode == "manual":
            ranges = []
            for i, img in enumerate(entry.images):
                if img.end_addr is None:
                    if not os.path.isfile(img.path):
                        continue
                    fsize = os.path.getsize(img.path)
                    end = _addr_int(img.start_addr) + max(fsize - 1, 0)
                else:
                    end = _addr_int(img.end_addr)
                ranges.append((_addr_int(img.start_addr), end, i))
            ranges.sort()
            for j in range(1, len(ranges)):
                if ranges[j][0] <= ranges[j - 1][1]:
                    errors.append(ConfigError(
                        code="IMAGE_RANGE_OVERLAP",
                        field_path=f"{path_prefix}.images",
                        message=(
                            f"image ranges overlap: index {ranges[j-1][2]} "
                            f"and index {ranges[j][2]}"
                        ),
                    ))

    return errors


def validate_board_config(sdk_root: str, alias: Optional[str] = None) -> List[ConfigError]:
    """Hidden tool: full check of board_info.json5."""
    try:
        info = load_board_info(sdk_root)
    except ConfigLoadError as ex:
        return ex.errors

    if alias is not None and alias not in info.boards:
        return [ConfigError(
            code="ALIAS_NOT_FOUND",
            field_path=f"boards.{alias}",
            message=f"alias '{alias}' is not configured in board_info.json5",
            hint=(
                f"Configured aliases: {sorted(info.boards.keys())}. "
                "Add a new entry under `boards`, or fix the alias spelling."
            ),
        )]

    errors: List[ConfigError] = []
    targets = {alias: info.boards[alias]} if alias else dict(info.boards)
    for a, entry in targets.items():
        if entry.transport == "remote":
            if entry.remote is None:
                errors.append(ConfigError(
                    code="REMOTE_SPEC_MISSING",
                    field_path=f"boards.{a}.remote",
                    message=f"transport=remote requires `remote: {{ host: ... }}`",
                ))
            elif not entry.remote.host:
                errors.append(ConfigError(
                    code="REMOTE_HOST_EMPTY",
                    field_path=f"boards.{a}.remote.host",
                    message=f"remote.host is empty for board '{a}'",
                ))
    return errors


# ---------------------------------------------------------------------------
# Resolution + masking
# ---------------------------------------------------------------------------

def resolve_alias_or_error(info: BoardInfo,
                           alias: Optional[str]
                           ) -> Tuple[Optional[str], Optional[List[ConfigError]]]:
    """Pick an alias from board_info, with single-board / default-alias auto-pick.

    Returns (alias, None) on success, or (None, [ConfigError]) on failure.

    Rules:
      - alias given and known          → (alias, None)
      - alias is None, exactly 1 board → (sole alias, None)
      - alias is None, 2+ boards AND
        info.default_alias set         → (info.default_alias, None)
      - alias is None, 0 boards        → ALIAS_REQUIRED (no choices)
      - alias is None, 2+ boards
        without default_alias          → ALIAS_REQUIRED (hint lists all)
      - alias given and unknown        → ALIAS_NOT_FOUND
    """
    if alias is not None:
        if alias in info.boards:
            return alias, None
        return None, [ConfigError(
            code="ALIAS_NOT_FOUND",
            field_path=f"boards.{alias}",
            message=f"alias '{alias}' is not configured in board_info.json5",
            hint=(
                "See `configured_aliases` for spelling, and "
                "`available_local_ports` for what's plugged into this machine. "
                "Add a new entry under `boards` if needed."
            ),
        )]
    aliases = list(info.boards.keys())
    if len(aliases) == 1:
        return aliases[0], None
    if not aliases:
        return None, [ConfigError(
            code="ALIAS_REQUIRED",
            field_path="boards",
            message="board_info.json5 has no boards configured",
            hint=(
                "Add at least one entry under `boards` per docs/board_info.md, "
                "or read MCP resource `debug://hardware` for setup guidance."
            ),
        )]
    # 2+ boards: fall back to default_alias if it's configured.
    # Schema validator already ensured default_alias points to a real board.
    if info.default_alias is not None:
        return info.default_alias, None
    return None, [ConfigError(
        code="ALIAS_REQUIRED",
        field_path="alias",
        message=(
            f"alias is required: board_info.json5 has {len(aliases)} boards. "
            "Pass one of the configured aliases."
        ),
        hint=(
            f"Configured aliases: {sorted(aliases)}. "
            "To pick one automatically when alias is omitted, set "
            "`default_alias` at the top level of board_info.json5."
        ),
    )]


def attach_selected_via_default(result: dict,
                                input_alias: Optional[str],
                                sdk_root: str) -> None:
    """Mutate `result` in place: when `alias` was omitted and resolved to
    info.default_alias (rather than a single-board auto-pick), add a
    `selected_via: "default"` field. Single-board / explicit cases are
    NOT marked — the envelope stays minimal in the boring case.

    Used by MCP tool wrappers as a soft "you didn't pass alias and the
    default_alias config kicked in" reminder, so the agent can complain
    if it actually meant to address a different board.
    """
    if not result.get("success"):
        return
    resolved = result.get("alias")
    if resolved is None:
        return
    try:
        info = load_board_info(sdk_root)
    except Exception:
        return
    sv = selected_via(info, input_alias, resolved)
    if sv == "default":
        result["selected_via"] = "default"


def selected_via(info: BoardInfo,
                 input_alias: Optional[str],
                 resolved_alias: Optional[str]) -> Optional[str]:
    """Classify how `resolved_alias` was picked, for envelope reporting.

    Returns one of:
      - "explicit"  user passed an alias and it matched
      - "single"    alias omitted, only one board configured
      - "default"   alias omitted, fell back to info.default_alias
      - None        unresolved (caller should not attach this field)
    """
    if resolved_alias is None:
        return None
    if input_alias is not None:
        return "explicit"
    if len(info.boards) == 1:
        return "single"
    if info.default_alias is not None and info.default_alias == resolved_alias:
        return "default"
    return None


def resolve_board(info: BoardInfo, alias: str) -> ResolvedBoard:
    """Apply defaults onto the BoardEntry and return a flat ResolvedBoard."""
    if alias not in info.boards:
        raise KeyError(alias)
    entry = info.boards[alias]
    d = info.defaults
    return ResolvedBoard(
        alias=alias,
        soc=entry.soc,
        transport=entry.transport,
        port=entry.port,
        memory_type=entry.memory_type if entry.memory_type is not None else d.memory_type,
        baudrate=entry.baudrate if entry.baudrate is not None else d.baudrate,
        monitor_baudrate=entry.monitor_baudrate if entry.monitor_baudrate is not None else d.monitor_baudrate,
        chip_erase=entry.chip_erase if entry.chip_erase is not None else d.chip_erase,
        add_crlf=entry.add_crlf if entry.add_crlf is not None else d.add_crlf,
        remote=entry.remote,
        serial_log_record=entry.serial_log_record,
    )


_PASSWORD_MASK = "***"


def masked_board_info_dict(info: BoardInfo) -> dict:
    """Serialise BoardInfo for MCP resource exposure with passwords redacted."""
    data = info.model_dump()
    for alias, board in data.get("boards", {}).items():
        rem = board.get("remote")
        if rem and rem.get("password"):
            rem["password"] = _PASSWORD_MASK
    return data


def masked_project_info_dict(info: ProjectInfo) -> dict:
    """Serialise ProjectInfo for MCP resource exposure (no secrets, but
    consistent helper for symmetry with board_info)."""
    return info.model_dump()
