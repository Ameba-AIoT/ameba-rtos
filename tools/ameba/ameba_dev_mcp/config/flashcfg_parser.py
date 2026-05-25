"""
Python port of vscode_plugin/cline/src/core/controller/ameba/amebaFlash.ts
flash-layout parsing logic.

Given an SoC name, this module produces a list of flashable images:
[{type, path, start_addr, end_addr}, ...]

It reads:
    component/soc/usrcfg/<ic>/ameba_flashcfg.c        - Flash_Layout[] table
    component/soc/usrcfg/<ic>/include/ameba_flashcfg.h - macro definitions
    build_<SOC>/build/.config                         - active CONFIG_X=y values
    component/soc/<ic>/project/CMakeLists.txt or
    <ic>_gcc_project/CMakeLists.txt                   - boot/app image names
    cmake/global_define.cmake                         - fallback boot image name

Address expressions are evaluated with ast.parse + ast.Expression in a
restricted node whitelist (no eval()).
"""

from __future__ import annotations

import ast
import operator as _op
import os
import re
from dataclasses import dataclass, field
from pathlib import Path
from typing import Dict, List, Optional, Tuple


# ---------------------------------------------------------------------------
# Data classes
# ---------------------------------------------------------------------------

@dataclass
class FlashRegion:
    type: str
    start_addr: str          # resolved "0xXXXXXXXX" or original raw on parse stage
    end_addr: str
    line_number: int = 0


@dataclass
class MacroDef:
    name: str
    value: str               # numeric value as decimal string, or fallback raw
    raw_value: str           # original text after the macro name


@dataclass
class ResolvedImage:
    type: str
    path: str                # absolute path
    start_addr: str          # 0xXXXXXXXX
    end_addr: str            # 0xXXXXXXXX (inclusive)
    exists: bool


# ---------------------------------------------------------------------------
# Errors
# ---------------------------------------------------------------------------

class FlashCfgParseError(Exception):
    """Any parse / resolution / validation failure."""


# ---------------------------------------------------------------------------
# Safe arithmetic evaluator
# ---------------------------------------------------------------------------

_AST_BIN_OPS = {
    ast.Add: _op.add,
    ast.Sub: _op.sub,
    ast.Mult: _op.mul,
    ast.Div: _op.truediv,
    ast.FloorDiv: _op.floordiv,
    ast.Mod: _op.mod,
}

_AST_UNARY_OPS = {
    ast.UAdd: _op.pos,
    ast.USub: _op.neg,
}


def _safe_eval_arith(expr: str) -> int:
    """Evaluate a pure-numeric arithmetic expression. Integers only."""
    node = ast.parse(expr, mode="eval").body

    def _walk(n):
        if isinstance(n, ast.Constant) and isinstance(n.value, (int, float)):
            return n.value
        if isinstance(n, ast.BinOp) and type(n.op) in _AST_BIN_OPS:
            return _AST_BIN_OPS[type(n.op)](_walk(n.left), _walk(n.right))
        if isinstance(n, ast.UnaryOp) and type(n.op) in _AST_UNARY_OPS:
            return _AST_UNARY_OPS[type(n.op)](_walk(n.operand))
        raise FlashCfgParseError(f"disallowed expression node: {ast.dump(n)}")

    return int(_walk(node))


_HEX_LITERAL_RE = re.compile(r"0x([0-9a-fA-F]+)")
_MACRO_REF_RE = re.compile(r"\b([A-Z_][A-Z0-9_]*)\b")
_CALC_END_ADDR_RE = re.compile(
    r"\bCALC_END_ADDR\(\s*([A-Z_][A-Z0-9_]*)\s*,\s*([A-Z_][A-Z0-9_]*)\s*\)",
    re.IGNORECASE,
)
_DOTCONFIG_VALUE_RE = re.compile(r"^(CONFIG_[\w]+)=(0x[0-9a-fA-F]+|\d+)$")


def _expand_calc_end_addr(s: str) -> str:
    """CALC_END_ADDR(offset, size) → offset + size - 1"""
    return _CALC_END_ADDR_RE.sub(lambda m: f"{m.group(1)} + {m.group(2)} - 1", s)


def evaluate_macro_expression(expr: str, macros: Dict[str, MacroDef]) -> int:
    """
    Replicates evaluateMacroExpression() in amebaFlash.ts (L42).
    """
    e = expr.strip()
    e = re.sub(r"//.*$", "", e)
    e = re.sub(r"/\*.*?\*/", "", e, flags=re.DOTALL)
    e = e.strip()

    # Replace macro references with their resolved value
    def _sub_macro(m):
        name = m.group(1)
        md = macros.get(name)
        return md.value if md else name

    e = _MACRO_REF_RE.sub(_sub_macro, e)

    # Strip parens (we re-wrap by ast)
    e_no_paren = e.replace("(", " ").replace(")", " ")

    # Replace hex literals with decimals
    e_no_paren = _HEX_LITERAL_RE.sub(lambda m: str(int(m.group(1), 16)), e_no_paren)

    if not re.fullmatch(r"[\d\s+\-*/]+", e_no_paren or ""):
        raise FlashCfgParseError(
            f'Invalid expression after macro substitution: "{e_no_paren}"'
        )

    # Re-evaluate with parens preserved (replace hex inside original e too)
    e_eval = _HEX_LITERAL_RE.sub(lambda m: str(int(m.group(1), 16)), e)
    return _safe_eval_arith(e_eval)


def _format_hex_address(value: int) -> str:
    return "0x{:08X}".format(value & 0xFFFFFFFF)


def resolve_address(address: str, macros: Dict[str, MacroDef]) -> str:
    """Replicates resolveAddress() in amebaFlash.ts (L93)."""
    a = address.strip()
    if re.fullmatch(r"0x[0-9a-fA-F]+", a):
        return _format_hex_address(int(a, 16))

    # CALC_END_ADDR(offset, size): mirrors the C ternary (size==0 ? offset : offset+size-1)
    cm = _CALC_END_ADDR_RE.fullmatch(a)
    if cm:
        offset = int(resolve_address(cm.group(1), macros), 16)
        size = int(resolve_address(cm.group(2), macros), 16)
        return _format_hex_address(offset if size == 0 else offset + size - 1)

    e = a

    def _sub_macro(m):
        name = m.group(1)
        md = macros.get(name)
        if md is None:
            raise FlashCfgParseError(f'Macro "{name}" not found in macro definitions')
        try:
            v = evaluate_macro_expression(md.raw_value, macros)
            return str(v)
        except FlashCfgParseError:
            return md.value

    e = _MACRO_REF_RE.sub(_sub_macro, e)
    e = _HEX_LITERAL_RE.sub(lambda m: str(int(m.group(1), 16)), e)
    e_no_paren = e.replace("(", " ").replace(")", " ")
    e_no_space = re.sub(r"\s+", "", e_no_paren)

    if not re.fullmatch(r"[\d+\-*/]+", e_no_space or ""):
        raise FlashCfgParseError(
            f'Invalid address expression after macro substitution: "{e_no_space}"'
        )

    return _format_hex_address(_safe_eval_arith(e))


# ---------------------------------------------------------------------------
# Macro parser
# ---------------------------------------------------------------------------

_DEFINE_RE = re.compile(r"#define\s+([A-Z_][A-Z0-9_]*)\s+(.+)")
_OUTER_COND_RE = re.compile(
    r"if\s*\(\s*(CONFIG_[\w_]+)\s*\)\s*([\s\S]*?)\s*else\s*\(\s*\)\s*([\s\S]*?)\s*endif\s*\(\s*\)",
    re.IGNORECASE,
)
# C-preprocessor style: #ifdef CONFIG_X / #ifndef ... / #else / #endif
_C_IFDEF_RE = re.compile(
    r"#\s*if(?:def)?\s+(CONFIG_[\w_]+)\s*([\s\S]*?)(?:#\s*else\s*([\s\S]*?))?#\s*endif",
)


def _config_is_set(config_content: str, name: str) -> bool:
    return f"{name}=y" in config_content


def parse_macros(header_content: str, dot_config_content: str) -> Dict[str, MacroDef]:
    """Replicates parseMacroDefinitions() in amebaFlash.ts (L147).

    Supports both CMake-style outer if(CONFIG_X)…else()…endif() and
    C-preprocessor #ifdef CONFIG_X…#else…#endif blocks.
    """
    macros: Dict[str, MacroDef] = {}
    search_scope = header_content

    m = _OUTER_COND_RE.search(header_content)
    if m:
        cfg, if_block, else_block = m.group(1), m.group(2), m.group(3)
        search_scope = if_block if _config_is_set(dot_config_content, cfg) else else_block
    else:
        cm = _C_IFDEF_RE.search(header_content)
        if cm:
            cfg = cm.group(1)
            if_block = cm.group(2) or ""
            else_block = cm.group(3) or ""
            search_scope = if_block if _config_is_set(dot_config_content, cfg) else else_block

    for line in search_scope.splitlines():
        m2 = _DEFINE_RE.match(line.strip())
        if not m2:
            continue
        name, raw = m2.group(1), m2.group(2).strip()
        try:
            v = evaluate_macro_expression(raw, macros)
            macros[name] = MacroDef(name, str(v), raw)
        except FlashCfgParseError:
            macros[name] = MacroDef(name, raw, raw)

    # Inject numeric CONFIG_* values from .config (hex or decimal).
    # These are not #defined in the header but are needed to resolve
    # CALC_END_ADDR(CONFIG_FLASH_*_OFFSET, CONFIG_FLASH_*_SIZE) expressions.
    for line in dot_config_content.splitlines():
        m3 = _DOTCONFIG_VALUE_RE.match(line.strip())
        if not m3:
            continue
        name, raw = m3.group(1), m3.group(2)
        if name in macros:
            continue
        try:
            v = evaluate_macro_expression(raw, macros)
            macros[name] = MacroDef(name, str(v), raw)
        except FlashCfgParseError:
            macros[name] = MacroDef(name, raw, raw)

    return macros


# ---------------------------------------------------------------------------
# Flash_Layout[] table parser
# ---------------------------------------------------------------------------

_LAYOUT_DEF_RE = re.compile(
    r"^\s*(?:const\s+)?FlashLayoutInfo_TypeDef\s+Flash_Layout\s*\[\s*\]"
)
_ENTRY_RE = re.compile(r"\{\s*([^,]+?)\s*,\s*([^,]+?)\s*,\s*([^}]+?)\s*\}")
_STRICT_HEX_RE = re.compile(
    r"^("
    r"0x[0-9a-fA-F]+(\s*[+-]\s*[A-Z_][A-Z0-9_]*)?"
    r"|[A-Z_][A-Z0-9_]*"
    r"|0xFFFFFFFF"
    r"|CALC_END_ADDR\(\s*[A-Z_][A-Z0-9_]*\s*,\s*[A-Z_][A-Z0-9_]*\s*\)"
    r")$",
    re.IGNORECASE,
)


@dataclass
class LayoutParseResult:
    layout: List[FlashRegion] = field(default_factory=list)
    definition_line: int = -1


def parse_flash_layout(c_content: str, file_path: str = "<flashcfg.c>") -> LayoutParseResult:
    """Replicates parseFlashLayout() in amebaFlash.ts (L247).

    If multiple `FlashLayoutInfo_TypeDef Flash_Layout[]` definitions exist in
    the file, the **last** one is used (matches TS behaviour).
    """
    lines = c_content.splitlines()
    defs: List[Tuple[int, int]] = []  # (lineNumber, lineIndex)
    for idx, line in enumerate(lines):
        if _LAYOUT_DEF_RE.match(line):
            defs.append((idx + 1, idx))

    if not defs:
        raise FlashCfgParseError(
            f"Could not find 'FlashLayoutInfo_TypeDef Flash_Layout' definition in {file_path}"
        )

    target_lineno, target_idx = defs[-1] if len(defs) >= 2 else defs[0]
    result = LayoutParseResult(definition_line=target_lineno)

    in_target = False
    brace_count = 0
    for idx in range(target_idx, len(lines)):
        line = lines[idx]
        line_no = idx + 1

        if not in_target and _LAYOUT_DEF_RE.match(line):
            in_target = True
            brace_count += line.count("{") - line.count("}")
            if brace_count > 0:
                continue

        if in_target:
            brace_count += line.count("{") - line.count("}")

            for m in _ENTRY_RE.finditer(line):
                raw_type = m.group(1).strip()
                raw_start = m.group(2).strip()
                raw_end = m.group(3).strip()

                if raw_type == "0xFF":
                    continue

                if not _STRICT_HEX_RE.match(raw_start):
                    raise FlashCfgParseError(
                        f'Invalid start address format in {file_path} '
                        f'(line {line_no}): "{raw_start}".'
                    )
                if not _STRICT_HEX_RE.match(raw_end):
                    raise FlashCfgParseError(
                        f'Invalid end address format in {file_path} '
                        f'(line {line_no}): "{raw_end}".'
                    )

                result.layout.append(FlashRegion(raw_type, raw_start, raw_end, line_no))

            if brace_count <= 0:
                break

    if not result.layout:
        raise FlashCfgParseError(
            f"No valid entries in 'Flash_Layout' table (line {result.definition_line}) in {file_path}"
        )
    return result


def resolve_layout_addresses(
    layout: List[FlashRegion], header_content: str, dot_config_content: str
) -> None:
    """Replicates resolveAddressExpression() in amebaFlash.ts (L205).

    Mutates `layout` in place: each region's `start_addr`/`end_addr` becomes
    a resolved 8-digit hex string.
    """
    macros = parse_macros(header_content, dot_config_content)
    for region in layout:
        region.start_addr = resolve_address(region.start_addr, macros)
        region.end_addr = resolve_address(region.end_addr, macros)
        # CALC_END_ADDR(offset, 0) → end == start (size=0, disabled region).
        # Normalize to the disabled sentinel so validate_layout skips it.
        if int(region.end_addr, 16) <= int(region.start_addr, 16):
            region.start_addr = "0x00000000"
            region.end_addr = "0x00000000"


def validate_layout(layout: List[FlashRegion]) -> Tuple[bool, Optional[str]]:
    """Replicates validateFlashLayout() in amebaFlash.ts (L345).

    Returns (is_valid, error_message_or_None).
    """
    valid = [r for r in layout if not (
        (r.start_addr == "0xFFFFFFFF" and r.end_addr == "0xFFFFFFFF") or
        (r.start_addr == "0x00000000" and r.end_addr == "0x00000000")
    )]
    try:
        valid.sort(key=lambda r: int(r.start_addr, 16))
    except ValueError:
        return False, "Failed to sort regions due to invalid hex address format."

    for i, region in enumerate(valid):
        try:
            s = int(region.start_addr, 16)
            e = int(region.end_addr, 16)
        except ValueError:
            return False, f"Region {region.type} (line {region.line_number}) has a non-hexadecimal address."
        if s >= e:
            return False, (
                f"Validation failed for region {region.type} (line {region.line_number}): "
                f"Start address ({region.start_addr}) must be less than end address ({region.end_addr})."
            )
        if not re.fullmatch(r"0x[0-9a-fA-F]+000", region.start_addr):
            return False, (
                f"Validation failed for region {region.type} (line {region.line_number}): "
                f"Start address ({region.start_addr}) is not 4K-aligned (must end in '000')."
            )
        if not re.fullmatch(r"0x[0-9a-fA-F]+FFF", region.end_addr):
            return False, (
                f"Validation failed for region {region.type} (line {region.line_number}): "
                f"End address ({region.end_addr}) is not 4K-aligned (must end in 'FFF')."
            )
        if i + 1 < len(valid):
            nxt = valid[i + 1]
            ns = int(nxt.start_addr, 16)
            if e >= ns:
                return False, (
                    f"Validation failed: Region {region.type} (line {region.line_number}) "
                    f"({region.start_addr} - {region.end_addr}) overlaps with region "
                    f"{nxt.type} (line {nxt.line_number}) ({nxt.start_addr})."
                )
    return True, None


def check_file_size_within(file_path: str, start_addr: str, end_addr: str) -> bool:
    """Replicates isFileOverLayout() in amebaFlash.ts (L399).

    Returns True if the file fits in [start_addr, end_addr] (inclusive),
    False if it exceeds the layout. Missing file → True (caller decides).
    """
    if not os.path.isfile(file_path):
        return True
    file_size = os.path.getsize(file_path)
    limit = int(end_addr, 16) - int(start_addr, 16) + 1
    return file_size <= limit


# ---------------------------------------------------------------------------
# Boot / app image name parsers
# ---------------------------------------------------------------------------

_BOOT_NAME_RE = re.compile(r"\bset\s*\(\s*boot_name\s+([\w._-]+)\s*\)")
_GLOBAL_BOOT_RE = re.compile(
    r"ameba_set_if_unset\s*\(\s*c_BOOT_BINARY_NAME\s+([\w._-]+)\s*\)"
)
_APP_NAME_RE = re.compile(
    r"ameba_set_if\s*\(\s*(CONFIG_[\w_]+)\s+app_name\s+([\w._-]+)\s+p_ELSE\s+([\w._-]+)\s*\)"
)
_FIRMWARE_PKG_RE = re.compile(r"ameba_firmware_package\s*\(\s*([\w._-]+)")
_CMAKE_OUTER_COND_RE = re.compile(
    r"if\s*\(\s*(CONFIG_[\w_]+)\s*\)\s*([\s\S]*?)\s*else\s*\(\s*\)\s*([\s\S]*?)\s*endif\s*\(\s*\)",
    re.IGNORECASE,
)

DEFAULT_BOOT_IMAGE = "km4_boot_all.bin"


def _read_text(p: str) -> Optional[str]:
    try:
        with open(p, "r", encoding="utf-8", errors="replace") as f:
            return f.read()
    except OSError:
        return None


def _project_cmake_path(sdk_root: str, ic: str) -> Optional[str]:
    """Mirror amebaFlash.ts: prefer component/soc/<ic>/project/CMakeLists.txt,
    fallback to <ic>_gcc_project/CMakeLists.txt."""
    soc_dir = os.path.join(sdk_root, "component", "soc", ic, "project", "CMakeLists.txt")
    if os.path.isfile(soc_dir):
        return soc_dir
    legacy = os.path.join(sdk_root, f"{ic}_gcc_project", "CMakeLists.txt")
    if os.path.isfile(legacy):
        return legacy
    return None


def get_boot_image_name(sdk_root: str, ic: str) -> str:
    """Replicates getBootImageName() in amebaFlash.ts (L453)."""
    cmake_path = _project_cmake_path(sdk_root, ic)
    if cmake_path:
        content = _read_text(cmake_path) or ""
        m = _BOOT_NAME_RE.search(content)
        if m:
            return m.group(1)

    global_cmake = os.path.join(sdk_root, "cmake", "global_define.cmake")
    content = _read_text(global_cmake)
    if content:
        m = _GLOBAL_BOOT_RE.search(content)
        if m:
            return m.group(1)
    return DEFAULT_BOOT_IMAGE


def get_app_image_name(sdk_root: str, ic: str, dot_config_content: str) -> str:
    """Replicates getAppImageName() in amebaFlash.ts (L495)."""
    cmake_path = _project_cmake_path(sdk_root, ic)
    if not cmake_path:
        raise FlashCfgParseError(
            f"CMakeLists.txt not found for IC '{ic}'. Cannot determine app image name."
        )
    content = _read_text(cmake_path)
    if content is None:
        raise FlashCfgParseError(f"Could not read {cmake_path}")

    search_scope = content
    outer = _CMAKE_OUTER_COND_RE.search(content)
    if outer:
        cfg, if_blk, else_blk = outer.group(1), outer.group(2), outer.group(3)
        search_scope = if_blk if _config_is_set(dot_config_content, cfg) else else_blk
        m = _APP_NAME_RE.search(search_scope)
        if m:
            inner_cfg, name_t, name_f = m.group(1), m.group(2), m.group(3)
            return name_t if _config_is_set(dot_config_content, inner_cfg) else name_f

    m = _APP_NAME_RE.search(content)
    if m:
        inner_cfg, name_t, name_f = m.group(1), m.group(2), m.group(3)
        return name_t if _config_is_set(dot_config_content, inner_cfg) else name_f

    m = _FIRMWARE_PKG_RE.search(search_scope)
    if m:
        return m.group(1)

    raise FlashCfgParseError(
        f"Could not parse app_name from {cmake_path}. "
        "Check 'ameba_set_if' or 'ameba_firmware_package' definitions."
    )


# ---------------------------------------------------------------------------
# Top-level entrypoint
# ---------------------------------------------------------------------------

def _resolve_ic_for_soc(sdk_root: str, soc: str) -> str:
    """Decode soc_map.dat via SocManager to map SoC name → IC dir."""
    import importlib.util
    scripts_dir = os.path.join(sdk_root, "tools", "scripts")
    spec = importlib.util.spec_from_file_location(
        "_ameba_soc_utils_for_flash",
        os.path.join(scripts_dir, "ameba_soc_utils.py"),
    )
    if spec is None or spec.loader is None:
        raise FlashCfgParseError(f"Cannot load ameba_soc_utils.py at {scripts_dir}")
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    mgr = mod.SocManager()
    ic = mgr.raw_soc_map.get(soc)
    if not ic:
        raise FlashCfgParseError(
            f"Unknown SoC '{soc}'. Known SoCs: {sorted(mgr.raw_soc_map)}"
        )
    return ic


@dataclass
class ParsedProject:
    soc: str
    ic: str
    build_dir: str             # absolute
    layout: List[FlashRegion]
    images: List[ResolvedImage]
    boot_image_name: str
    app_image_name: str


def parse_project(sdk_root: str, soc: str) -> ParsedProject:
    """End-to-end: parse Flash_Layout, resolve macros, locate images,
    return a ParsedProject ready to drive flashing.

    Mirrors the body of amebaFlash() in amebaFlash.ts (L576-786) for the
    layout parsing + image-name resolution stages, **without** issuing
    any flash command.

    `build_dir` resolves to <sdk_root>/build_<soc>/.
    """
    sdk_root = os.path.abspath(sdk_root)
    ic = _resolve_ic_for_soc(sdk_root, soc)

    flash_cfg_c = os.path.join(sdk_root, "component", "soc", "usrcfg", ic, "ameba_flashcfg.c")
    flash_cfg_h = os.path.join(sdk_root, "component", "soc", "usrcfg", ic, "include", "ameba_flashcfg.h")
    if not os.path.isfile(flash_cfg_c):
        raise FlashCfgParseError(f"ameba_flashcfg.c not found: {flash_cfg_c}")

    c_content = _read_text(flash_cfg_c) or ""
    parsed = parse_flash_layout(c_content, flash_cfg_c)

    h_content = _read_text(flash_cfg_h) or ""
    build_dir = os.path.join(sdk_root, f"build_{soc}")
    dot_config_path = os.path.join(build_dir, "build", ".config")
    dot_config = _read_text(dot_config_path) or ""

    if h_content:
        resolve_layout_addresses(parsed.layout, h_content, dot_config)

    ok, err = validate_layout(parsed.layout)
    if not ok:
        raise FlashCfgParseError(f"Invalid flash layout in {flash_cfg_c}: {err}")

    boot_name = get_boot_image_name(sdk_root, ic)
    app_name = get_app_image_name(sdk_root, ic, dot_config)

    type_to_name = {
        "IMG_BOOT": boot_name,
        "IMG_APP_OTA1": app_name,
        "VFS1": "vfs.bin",
    }

    images: List[ResolvedImage] = []
    for region in parsed.layout:
        if region.start_addr == "0xFFFFFFFF" and region.end_addr == "0xFFFFFFFF":
            continue
        fname = type_to_name.get(region.type)
        if not fname:
            continue
        path = os.path.abspath(os.path.join(build_dir, fname))
        images.append(
            ResolvedImage(
                type=region.type,
                path=path,
                start_addr=region.start_addr,
                end_addr=region.end_addr,
                exists=os.path.isfile(path),
            )
        )

    return ParsedProject(
        soc=soc,
        ic=ic,
        build_dir=build_dir,
        layout=parsed.layout,
        images=images,
        boot_image_name=boot_name,
        app_image_name=app_name,
    )
