#!/usr/bin/env python3

# Sync the DSP LSP(s) to the MCU memory layout.
#
# The MCU side (Kconfig + component/soc/amebalite/project/ameba_layout.ld) is the
# single source of truth for where the DSP lives. This tool derives the DSP
# window(s) from that source and (re)generates the matching DSP Link Support
# Package(s) by driving the DSP SDK's img_utility/lsp_modify.py:
#
#   RTK_LSP_SRAM  DSP runs from on-chip SRAM. Window = [__dsp_sram_start__,
#                 SRAM_END). Start mirrors DSP_BD_SRAM in ameba_layout.ld.
#   RTK_LSP       DSP runs from PSRAM. Window = [PSRAM_BASE + KR4 + KM4 KB,
#                 PSRAM_END), i.e. it fills the PSRAM remainder after TZ/KR4/KM4.
#
# Data source: parse the MCU .config (Kconfig KB values) + ameba_layout.ld
# (window constants), NOT the compiled target_loader.map, so the LSP can be
# regenerated without a prior MCU build. The .config read is the menuconfig
# source of truth (build_<SOC>/menuconfig/.config), NOT the derived, per-image
# build/project_km4/.config_km4 which can lag behind a fresh menuconfig edit.
#
# The DSP SDK root is derived from CONFIG_DSP_SDK_IMAGE_DIR in the MCU .config
# (the SDK image output dir, two levels below the SDK root) unless --dsp-sdk is
# given. An absolute CONFIG_DSP_SDK_IMAGE_DIR is used as-is; a relative one is
# resolved against the SoC project dir (same rule as build.py / postbuild.cmake).
#
# Examples:
#   # CONFIG_DSP_SDK_IMAGE_DIR="/home/me/dsp_sdk/heap/source/project/image"
#   python3 tools/scripts/dsp_layout_sync.py --which sram
#   python3 tools/scripts/dsp_layout_sync.py --which both
#   python3 tools/scripts/dsp_layout_sync.py --dsp-sdk /home/me/dsp_sdk/heap/source

import argparse
import os
import re
import shutil
import subprocess
import sys
from pathlib import Path

KBYTES = 1024

# LSP name -> (layout for lsp_modify.py, relative dir under <sdk>/project glob)
LSPS = {
    "sram": ("sram", "RTK_LSP_SRAM"),
    "psram": ("psram", "RTK_LSP"),
    "xip": ("xip", "RTK_LSP_XIP"),
}


def die(msg):
    print("dsp_layout_sync: " + msg, file=sys.stderr)
    sys.exit(1)


def read_config(config_path):
    """Parse a Kconfig .config into {CONFIG_X: value}. `y` kept, quotes stripped,
    `# ... is not set` omitted."""
    cfg = {}
    try:
        for line in Path(config_path).read_text().splitlines():
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            if "=" not in line:
                continue
            k, v = line.split("=", 1)
            v = v.strip().strip('"')
            cfg[k.strip()] = v
    except OSError as e:
        die("cannot read .config %s: %s" % (config_path, e))
    return cfg


def _isint(v):
    try:
        int(v, 0)
        return True
    except (ValueError, TypeError):
        return False


def eval_cpp_cond(expr, cfg, defs=None, counts=None):
    """Evaluate a C-preprocessor boolean condition (the part after #if/#elif)
    against the .config: defined(X)/defined X -> membership, &&/||/! and parens,
    KBYTES(x) and already-#define'd ld macros -> their int, remaining CONFIG_*
    value macros -> their int (or 0 if unset/non-int). defs/counts let #if
    conditions test ld macro values (e.g. `#if KM4_RAM_TZ_SIZE > 0`)."""
    defs = defs or {}
    counts = counts or {}
    e = re.sub(r'defined\s*\(\s*(\w+)\s*\)', lambda m: 'True' if m.group(1) in cfg else 'False', expr)
    e = re.sub(r'defined\s+(\w+)', lambda m: 'True' if m.group(1) in cfg else 'False', e)
    e = re.sub(r'KBYTES\s*\(([^()]*)\)', r'((\1)*1024)', e)

    def _ident(m):
        nm = m.group(0)
        if nm in ("True", "False"):
            return nm
        # ld macro defined earlier in this file (single active definition) -> its int
        if nm in defs and counts.get(nm, 0) <= 1:
            return "(" + str(resolve(nm, defs, counts, cfg)) + ")"
        # bare CONFIG_* value -> its int; any other identifier -> 0 (undefined)
        if nm in cfg and _isint(cfg[nm]):
            return str(int(cfg[nm], 0))
        return "0"
    e = re.sub(r'[A-Za-z_]\w*', _ident, e)
    e = e.replace('&&', ' and ').replace('||', ' or ').replace('!', ' not ')
    try:
        return bool(eval(e, {"__builtins__": {}}, {}))  # keywords only, sanitized above
    except Exception:
        die("cannot evaluate preprocessor condition %r (from %r)" % (e, expr))


def _eval_directive(s, cfg, defs=None, counts=None):
    if s.startswith("#ifdef"):
        return s.split()[1] in cfg
    if s.startswith("#ifndef"):
        return s.split()[1] not in cfg
    # #if / #elif <expr>
    parts = s.split(None, 1)
    return eval_cpp_cond(parts[1] if len(parts) > 1 else "", cfg, defs, counts)


def read_ld_defines(ld_path, cfg):
    """Preprocess ameba_layout.ld against the .config and collect the `#define`s
    that are active under the current configuration. Handles nested
    #if/#ifdef/#ifndef/#elif/#else/#endif so #if-branched macros (IMG1 size, TZ,
    DSP_BD_SRAM_START, ...) resolve to exactly the active branch - no stale
    target_loader.map dependency."""
    defs = {}
    counts = {}
    try:
        text = Path(ld_path).read_text()
    except OSError as e:
        die("cannot read ameba_layout.ld %s: %s" % (ld_path, e))
    stack = []  # each: {'parent': bool, 'active': bool, 'taken': bool}

    def cur_active():
        return stack[-1]['active'] if stack else True

    for raw in text.splitlines():
        s = raw.strip()
        if s.startswith("#if"):  # #if / #ifdef / #ifndef
            parent = cur_active()
            cond = _eval_directive(s, cfg, defs, counts) if parent else False
            stack.append({'parent': parent, 'active': parent and cond, 'taken': parent and cond})
        elif s.startswith("#elif"):
            if not stack:
                continue
            f = stack[-1]
            if f['parent'] and not f['taken']:
                cond = _eval_directive(s, cfg, defs, counts)
                f['active'] = cond
                f['taken'] = f['taken'] or cond
            else:
                f['active'] = False
        elif s.startswith("#else"):
            if not stack:
                continue
            f = stack[-1]
            f['active'] = f['parent'] and not f['taken']
            f['taken'] = True
        elif s.startswith("#endif"):
            if stack:
                stack.pop()
        elif s.startswith("#define") and cur_active():
            m = re.match(r'#define\s+(\w+)\s+(.+?)\s*(?:/\*.*)?$', s)
            if m:
                name, val = m.group(1), re.sub(r'\s*/\*.*$', '', m.group(2)).strip()
                defs[name] = val
                counts[name] = counts.get(name, 0) + 1
    return defs, counts


def resolve(name, defs, counts, cfg, depth=0):
    """Expand an ameba_layout.ld macro to an integer, substituting nested macros,
    KBYTES(x) and CONFIG_* values. Refuses names with multiple (#if-branched)
    definitions to avoid picking the wrong branch."""
    if depth > 24:
        die("macro expansion too deep at %s" % name)
    if name not in defs:
        die("macro %s not found in ameba_layout.ld" % name)
    if counts.get(name, 0) > 1:
        die("macro %s has multiple (#if-branched) definitions; refusing to resolve it here" % name)
    return _eval_expr(defs[name], defs, counts, cfg, depth)


def _eval_expr(expr, defs, counts, cfg, depth):
    # KBYTES(x) -> ((x)*1024)
    expr = re.sub(r'KBYTES\s*\(([^()]*)\)', r'((\1)*1024)', expr)
    # CONFIG_* -> value from .config (must be integer)
    def _cfg(m):
        key = m.group(0)
        if key not in cfg:
            die("%s referenced by ameba_layout.ld is not set in .config" % key)
        try:
            return "(" + str(int(cfg[key], 0)) + ")"
        except ValueError:
            die("%s = %r is not an integer" % (key, cfg[key]))
    expr = re.sub(r'CONFIG_\w+', _cfg, expr)
    # Nested macro names -> their expanded value
    def _macro(m):
        nm = m.group(0)
        if nm in defs:
            return "(" + str(resolve(nm, defs, counts, cfg, depth + 1)) + ")"
        return nm
    prev = None
    while prev != expr:
        prev = expr
        expr = re.sub(r'[A-Za-z_]\w*', _macro, expr)
    if not re.fullmatch(r'[0-9xXa-fA-F+\-*/()\s]+', expr):
        die("unresolved tokens in expression: %r" % expr)
    try:
        return int(eval(expr, {"__builtins__": {}}, {}))  # arithmetic-only, sanitized above
    except Exception as e:
        die("cannot evaluate %r: %s" % (expr, e))


def compute_windows(cfg, defs, counts, which):
    """Return {lsp_key: (start, end)} for the requested LSP(s)."""
    out = {}
    if which in ("sram", "both"):
        if cfg.get("CONFIG_DSP_RUN_IN_SRAM") != "y":
            die("CONFIG_DSP_RUN_IN_SRAM is not y; the SRAM DSP window is only defined then "
                "(DSP_BD_SRAM collapses to 0 otherwise). Nothing to generate for RTK_LSP_SRAM.")
        start = resolve("DSP_BD_SRAM_START", defs, counts, cfg)
        end = resolve("SRAM_END", defs, counts, cfg)
        out["sram"] = (start, end)
    # psram (RTK_LSP) and xip (RTK_LSP_XIP) share the same PSRAM DSP window; xip
    # only differs in the LSP template (DSP code XIP'd from flash, not psram0).
    if which in ("psram", "xip", "both"):
        if cfg.get("CONFIG_DSP_EN") != "y":
            die("CONFIG_DSP_EN is not y; the PSRAM DSP window is only defined with DSP on.")
        if cfg.get("CONFIG_DSP_RUN_IN_SRAM") == "y":
            die("CONFIG_DSP_RUN_IN_SRAM=y; DSP is in SRAM, the PSRAM DSP segment collapses. "
                "Nothing to generate for RTK_LSP / RTK_LSP_XIP.")
        psram_base = resolve("PSRAM_BASE", defs, counts, cfg)
        psram_end = resolve("PSRAM_END", defs, counts, cfg)
        try:
            kr4 = int(cfg["CONFIG_PSRAM_KR4_IMG2_KB"])
            km4 = int(cfg["CONFIG_PSRAM_KM4_IMG2_KB"])
        except (KeyError, ValueError):
            die("CONFIG_PSRAM_KR4_IMG2_KB / CONFIG_PSRAM_KM4_IMG2_KB missing or non-integer "
                "(needed for the PSRAM DSP window; is IMG2_PSRAM/DATA_HEAP_PSRAM + DSP_EN on?)")
        # TZ cancels: PSRAM_BASE + TZ + KR4 + (KM4 - TZ) = PSRAM_BASE + KR4 + KM4
        start = psram_base + (kr4 + km4) * KBYTES
        out["xip" if which == "xip" else "psram"] = (start, psram_end)
    return out


def find_lsp_dir(sdk, lsp_name, core):
    matches = sorted(Path(sdk).glob("project/%s/*/%s/%s" % (lsp_name, core, lsp_name)))
    if not matches:
        # fall back: any RI version dir
        matches = sorted(Path(sdk).glob("project/%s/*/*/%s" % (lsp_name, lsp_name)))
    if not matches:
        die("cannot locate LSP dir for %s under %s/project" % (lsp_name, sdk))
    return matches[0]


def run_lsp_modify(sdk, layout, lsp_dir, start, end, stack, core, project_mpu):
    script = Path(sdk) / "project" / "img_utility" / "lsp_modify.py"
    if not script.exists():
        die("lsp_modify.py not found at %s" % script)
    cmd = [sys.executable, str(script), "--layout", layout, "--lsp-dir", str(lsp_dir),
           "--start", hex(start), "--end", hex(end), "--stack-size", hex(stack), "--core", core]
    if project_mpu:
        cmd += ["--project-mpu", str(project_mpu)]
    print("  $ " + " ".join(cmd))
    r = subprocess.run(cmd)
    return r.returncode


def parse_x_segments(x_path):
    """Return {seg_name: (org, len)} from the MEMORY block of elf32xtensa.x."""
    segs = {}
    for line in Path(x_path).read_text().splitlines():
        m = re.match(r'\s*(\w+)_seg\s*:\s*org\s*=\s*(0x[0-9a-fA-F]+)\s*,\s*len\s*=\s*(0x[0-9a-fA-F]+)', line)
        if m:
            segs[m.group(1)] = (int(m.group(2), 16), int(m.group(3), 16))
    return segs


def validate_x(x_path, start, end):
    """Read back the generated .x and assert the DSP window landed correctly."""
    if not Path(x_path).exists():
        return ["%s not generated" % x_path]
    errs = []
    segs = parse_x_segments(x_path)
    text = Path(x_path).read_text()

    if "entry_table_0" in segs:
        org, _ = segs["entry_table_0"]
        if org != start + 0x20:
            errs.append("entry_table_0 org %s != start+0x20 %s" % (hex(org), hex(start + 0x20)))
    else:
        errs.append("entry_table_0_seg missing")

    if "extra_reset_mem0" in segs:
        org, _ = segs["extra_reset_mem0"]
        if org != start + 0x1000:
            errs.append("extra_reset_mem0 org %s != start+0x1000 %s" % (hex(org), hex(start + 0x1000)))
    else:
        errs.append("extra_reset_mem0_seg missing")

    if "psram0" in segs:
        org, ln = segs["psram0"]
        if org + ln != end:
            errs.append("psram0 org+len %s != end %s" % (hex(org + ln), hex(end)))
    else:
        errs.append("psram0_seg missing")

    m = re.search(r'_memmap_reset_vector\s*=\s*(0x[0-9a-fA-F]+)', text)
    if m and int(m.group(1), 16) != start + 0x1000:
        errs.append("_memmap_reset_vector %s != start+0x1000 %s" % (m.group(1), hex(start + 0x1000)))

    for sec in ("*(.ipc_table)", "*(.command)"):
        if ("KEEP(%s)" % sec) not in text:
            errs.append("missing KEEP(%s)" % sec)
    return errs


def main():
    ap = argparse.ArgumentParser(description="Regenerate the DSP LSP(s) from the MCU layout.")
    ap.add_argument("--which", choices=["sram", "psram", "xip", "both", "auto"], default="auto",
                    help="Which LSP to regenerate. Default auto: sram iff CONFIG_DSP_RUN_IN_SRAM=y, else psram. "
                         "xip (RTK_LSP_XIP: DSP code XIP from flash) is explicit-only.")
    ap.add_argument("--mcu-root", default=None, help="MCU repo root (default: this script's repo).")
    ap.add_argument("--soc", default=None, help="SoC name (default: read soc_info.json).")
    ap.add_argument("--config", default=None, help="Path to the MCU .config (default: build_<SOC>/menuconfig/.config, the "
                    "menuconfig source of truth -- not the derived, possibly-stale build/project_km4/.config_km4).")
    ap.add_argument("--dsp-sdk", default=None,
                    help="DSP SDK root (override; default: derived from CONFIG_DSP_SDK_IMAGE_DIR).")
    ap.add_argument("--core", default="HIFI5_PROD_1123_asic_UPG", help="Xtensa core name.")
    ap.add_argument("--stack-size", default="0x1000", help="reset_stack size (hex). Default 0x1000.")
    ap.add_argument("--project-mpu", default=None,
                    help="Optional project_dsp/mpu_table.c to sync to the ACTIVE LSP's mpu (see --active).")
    ap.add_argument("--active", choices=["sram", "psram", "xip"], default=None,
                    help="Which regenerated LSP the DSP build links; its mpu is copied to --project-mpu.")
    ap.add_argument("--no-validate", action="store_true", help="Skip .x readback validation.")
    args = ap.parse_args()

    mcu_root = Path(args.mcu_root) if args.mcu_root else Path(__file__).resolve().parents[2]
    soc = args.soc or _read_soc(mcu_root)
    config = Path(args.config) if args.config else (
        mcu_root / ("build_%s" % soc) / "menuconfig" / ".config")
    ld = mcu_root / "component" / "soc" / "amebalite" / "project" / "ameba_layout.ld"
    if not config.exists():
        die("MCU .config not found: %s\n"
            "  It only needs a Kconfig configure (menuconfig / kconfig_set / any build) to exist -\n"
            "  a full firmware build is NOT required to generate the LSP. Or pass --config <path>." % config)
    if not ld.exists():
        die("ameba_layout.ld not found: %s" % ld)

    cfg = read_config(config)

    # DSP SDK root: explicit --dsp-sdk wins; otherwise derive from
    # CONFIG_DSP_SDK_IMAGE_DIR (the SDK image dir = <sdk>/project/image, so the
    # root is two levels up). Relative image dir -> resolved against the SoC
    # project dir (ld.parent), same rule as build.py / postbuild.cmake.
    if args.dsp_sdk:
        sdk = os.path.abspath(os.path.expanduser(args.dsp_sdk))
    else:
        img = (cfg.get("CONFIG_DSP_SDK_IMAGE_DIR") or "").strip()
        if not img:
            die("DSP SDK root unknown: pass --dsp-sdk, or set CONFIG_DSP_SDK_IMAGE_DIR in\n"
                "  %s to the DSP SDK image output dir\n"
                "  (e.g. /path/to/dsp_sdk/heap/source/project/image)." % config)
        img = os.path.expanduser(img)
        if not os.path.isabs(img):
            img = os.path.join(str(ld.parent), img)
        img = os.path.abspath(img)
        sdk = os.path.dirname(os.path.dirname(img))  # project/image -> SDK root
    if not Path(sdk).is_dir():
        die("DSP SDK root does not exist: %s" % sdk)
    if not (Path(sdk) / "project" / "img_utility" / "lsp_modify.py").exists():
        die("DSP SDK root looks wrong (no project/img_utility/lsp_modify.py): %s\n"
            "  It is derived from CONFIG_DSP_SDK_IMAGE_DIR by going up two levels;\n"
            "  point that at <sdk>/project/image, or pass --dsp-sdk explicitly." % sdk)

    # The LSP (.x) is produced by the Xtensa toolchain's xt-genldscripts; without
    # it on PATH lsp_modify.py fails with a cryptic 'xtensa-core templates' error.
    # Fail fast here with an actionable hint instead.
    if shutil.which("xt-genldscripts") is None:
        die("xt-genldscripts not found on PATH -- the Xtensa toolchain is required to\n"
            "  generate the DSP LSP (.x). Add the Xtensa tool bin to PATH first, e.g.:\n"
            "    export PATH=$PATH:/opt/xtensa/XtDevTools/install/tools/RI-2021.8-linux/XtensaTools/bin")

    defs, counts = read_ld_defines(ld, cfg)
    which = args.which
    if which == "auto":
        which = "sram" if cfg.get("CONFIG_DSP_RUN_IN_SRAM") == "y" else "psram"
        print("auto-selected --which %s (CONFIG_DSP_RUN_IN_SRAM=%s)"
              % (which, cfg.get("CONFIG_DSP_RUN_IN_SRAM", "n")))
    windows = compute_windows(cfg, defs, counts, which)
    stack = int(args.stack_size, 16)

    print("MCU: soc=%s config=%s" % (soc, config))
    for key, (start, end) in windows.items():
        print("  %-5s window: start=%s end=%s size=%s" % (key, hex(start), hex(end), hex(end - start)))

    rc = 0
    for key, (start, end) in windows.items():
        layout, lsp_name = LSPS[key]
        lsp_dir = find_lsp_dir(sdk, lsp_name, args.core)
        project_mpu = args.project_mpu if (args.active == key and args.project_mpu) else None
        print("\n[%s] -> %s" % (lsp_name, lsp_dir))
        if run_lsp_modify(sdk, layout, lsp_dir, start, end, stack, args.core, project_mpu):
            print("  lsp_modify.py FAILED for %s" % lsp_name)
            rc = 1
            continue
        if not args.no_validate:
            errs = validate_x(lsp_dir / "ldscripts" / "elf32xtensa.x", start, end)
            if errs:
                print("  VALIDATION FAILED:")
                for e in errs:
                    print("    - " + e)
                rc = 1
            else:
                print("  validation OK (entry/reset/psram0 addresses + KEEPs match)")
    return rc


def _read_soc(mcu_root):
    import json
    p = Path(mcu_root) / "soc_info.json"
    try:
        return json.loads(p.read_text())["soc"]["name"]
    except Exception:
        die("cannot read SoC from %s; pass --soc" % p)


if __name__ == "__main__":
    sys.exit(main())
