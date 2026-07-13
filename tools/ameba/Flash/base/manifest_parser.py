#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

"""manifest_parser.py — RSIP META builder (dumb-pipe wire format).

Parses an Ameba partition image and produces the RSIP META payload that floader
needs before an encrypted WRITE sequence.  Floader is a dumb pipe: this module
fully describes every encrypted region.

MetaData wire format (matches floader_intf.h):
    [0]      meta_type     u8       = META_TYPE_ENCRYPTION (0x00)
    [1]      num_regions   u8
    [2:32)   Reserved      u8[30]   0xFF (header 32B-aligned)
    [32:..)  region[i]     u8[32]   N region descriptors

Each region descriptor (32B = 23B used + 9B reserved 0xFF):
    [0:4]    phys_offset   u32 LE   relative to FS_ERASE phys_start
    [4:8]    phys_size     u32 LE   region size
    [8:12]   logical_start u32 LE   bootloader RSIP IV-derivation base
    [12]     enc_mode      u8       1=CTR, 2=XTS, 3=GCM
    [13:21]  rsip_iv       u8[8]
    [21]     key_id_ecb    u8       km_aes_key_id (XTS only; 0xFF for CTR)
    [22]     key_id_ctr    u8       km_aes_key_id
    [23:32)  Reserved      u8[9]    0xFF

Partition layout auto-detection (by manifest/cert header pattern):

  ● image1 / boot.bin — no cert prefix:
      [0x0000] manifest  (FlashCalibPattern = 999996963fcc66fc)
      [0x1000] sub-bins  (FlashCalibPattern headers)
      RsipCfg bits[3:2] = image1 rsip_mode

  ● image2 / app.bin — cert prefix:
      [0x0000] cert      (img2sign pattern = 3831393538373131)
      [0x1000] manifest  (img2sign pattern)
      [0x2000] sub-bins  (img2sign headers)
      RsipCfg bits[5:4] = image2 rsip_mode

  ● image2 / app.bin — extended cert (8 KB cert):
      [0x0000] cert      (img2sign pattern, 8 KB)
      [0x2000] manifest  (img2sign pattern)
      [0x3000] sub-bins  (img2sign headers)
      RsipCfg bits[5:4] = image2 rsip_mode

RsipCfg bit layout (manifest_manager.py uses shift i*2, i=1/2/3):
    bits[3:2] = image1,  bits[5:4] = image2,  bits[7:6] = image3
manifest convention: 0=CTR, 1=XTS, 2=GCM → floader enc_mode = convention+1

XIP range map (amebagreen2 ameba_layout.ld):
    KM4TZ_BOOT_XIP  0x10400020-0x10C00000  image1
    KM4TZ_IMG3_XIP  0x00C00020-0x01000000  image3
    KM4NS_IMG2_XIP  0x02000020-0x04000000  image2 NP
    KM4TZ_IMG2_XIP  0x04000020-0x06000000  image2 AP
  Non-XIP sub-bins (SRAM/PSRAM) do not start a new region; they are absorbed
  into the current major-bin region until the next XIP sub-bin.
"""

import struct
import os
import logging

logger = logging.getLogger(__name__)

# ── Constants ─────────────────────────────────────────────────────────────────

MANIFEST_SIZE     = 0x1000   # Manifest_TypeDef binary is 4 KB
CERT_SIZE_STD     = 0x1000   # Standard Certificate_TypeDef is 4 KB
CERT_SIZE_EXT     = 0x2000   # Extended Certificate_TypeDef is 8 KB
IMAGE_HEADER_LEN  = 0x20     # 32-byte prepend header per sub-bin

# Offsets within Manifest_TypeDef (relative to manifest section start)
_RSIP_CFG_OFFSET  = 15
_RSIP_IV_OFFSET   = 32

# RsipCfg bit shifts per image type (manifest_manager.py: shift = i*2)
_IMG1_MODE_SHIFT  = 2   # image1 bits[3:2]
_IMG2_MODE_SHIFT  = 4   # image2 bits[5:4]
_IMG3_MODE_SHIFT  = 6   # image3 bits[7:6]

# Enc-mode values (floader_intf.h META_ENC_MODE_*)
ENC_MODE_NONE = 0x00
ENC_MODE_CTR  = 0x01
ENC_MODE_XTS  = 0x02
ENC_MODE_GCM  = 0x03

META_TYPE_ENCRYPTION = 0x00
META_HEADER_LEN        = 32
META_REGION_LEN        = 32

# ── Prepend header patterns (op_prepend_header.py constants, big-endian) ──────

_PATTERN_IMG1 = (0x99999696).to_bytes(4, 'big') + (0x3FCC66FC).to_bytes(4, 'big')
_PATTERN_IMG2 = (0x3831393538373131).to_bytes(8, 'big')

# image3 XIP logical bases (KM4TZ_IMG3_XIP origin per SoC).  Detection of the
# image3 region within app.bin depends on the IMG3 run-mode (Kconfig choice
# IMG3_FLASH / IMG3_SRAM / IMG3_PSRAM):
#   IMG3_FLASH (XIP): the real xip_3_s sub-bin header carries this base, so the
#       region scan matches it directly (no anchor in the image — prepending one
#       would shift the XIP code and break boot).
#   IMG3_SRAM/PSRAM:  sub-bin headers carry RAM run addresses, so the CMake build
#       prepends a size=0 anchor header with image_addr=this base as the marker.
# Either way the scanner keys off these bases, so each TrustZone SoC with RSIP
# floader support must list its image3 XIP base here.
_IMAGE3_XIP_BASES = {
    0x00C00020,   # amebagreen2 (RTL8721F): KM4TZ_IMG3_XIP 0x00C00020–0x01000000
    0x10C00020,   # RTL8720F:               KM4TZ_IMG3_XIP 0x10C00020–0x11000000
}

# Exported alias kept for external import compatibility
_PREPEND_PATTERN_IMG2 = _PATTERN_IMG2

_MAX_SUB_BIN_SIZE = 16 * 1024 * 1024

# ── SoC-specific km_aes_key_id (amebagreen2 bootloader hardcoded) ─────────────

_KM_AES_KEY_S_IPSEC_KEY1   = 0x00   # image2 ECB (OTP 0x200)
_KM_AES_KEY_S_IPSEC_KEY2   = 0x01   # image2 CTR (OTP 0x220)
_KM_AES_KEY_S_RSIP_ECB_KEY = 0x04   # image1/3 ECB (OTP 0x2C0)
_KM_AES_KEY_S_RSIP_CTR_KEY = 0x05   # image1/3 CTR (OTP 0x2E0)

# Per-partition-type key_id pair (matches bootloader RSIP_KEY_NUM0/1 assignment)
_KEY_IDS = {
    'image1': (_KM_AES_KEY_S_RSIP_ECB_KEY, _KM_AES_KEY_S_RSIP_CTR_KEY),  # RSIP_KEY_NUM0: OTP 0x2C0/0x2E0
    'image2': (_KM_AES_KEY_S_IPSEC_KEY1,   _KM_AES_KEY_S_IPSEC_KEY2),    # RSIP_KEY_NUM1: OTP 0x200/0x220
    'image3': (_KM_AES_KEY_S_RSIP_ECB_KEY, _KM_AES_KEY_S_RSIP_CTR_KEY),  # same as image1 (inherit_from: image1)
}



# ── Partition type detection ──────────────────────────────────────────────────

def _detect_partition_type(data: bytes):
    """Return (partition_type, manifest_start, image_data_start) or raise ValueError.

    Detection is by manifest/cert header pattern at known offsets:
    ● image1/boot.bin: FlashCalibPattern at byte 0 → manifest at 0, data at 0x1000
    ● image2/app.bin std cert (4KB): img2sign at 0x0 (cert) and 0x1000 (manifest)
    ● image2/app.bin ext cert (8KB): img2sign at 0x0 (cert) and 0x2000 (manifest)
    """
    if len(data) < MANIFEST_SIZE:
        raise ValueError(f"image too small ({len(data)} bytes)")

    # image1: FlashCalibPattern at offset 0
    if data[:8] == _PATTERN_IMG1:
        manifest_start    = 0
        image_data_start  = MANIFEST_SIZE
        return 'image1', manifest_start, image_data_start

    # image3 with RSIP anchor header: img2sign at byte 0, size==0, image_addr==KM4TZ_IMG3_XIP.
    # Check BEFORE image2 — image2 cert always has non-zero size, so size==0 is unambiguous.
    # manifest_start=None signals that image3 has no embedded manifest; enc_mode must be
    # supplied by the caller (inherited from image1).
    if (data[:8] == _PATTERN_IMG2
            and len(data) >= IMAGE_HEADER_LEN
            and struct.unpack_from('<I', data, 8)[0] == 0
            and struct.unpack_from('<I', data, 12)[0] in _IMAGE3_XIP_BASES):
        return 'image3', None, IMAGE_HEADER_LEN

    # image2 with standard cert (4 KB): img2sign at 0 AND at 0x1000
    if (data[:8] == _PATTERN_IMG2
            and len(data) >= CERT_SIZE_STD + MANIFEST_SIZE
            and data[CERT_SIZE_STD:CERT_SIZE_STD + 8] == _PATTERN_IMG2):
        manifest_start   = CERT_SIZE_STD
        image_data_start = CERT_SIZE_STD + MANIFEST_SIZE
        return 'image2', manifest_start, image_data_start

    # image2 with extended cert (8 KB): img2sign at 0 AND at 0x2000
    if (data[:8] == _PATTERN_IMG2
            and len(data) >= CERT_SIZE_EXT + MANIFEST_SIZE
            and data[CERT_SIZE_EXT:CERT_SIZE_EXT + 8] == _PATTERN_IMG2):
        manifest_start   = CERT_SIZE_EXT
        image_data_start = CERT_SIZE_EXT + MANIFEST_SIZE
        return 'image2', manifest_start, image_data_start

    raise ValueError(
        f"unrecognised partition: "
        f"byte[0:8]={data[:8].hex()}, "
        f"byte[0x1000:0x1008]={data[CERT_SIZE_STD:CERT_SIZE_STD+8].hex() if len(data)>CERT_SIZE_STD+8 else '(too short)'}, "
        f"byte[0x2000:0x2008]={data[CERT_SIZE_EXT:CERT_SIZE_EXT+8].hex() if len(data)>CERT_SIZE_EXT+8 else '(too short)'}")


# ── Manifest parsing ──────────────────────────────────────────────────────────

def _parse_manifest(data: bytes, partition_type: str, manifest_start: int):
    """Extract (enc_mode, rsip_iv) for the given partition type.

    Returns (ENC_MODE_NONE, None) if RSIP is not enabled.
    """
    if len(data) < manifest_start + _RSIP_IV_OFFSET + 8:
        raise ValueError(
            f"image too short for manifest at 0x{manifest_start:x}")

    rsip_cfg = data[manifest_start + _RSIP_CFG_OFFSET]
    if rsip_cfg == 0xFF:
        return ENC_MODE_NONE, None

    shift        = _IMG1_MODE_SHIFT if partition_type == 'image1' else _IMG2_MODE_SHIFT
    manifest_mode = (rsip_cfg >> shift) & 0x03
    enc_mode     = manifest_mode + 1   # 0→1(CTR), 1→2(XTS), 2→3(GCM)
    rsip_iv      = bytes(data[manifest_start + _RSIP_IV_OFFSET:
                               manifest_start + _RSIP_IV_OFFSET + 8])
    logger.debug(
        f"manifest @ 0x{manifest_start:x}: RsipCfg=0x{rsip_cfg:02x} "
        f"shift={shift} mode={manifest_mode} enc_mode={enc_mode} iv={rsip_iv.hex()}")
    return enc_mode, rsip_iv


# ── Region scan ───────────────────────────────────────────────────────────────

class RegionScanError(ValueError):
    """Raised when RSIP is enabled but the plaintext sub-bin chain is invalid."""


def _scan_regions(image_data: bytes, partition_type: str,
                  image_data_start: int,
                  enc_mode_hint: int = 0,
                  rsip_iv_hint: bytes = None,
                  image3_enc_mode: int = ENC_MODE_NONE) -> list:
    """Walk the sub-bin chain and build RSIP region descriptors.

    Region boundary detection — address-descending heuristic:
      The packager always emits sub-bins in order: XIP first, then SRAM/PSRAM.
      A new major-bin region starts whenever image_addr is LESS THAN the
      previous sub-bin's image_addr — i.e. the address "resets downward" to a
      new XIP base.

      ⚠️ PORTABILITY ASSUMPTION:
        This works only when XIP virtual addresses are numerically smaller than
        SRAM/PSRAM addresses for all sub-bins in the partition.
        Verified on amebagreen2 (RTL8721F):
          XIP:   0x02xxxxxxx (NP), 0x04xxxxxxx (AP), 0x10xxxxxxx (boot)
          SRAM:  0x20xxxxxxx
          PSRAM: 0x60xxxxxxx
        If a future SoC assigns XIP to high virtual addresses (e.g. XIP >
        SRAM), the downward-address rule will mis-detect region boundaries.
        In that case replace this heuristic with an explicit SoC-keyed XIP
        range table (see ameba_layout.ld for the authoritative ranges).

    key_id_ecb/key_id_ctr are derived from partition_type, not sub-bin address.

    Validation: header pattern + size-bounds only — no address range checks.
    """
    sub_bin_pattern = _PATTERN_IMG1 if partition_type == 'image1' else _PATTERN_IMG2
    key_id_ecb, key_id_ctr = _KEY_IDS[partition_type]

    offset       = image_data_start
    total        = len(image_data)
    regions      = []
    cur          = None
    prev_addr    = 0      # image_addr of the previous sub-bin

    while offset + IMAGE_HEADER_LEN <= total:
        hdr = image_data[offset:offset + IMAGE_HEADER_LEN]

        if hdr[:8] != sub_bin_pattern:
            raise RegionScanError(
                f"sub-bin header pattern mismatch at 0x{offset:x}: "
                f"got {hdr[:8].hex()}, expected {sub_bin_pattern.hex()}")

        image_size = struct.unpack_from('<I', hdr, 8)[0]
        image_addr = struct.unpack_from('<I', hdr, 12)[0]

        # Sentinel: size=0, addr=0 marks the end of the sub-bin chain.
        if image_size == 0 and image_addr == 0:
            break

        # Anchor header: size=0, addr in _IMAGE3_XIP_BASES.
        # IMPORTANT: only treat as anchor when addr is in the known image3 XIP ranges.
        # Other size=0 sub-bins (e.g. empty PSRAM/SRAM placeholders with high addresses)
        # must fall through to normal processing below.
        if image_size == 0 and image_addr in _IMAGE3_XIP_BASES:
            if image3_enc_mode != ENC_MODE_NONE:
                # image3 RSIP: start a region and continue scanning.
                if cur is None or image_addr < prev_addr:
                    if cur is not None:
                        cur['phys_size'] = offset - cur['phys_offset']
                        regions.append(cur)
                    img3_key_ecb, img3_key_ctr = _KEY_IDS['image3']
                    cur = {
                        'phys_offset':   offset,
                        'logical_start': image_addr - IMAGE_HEADER_LEN,
                        'kind':          f"image3_region{len(regions)}",
                        'key_id_ecb':    img3_key_ecb,
                        'key_id_ctr':    img3_key_ctr,
                        'enc_mode':      image3_enc_mode,
                    }
                prev_addr = image_addr
                offset += IMAGE_HEADER_LEN
                continue
            else:
                # image3 not RSIP-encoded.  Stop scanning.
                if cur is not None:
                    cur['phys_size'] = offset - cur['phys_offset']
                    regions.append(cur)
                break

        if image_size > _MAX_SUB_BIN_SIZE:
            raise RegionScanError(
                f"sub-bin size 0x{image_size:x} too large at 0x{offset:x}")

        if offset + IMAGE_HEADER_LEN + image_size > total:
            raise RegionScanError(
                f"sub-bin exceeds file at 0x{offset:x}: "
                f"size=0x{image_size:x}, file_end=0x{total:x}")

        logical_addr = image_addr - IMAGE_HEADER_LEN

        # New major-bin region starts when the address "resets downward" to a
        # new XIP base (or at the very first sub-bin).
        #
        # ⚠️ ASSUMPTION: XIP flash-window addresses are numerically much smaller
        # than SRAM/PSRAM addresses, so a downward jump in image_addr reliably
        # signals a new XIP sub-bin (new major-bin).
        # Verified on amebagreen2 (RTL8721F):
        #   XIP:  0x02xxxxxx / 0x04xxxxxx / 0x10xxxxxx
        #   SRAM: 0x20xxxxxx   PSRAM: 0x60xxxxxx
        # If a future SoC places XIP at higher addresses than SRAM (e.g. XIP >
        # 0x20000000), this heuristic breaks and a proper XIP range table will
        # be required.  Verify against ameba_layout.ld before enabling on a new
        # SoC.
        if cur is None or image_addr < prev_addr:
            if image_addr in _IMAGE3_XIP_BASES:
                if cur is not None:
                    cur['phys_size'] = offset - cur['phys_offset']
                    regions.append(cur)
                    cur = None
                if image3_enc_mode != ENC_MODE_NONE:
                    # image3 RSIP: start region and continue scanning.
                    img3_key_ecb, img3_key_ctr = _KEY_IDS['image3']
                    cur = {
                        'phys_offset':   offset,
                        'logical_start': logical_addr,
                        'kind':          f"image3_region{len(regions)}",
                        'key_id_ecb':    img3_key_ecb,
                        'key_id_ctr':    img3_key_ctr,
                        'enc_mode':      image3_enc_mode,
                    }
                else:
                    break
            else:
                # Normal new major-bin region.
                if cur is not None:
                    cur['phys_size'] = offset - cur['phys_offset']
                    regions.append(cur)
                cur = {
                    'phys_offset':   offset,
                    'logical_start': logical_addr,
                    'kind':          f"{partition_type}_region{len(regions)}",
                    'key_id_ecb':    key_id_ecb,
                    'key_id_ctr':    key_id_ctr,
                }

        prev_addr = image_addr
        offset += IMAGE_HEADER_LEN + image_size

    if cur is not None:
        cur['phys_size'] = offset - cur['phys_offset']
        regions.append(cur)

    return regions


def _pack_region(r: dict, enc_mode: int, rsip_iv: bytes) -> bytes:
    desc = bytearray(b'\xff' * META_REGION_LEN)
    struct.pack_into('<I', desc, 0,  r['phys_offset'])
    struct.pack_into('<I', desc, 4,  r['phys_size'])
    struct.pack_into('<I', desc, 8,  r['logical_start'])
    desc[12]    = r.get('enc_mode', enc_mode)   # per-region override (e.g. image3)
    desc[13:21] = rsip_iv
    desc[21]    = r['key_id_ecb']
    desc[22]    = r['key_id_ctr']
    return bytes(desc)


def build_rsip_meta(image_path: str,
                    enc_mode_hint: int = ENC_MODE_NONE,
                    rsip_iv_hint: bytes = None) -> tuple:
    """Build the RSIP META payload for a partition image.

    For image3 (TrustZone, inherit_from: image1) the binary has no embedded
    manifest.  Pass enc_mode_hint and rsip_iv_hint from image1's parsed values.

    Returns:
        (enc_mode, rsip_iv, meta_bytes, regions) on success, or
        (ENC_MODE_NONE, None, None, []) if RSIP is disabled / not applicable /
        plaintext chain not parseable (treat as pre-encrypted legacy image).
    """
    if not os.path.isfile(image_path):
        logger.debug(f"manifest_parser: {image_path} not found, skip RSIP")
        return ENC_MODE_NONE, None, None, []

    with open(image_path, 'rb') as f:
        image_data = f.read()

    try:
        partition_type, manifest_start, image_data_start = \
            _detect_partition_type(image_data)
    except ValueError as e:
        logger.debug(f"manifest_parser: {os.path.basename(image_path)}: {e}; skip RSIP")
        return ENC_MODE_NONE, None, None, []

    logger.debug(f"manifest_parser: {os.path.basename(image_path)}: "
                 f"type={partition_type} manifest={'None' if manifest_start is None else hex(manifest_start)} "
                 f"data=0x{image_data_start:x}")

    # image3 has no embedded manifest; enc_mode and rsip_iv come from image1 (inherit).
    if manifest_start is None:
        if enc_mode_hint == ENC_MODE_NONE or rsip_iv_hint is None:
            logger.debug(
                f"manifest_parser: {os.path.basename(image_path)} is image3 but no "
                f"enc_mode_hint/rsip_iv_hint provided; skip RSIP")
            return ENC_MODE_NONE, None, None, []
        enc_mode = enc_mode_hint
        rsip_iv  = rsip_iv_hint
    else:
        try:
            enc_mode, rsip_iv = _parse_manifest(image_data, partition_type, manifest_start)
        except ValueError as e:
            logger.warning(f"manifest_parser: {e}")
            return ENC_MODE_NONE, None, None, []

    if enc_mode == ENC_MODE_NONE:
        return ENC_MODE_NONE, None, None, []

    # For image2 partitions, read image3's enc_mode from bits[7:6] of RsipCfg.
    # Prerequisite: image2.rsip_enable must be True (otherwise bits[7:6] stays
    # 0xFF / unset).  "image2 off, image3 on" is not a supported configuration.
    #
    # For standalone image3 (no embedded manifest, inherit_from: image1):
    # enc_mode is already resolved from enc_mode_hint above.  The whole image
    # is the image3 region, so set image3_enc_mode = enc_mode so _scan_regions
    # recognises the XIP sub-bin anchor and builds the region descriptor instead
    # of silently breaking out of the scan and returning no regions.
    image3_enc_mode = ENC_MODE_NONE
    if partition_type == 'image3':
        image3_enc_mode = enc_mode
    elif partition_type == 'image2' and manifest_start is not None:
        rsip_cfg = image_data[manifest_start + _RSIP_CFG_OFFSET]
        img3_bits = (rsip_cfg >> _IMG3_MODE_SHIFT) & 0x03
        if img3_bits != 0x03:   # 0x03 = unset (0xFF default)
            image3_enc_mode = img3_bits + 1  # 0→CTR, 1→XTS, 2→GCM

    try:
        regions = _scan_regions(image_data, partition_type, image_data_start,
                        enc_mode_hint=enc_mode_hint, rsip_iv_hint=rsip_iv_hint,
                        image3_enc_mode=image3_enc_mode)
    except RegionScanError as e:
        logger.warning(
            f"manifest_parser: enc_mode={enc_mode}, plaintext sub-bin scan failed "
            f"in {os.path.basename(image_path)}: {e}; "
            f"treat as pre-encrypted legacy image")
        return ENC_MODE_NONE, None, None, []

    if not regions:
        logger.warning(
            f"manifest_parser: enc_mode={enc_mode} but no XIP regions found in "
            f"{os.path.basename(image_path)}; treat as pre-encrypted legacy image")
        return ENC_MODE_NONE, None, None, []

    header = bytearray(b'\xff' * META_HEADER_LEN)
    header[0] = META_TYPE_ENCRYPTION
    header[1] = len(regions)
    region_bytes = b''.join(_pack_region(r, enc_mode, rsip_iv) for r in regions)
    meta_bytes = bytes(header) + region_bytes

    logger.info(
        f"RSIP META built: partition={partition_type}, "
        f"meta_type=0x{META_TYPE_ENCRYPTION:02x}, "
        f"{len(regions)} region(s), enc_mode={enc_mode}, "
        f"total {len(meta_bytes)} bytes")
    for i, r in enumerate(regions):
        logger.info(
            f"  region[{i}] {r['kind']}: "
            f"phys=[0x{r['phys_offset']:08x}+0x{r['phys_size']:08x}] "
            f"logical=0x{r['logical_start']:08x} "
            f"ecb=0x{r['key_id_ecb']:02x} ctr=0x{r['key_id_ctr']:02x}")
    return enc_mode, rsip_iv, meta_bytes, regions
