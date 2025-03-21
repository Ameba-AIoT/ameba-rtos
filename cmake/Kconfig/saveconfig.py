#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import argparse
import kconfiglib

def save_config(kconf, output_path):
    with open(output_path, "w") as f:
        for sym in kconf.unique_defined_syms:
            if sym.visibility:
                # BOOL TRISTATE
                if sym.type in (kconfiglib.BOOL, kconfiglib.TRISTATE):
                    tri_value = sym.tri_value
                    value = {0: "n", 1: "m", 2: "y"}[tri_value]
                    f.write(f"CONFIG_{sym.name}={value}\n")
                # STR INT
                else:
                    f.write(f"CONFIG_{sym.name}={sym.str_value}\n")

def main():
    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawDescriptionHelpFormatter,
        description=__doc__)

    parser.add_argument(
        "--kconfig",
        default="Kconfig",
        help="Top-level Kconfig file (default: Kconfig)")

    parser.add_argument(
        "--out",
        default="save.conf",
        help="Output filename for saved config)")

    args = parser.parse_args()

    kconf = kconfiglib.Kconfig(args.kconfig, suppress_traceback=True)
    print(kconf.load_config())

    save_config(kconf, args.out)



if __name__ == "__main__":
    main()