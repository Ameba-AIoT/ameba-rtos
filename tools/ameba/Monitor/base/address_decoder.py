#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0


import fnmatch
import os.path
from typing import Optional, Union
import re
import subprocess
from elftools.elf.constants import SH_FLAGS
from elftools.elf.elffile import ELFFile
from .color_output import print_red, print_yellow

# regex matches a potential address
ADDRESS_RE = re.compile(r'0x[0-9a-f]{8}', re.IGNORECASE)


class AddressDecoder:
    """
    Class for decoding possible addresses
    """

    def __init__(self, toolchain_path: str, elf_file: str, rom_file: Optional[str] = None) -> None:
        """
        :param toolchain_path: the path to the toolchain
        :param elf_file: the path to the ELF file
        :param rom_file: the path to the ROM ELF file
        """
        self.toolchain_path = toolchain_path
        self.elf_file = elf_file
        self.rom_file = rom_file
        self.address_buffer = ""
        self.address_matcher = AddressMatcher(elf_file)
        if rom_file is not None:
            self.rom_address_matcher = AddressMatcher(rom_file)
        self.addr2line = None

    def _get_addr2line(self):
        """
        Get the path to the addr2line tool.
        If the toolchain path is not None, it will look for the addr2line tool in the bin directory.
        """
        if self.addr2line is None:
            if os.path.exists(self.toolchain_path):
                bin_dir = os.path.join(self.toolchain_path, "bin")
                if os.path.exists(bin_dir):
                    for file in os.listdir(bin_dir):
                        if fnmatch.fnmatch(file, "*addr2line.exe"):
                            self.addr2line = os.path.join(bin_dir, file)
                            return
                else:
                    print_yellow(f"[Warning]Not exists {bin_dir}!")
            print_yellow(f"[Warning]Can not find addr2line tool!")

    def decode_address(self, line: str) -> str:
        """
        Decode the address in the line.
        """
        line = self.address_buffer + line  # because the address may be cut in half
        self.address_buffer = ""
        out = ""
        for match in re.finditer(ADDRESS_RE, line):
            num = match.group()
            address_int = int(num, 16)
            translation = None

            # Try looking for the address in the app ELF file
            if self.address_matcher.is_executable_address(address_int):
                translation = self.lookup_address(num)
            # Not found in app ELF file, check ROM ELF file (if it is available)
            if translation is None and self.rom_file is not None and \
                    self.rom_address_matcher.is_executable_address(address_int):
                translation = self.lookup_address(num, is_rom=True)
            # Translation found either in the app or ROM ELF file
            if translation is not None:
                out += translation
            else:
                pass
        return out

    def lookup_address(self, addr: str, is_rom: bool = False) -> Union[str, None]:
        """
        Lookup the address in the ELF file.
        """
        self._get_addr2line()
        if self.addr2line:
            elf_file: str = self.rom_file if is_rom else self.elf_file  # type: ignore
            cmd = [self.addr2line, '-pfiaC', '-e', elf_file, addr]
            try:
                translation = subprocess.check_output(cmd, cwd='.')
                if b'?? ??:0' not in translation:
                    decoded = translation.decode()
                    return decoded if not is_rom else decoded.replace('at ??:?', 'in ROM')
            except OSError as err:
                print_red(f'{" ".join(cmd)}: {err}')
        return None


class AddressMatcher:
    """
    Class for detecting potential addresses which will consequently run through the external addr2line command to
    identify and print information about it.

    The input to this class is the path to the ELF file. Addresses of sections with executable flag are stored and
    used later for lookup.
    """

    def __init__(self, elf_path: Union[str, None] = None):
        """
        :param elf_path: the path to the ELF file
        """
        self.intervals = []
        if elf_path and os.path.exists(elf_path):
            with open(elf_path, 'rb') as file:
                elf = ELFFile(file)

                for section in elf.iter_sections():
                    if section['sh_flags'] & SH_FLAGS.SHF_EXECINSTR:  # check executable flag
                        start = section['sh_addr']
                        size = section['sh_size']
                        end = start + size
                        self.intervals.append((start, end))

        self.intervals = sorted(self.intervals)

    def is_executable_address(self, addr: int) -> bool:
        """
        check if the address is in executable section

        :param addr: the address to check
        :return: True if the address is in executable section, False otherwise
        """
        for start, end in self.intervals:
            if start > addr:
                return False
            if start <= addr < end:
                return True

        return False
