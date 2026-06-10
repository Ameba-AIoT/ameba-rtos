#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2026 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

"""
Single source of truth for per-IC AGG path layout.

Each IC entry describes all five AGG paths (1-5): how bytes are dispatched (AggKind) and a
label. The label doubles as the [label] per-line prefix for TEXT_LOG paths and
as the output-file prefix (<label>_*.bin / <label>_*.log) for binary paths.

Adding a new IC only requires a new row in CHIP_TABLE — no changes to the
dispatch logic in log_handler.py.

Fallback rule (matches ResolveEntry in ChipInfo.cs): when an IC's own entry
for a path is NONE (path unassigned in hardware), resolve_path_kind falls
back to the Generic entry for that path. This keeps data visible as
Core0/Core1/Core2 instead of being silently dropped when the user selects
the wrong IC or a legacy pre-AGG chip on a board that actually emits AGG.
"""

from enum import Enum
from typing import Dict, List, Optional


class AggKind(Enum):
    NONE     = 0   # path unused on this IC
    TEXT_LOG = 1   # text   → stdout with [label] prefix
    BIN_FILE = 2   # binary → <label>_*.bin
    LOG_FILE = 3   # binary → <label>_*.log


# Output-file suffix for each binary kind. The label on the AggCore supplies
# the filename prefix; the kind picks the suffix. Keeping this here means a new
# IC row never has to know about file naming — it just names a kind + label.
_BIN_SUFFIX: Dict['AggKind', str] = {
    AggKind.BIN_FILE: 'bin',
    AggKind.LOG_FILE: 'log',
}


def bin_suffix(kind: 'AggKind') -> Optional[str]:
    """Filename suffix ('bin'/'log') for a binary kind, else None."""
    return _BIN_SUFFIX.get(kind)


class AggCore:
    __slots__ = ('path_num', 'kind', 'label')

    def __init__(self, path_num: int, kind: AggKind, label: str):
        self.path_num = path_num
        self.kind     = kind
        self.label    = label


# AGG path number constants (matches AggPath in ChipInfo.cs)
PATH1 = 0x1   # first  CPU text log
PATH2 = 0x2   # second CPU text log
PATH3 = 0x3   # BT host upper-stack binary
PATH4 = 0x4   # third  CPU text log (or binary on some ICs)
PATH5 = 0x5   # BT firmware binary

# ---------------------------------------------------------------------------
# Generic fallback — used when the IC's own entry is NONE or the chip name is
# unknown. Keeps packets visible as Core0/Core1/Core2 / BT streams rather than
# dropping them silently. Matches RtkChipType.Generic in ChipInfo.cs.
# ---------------------------------------------------------------------------
_GENERIC: List[AggCore] = [
    AggCore(PATH1, AggKind.TEXT_LOG,       'Core0'),
    AggCore(PATH2, AggKind.TEXT_LOG,       'Core1'),
    AggCore(PATH3, AggKind.BIN_FILE,      'BT'),
    AggCore(PATH4, AggKind.TEXT_LOG,       'Core2'),
    AggCore(PATH5, AggKind.LOG_FILE,      'BT_FW'),
]

# Shared row for legacy pre-AGG ICs. All paths are NONE so they resolve to
# Generic, meaning a real AGG board paired with a wrong legacy selection still
# produces Core0/Core1/Core2 output. Matches LegacyAllNone in ChipInfo.cs.
_LEGACY_ALL_NONE: List[AggCore] = [
    AggCore(PATH1, AggKind.NONE, ''),
    AggCore(PATH2, AggKind.NONE, ''),
    AggCore(PATH3, AggKind.NONE, ''),
    AggCore(PATH4, AggKind.NONE, ''),
    AggCore(PATH5, AggKind.NONE, ''),
]

# ---------------------------------------------------------------------------
# Per-IC AGG path table. Each entry covers all five paths in path-number
# order. 'Generic' is the internal fallback key and is not exposed in IC_LIST.
# Matches mCoreMap in ChipInfo.cs.
# ---------------------------------------------------------------------------
CHIP_TABLE: Dict[str, List[AggCore]] = {
    'Generic': _GENERIC,

    'RTL8721Dx': [
        AggCore(PATH1, AggKind.TEXT_LOG,       'KM4'),
        AggCore(PATH2, AggKind.TEXT_LOG,       'KM0'),
        AggCore(PATH3, AggKind.BIN_FILE,      'BT'),
        AggCore(PATH4, AggKind.NONE,           ''),
        AggCore(PATH5, AggKind.LOG_FILE,      'BT_FW'),
    ],
    'RTL8721F': [
        AggCore(PATH1, AggKind.TEXT_LOG,       'KM4TZ'),
        AggCore(PATH2, AggKind.TEXT_LOG,       'KM4NS'),
        AggCore(PATH3, AggKind.BIN_FILE,      'BT'),
        AggCore(PATH4, AggKind.NONE,           ''),
        AggCore(PATH5, AggKind.LOG_FILE,      'BT_FW'),
    ],
    'RTL8720F': [
        AggCore(PATH1, AggKind.TEXT_LOG,       'KM4TZ'),
        AggCore(PATH2, AggKind.TEXT_LOG,       'KM4NS'),
        AggCore(PATH3, AggKind.BIN_FILE,      'BT'),
        AggCore(PATH4, AggKind.BIN_FILE,      'SDN'),
        AggCore(PATH5, AggKind.NONE,           ''),
    ],
    'RTL8720E': [
        AggCore(PATH1, AggKind.TEXT_LOG,       'KM4'),
        AggCore(PATH2, AggKind.TEXT_LOG,       'KR4'),
        AggCore(PATH3, AggKind.BIN_FILE,      'BT'),
        AggCore(PATH4, AggKind.NONE,           ''),
        AggCore(PATH5, AggKind.LOG_FILE,      'BT_FW'),
    ],
    'RTL8710E': [
        AggCore(PATH1, AggKind.TEXT_LOG,       'KM4'),
        AggCore(PATH2, AggKind.TEXT_LOG,       'KR4'),
        AggCore(PATH3, AggKind.BIN_FILE,      'BT'),
        AggCore(PATH4, AggKind.NONE,           ''),
        AggCore(PATH5, AggKind.LOG_FILE,      'BT_FW'),
    ],
    'RTL8726E': [
        AggCore(PATH1, AggKind.TEXT_LOG,       'KM4'),
        AggCore(PATH2, AggKind.TEXT_LOG,       'KR4'),
        AggCore(PATH3, AggKind.BIN_FILE,      'BT'),
        AggCore(PATH4, AggKind.TEXT_LOG,       'DSP'),
        AggCore(PATH5, AggKind.LOG_FILE,      'BT_FW'),
    ],
    'RTL8713E': [
        AggCore(PATH1, AggKind.TEXT_LOG,       'KM4'),
        AggCore(PATH2, AggKind.TEXT_LOG,       'KR4'),
        AggCore(PATH3, AggKind.BIN_FILE,      'BT'),
        AggCore(PATH4, AggKind.TEXT_LOG,       'DSP'),
        AggCore(PATH5, AggKind.LOG_FILE,      'BT_FW'),
    ],
    'RTL8730E': [
        AggCore(PATH1, AggKind.TEXT_LOG,       'KM4'),
        AggCore(PATH2, AggKind.TEXT_LOG,       'KM0'),
        AggCore(PATH3, AggKind.BIN_FILE,      'BT'),
        AggCore(PATH4, AggKind.TEXT_LOG,       'CA32'),
        AggCore(PATH5, AggKind.LOG_FILE,      'BT_FW'),
    ],
    'RTL8735C': [
        AggCore(PATH1, AggKind.TEXT_LOG,       'KM4NP'),
        AggCore(PATH2, AggKind.TEXT_LOG,       'CA32'),
        AggCore(PATH3, AggKind.BIN_FILE,      'BT'),
        AggCore(PATH4, AggKind.TEXT_LOG,       'KM0'),
        AggCore(PATH5, AggKind.LOG_FILE,      'BT_FW'),
    ],

    # Legacy pre-AGG ICs — no AGG hardware. All paths are NONE; the Generic
    # fallback in resolve_path_kind keeps data visible if a real AGG board is
    # mistakenly paired with one of these selections.
    'RTL8195X': _LEGACY_ALL_NONE,
    'RTL871XB':  _LEGACY_ALL_NONE,
    'RTL872xD':  _LEGACY_ALL_NONE,
}

# Selectable chip names for --chip (excludes the internal 'Generic' entry).
IC_LIST: List[str] = [k for k in CHIP_TABLE if k != 'Generic']


# ---------------------------------------------------------------------------
# Lookup helpers (mirror CoresForIC / FindEntry / ResolveEntry in ChipInfo.cs)
# ---------------------------------------------------------------------------

def _find_entry(chip: str, path_num: int) -> Optional[AggCore]:
    for core in CHIP_TABLE.get(chip, _GENERIC):
        if core.path_num == path_num:
            return core
    return None


def resolve_path(chip: Optional[str], path_num: int) -> Optional[AggCore]:
    """Resolved AggCore (kind + label) for path_num on chip.

    Falls back to the Generic entry when the IC's own entry is NONE (wrong-IC
    or unassigned path keeps data visible). Returns None only for path numbers
    Generic itself does not define (header src 0/6/7, never emitted in practice).
    """
    if not chip:
        chip = 'Generic'
    entry = _find_entry(chip, path_num)
    if entry is not None and entry.kind != AggKind.NONE:
        return entry
    return _find_entry('Generic', path_num)


def resolve_path_kind(chip: Optional[str], path_num: int) -> AggKind:
    """Dispatch kind for path_num on chip, with Generic fallback."""
    entry = resolve_path(chip, path_num)
    return entry.kind if entry else AggKind.NONE


def get_core_tag(chip: Optional[str], path_num: int) -> str:
    """Return ' [label]' for TextLog paths, '' for binary/unused paths.
    Uses the resolved entry so wrong-IC selections still produce labelled
    output via the Generic Core0/1/2 fallback."""
    if not chip:
        return ''
    entry = resolve_path(chip, path_num)
    if entry and entry.kind == AggKind.TEXT_LOG:
        return f' [{entry.label}]'
    return ''


def normalize_chip_name(value: str) -> Optional[str]:
    """Return the canonical IC name for value (case-insensitive), or None."""
    lower = value.lower()
    for name in IC_LIST:
        if name.lower() == lower:
            return name
    return None
