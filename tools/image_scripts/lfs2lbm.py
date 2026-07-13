#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# lfs2lbm.py - wrap a raw littlefs image into an LBM physical NAND image.
#
# The device runs littlefs on top of the LBM layer (Logical Block Mapping),
# which expects every physical erase block (PEB) to carry two header pages:
#   page 0 : meta  header (erase count)
#   page 1 : block header (which logical block this PEB holds + sequence)
#   page 2.. : the logical block's data
# A plain littlefs image (linear logical blocks, no headers) is therefore not
# recognised by LBM attach.  This tool converts:
#
#     littlefs.bin  (linear logical blocks, each = lblk_size bytes)
#         |
#         v   identity mapping: logical block N -> physical block N
#     lfs-lbm.bin   (array of 128 KB PEBs, each with LBM headers + data)
#
# The on-flash layout here MUST stay byte-for-byte in sync with
# component/file_system/vfs/lbm.h (lbm_meta_hdr_t / lbm_blk_hdr_t, magics, CRC).
#
# IMPORTANT: the input littlefs.bin must be built with
#     block_size  = lblk_size = (pages_per_peb - 2) * page_size   (124 KB here)
#     block_count = device's usable logical block count (lbm_block_count)
# otherwise littlefs mount on the device fails with "Invalid block count".

import argparse
import struct
import sys

# --- LBM on-flash constants (mirror lbm.h) ---
LBM_META_HDR_MAGIC = 0x304D424C   # "LBM0"
LBM_BLK_HDR_MAGIC  = 0x314D424C   # "LBM1"
LBM_HDR_VERSION    = 1

META_HDR_PAGE = 1
DATA_PAGE     = 2

# --- reserved-block policy ---------------------------------------------------
# !!! MUST stay in sync with lbm_core.c lbm_init():
#         reserved_blocks = max(total_blocks * RESERVED_PCT / 100, RESERVED_FLOOR)
#         usable_blocks   = total_blocks - reserved_blocks   (FIXED, no bad sub)
# The littlefs image MUST be built with block_count == usable_blocks, otherwise
# the device mounts with a different block_count and littlefs returns
# LFS_ERR_INVAL. ---------------------------------------------------------------
LBM_RESERVED_PCT   = 2
LBM_RESERVED_FLOOR = 20


def lbm_usable_blocks(partition_size, peb_size):
    """Compute the fixed usable (logical) block count exactly as lbm_core.c does."""
    total = partition_size // peb_size
    reserved = total * LBM_RESERVED_PCT // 100
    if reserved < LBM_RESERVED_FLOOR:
        reserved = LBM_RESERVED_FLOOR
    if total <= reserved:
        return 0, total, reserved
    return total - reserved, total, reserved


def crc32_lbm(data: bytes) -> int:
    """CRC32, std reflected poly 0xEDB88320, init/xorout 0xFFFFFFFF — same as lbm_crc32()."""
    crc = 0xFFFFFFFF
    for byte in data:
        crc ^= byte
        for _ in range(8):
            crc = (crc >> 1) ^ (0xEDB88320 & (-(crc & 1) & 0xFFFFFFFF))
    return crc ^ 0xFFFFFFFF


def make_meta_hdr(ec: int) -> bytes:
    """lbm_meta_hdr_t: magic,version,pad[3],ec(u64),blk_hdr_page,data_page,hdr_crc."""
    body = struct.pack("<IB3sQII",
                       LBM_META_HDR_MAGIC, LBM_HDR_VERSION, b"\xff\xff\xff",
                       ec, META_HDR_PAGE, DATA_PAGE)
    return body + struct.pack("<I", crc32_lbm(body))


def make_blk_hdr(lblk: int, seq: int) -> bytes:
    """lbm_blk_hdr_t: magic,version,copy_flag,pad[2],lblk,seq(u64),data_crc,hdr_crc."""
    body = struct.pack("<IBB2sIQI",
                       LBM_BLK_HDR_MAGIC, LBM_HDR_VERSION, 0, b"\xff\xff",
                       lblk, seq, 0xFFFFFFFF)
    return body + struct.pack("<I", crc32_lbm(body))


def page(data: bytes, page_size: int) -> bytes:
    """Pad a header to a full page with 0xFF (erased NAND)."""
    if len(data) > page_size:
        raise ValueError("header larger than page")
    return data + b"\xff" * (page_size - len(data))


def convert(in_path, out_path, page_size, pages_per_peb, partition_size=None):
    peb_size  = page_size * pages_per_peb
    lblk_size = page_size * (pages_per_peb - 2)

    with open(in_path, "rb") as f:
        img = f.read()

    if len(img) % lblk_size != 0:
        sys.exit(f"error: input size {len(img)} is not a multiple of lblk_size "
                 f"{lblk_size} (block_size must be {lblk_size} = "
                 f"{lblk_size // 1024} KB)")
    block_count = len(img) // lblk_size

    # Validate block_count against the device's fixed usable_blocks.
    if partition_size is not None:
        usable, total, reserved = lbm_usable_blocks(partition_size, peb_size)
        if usable == 0:
            sys.exit(f"error: partition too small (total={total} blocks <= "
                     f"reserved={reserved})")
        if block_count != usable:
            sys.exit(f"error: input block_count {block_count} != device "
                     f"usable_blocks {usable} (partition={partition_size} -> "
                     f"total={total}, reserved={reserved}).\n"
                     f"       rebuild littlefs.bin with: vfs.py ... "
                     f"-s {lblk_size} -c {usable}")
        print(f"[ok] block_count {block_count} matches device usable_blocks "
              f"(total={total}, reserved={reserved})")
    else:
        print("[warn] --partition-size not given: block_count NOT validated "
              "against device usable_blocks; ensure it equals total-reserved.")

    blank_lblk = b"\xff" * lblk_size

    # Trim trailing blank logical blocks (they stay erased on flash).
    last_used = -1
    for n in range(block_count):
        if img[n * lblk_size:(n + 1) * lblk_size] != blank_lblk:
            last_used = n
    emit_count = last_used + 1  # at least the superblock pair (blocks 0/1)

    used = 0
    seq = 1
    with open(out_path, "wb") as out:
        for n in range(emit_count):
            data = img[n * lblk_size:(n + 1) * lblk_size]
            if data == blank_lblk:
                # unused logical block -> blank PEB (LBM attach: DIRTY, allocatable)
                out.write(b"\xff" * peb_size)
                continue
            peb = page(make_meta_hdr(0), page_size)          # page 0
            peb += page(make_blk_hdr(n, seq), page_size)     # page 1
            peb += data                                      # pages 2..
            out.write(peb)
            used += 1
            seq += 1

    print(f"{out_path} generated.")
    print(f"├─ page_size     : {page_size}")
    print(f"├─ pages_per_peb : {pages_per_peb}  (peb_size={peb_size}, lblk_size={lblk_size})")
    print(f"├─ block_count   : {block_count}  (input)")
    print(f"├─ used blocks   : {used}")
    print(f"├─ pebs emitted  : {emit_count}  (trailing blanks trimmed)")
    print(f"└─ output size   : {emit_count * peb_size} bytes")


def main():
    p = argparse.ArgumentParser(
        description="Wrap a littlefs image into an LBM physical NAND image.")
    p.add_argument("input", help="raw littlefs image (e.g. littlefs.bin)")
    p.add_argument("-o", "--output", default="lfs-lbm.bin",
                   help="output LBM image (default: lfs-lbm.bin)")
    p.add_argument("--page-size", type=int, default=2048,
                   help="NAND page size in bytes (default: 2048)")
    p.add_argument("--pages-per-peb", type=int, default=64,
                   help="pages per physical erase block (default: 64)")
    p.add_argument("--partition-size", type=lambda x: int(x, 0), default=None,
                   help="VFS1 partition size (e.g. 0x2000000); enables "
                        "block_count validation against device usable_blocks")
    args = p.parse_args()

    if args.pages_per_peb < 3:
        sys.exit("error: pages-per-peb must be >= 3 (2 header pages + data)")

    convert(args.input, args.output, args.page_size, args.pages_per_peb,
            args.partition_size)


if __name__ == "__main__":
    main()
