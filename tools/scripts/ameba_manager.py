# -*- coding: utf-8 -*-

# Copyright (c) 2025 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import os
import sys
import shutil
from typing import List, Dict, Callable, Any, Optional
from enum import IntEnum

from ameba_soc_utils import *

__all__ = ['AmebaManager']

class AmebaManager:
    def __init__(self):
        self.is_ready = False
        self.current_dir = os.getcwd()
        self.build_dir = os.path.join(self.current_dir, "build")
        self.utils = SocManager()
        self.info_file = self.utils.info_file
        self.sdk_root = self.utils.sdk_root

        if not self.sdk_root or not os.path.isdir(self.sdk_root):
            print(f"{RED}ERROR: Run 'source <sdk>/env.sh' or 'env.bat' to set environment variables first.{RESET}", file=sys.stderr)
            return

        self.valid_socs = self.utils.get_available_socs()
        self.valid_socs_lower = [name.lower() for name in self.valid_socs]

        self.command_table: Dict[str, Dict[str, Any]] = {
            'build': {
                'action': self.op_build,
                'help': 'Build current project. e.g. ameba.py buid [soc_name] [args]'
             },
            'menuconfig': {
                'action': self.op_menuconfig,
                'help': 'Run menuconfig tool for current SoC. e.g. ameba.py menuconfig [args]'
            },
            'flash': {
                'action': self.op_flash,
                'help': 'Firmware flashing.'
            },
            'monitor': {
                'action': self.op_monitor,
                'help': 'Serial monitor.'
            },
            'new-project': {
                'action': self.create_new_project,
                'help': 'Create a new empty project in the given path. e.g. ameba.py new-project path/to/your/project [-a app_name]'
            },
            'soc': {
                'action': lambda args: self.op_set_project(args[0]) if len(args) > 0 else self.op_select_interactive(),
                'help': 'Select a SoC to build. If no SoC name given, enter the interactive selection.'
            },
            'list': {
                'action': lambda args: self.list_socs(),
                'help': 'List all available SoC names.'
            },
            'show': {
                'action': lambda args: self.show_current_soc(),
                'help': 'Show the current selected SoC name.'
            },
            'clean': {
                'action': lambda args: self.op_clean_build(),
                'help': 'Clean build products of current project.'
            },
            'cleanall': {
                'action': lambda args: self.op_clean_soc(),
                'help': 'Clean the current SoC info and products.'
            },
            'help': {
                'action': lambda args: self.show_help(),
                'help': 'Display this help information.'
            }
        }

        self.is_ready = True

    def _check_ready(self) -> bool:
        if not self.is_ready:
            print(f"{RED}ERROR: AmebaManager initialization failed, invalid operation.{RESET}", file=sys.stderr)
        return self.is_ready

    def get_command_handler(self, name: str) -> Callable:
        if not self.check_command_permission(name):
            return None

        command_info = self.command_table.get(name)
        if command_info:
            handler = command_info['action']
            if handler: return handler

        print(f"{RED}ERROR: Unknown command '{name}'{RESET}\n")
        self.show_help()
        return None

    def show_help(self) -> None:
        print("Ameba CLI Development Tool")
        print("Usage: ameba.py [COMMAND] [ARGS]\n")
        print("Available commands:")

        if not self.command_table:
            return
        max_len = max(len(cmd) for cmd in self.command_table) + 2

        for name, info in sorted(self.command_table.items()):
            print(f"  {name.ljust(max_len)}: {info['help']}")
        print()
        print("Run 'ameba.py <COMMAND> -h' for more information of command build/menuconfig/flash/monitor.")

    def show_current_soc(self) -> bool:
        if not self._check_ready(): return False

        if os.path.exists(self.info_file):
            soc_info = self.utils.parse_soc_info()
            if soc_info and soc_info['dir']:
                print(f"Current SoC: {GREEN}{soc_info['name']}{RESET}")
                return True
            else:
                print(f"{YELLOW}WARNING: The saved SoC name in '{self.info_file}' is invalid, or its SDK directory no longer exists.{RESET}")
                return False
        else:
            print("SoC is not set yet.")
            print(f"Run 'ameba.py soc [soc_name]' to select one.")
            return False

    def list_socs(self) -> None:
        if not self._check_ready(): return

        soc_names = self.valid_socs
        if not soc_names:
            print("No available SoC.")
            return

        print("Available SoCs:")
        for soc_name in soc_names:
            print(f"  - {GREEN}{soc_name}{RESET}")

    def create_new_project(self, args: List) -> bool:
        if not self._check_ready(): return False
        if not args:
            print(f"{RED}ERROR: The path of the new project must be given.{RESET}")
            return False
        proj_dir = os.path.abspath(args[0])

        dir_type = self.check_env(proj_dir)
        if dir_type <= 2:
            print(f"{RED}ERROR: Creating new projects within the SDK or an existing project is not allowed.{RESET}")
            return False

        op_type = 0
        if len(args) > 1 and args[1] in ["app", "-a"]:
            if len(args) == 2:
                print(f"{YELLOW}WARNING: Expect an app name, but not received.{RESET}")
            else:
                app_name = args[2]
                copy_args = [proj_dir, "--app", app_name]
                build_args = ["--new", proj_dir, "-a", app_name]
                submodule_info = os.path.join(self.build_dir, 'submodule_info.json')
                if app_name == "list-apps":
                    op_type = 2
                else:
                    op_type = 1

        script_dir = os.path.join(self.sdk_root, 'tools/scripts')

        if op_type == 2:
            if os.path.exists(submodule_info):
                run_script(script_dir, "build_copy.py", copy_args)
            else:
                self.op_build(build_args)
            return True

        if os.path.exists(proj_dir):
            try:
                prompt_message = (
                    f"{YELLOW}WARNING: '{proj_dir}' already exists, it will be removed first.{RESET}\n"
                    f"{YELLOW}Press [Enter] to continue, or [Ctrl+C] to cancel:{RESET} "
                )
                choice = input(prompt_message)
            except (KeyboardInterrupt, EOFError):
                print("\nOperation cancelled.")
                return False
            if choice != '':
                print("Operation cancelled.")
                return False
            else:
                print(f"Clean up the directory '{proj_dir}' ...")

        if op_type == 0:
            if run_script(script_dir, "build_copy.py", [proj_dir]):
                print(f"A new project is created in '{proj_dir}'.{RESET}")
                return True

        if op_type == 1:
            if not run_script(script_dir, "build_copy.py", copy_args):
                if not os.path.exists(submodule_info):
                    self.op_build(build_args)

            if os.path.isdir(os.path.join(proj_dir, app_name)):
                print(f"A new project based on app '{app_name}' is created in '{proj_dir}'.{RESET}")
                return True
            else:
                print(f"{RED}ERROR: Invalid app name: '{app_name}'.{RESET}", file=sys.stderr)
                return False

        return False

    def op_set_project(self, soc_name: str) -> bool:
        if not self._check_ready(): return False

        if soc_name.lower() not in self.valid_socs_lower:
            print(f"{RED}ERROR: Invalid name: '{soc_name}'.{RESET}", file=sys.stderr)
            self.list_socs()
            return False

        soc_name = self.utils.socs_lower.get(soc_name.lower())

        previous_soc = self.utils.parse_soc_info()
        if previous_soc and previous_soc['name'] == soc_name:
            print(f"Current SoC is: {GREEN}{soc_name}{RESET}")
            return True

        self.op_clean_soc() # clean up first

        self.soc_info_set = {'name':soc_name}
        if not self.utils.save_soc_info(self.soc_info_set):
            return False

        print(f"Current SoC is set to: {GREEN}{soc_name}{RESET}. Saved as {self.info_file}")

        return True

    def _select_interactive_internal(self) -> str:
        soc_names = self.valid_socs
        if not soc_names:
            print(f"{RED}ERROR: No available SoC.{RESET}", file=sys.stderr)
            return None

        print("\nAvailable SoCs:")
        print("-" * 40)
        for i, soc_name in enumerate(soc_names, 1):
            print(f"{i:2d}. {soc_name}")
        print("-" * 40)

        while True:
            try:
                choice = input(f"Enter a SoC number (1-{len(soc_names)}): ").strip()
                if not choice: continue
                idx = int(choice) - 1
                if 0 <= idx < len(soc_names):
                    return soc_names[idx]
                else:
                    print(f"{RED}ERROR: Please enter a number between 1-{len(soc_names)}.{RESET}", file=sys.stderr)
            except ValueError:
                print(f"{RED}ERROR: Please enter a valid number.{RESET}", file=sys.stderr)
            except (KeyboardInterrupt, EOFError):
                print("\nOperation cancelled.")
                return None

    def op_select_interactive(self) -> bool:
        if not self._check_ready(): return False

        selected_soc = self._select_interactive_internal()
        if selected_soc:
            return self.op_set_project(selected_soc)
        return False

    def _get_current_soc(self) -> Dict:
        soc_info = self.utils.parse_soc_info()
        if soc_info:
            return soc_info

        print(f"{YELLOW}Current SoC is not set or invalid. Please select one first.{RESET}")

        selected_soc = self._select_interactive_internal()
        if selected_soc and self.op_set_project(selected_soc):
             return self.utils.parse_soc_info()

        return None

    def op_build(self, args: List) -> bool:
        if not self._check_ready(): return False
        if self.env_type == 2:
            return run_script(self.current_dir, "build.py", args)

        if args and not args[0].startswith('-'):
            if args[0].lower() in self.valid_socs_lower:
                self.op_set_project(args[0])
                args = args[1:]

        soc_info = self._get_current_soc()
        if not soc_info: return False

        build_args = args
        defs = [f"EXTERN_DIR={self.current_dir}", f"FINAL_IMAGE_DIR={self.build_dir}"]
        other_args = ['-d', self.build_dir]
        if '-D' in build_args:
            idx = build_args.index('-D')
            build_args[idx + 1 : idx + 1] = defs
            build_args += other_args
        else:
            build_args += other_args + ['-D'] + defs

        is_external_app = (self.env_type > 2)

        if is_external_app and '-a' not in args:
             build_args += ['-a', self.current_dir]

        return run_script(soc_info['dir'], "build.py", build_args)

    def op_menuconfig(self, args: List) -> bool:
        if not self._check_ready(): return False
        if self.env_type == 2:
            return run_script(self.current_dir, "menuconfig.py", args)

        if args and not args[0].startswith('-'):
            if args[0].lower() in self.valid_socs_lower:
                self.op_set_project(args[0])
                args = args[1:]

        soc_info = self._get_current_soc()
        if not soc_info: return False

        config_args = args
        if self.current_dir != soc_info['dir']:
            config_args += ['-d', self.current_dir, '-e', self.current_dir]

        return run_script(soc_info['dir'], "menuconfig.py", config_args)

    def op_monitor(self, args: List) -> bool:
        if not self._check_ready(): return False
        if self.env_type == 2:
            return run_script(self.current_dir, "monitor.py", args)

        soc_info = self._get_current_soc()
        if not soc_info: return False

        monitor_args = args
        return run_script(soc_info['dir'], "monitor.py", monitor_args)

    def op_flash(self, args: List) -> bool:
        if not self._check_ready(): return False
        if self.env_type == 2:
            return run_script(self.current_dir, "flash.py", args)

        soc_info = self._get_current_soc()
        if not soc_info: return False

        flash_args = args
        return run_script(soc_info['dir'], "flash.py", flash_args)

    def op_clean_soc(self) -> bool:
        if not self._check_ready(): return False
        if os.path.exists(self.info_file):
            print(f"{YELLOW}Cleanup previous selected SoC info and products...{RESET}")
            self.op_menuconfig(["-c"])      # ninja clean, remove build and menuconfig
            os.remove(self.info_file)       # remove soc_info
        else:
            for dir_name in ["build", "menuconfig"]:    # remove build and menuconfig
                dir_path = os.path.join(self.current_dir, dir_name)
                if os.path.exists(dir_path):
                    print(f"Clean up the '{dir_name}' directory...")
                    try:
                        shutil.rmtree(dir_path)
                    except OSError as e:
                        print(f"{YELLOW}WARNING: Remove '{dir_path}' failed: {e}{RESET}", file=sys.stderr)

        return True

    def op_clean_build(self) -> bool:
        if not self._check_ready(): return False
        if os.path.exists(self.info_file):
            self.op_build(["-c"])       # ninja clean

        dir_path = os.path.join(self.current_dir, "build")  # remove build
        if os.path.exists(dir_path):
            print("Clean up the 'build' directory...")
            try:
                shutil.rmtree(dir_path)
            except OSError as e:
                print(f"{YELLOW}WARNING: Remove '{dir_path}' failed: {e}{RESET}", file=sys.stderr)

        return True

    def cmd_preprocess(self):
        if not os.environ.get('AMEBAPY_COMPLETE'):
            return
        # press tab to auto complete cmd
        words = os.environ.get('COMP_WORDS', '').split()
        cword = int(os.environ.get('COMP_CWORD', '0'))

        current_word = words[cword] if cword < len(words) else ""
        prev_word = words[cword - 1] if cword > 0 else ""
        commands = self.command_table.keys()
        suggestions = []
        if cword == 1:
            suggestions = [cmd for cmd in commands if cmd.startswith(current_word)]
        elif cword == 2 and prev_word in ['soc']:
            suggestions = [soc for soc in self.valid_socs if soc.startswith(current_word)]

        print(' '.join(suggestions))
        sys.exit(0)

    def check_env(self, dir: str = None) -> int:
        if not dir:
            dir = self.current_dir

        if dir == self.sdk_root:
            return EnvType.SDK_ROOT

        filename = "env.bat" if (os.name == 'nt') else "env.sh"
        current_dir = os.path.abspath(dir)

        target_path = os.path.join(current_dir, filename)
        if os.path.exists(target_path):
            self.project_root = current_dir # external project dir
            return EnvType.EXT_PROJECT

        parent_dir = os.path.dirname(current_dir)

        if parent_dir == self.sdk_root and current_dir.endswith("_gcc_project"):
            return EnvType.SDK_PROJECT     # sdk project dir

        while True:
            if parent_dir == current_dir:   # standalone dir
                return EnvType.EXT_DIR
            target_path = os.path.join(parent_dir, filename)
            if os.path.exists(target_path):
                self.project_root = parent_dir # subdir of a project or sdk
                return EnvType.SUB_DIR
            current_dir = parent_dir
            parent_dir = os.path.dirname(current_dir)

    def check_command_permission(self, cmd: str) -> bool:
        env_type = self.check_env()

        cmd_list = ["build", "menuconfig", "flash", "monitor", "soc", "show"]
        if not env_type and cmd in cmd_list:
            print(f"{RED}ERROR: Command '{cmd}' cannot run here.{RESET}", file=sys.stderr)
            print(f"Please run in SDK or project root directory.", file=sys.stderr)
            return False

        if env_type == EnvType.SDK_PROJECT and cmd in ["soc", "show"]: # cannot select soc
            print(f"{YELLOW}WARNING: Command '{cmd}' will be ignored here.{RESET}", file=sys.stderr)
            return False

        if env_type == EnvType.EXT_DIR and cmd in cmd_list:
            if not os.path.exists(os.path.join(self.current_dir, "CMakeLists.txt")):
                print(f"{RED}ERROR: Command '{cmd}' cannot run here.{RESET}", file=sys.stderr)
                print(f"Please run in SDK or project root directory.", file=sys.stderr)
                return False

        self.env_type = env_type
        return True

class EnvType(IntEnum):
    SUB_DIR = 0
    SDK_ROOT = 1
    SDK_PROJECT = 2
    EXT_PROJECT = 3
    EXT_DIR = 4
