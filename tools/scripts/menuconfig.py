#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import json
import os
import re
import sys
import argparse
import shutil
from configparser import ConfigParser

script_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(script_dir, '../../cmake'))
sys.path.append(os.path.join(script_dir, '../../cmake/Kconfig'))
from Kconfig.manager import Manager
import kconfiglib

def main():

    parser = argparse.ArgumentParser(description=None)

    parser.add_argument(
        '-proj',
        '--project-dir',
        required=True,
        help='project directory'
    )

    parser.add_argument(
        "-f",
        "--file",
        nargs='+',
        help="apply extra config file"
    )

    parser.add_argument(
        "-r",
        "--reset",
        action='store_true',
        help="generate default config file"
    )

    parser.add_argument(
        "-c",
        "--clean",
        action='store_true',
        help="cleanup menuconfig and build products"
    )

    parser.add_argument(
        "-d",
        "--menuconfig-dir",
        help=argparse.SUPPRESS # directory where menuconfig floader will be saved
    )

    parser.add_argument(
        "-s",
        "--save",
        help="save current config as a file"
    )

    parser.add_argument(
        "--check",
        action='store_true',
        help=argparse.SUPPRESS #check Kconfig update
    )

    parser.add_argument(
        "-e",
        "--external-kconfig",
        help=argparse.SUPPRESS #an external sub-kconfig under this path will be included
    )

    parser.add_argument(
        "--get",
        nargs='+',
        metavar='SYMBOL',
        help="get current value(s) of config symbols (JSON output, CONFIG_ prefix optional)"
    )

    parser.add_argument(
        "--set",
        nargs='+',
        metavar='SYMBOL=VALUE',
        help="set config symbol values and regenerate per-core headers (JSON output, strict)"
    )

    parser.add_argument(
        "--search",
        metavar='PATTERN',
        help="search symbols by name/prompt, case-insensitive regex (JSON output)"
    )

    parser.add_argument(
        "--apply-file",
        nargs='+',
        metavar='FILE',
        help="apply config file(s) like prj.conf (reset .config to default.conf + files, JSON output)"
    )

    parser.add_argument(
        "--save-file",
        metavar='FILE',
        help="save current config to a prj.conf-style file (diff vs default.conf, JSON output)"
    )

    args = parser.parse_args()

    project_dir = os.path.abspath(args.project_dir)
    if not project_dir or not os.path.isdir(project_dir):
        print(f"\033[31mError: '{project_dir}' is not a valid project.\033[0m")
        sys.exit(1)
    if not os.path.exists(os.path.join(project_dir, 'Kconfig')):
        print(f"\033[31mError: Kconfig file not found in {project_dir}\033[0m")
        sys.exit(1)

    if args.external_kconfig:
        os.environ['RTK_APP_DIR'] = args.external_kconfig

    if args.menuconfig_dir:
        menuconfigdir = os.path.abspath(args.menuconfig_dir)
    else:
        menuconfigdir = project_dir

    os.environ['KCONFIG_CONFIG']= os.path.join(menuconfigdir, 'menuconfig/.config')


    manager = Manager(
        out_dir = menuconfigdir,
        top_kconfig = os.path.join(project_dir, 'Kconfig')
    )

    top_kconfig = os.path.join(project_dir, 'Kconfig')

    if args.get:
        if not os.path.exists(manager.config_in):
            print(json.dumps({"success": False, "error": ".config not found — run menuconfig first"}))
            sys.exit(1)
        kconf = kconfiglib.Kconfig(top_kconfig, suppress_traceback=True)
        kconf.load_config(manager.config_in)
        values = {}
        for raw in args.get:
            name = raw.removeprefix('CONFIG_')
            values[f'CONFIG_{name}'] = kconf.syms[name].str_value if name in kconf.syms else None
        print(json.dumps({"success": True, "values": values}))
        return

    if args.search:
        kconf = kconfiglib.Kconfig(top_kconfig, suppress_traceback=True)
        if os.path.exists(manager.config_in):
            kconf.load_config(manager.config_in)
        pat = re.compile(args.search, re.IGNORECASE)
        matches = []
        for name, sym in kconf.syms.items():
            prompt = next((n.prompt[0] for n in sym.nodes if n.prompt), '')
            if pat.search(name) or pat.search(prompt):
                matches.append({
                    "name": f'CONFIG_{name}',
                    "type": kconfiglib.TYPE_TO_STR[sym.orig_type],
                    "prompt": prompt,
                    "value": sym.str_value,
                })
        print(json.dumps({"success": True, "matches": matches}))
        return

    if args.set:
        if not os.path.exists(manager.config_in):
            print(json.dumps({"success": False, "error": ".config not found — run menuconfig first"}))
            sys.exit(1)
        kconf = kconfiglib.Kconfig(top_kconfig, suppress_traceback=True)
        kconf.load_config(manager.config_in)
        errors = []
        applied = {}
        for assignment in args.set:
            if '=' not in assignment:
                errors.append({"symbol": assignment, "error": "missing '='"})
                continue
            raw_name, value = assignment.split('=', 1)
            name = raw_name.removeprefix('CONFIG_')
            if name not in kconf.syms:
                errors.append({"symbol": f'CONFIG_{name}', "error": "symbol not found"})
                continue
            sym = kconf.syms[name]
            if not sym.set_value(value):
                errors.append({"symbol": f'CONFIG_{name}',
                               "error": f"invalid value {value!r} for {kconfiglib.TYPE_TO_STR[sym.orig_type]} symbol"})
                continue
            if sym.str_value != value:
                errors.append({"symbol": f'CONFIG_{name}',
                               "error": f"dependency not satisfied: tried {value!r}, got {sym.str_value!r}"})
                continue
            applied[f'CONFIG_{name}'] = value
        if errors:
            print(json.dumps({"success": False, "errors": errors, "applied": applied}))
            sys.exit(1)
        kconf.write_config(manager.config_in)
        if manager.check_invalid_config():
            print(json.dumps({"success": False, "error": "invalid config detected after set"}))
            sys.exit(1)
        manager.parse_general_config()
        manager.process_projects()
        print(json.dumps({"success": True, "applied": applied}))
        return

    if args.apply_file:
        missing = [f for f in args.apply_file if not os.path.isfile(f)]
        if missing:
            print(json.dumps({"success": False, "error": "config file(s) not found", "missing": missing}))
            sys.exit(1)
        files = [os.path.abspath(f) for f in args.apply_file]
        if manager.apply_files_config(files) != 0:
            print(json.dumps({"success": False, "error": "apply config failed", "applied_files": files}))
            sys.exit(1)
        if manager.check_invalid_config():
            print(json.dumps({"success": False, "error": "invalid config detected after apply"}))
            sys.exit(1)
        manager.parse_general_config()
        manager.process_projects()
        print(json.dumps({"success": True, "applied_files": files, "config_path": manager.config_in}))
        return

    if args.save_file:
        if not os.path.exists(manager.config_in):
            print(json.dumps({"success": False, "error": ".config not found — run menuconfig first"}))
            sys.exit(1)
        out_path = os.path.abspath(args.save_file)
        parent = os.path.dirname(out_path)
        if parent and not os.path.isdir(parent):
            print(json.dumps({"success": False, "error": "output directory not found", "saved_file": out_path}))
            sys.exit(1)
        if manager.save_confs(out_path) != 0:
            print(json.dumps({"success": False, "error": "save config failed", "saved_file": out_path}))
            sys.exit(1)
        print(json.dumps({"success": True, "saved_file": out_path}))
        return

    if args.clean:
        manager.clean_all(script_dir, project_dir)
        return

    if args.file:
        code = manager.apply_files_config(args.file)
    elif args.reset:
        code = manager.apply_default_config()
    elif args.save:
        code = manager.save_confs(args.save)
    elif args.check:
        code = manager.apply_check_config()
    else:
        code = manager.apply_manual_config()

    if code != 0:
        print("apply config failed")
        sys.exit(1)

    if manager.check_invalid_config():
        sys.exit(1)
    manager.parse_general_config()
    manager.process_projects()

if __name__ == '__main__':
    main()

