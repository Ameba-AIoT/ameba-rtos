#!/usr/bin/env python3
"""
apply_sections.py — Apply JSON5-driven section placement to static libraries.

Reads a JSON5 config where every entry is one line:
    { func: "name",  section: ".target.section" }
    { func: "name",  section: ".target.section", core: "km4tz" }

Algorithm (efficient – each .a is extracted/repacked at most once):
  1. Parse JSON5, filter by --core, build {func → section} map.
  2. Parallel nm -A scan: find which archive and .o contain each target function.
  3. Group results by archive.
  4. For each archive that needs changes:
       ar x  →  batch objcopy --rename-section  →  ar r (only modified .o files)

Usage:
    python3 tools/apply_sections.py \\
        --config component/soc/RTL8720F/project/section_placement.json5 \\
        --build-dir build_RTL8720F/project_km4tz \\
        --core km4tz \\
        [--cross arm-none-eabi-] \\
        [--dry-run] \\
        [--jobs N]
"""

import argparse
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile
from collections import defaultdict
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path


# ---------------------------------------------------------------------------
# JSON5 parser (no external dependencies)
# ---------------------------------------------------------------------------

def _strip_json5(text: str) -> str:
    """Convert JSON5 to strict JSON so json.loads() can handle it."""
    # Block comments  /* ... */
    text = re.sub(r'/\*.*?\*/', '', text, flags=re.DOTALL)
    # Line comments   // ...
    text = re.sub(r'//[^\n]*', '', text)
    # Trailing comma before ] or }
    text = re.sub(r',(\s*[}\]])', r'\1', text)
    # Unquoted object keys:  { key: → { "key":
    text = re.sub(r'([{\[,])\s*([A-Za-z_]\w*)\s*:', r'\1 "\2":', text)
    return text


def load_json5(path: Path) -> list:
    try:
        raw = path.read_text(encoding='utf-8')
    except FileNotFoundError:
        print(f'[warn] Config not found, skipping: {path}', file=sys.stderr)
        return []
    except OSError as e:
        sys.exit(f'Error reading {path}: {e}')
    try:
        return json.loads(_strip_json5(raw))
    except json.JSONDecodeError as e:
        sys.exit(f'JSON5 parse error in {path}: {e}')


# ---------------------------------------------------------------------------
# Toolchain helpers
# ---------------------------------------------------------------------------

def _find_toolchain_prefix() -> str:
    """Return 'arm-none-eabi-' after ensuring the tools are on PATH."""
    prefix = 'arm-none-eabi-'
    if shutil.which(prefix + 'nm'):
        return prefix

    # Search /opt/rtk-toolchain
    base = Path('/opt/rtk-toolchain')
    if base.is_dir():
        hits = sorted(base.rglob('arm-none-eabi-nm'))
        if hits:
            tc_bin = str(hits[-1].parent)
            os.environ['PATH'] = tc_bin + os.pathsep + os.environ.get('PATH', '')
            return prefix

    sys.exit(
        'arm-none-eabi-nm not found. '
        'Set --cross, add toolchain to PATH, or install under /opt/rtk-toolchain.'
    )


def _check_tools(cross: str) -> None:
    for tool in ('nm', 'objdump', 'objcopy', 'ar'):
        if not shutil.which(cross + tool):
            sys.exit(f'Tool not found: {cross}{tool}')


# ---------------------------------------------------------------------------
# nm scan  (no archive extraction needed)
# ---------------------------------------------------------------------------

def _nm_scan(archive: Path, wanted: set, cross: str) -> list[tuple[str, str]]:
    """
    Run `nm -A archive` and return [(obj_name, func_name)] for every symbol
    in `wanted` that has a text/weak type (T t W w).

    nm -A output format:
        /path/archive.a:member.o:  00001234 T symbol_name
    """
    try:
        r = subprocess.run(
            [cross + 'nm', '-A', str(archive)],
            capture_output=True, text=True, timeout=60,
        )
    except (subprocess.TimeoutExpired, FileNotFoundError) as e:
        print(f'  [warn] nm failed on {archive.name}: {e}', file=sys.stderr)
        return []

    hits = []
    for line in r.stdout.splitlines():
        # Split on first two colons: archive : obj : rest
        c1 = line.find(':')
        if c1 < 0:
            continue
        c2 = line.find(':', c1 + 1)
        if c2 < 0:
            continue

        obj_name = Path(line[c1 + 1: c2]).name   # basename of .o
        tokens   = line[c2 + 1:].split()

        if len(tokens) < 2:
            continue
        sym_type = tokens[-2]
        sym_name = tokens[-1]

        if sym_type in ('T', 't', 'W', 'w') and sym_name in wanted:
            hits.append((obj_name, sym_name))

    return hits


# ---------------------------------------------------------------------------
# Current section query
# ---------------------------------------------------------------------------

def _current_section(obj_path: Path, func: str, cross: str) -> str | None:
    """
    Use objdump -t to find the actual section name of `func` in `obj_path`.
    Falls back to None if not found.
    """
    try:
        r = subprocess.run(
            [cross + 'objdump', '-t', str(obj_path)],
            capture_output=True, text=True, timeout=15,
        )
    except (subprocess.TimeoutExpired, FileNotFoundError):
        return None

    # objdump -t line example:
    #   00000000 l     F .text.foo  00000020 foo
    # The section token is the first that starts with '.'
    for line in r.stdout.splitlines():
        tokens = line.split()
        if not tokens or tokens[-1] != func:
            continue
        for tok in tokens:
            if tok.startswith('.'):
                return tok
    return None


# ---------------------------------------------------------------------------
# Per-archive modification  (extract once, patch all, repack once)
# ---------------------------------------------------------------------------

def _apply_archive(
    archive: Path,
    changes: list[tuple[str, str, str]],   # (obj_name, func, target_section)
    cross:   str,
    dry_run: bool,
    build_dir: Path,
) -> int:
    """
    Returns the number of successful renames.
    """
    ar_path      = cross + 'ar'
    objcopy_path = cross + 'objcopy'
    rel_archive  = archive.relative_to(build_dir) if archive.is_relative_to(build_dir) \
                   else archive

    # dry-run: skip extraction — predict section name from -ffunction-sections convention
    if dry_run:
        ok = 0
        for obj_name, func, target_sec in changes:
            cur_sec = f'.text.{func}'
            print(f'  [dry]   {rel_archive}:{obj_name}  '
                  f'{func}: {cur_sec} → {target_sec}')
            ok += 1
        return ok

    with tempfile.TemporaryDirectory(prefix='apply_sec_') as tmpdir:
        tmp = Path(tmpdir)

        # Extract archive (once)
        r = subprocess.run(
            [ar_path, 'x', str(archive)],
            cwd=tmpdir, capture_output=True, timeout=120,
        )
        if r.returncode != 0:
            print(f'  [error] ar x failed: {rel_archive}: '
                  f'{r.stderr.decode(errors="replace")[:120]}', file=sys.stderr)
            return 0

        modified: list[Path] = []   # collect for ar r
        ok = 0

        for obj_name, func, target_sec in changes:
            obj_path = tmp / obj_name
            if not obj_path.exists():
                print(f'  [warn]  {rel_archive}:{obj_name}: '
                      f'not found after extraction', file=sys.stderr)
                continue

            # Determine current section
            cur_sec = _current_section(obj_path, func, cross)
            if cur_sec is None:
                # Fallback: -ffunction-sections naming convention
                cur_sec = f'.text.{func}'

            if cur_sec == target_sec:
                continue   # already correct, nothing to do

            # Rename section in the .o file
            r2 = subprocess.run(
                [objcopy_path,
                 '--rename-section', f'{cur_sec}={target_sec}',
                 str(obj_path)],
                capture_output=True, timeout=15,
            )
            if r2.returncode != 0:
                print(f'  [error] objcopy failed for {func} in '
                      f'{rel_archive}:{obj_name}: '
                      f'{r2.stderr.decode(errors="replace")[:120]}',
                      file=sys.stderr)
                continue

            print(f'  [ok]    {rel_archive}:{obj_name}  '
                  f'{func}: {cur_sec} → {target_sec}')

            # Track unique modified .o paths for ar r
            if obj_path not in modified:
                modified.append(obj_path)
            ok += 1

        # Repack only the modified members (ar r replaces in-place)
        if modified:
            r3 = subprocess.run(
                [ar_path, 'r', str(archive)] + [str(p) for p in modified],
                capture_output=True, timeout=120,
            )
            if r3.returncode != 0:
                print(f'  [error] ar r failed: {rel_archive}: '
                      f'{r3.stderr.decode(errors="replace")[:120]}',
                      file=sys.stderr)

    return ok


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main() -> None:
    parser = argparse.ArgumentParser(
        description='Apply JSON5 section placements to .a files (pre-link step).',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )
    parser.add_argument('--config',    required=True, action='append',
                        metavar='FILE',
                        help='JSON5 placement config (repeatable for multiple files)')
    parser.add_argument('--build-dir', default=None, action='append', metavar='DIR',
                        help='Root directory to search for *.a files (repeatable for multiple dirs; '
                             'use --libs-file for precise library selection)')
    parser.add_argument('--libs-file', default=None, metavar='FILE',
                        help='Text file with one .a path per line '
                             '(generated by CMake from the actual link inputs)')
    parser.add_argument('--core',      default=None, metavar='NAME',
                        help='Current core (e.g. km4tz, km4ns); '
                             'filters core-qualified entries')
    parser.add_argument('--cross',     default=None, metavar='PREFIX',
                        help='Toolchain prefix (default: auto-detect arm-none-eabi-)')
    parser.add_argument('--dry-run',   action='store_true',
                        help='Show what would change without modifying any file')
    parser.add_argument('--jobs',      type=int,
                        default=min(os.cpu_count() or 4, 16),
                        metavar='N',
                        help='Parallel nm-scan threads (default: cpu_count, max 16)')
    args = parser.parse_args()

    # ── Toolchain ─────────────────────────────────────────────────────────
    cross = args.cross if args.cross else _find_toolchain_prefix()
    _check_tools(cross)

    # ── Load and merge JSON5 configs ──────────────────────────────────────
    # Each entry: { func, section [, core] }
    # Entries without 'core' apply to all cores.
    # If the same function appears in two configs with different sections → error.
    placements: dict[str, str] = {}   # func → target_section
    conflicts:  list           = []

    for cfg_str in args.config:
        cfg_path = Path(cfg_str)
        entries  = load_json5(cfg_path)

        for entry in entries:
            func    = entry.get('func')
            section = entry.get('section')
            core    = entry.get('core')

            if not func or not section:
                continue

            # Skip entries that are for a different core
            if core and core != args.core:
                continue

            if func in placements and placements[func] != section:
                conflicts.append((func, placements[func], section, cfg_str))
            else:
                placements[func] = section

    if conflicts:
        print('ERROR: conflicting section assignments for the same function:',
              file=sys.stderr)
        for func, s1, s2, path in conflicts:
            print(f'  {func}: "{s1}" vs "{s2}"  (in {path})', file=sys.stderr)
        sys.exit(1)

    if not placements:
        print('No placements to apply'
              + (f' for core={args.core}' if args.core else '') + '.')
        sys.exit(0)

    core_tag = f' [core={args.core}]' if args.core else ''
    print(f'Placements loaded: {len(placements)} entries{core_tag}')

    # ── Discover .a files ─────────────────────────────────────────────────
    if args.libs_file:
        try:
            lines = Path(args.libs_file).read_text(encoding='utf-8').splitlines()
        except OSError as e:
            sys.exit(f'Error reading --libs-file {args.libs_file}: {e}')
        archives = [Path(p.strip()) for p in lines if p.strip() and p.strip().endswith('.a')]
        build_dir = Path(args.libs_file).parent   # used only for display
        if not archives:
            sys.exit(f'No .a paths found in {args.libs_file}')
    elif args.build_dir:
        build_dirs = [Path(d).resolve() for d in args.build_dir]
        build_dir  = build_dirs[0]   # used only for display
        archives   = sorted({a for d in build_dirs for a in d.rglob('*.a')})
        if not archives:
            sys.exit(f'No .a files found under {", ".join(str(d) for d in build_dirs)}')
    else:
        sys.exit('Either --libs-file or --build-dir is required.')

    print(f'Scanning {len(archives)} archive(s) with {args.jobs} thread(s)...')

    # ── Parallel nm scan ──────────────────────────────────────────────────
    wanted = set(placements.keys())
    # archive → [(obj_name, func_name)]
    index: dict[Path, list] = defaultdict(list)

    def _scan(arch: Path):
        return arch, _nm_scan(arch, wanted, cross)

    with ThreadPoolExecutor(max_workers=args.jobs) as pool:
        futs = {pool.submit(_scan, a): a for a in archives}
        for fut in as_completed(futs):
            arch, hits = fut.result()
            if hits:
                index[arch].extend(hits)

    # ── Report not-found functions ────────────────────────────────────────
    found_funcs = {fn for hits in index.values() for _, fn in hits}
    missing     = wanted - found_funcs
    if missing:
        print(f'  [info] {len(missing)} function(s) not found in any .a '
              f'(ROM / not compiled for this core / already correct):')
        for fn in sorted(missing):
            print(f'    {fn}')

    if not index:
        print('Nothing to modify.')
        sys.exit(0)

    # ── Build per-archive change list ─────────────────────────────────────
    # archive → [(obj_name, func, target_section)]
    changes_map: dict[Path, list] = defaultdict(list)
    for arch, hits in index.items():
        for obj_name, func in hits:
            changes_map[arch].append((obj_name, func, placements[func]))

    dry_tag = ' [DRY RUN]' if args.dry_run else ''
    print(f'Modifying {len(changes_map)} archive(s){dry_tag}:')

    # ── Apply changes ─────────────────────────────────────────────────────
    total = 0
    for arch in sorted(changes_map.keys()):
        changes = changes_map[arch]
        rel = arch.relative_to(build_dir) if arch.is_relative_to(build_dir) else arch
        print(f'\n  {rel}  ({len(changes)} change(s)):')
        n = _apply_archive(arch, changes, cross, args.dry_run, build_dir)
        total += n

    result_tag = ' (dry run)' if args.dry_run else ''
    print(f'\nDone: {total} rename(s) applied{result_tag}.')


if __name__ == '__main__':
    main()
