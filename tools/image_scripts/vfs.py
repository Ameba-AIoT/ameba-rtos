#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import argparse
import math
import struct
# NOTE: the filesystem backends are imported lazily inside their own code paths
# only (littlefs -> `littlefs`, FATFS -> `pyfatfs`, REALFS -> `realfs`), so that
# using one backend does not require the others' packages to be installed.

def calculate_directory_stats(source_dir):
    total_size = 0
    dir_count = 0
    file_count = 0

    for root, dirs, files in os.walk(source_dir):
        dir_count += len(dirs)
        file_count += len(files)
        for f in files:
            fp = os.path.join(root, f)
            if not os.path.islink(fp) and os.path.isfile(fp):
                total_size += os.path.getsize(fp)

    return total_size, dir_count, file_count

def process_directory(source_dir, mkdir_func, write_func):
    for root, _, files in os.walk(source_dir):
        rel_path = os.path.relpath(root, source_dir)
        fs_path = rel_path.replace(os.sep, '/') if rel_path != '.' else ''

        if fs_path:
            mkdir_func(fs_path)

        for filename in files:
            src_file = os.path.join(root, filename)
            if os.path.islink(src_file):
                continue

            dest_path = os.path.join(fs_path, filename).replace(os.sep, '/')

            with open(src_file, 'rb') as f:
                write_func(dest_path, f.read())

def create_littlefs_image(source_dir, output_image, block_size, block_count):
    from littlefs import LittleFS
    lfs = LittleFS(block_size=block_size, block_count=block_count, disk_version=0x00020000)

    def mkdir(path):
        lfs.mkdir(path)

    def write_file(path, data):
        with lfs.open(path, 'wb') as f:
            f.write(data)

    process_directory(source_dir, mkdir, write_file)

    with open(output_image, 'wb') as f:
        f.write(lfs.context.buffer)

# Read-only "rolfs:" image header. The runtime VFS (component/file_system/vfs/vfs.c)
# scans the app image for this signature and mounts the littlefs payload that
# immediately follows the 32-byte header, read-only. Header matches the SDK
# IMAGE_HEADER: u32 signature[2]; u32 image_size; u32 image_addr; u32 boot_index;
# u32 reserved[3] (0x20 bytes). Only signature and image_size are meaningful here;
# image_size is the littlefs payload length. No 4KB alignment: the header is packed
# right after the app (32B aligned) and the payload right after the header.
PATTERN_ROLFS_1 = 0x464c4f52   # "ROLF", must match vfs.h
PATTERN_ROLFS_2 = 0x53464C53   # "SLFS", must match vfs.h
ROLFS_IMAGE_HEADER_LEN = 0x20

def create_littlefs_image_minfit(source_dir, output_image, block_size):
    """Pack a directory into the SMALLEST littlefs image that holds it.

    A read-only "rolfs:" image never grows, so the usual free-space safety margin
    is pure waste of the (scarce) app-image slot. Start from a tight lower bound
    and grow the block count by 1 until littlefs accepts all files, then keep
    that minimum. Returns the chosen block_count.
    """
    total_size, dir_count, file_count = calculate_directory_stats(source_dir)
    # lower bound: data blocks + a little metadata (superblocks + per-entry).
    block_count = max(4, math.ceil(total_size / block_size) + dir_count + 2)
    while True:
        try:
            create_littlefs_image(source_dir, output_image, block_size, block_count)
            return block_count
        except Exception:
            block_count += 1
            if block_count > math.ceil(total_size / block_size) + file_count + dir_count + 64:
                raise  # something else is wrong; do not loop forever


# ---------------------------------------------------------------------------
# LBM physical NAND layout (folded in from the former lfs2lbm.py).
#
# LBM (Logical Block Mapping, component/file_system/vfs/lbm.c) expects every
# physical erase block (PEB) to carry two header pages:
#   page 0    : meta  header (erase count)          + 0xFF pad
#   page 1    : block header (which logical block)  + 0xFF pad
#   page 2..  : the logical block's data
# A raw littlefs image (linear logical blocks, no headers) is wrapped into this
# layout with identity mapping (logical block N -> physical block N).  Trailing
# blank blocks are trimmed (they stay erased on flash / are DIRTY-allocatable).
#
# The on-flash layout MUST stay byte-for-byte in sync with lbm.h
# (lbm_meta_hdr_t / lbm_blk_hdr_t, magics, CRC), and the reserved-block policy
# MUST match lbm_core.c lbm_init().
# ---------------------------------------------------------------------------
_LBM_META_HDR_MAGIC = 0x304D424C   # "LBM0"
_LBM_BLK_HDR_MAGIC = 0x314D424C    # "LBM1"
_LBM_HDR_VERSION = 1
_LBM_RESERVED_PCT = 2
_LBM_RESERVED_FLOOR = 2             # matches lbm_core.c: max(total*2/100, 2)


def _lbm_usable_blocks(partition_size, peb_size):
    """Fixed usable (logical) block count, exactly as lbm_core.c lbm_init():
        reserved = max(total*2/100, 2);  usable = total - reserved."""
    total = partition_size // peb_size
    reserved = total * _LBM_RESERVED_PCT // 100
    if reserved < _LBM_RESERVED_FLOOR:
        reserved = _LBM_RESERVED_FLOOR
    if total <= reserved:
        return 0, total, reserved
    return total - reserved, total, reserved


def _lbm_crc32(data):
    """CRC32 (reflected poly 0xEDB88320, init/xorout 0xFFFFFFFF) == lbm_crc32()."""
    crc = 0xFFFFFFFF
    for byte in data:
        crc ^= byte
        for _ in range(8):
            crc = (crc >> 1) ^ (0xEDB88320 & (-(crc & 1) & 0xFFFFFFFF))
    return crc ^ 0xFFFFFFFF


def _lbm_meta_hdr(ec):
    body = struct.pack("<IB3sQII", _LBM_META_HDR_MAGIC, _LBM_HDR_VERSION,
                       b"\xff\xff\xff", ec, 1, 2)   # blk_hdr_page=1, data_page=2
    return body + struct.pack("<I", _lbm_crc32(body))


def _lbm_blk_hdr(lblk, seq):
    body = struct.pack("<IBB2sIQI", _LBM_BLK_HDR_MAGIC, _LBM_HDR_VERSION,
                       0, b"\xff\xff", lblk, seq, 0xFFFFFFFF)   # copy_flag=0, data_crc unused
    return body + struct.pack("<I", _lbm_crc32(body))


def _lbm_page(data, page_size):
    if len(data) > page_size:
        raise ValueError("LBM header larger than page")
    return data + b"\xff" * (page_size - len(data))


def _lbm_wrap(logical, output_image, page_size, block_pages, partition_size):
    """Wrap a linear logical-block image into an LBM physical NAND image."""
    peb_size = page_size * block_pages
    lblk_size = page_size * (block_pages - 2)
    if len(logical) % lblk_size != 0:
        print(f"Error: image size {len(logical)} is not a multiple of lblk_size "
              f"{lblk_size} ({lblk_size // 1024} KB)")
        exit(1)
    block_count = len(logical) // lblk_size
    usable, total, reserved = _lbm_usable_blocks(partition_size, peb_size)
    if usable == 0:
        print(f"Error: partition too small (total={total} <= reserved={reserved}).")
        exit(1)
    if block_count != usable:
        print(f"Error: littlefs block_count {block_count} != device usable_blocks "
              f"{usable} (partition -> total={total}, reserved={reserved}).")
        exit(1)

    blank = b"\xff" * lblk_size
    last_used = -1
    for n in range(block_count):
        if logical[n * lblk_size:(n + 1) * lblk_size] != blank:
            last_used = n
    emit_count = last_used + 1   # trim trailing blank PEBs

    seq = 1
    with open(output_image, "wb") as out:
        for n in range(emit_count):
            data = logical[n * lblk_size:(n + 1) * lblk_size]
            if data == blank:
                out.write(b"\xff" * peb_size)         # free/allocatable PEB
                continue
            peb = _lbm_page(_lbm_meta_hdr(0), page_size)       # page 0
            peb += _lbm_page(_lbm_blk_hdr(n, seq), page_size)  # page 1
            peb += data                                        # pages 2..
            out.write(peb)
            seq += 1
    return lblk_size, usable, total, reserved, emit_count


def create_littlefs_nand_image(source_dir, output_image, size,
                               page_size=2048, block_pages=64):
    """Build a littlefs image AND wrap it into an LBM physical NAND image in one
    step (this folds in the former separate lfs2lbm.py).

    The littlefs image is built with the geometry LBM/littlefs expect on NAND:
        block_size  = (block_pages - 2) * page_size    (LBM usable data per PEB)
        block_count = LBM usable_blocks(partition size) (total - reserved)
    """
    peb_size = page_size * block_pages
    lblk_size = page_size * (block_pages - 2)
    usable, total, reserved = _lbm_usable_blocks(size, peb_size)
    if usable == 0:
        print(f"Error: partition too small (total={total} blocks <= reserved={reserved}).")
        exit(1)

    # 1) build the raw littlefs image (block_size/count matched to the device)
    tmp = output_image + ".lfs.tmp"
    try:
        create_littlefs_image(source_dir, tmp, lblk_size, usable)
        with open(tmp, "rb") as f:
            logical = f.read()
    finally:
        if os.path.exists(tmp):
            os.remove(tmp)

    # 2) wrap into the LBM physical NAND layout
    _lbm_wrap(logical, output_image, page_size, block_pages, size)
    return lblk_size, usable, total, reserved

def wrap_rolfs_image_header(image_path):
    """Prepend the 32-byte PATTERN_ROLFS header to an existing littlefs image."""
    with open(image_path, 'rb') as f:
        payload = f.read()
    header = struct.pack(
        "<8I",
        PATTERN_ROLFS_1, PATTERN_ROLFS_2,
        len(payload),     # image_size
        0, 0, 0, 0, 0,    # image_addr / boot_index / reserved[3]
    )
    assert len(header) == ROLFS_IMAGE_HEADER_LEN
    with open(image_path, 'wb') as f:
        f.write(header)
        f.write(payload)
    return len(payload)

class BlockDevice:
    """Block device wrapper for littlefs-python that supports slice operations"""
    def __init__(self, data):
        self.data = bytearray(data)

    def __getitem__(self, key):
        return self.data[key]

    def __setitem__(self, key, value):
        self.data[key] = value

def unpack_littlefs_image(input_image, output_dir, block_size=None):
    from littlefs import LittleFS
    with open(input_image, 'rb') as f:
        image_data = f.read()

    block_size = block_size or 4096
    block_count = len(image_data) // block_size

    # Create block device wrapper
    bd = BlockDevice(image_data)

    # Create LittleFS instance and mount
    lfs = LittleFS(block_size=block_size, block_count=block_count)
    lfs.context.buffer = bd

    try:
        lfs.mount()
    except Exception as e:
        print(f"Failed to mount littlefs image: {e}")
        raise

    # Read all files recursively
    def walk_lfs(path, prefix=''):
        try:
            entries = list(lfs.scandir(path))
        except Exception:
            return

        for entry in entries:
            # entry.name is the filename, entry.type is 1 for file, 2 for directory
            full_path = os.path.join(path, entry.name)
            try:
                # entry.type: 1 = file, 2 = directory
                if entry.type == 2:
                    os.makedirs(os.path.join(output_dir, prefix, entry.name), exist_ok=True)
                    walk_lfs(full_path, os.path.join(prefix, entry.name))
                else:
                    with lfs.open(full_path, 'rb') as f:
                        data = f.read()
                    rel_path = os.path.join(prefix, entry.name) if prefix else entry.name
                    out_file = os.path.join(output_dir, rel_path)
                    os.makedirs(os.path.dirname(out_file), exist_ok=True)
                    with open(out_file, 'wb') as of:
                        of.write(data)
            except Exception as e:
                print(f"Warning: Failed to read {full_path}: {e}")

    walk_lfs('/')
    print(f"Unpacked {input_image} -> {output_dir}")

def create_fatfs_image(source_dir, output_image, sector_size, sector_count):
    from pyfatfs.PyFat import PyFat
    from pyfatfs.PyFatFS import PyFatFS
    total_bytes = sector_size * sector_count
    fat_type = 12 if total_bytes <= 32*1024*1024 else 16 if total_bytes <= 2*1024*1024*1024 else 32

    with open(output_image, 'wb') as f:
        f.truncate(sector_size * sector_count)

    pf = PyFat()
    pf.mkfs(filename=output_image, fat_type=fat_type, sector_size=sector_size)
    pf.close()

    fat_fs = PyFatFS(output_image)

    def mkdir(path):
        fat_fs.makedirs(path)

    def write_file(path, data):
        with fat_fs.open(path, 'wb') as f:
            f.write(data)

    process_directory(source_dir, mkdir, write_file)

    fat_fs.close()

def unpack_fatfs_image(input_image, output_dir):
    from pyfatfs.PyFatFS import PyFatFS
    fat_fs = PyFatFS(input_image)

    # Use walk to traverse all files and directories
    # Note: dirs and files are lists of Info objects, not strings
    for root, dirs, files in fat_fs.walk('/'):
        # root is like '/', '/subdir', etc.
        # Convert to relative path for output
        rel_root = root.lstrip('/')

        # Create directories (dirs is list of Info objects)
        for d in dirs:
            dir_name = d.name
            out_dir = os.path.join(output_dir, rel_root, dir_name) if rel_root else os.path.join(output_dir, dir_name)
            os.makedirs(out_dir, exist_ok=True)

        # Copy files (files is list of Info objects)
        for f in files:
            file_name = f.name
            src_path = os.path.join(root, file_name).replace(os.sep, '/')
            rel_path = os.path.join(rel_root, file_name) if rel_root else file_name
            out_file = os.path.join(output_dir, rel_path)
            os.makedirs(os.path.dirname(out_file), exist_ok=True)

            try:
                data = fat_fs.getbytes(src_path)
                with open(out_file, 'wb') as of:
                    of.write(data)
            except Exception as e:
                print(f"Warning: Failed to read {src_path}: {e}")

    fat_fs.close()
    print(f"Unpacked {input_image} -> {output_dir}")

def dynamic_safety_factor(fs_type, avg_file_size_kb):
    if fs_type == "FATFS":
        # FATFS：1.2~1.8
        if avg_file_size_kb <= 0:
            return 1.8
        base_factor = 1.2
        dynamic_term = 0.6 * (1 / (avg_file_size_kb + 10))
        return max(1.2, min(base_factor + dynamic_term, 1.8))
    else:
        # LittleFS：1.1~1.5
        if avg_file_size_kb <= 0:
            return 1.5
        base_factor = 1.1
        dynamic_term = 0.4 * (1 / (avg_file_size_kb + 20))
        return max(1.1, min(base_factor + dynamic_term, 1.5))

# ---------------------------------------------------------------------------
# REALFS support (via the pip package `realfs-python`, imported lazily so that
# LITTLEFS/FATFS use does not require it to be installed).
# ---------------------------------------------------------------------------
def _realfs_human_size(s):
    """Parse '2M' / '512K' / '0x200000' / '2097152' -> int bytes."""
    s = str(s).strip()
    mult = 1
    if s and s[-1] in "kKmMgG":
        mult = {"k": 1024, "m": 1024 ** 2, "g": 1024 ** 3}[s[-1].lower()]
        s = s[:-1]
    return int(s, 0) * mult


def _realfs_import():
    try:
        import realfs
        return realfs
    except ImportError:
        print("Error: the 'realfs-python' package is required for REALFS images.\n"
              "       Install it with:  pip install realfs-python")
        exit(1)


def _realfs_collect(source_dir):
    """Read every file under source_dir into {realfs_path: bytes}."""
    files = {}
    for root, _dirs, fs in os.walk(source_dir):
        for name in fs:
            host = os.path.join(root, name)
            if os.path.islink(host):
                continue
            rel = os.path.relpath(host, source_dir).replace(os.sep, "/")
            with open(host, "rb") as f:
                files[rel] = f.read()
    return files


def create_realfs_image(source_dir, output_image, mode, size,
                        page_size=2048, block_pages=64, buckets=0, full=False):
    """Build a REALFS image. `mode` = nor-direct | nor-512 | nand."""
    realfs = _realfs_import()
    files = _realfs_collect(source_dir)
    sz = _realfs_human_size(size)
    if mode == "nor-direct":
        img = realfs.nor_direct_image(sz, files, buckets=buckets)
    elif mode == "nor-512":
        img = realfs.nor512_image(sz, files, buckets=buckets)
    elif mode == "nand":
        img = realfs.nand_image(sz, files, page_size=page_size,
                                block_pages=block_pages, buckets=buckets, full=full)
    else:
        print(f"Error: unknown REALFS mode '{mode}'")
        exit(1)
    with open(output_image, "wb") as f:
        f.write(img)
    return img, sz


def unpack_realfs_image(input_image, output_dir, mode, size,
                        page_size=2048, block_pages=64):
    realfs = _realfs_import()
    with open(input_image, "rb") as f:
        img = f.read()
    sz = _realfs_human_size(size) if size else 0
    if mode == "nor-direct":
        listing = realfs.nor_direct_list(img)
        def rd(p): return realfs.nor_direct_read(img, p)
    elif mode == "nor-512":
        listing = realfs.nor512_list(img)
        def rd(p): return realfs.nor512_read(img, p)
    elif mode == "nand":
        listing = realfs.nand_list(img, sz, page_size=page_size, block_pages=block_pages)
        def rd(p): return realfs.nand_read(img, p, sz, page_size=page_size, block_pages=block_pages)
    else:
        print(f"Error: unknown REALFS mode '{mode}'")
        exit(1)
    for path, _size in listing:
        data = rd(path)
        out_file = os.path.join(output_dir, path)
        os.makedirs(os.path.dirname(out_file) or ".", exist_ok=True)
        with open(out_file, "wb") as f:
            f.write(data)
    print(f"Unpacked {input_image} -> {output_dir}")


def main():
    parser = argparse.ArgumentParser(description="VFS Bin File Generation")
    parser.add_argument("-t", "--type",
                        choices=["LITTLEFS", "FATFS", "REALFS"],
                        default="LITTLEFS",
                        help="Filesystem type (default: LITTLEFS)")
    parser.add_argument("-s", "--block_size",
                        type=int,
                        help="Block/sector size (default: LITTLEFS=4096, FATFS=512)")
    parser.add_argument("-c", "--block_count",
                        type=int,
                        help="Block/sector count (default: LITTLEFS=32, FATFS=256)")
    parser.add_argument("-dir", "--source_directory",
                        type=str,
                        required=True,
                        help="Source directory path")
    parser.add_argument("-out", "--output_image",
                        type=str,
                        default="vfs.bin",
                        help="Output image file (default: vfs.bin)")
    parser.add_argument("-u", "--unpack",
                        type=str,
                        help="Unpack image to specified directory")
    parser.add_argument("--rolfs-header",
                        action="store_true",
                        help="Prepend a 32-byte PATTERN_ROLFS IMAGE_HEADER so the runtime "
                             "VFS can locate this image inside the app image and mount "
                             "it read-only at \"rolfs:\" (LITTLEFS only)")

    # ---- littlefs target: NOR (default, uses -s/-c) or NAND (LBM-wrapped) ----
    parser.add_argument("--littlefs-target",
                        choices=["nor", "nand"],
                        default="nor",
                        help="LITTLEFS storage target: 'nor' (default, uses -s/-c) or "
                             "'nand' (wrap into an LBM physical image in one step; "
                             "uses --size/--page-size/--block-pages, ignores -s/-c)")

    # ---- REALFS options (used when --type REALFS) ----
    parser.add_argument("--realfs-mode",
                        choices=["nor-direct", "nor-512", "nand"],
                        default="nor-direct",
                        help="REALFS backend layout (default: nor-direct)")

    # ---- geometry shared by REALFS and LITTLEFS-on-NAND ----
    parser.add_argument("--size", type=str,
                        help="VFS partition size, e.g. 2M / 512K / 0x200000 "
                             "(required for REALFS and for LITTLEFS --littlefs-target nand; "
                             "must match the firmware partition)")
    parser.add_argument("--page-size", type=int, default=2048,
                        help="NAND page size (default: 2048)")
    parser.add_argument("--block-pages", type=int, default=64,
                        help="NAND pages per erase block (default: 64)")
    parser.add_argument("--buckets", type=int, default=0,
                        help="REALFS hash-bucket count (0 = auto, default)")
    parser.add_argument("--full", action="store_true",
                        help="REALFS nand: emit all usable blocks (no trailing-blank trim)")

    args = parser.parse_args()

    # Handle unpack mode
    if args.unpack:
        if not os.path.exists(args.source_directory):
            print(f"Error: Input image '{args.source_directory}' does not exist.")
            exit(1)

        os.makedirs(args.unpack, exist_ok=True)

        if args.type == "REALFS":
            if args.realfs_mode == "nand" and not args.size:
                print("Error: REALFS nand unpack needs --size (partition size).")
                exit(1)
            try:
                unpack_realfs_image(args.source_directory, args.unpack, args.realfs_mode,
                                    args.size, args.page_size, args.block_pages)
                print(f"Unpack completed: {args.source_directory} -> {args.unpack}")
                return
            except Exception as e:
                print(f"Failed to unpack REALFS image: {e}")
                exit(1)

        if args.type == "LITTLEFS":
            try:
                unpack_littlefs_image(args.source_directory, args.unpack, args.block_size or 4096)
                print(f"Unpack completed: {args.source_directory} -> {args.unpack}")
                return
            except Exception as e:
                print(f"Failed to unpack LittleFS image: {e}")
                exit(1)
        else:  # FATFS
            try:
                unpack_fatfs_image(args.source_directory, args.unpack)
                print(f"Unpack completed: {args.source_directory} -> {args.unpack}")
                return
            except Exception as e:
                print(f"Failed to unpack FATFS image: {e}")
                exit(1)

    if not os.path.exists(args.source_directory):
        print(f"Source directory '{args.source_directory}' does not exist.")
        exit(1)

    # REALFS uses its own geometry model (--size + mode), not block_size/count;
    # handle it here and return before the LITTLEFS/FATFS sizing logic below.
    if args.type == "REALFS":
        if not args.size:
            print("Error: REALFS needs --size (partition size), e.g. --size 2M "
                  "(must match the firmware VFS partition).")
            exit(1)
        img, sz = create_realfs_image(args.source_directory, args.output_image,
                                      args.realfs_mode, args.size,
                                      page_size=args.page_size, block_pages=args.block_pages,
                                      buckets=args.buckets, full=args.full)
        print(f"{args.output_image} has been successfully generated.")
        print("args:")
        print(f"├─ type: REALFS")
        print(f"├─ mode: {args.realfs_mode}")
        print(f"├─ partition_size: {sz}")
        if args.realfs_mode == "nand":
            print(f"├─ page_size: {args.page_size}")
            print(f"├─ block_pages: {args.block_pages}")
        print(f"├─ image_size: {len(img)}")
        print(f"├─ source_directory: {args.source_directory}")
        print(f"└─ output_image: {args.output_image}")
        return

    # LITTLEFS on NAND: build littlefs + wrap into LBM physical image in one step
    # (folds lfs2lbm.py).  Uses --size/--page-size/--block-pages, not -s/-c.
    if args.type == "LITTLEFS" and args.littlefs_target == "nand":
        if not args.size:
            print("Error: LITTLEFS --littlefs-target nand needs --size (partition size), "
                  "e.g. --size 32M (must match the firmware VFS partition).")
            exit(1)
        sz = _realfs_human_size(args.size)
        lblk_size, usable, total, reserved = create_littlefs_nand_image(
            args.source_directory, args.output_image, sz,
            page_size=args.page_size, block_pages=args.block_pages)
        print(f"{args.output_image} has been successfully generated.")
        print("args:")
        print(f"├─ type: LITTLEFS (target: nand / LBM)")
        print(f"├─ partition_size: {sz}")
        print(f"├─ page_size: {args.page_size}")
        print(f"├─ block_pages: {args.block_pages}")
        print(f"├─ littlefs block_size: {lblk_size}  block_count(usable): {usable}")
        print(f"├─ lbm blocks: total={total} reserved={reserved}")
        print(f"├─ source_directory: {args.source_directory}")
        print(f"└─ output_image: {args.output_image}")
        return

    if args.type == "LITTLEFS":
        block_size = args.block_size or 4096
        block_count = args.block_count or 32
    else:
        block_size = args.block_size or 512
        block_count = args.block_count or 256

    # A read-only rolfs image is packed to the minimum below (no safety margin),
    # so skip the free-space sizing estimate that the read/write images use.
    rolfs_fit = args.rolfs_header and args.block_count is None

    if not rolfs_fit:
        # Calculate storage requirements
        total_size, dir_count, file_count = calculate_directory_stats(args.source_directory)
        avg_file_size_kb = total_size / file_count / 1024 if file_count > 0 else 0

        safety_factor = dynamic_safety_factor(args.type, avg_file_size_kb)

        # Metadata calculation
        if args.type == "LITTLEFS":
            meta_per_file = block_size
            meta_per_dir = block_size
        else:
            dir_entry_size = 32
            entries_per_block = block_size // dir_entry_size
            long_name_factor = 1.5  # for LFN
            meta_per_file = (block_size / entries_per_block) * long_name_factor
            meta_per_dir = block_size

        meta_size = (file_count * meta_per_file + dir_count * meta_per_dir)
        total_required = (total_size + meta_size) * safety_factor
        required_blocks = math.ceil(total_required / block_size)

        if required_blocks > block_count:
            print(f"**********************************************")
            print(f"Warning: Adjusting block count from {block_count} to {required_blocks}")
            print(f"**********************************************")
            block_count = required_blocks

    if args.rolfs_header and args.type != "LITTLEFS":
        print("Error: --rolfs-header is only supported for LITTLEFS.")
        exit(1)

    if args.type == "LITTLEFS":
        if args.rolfs_header and args.block_count is None:
            # read-only image: pack to the minimum, no free-space margin
            block_count = create_littlefs_image_minfit(
                args.source_directory, args.output_image, block_size)
        else:
            create_littlefs_image(args.source_directory, args.output_image,
                           block_size, block_count)
    else:
        create_fatfs_image(args.source_directory, args.output_image,
                    block_size, block_count)

    if args.rolfs_header:
        wrap_rolfs_image_header(args.output_image)

    print(f"{args.output_image} has been successfully generated.")
    print("args:")
    print(f"├─ type: {args.type}")
    if args.type == "LITTLEFS":
        print(f"├─ block_size: {block_size}")
        print(f"├─ block_count: {block_count}")
    else:
        print(f"├─ sector_size: {block_size}")
        print(f"├─ sector_count: {block_count}")
    print(f"├─ image_size: {block_size * block_count}")
    if args.rolfs_header:
        print(f"├─ rolfs_header: 32-byte PATTERN_ROLFS prepended")
    print(f"├─ source_directory: {args.source_directory}")
    print(f"└─ output_image: {args.output_image}")


if __name__ == "__main__":
    main()
