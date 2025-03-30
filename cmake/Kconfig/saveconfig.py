#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import argparse
import kconfiglib
import sys

#modify _str_default in kconfiglib, only consider default
def _str_default(sym):
    # write_min_config() helper function. Returns the value the symbol
    # would get from defaults if it didn't have a user value. Uses exactly
    # the same algorithm as the C implementation (though a bit cleaned up),
    # for compatibility.

    if sym.orig_type in kconfiglib._BOOL_TRISTATE:
        val = 0

        # Defaults do not affect choice symbols
        if not sym.choice:
            for default, cond in sym.defaults:
                cond_val = kconfiglib.expr_value(cond)
                if cond_val:
                    val = min(kconfiglib.expr_value(default), cond_val)
                    break

            # Transpose mod to yes if type is bool (possibly due to modules
            # being disabled)
            if val == 1 and sym.type is BOOL:
                val = 2

        return kconfiglib.TRI_TO_STR[val]

    if sym.orig_type:  # STRING/INT/HEX
        for default, cond in sym.defaults:
            if kconfiglib.expr_value(cond):
                return default.str_value

    return ""

def save_config(kconf, output_path, default_conf=None):

    default_dic = {}

    if default_conf:
        with open(default_conf, "r") as f:
            for line in f:
                stripped_line = line.strip()
                if stripped_line.startswith("CONFIG_"): #strip CONFIG_
                    stripped_line = stripped_line[len("CONFIG_"):]
                if stripped_line.startswith("#"):       #ignore comment
                    continue
                if stripped_line:
                    if "=" not in stripped_line:        #check default.conf format
                        sys.exit("default.conf error: no '=' in assignment: '{}'".format(arg))
                    name, value = stripped_line.split("=", 1)

                    if name not in kconf.syms:          #check sym exists
                        if not stripped_line.check_exists:
                            continue
                        sys.exit("default.conf error: no symbol '{}' in configuration".format(name))

                    sym = kconf.syms[name]              #save default value from default.conf in dic
                    if sym.choice:
                        default_dic[sym.choice.name_and_loc] = sym
                    else:
                        default_dic[name] = value


    chunks = []  # "".join()ed later
    add = chunks.append

    for sym in kconf.unique_defined_syms:
        # Skip symbols that cannot be changed. Only check
        # non-choice symbols, as selects don't affect choice
        # symbols.
        if not sym.choice and \
            sym.visibility < kconfiglib.expr_value(sym.rev_dep):
            continue

        # Skip symbols whose value matches their default
        if sym.name in default_dic:
            d_value = default_dic[sym.name]
        else:
            d_value = _str_default(sym)


        if sym.str_value == d_value:
            continue

        # Skip symbols that would be selected by default in a
        # choice, unless the choice is optional or the symbol type
        # isn't bool (it might be possible to set the choice mode
        # to n or the symbol to m in those cases).

        if sym.choice:
            if sym.choice.name_and_loc in default_dic:
                d_sym = default_dic[sym.choice.name_and_loc]
            else:
                d_sym = sym.choice._selection_from_defaults()

        if sym.choice and \
            not sym.choice.is_optional and \
            d_sym is sym and \
            sym.orig_type is kconfiglib.BOOL and \
            sym.tri_value == 2:
            continue

        # BOOL TRISTATE
        if sym.type in (kconfiglib.BOOL, kconfiglib.TRISTATE):
            tri_value = sym.tri_value
            value = {0: "n", 1: "m", 2: "y"}[tri_value]
            add(f"CONFIG_{sym.name}={value}\n")
        # STR INT
        else:
            add(f"CONFIG_{sym.name}={sym.str_value}\n")

    with open(output_path, "w") as f:
        f.write(f"# This file is generated automatically by : python menuconfig.py -s {output_path}\r\n")
        f.write("".join(chunks))


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

    parser.add_argument(
        "--default",
        default=None,
        help="Output filename for saved config)")

    args = parser.parse_args()

    kconf = kconfiglib.Kconfig(args.kconfig, suppress_traceback=True)
    print(kconf.load_config())

    save_config(kconf, args.out, args.default)



if __name__ == "__main__":
    main()