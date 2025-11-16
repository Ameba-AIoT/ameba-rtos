#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0
import os
import sys
import subprocess
import re
import shutil
import signal
from typing import List, Dict
from difflib import unified_diff

IS_POSIX = (os.name == 'posix')
if not IS_POSIX:
    import win32api
    import win32con

BLACK = '\033[30m'
RED = '\033[31m'
GREEN = '\033[32m'
YELLOW = '\033[33m'
BLUE = '\033[34m'
MAGENTA = '\033[35m'
CYAN = '\033[36m'
WHITE = '\033[37m'
RESET = '\033[0m'


def is_pure_filename(path):
    return os.path.basename(path) == path

class Manager(object):
    def __init__(self,
                 projects:Dict[str, str],
                 section_begin_marker_prefix='',
                 section_begin_marker_suffix='',
                 section_end_marker_prefix='',
                 section_end_marker_suffix='',
                 general_section_name = 'General',
                 private_section_name = 'Private',
                 out_dir = '.',
                 top_kconfig = 'Kconfig'
                 ) -> None:
        self.projects = projects
        self.section_begin_marker_prefix = section_begin_marker_prefix
        self.section_begin_marker_suffix = section_begin_marker_suffix
        self.section_end_marker_prefix = section_end_marker_prefix
        self.section_end_marker_suffix = section_end_marker_suffix

        self.out_dir = out_dir
        self.config_root_dir = os.path.join(out_dir, 'menuconfig')
        self.top_kconfig = top_kconfig
        if not os.path.exists(self.config_root_dir):
            os.makedirs(self.config_root_dir)

        self.general_section_name = general_section_name
        self.private_section_name = private_section_name # bottom section of Kconfig
        self.config_in = os.path.join(self.config_root_dir, '.config')
        self.config_default = os.path.join(self.config_root_dir, '.config_default')
        self.config_default_old = os.path.join(self.config_root_dir, '.config_default.old')

        self.default_conf = os.path.join(os.path.dirname(top_kconfig), 'default.conf')
        self.prj_conf = os.path.join(self.config_root_dir, 'prj.conf')
        self.prj_conf_external = os.path.join(out_dir, 'prj.conf') #for user external project

        self.general_configs = []
        self.lock_file = os.path.join(self.config_root_dir, 'config_tmp')

    def run_command(self, script, *args):
        script_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), script)
        cmd = f'python {script_path} {" ".join(args)}'
        result = subprocess.run(cmd, shell=True)
        if result.returncode != 0:
            print(f"run command <{cmd}> error: {result.returncode}")
        return result.returncode

    def run_set_config(self,
                      config_in:str = '',
                      config_out:str = '',
                      content:List = []
                      ):
        args = ['setconfig.py']
        if config_in: args.extend(['--config-in', config_in])
        if config_out: args.extend(['--config-out', config_out])
        args.extend(['--kconfig', self.top_kconfig])
        return self.run_command(*args, *content)

    def run_gen_config(self,
                      config_in:str = '',
                      header_path:str = '',
                      ):
        args = ['genconfig.py']
        if config_in: args.extend(['--config-in', config_in])
        if header_path: args.extend(['--header-path', header_path])
        args.extend(['--kconfig', self.top_kconfig])
        return self.run_command(*args)

    def unified_file_diff(self, file_path1, file_path2):
        with open(file_path1, 'r', encoding='utf-8') as file1, open(file_path2, 'r', encoding='utf-8') as file2:
            diff = unified_diff(file1.readlines(), file2.readlines(), lineterm='')

        for line in diff:
            if line.startswith('+') and not line.startswith('+++'):
                print(f"{GREEN}{line}{RESET}", end='')
            elif line.startswith('-') and not line.startswith('---'):
                print(f"{RED}{line}{RESET}", end='')
            else:
                print(line, end='')


    def check_invalid_config(self) -> int:
        lines = self.extract_lines(self.config_in, [self.private_section_name])
        invalid_pattern = ["CONFIG_WHC_HOST=y", "CONFIG_WHC_DEV=y", "CONFIG_WHC_NONE=y"]
        for line in lines:
            stripped_line = line.strip()
            if not stripped_line:
                continue
            if stripped_line in invalid_pattern:
                print("FATAL ERROR! Please do not select the configs in last line which signs <DO NOT ENTER IN THIS BUTTON> ")
                return 1
        return 0

    def lock_acquire(self):
        if os.path.exists(self.lock_file):
            return False
        else:
            open(self.lock_file, "w")
            return True

    def lock_release(self):
        if os.path.exists(self.lock_file):
            os.remove(self.lock_file)

    def lock_cleanup(self):
        def cleanup_handler(signum, frame):
            self.lock_release()
            sys.exit(0)
        return cleanup_handler

    def lock_cleanup_windows(self):
        def cleanup_handler(ctrl_type):
            if ctrl_type in (win32con.CTRL_CLOSE_EVENT, win32con.CTRL_C_EVENT, win32con.CTRL_LOGOFF_EVENT, win32con.CTRL_SHUTDOWN_EVENT, win32con.CTRL_BREAK_EVENT):
                self.lock_release()
                os._exit(0)
                return True
            return False
        return cleanup_handler

    def apply_manual_config(self) -> int:
        if not os.path.exists(self.config_in): #load default value to .config
            self.apply_default_config()

        self.apply_files_config(files = [], output_config_default = True) #generate .config_default

        if os.path.exists(self.config_default_old):
            os.remove(self.config_default_old)

        # cleanup the lock file when abnormal termination occurs
        if IS_POSIX: # for linux
            signal_handler = self.lock_cleanup()
            signal.signal(signal.SIGTERM, signal_handler)
            signal.signal(signal.SIGINT, signal_handler) # ctrl+C
            signal.signal(signal.SIGHUP, signal_handler) # close the terminal or SSH disconnected
        else: # for windows
            windows_handler = self.lock_cleanup_windows()
            win32api.SetConsoleCtrlHandler(windows_handler, True) # close/logoff/shutdown

        if self.lock_acquire():
            result = self.run_command('menuconfig.py', self.top_kconfig)
            self.lock_release()
            return result
        else:
            print("error: menuconfig is running in another terminal")
            return None

    def apply_default_config(self) ->int:
        if os.path.exists(self.config_in):
            os.remove(self.config_in)
        file_list = []
        if os.path.exists(self.prj_conf):
            file_list.append(self.prj_conf)
        elif os.path.exists(self.prj_conf_external):
            file_list.append(self.prj_conf_external)

        return self.apply_files_config(file_list)

    def apply_check_config(self) ->int:

        if not os.path.exists(self.config_in):  # a pristine project, empty menuconfig floder, need generate .config
            # print("INFO: no .config exists, create with default configurations")
            if self.apply_default_config(): return 1

        if self.apply_files_config(files = [], output_config_default = True): return 2 #generate .config_default each time

        if os.path.exists(self.config_default_old): # differece exists in .config_default, imply that Kconfig is out of date
            print(f"{YELLOW}WARNING: .config file is out of date.{RESET}")

            print(f"{MAGENTA}Difference between: menuconfig/.config_default.old and menuconfig/.config_default{RESET}")
            self.unified_file_diff(self.config_default_old, self.config_default)

            print(f"{YELLOW}Press [Enter] to continue: Skip Kconfig updating and using current .config{RESET}")
            print(f"{YELLOW}Press [CTRL+C] to exit: Run menuconfig.py to reconfiguration after exiting\r\n{RESET}")

            user_input = input(f"{YELLOW}Press [Enter] to continue or Press [CTRL+C] to exit\r\n{RESET}")
            if user_input == '': # Enter is press
                print(f"{MAGENTA}[Enter] is pressed, kconfig updating is ignored{RESET}")
                os.remove(self.config_default_old)
                return 0
            else:
                print(f"{MAGENTA}Please run menuconfig.py{RESET}")
                return 3
        else:
            return 0

    def apply_files_config(self, files:List[str], output_config_default = False) -> int:

        file_list = []
        confs = []

        # default conf file default.conf is added each time
        if os.path.exists(self.default_conf):
            file_list.append(self.default_conf)

        file_list += files

        for file in file_list:
            file_path = file  #use file path directly
            if not os.path.isfile(file_path):
                print(file_path + " does not exist")
                return 2

            with open(file_path, 'r') as f:
                for line in f:
                    stripped_line = line.strip()
                    if stripped_line.startswith("CONFIG_"):
                        stripped_line = stripped_line[len("CONFIG_"):]
                    if stripped_line and not stripped_line.startswith("#"):
                        confs.append(stripped_line)

        if output_config_default:
            config_in  = "__empty__"
            config_out = self.config_default
        else:
            config_in = None
            config_out = None
            if os.path.exists(self.config_root_dir):
                shutil.rmtree(self.config_root_dir)
                os.makedirs(self.config_root_dir)

        return self.run_set_config(config_in = config_in, config_out=config_out, content = confs)

    def clean_all(self, script_dir:str):
        if os.path.exists(self.config_root_dir):
            shutil.rmtree(self.config_root_dir)

        build_dir = os.path.join(self.out_dir, 'build')
        build_py = os.path.join(script_dir, 'build.py')

        if os.path.exists(build_py):
            cmd = 'python ' + build_py + ' -d ' + build_dir + ' -c '
            print(cmd)
            os.system(cmd)

        if os.path.exists(build_dir):
            shutil.rmtree(build_dir)

        if os.path.exists(self.config_root_dir):
            shutil.rmtree(self.config_root_dir)

    def save_confs(self, file:str) -> int:
        if os.path.isabs(file):
            path = file
        else:
            path = os.path.join(self.out_dir, file)

        if os.path.realpath(path) == os.path.realpath(self.default_conf):
            print("WARNING: You are touching default.conf")
            return self.run_command('saveconfig.py --kconfig ', self.top_kconfig, ' --out ', path)
        else:
            return self.run_command('saveconfig.py --kconfig ', self.top_kconfig, ' --out ', path, ' --default ', self.default_conf)

    def extract_core_role(self, core:str, lines:List[str] = []) -> List[str]:
        pattern = rf'^CONFIG_(WHC_HOST|WHC_DEV|WHC_NONE)(_FOR_{core.upper()})=(.*)$'
        configs = []
        for line in lines if lines else self.general_configs:
            stripped_line = line.strip()
            if not stripped_line:
                continue

            match = re.match(pattern, line)
            if match:
                configs.append(f"{match.group(1)}={match.group(3)}")

        return configs

    def extract_lines(self, filename, sections:List[str]) -> List[str]:
        results = []
        with open(filename, 'r') as file:
            lines = file.readlines()
            for section in sections:
                begin_marker = f'{self.section_begin_marker_prefix}{section}{self.section_begin_marker_suffix}'
                end_marker = f'{self.section_end_marker_prefix}{section}{self.section_end_marker_suffix}'

                start_index = None
                end_index = None
                find_start = False
                for i, line in enumerate(lines):
                    if (not find_start) and (begin_marker in line):  #avoid start_marker is a substr of end_marker
                        start_index = i
                        find_start = True
                    elif end_marker in line:
                        end_index = i
                        break
                if start_index and end_index:
                    results.extend(lines[start_index - 1:end_index + 2])

            #NOTE: add the Private part to the result
            end_markers = [f'{self.section_end_marker_prefix}{core}{self.section_end_marker_suffix}' for core in self.projects.values()]
            no = 0
            for i, line in enumerate(lines):
                # Find the last end mark
                if any(end_marker in line for end_marker in end_markers):
                    no = max(no, i)
            if no > 0:
                results.extend(lines[no+1:])

        return results

    def parse_general_config(self) -> int:
        self.general_configs = self.extract_lines(self.config_in, [self.general_section_name])
        return len(self.general_configs) > 0

    def process_section_suffix(self, filename_input, filename_ouput, skip_pattern, replace_pattern, input_lines = []):
        processed_lines = []
        if not input_lines:
            with open(filename_input, 'r') as f:
                input_lines = f.readlines()

        for line in input_lines:
            if not any(re.search(pattern, line) for pattern in skip_pattern):
                line = re.sub(replace_pattern, '', line)
                processed_lines.append(line)
        with open(filename_ouput, 'w') as file:
            file.writelines(list(dict.fromkeys(processed_lines)))

    def process_projects(self) -> int:
        for proj, core in self.projects.items():
            role_config = self.extract_core_role(core)
            core_config_file = os.path.join(self.config_root_dir, f'.config_{core.lower()}')
            core_config_file_tmp = os.path.join(self.config_root_dir, f'.config_{core.lower()}_tmp')
            core_config_file_old = os.path.join(self.config_root_dir, f'.config_{core.lower()}.old')
            core_hearder_dir = os.path.join(self.config_root_dir, f'project_{proj.lower()}')
            if not os.path.exists(core_hearder_dir):
                os.makedirs(core_hearder_dir)
            core_header_file = os.path.join(core_hearder_dir, 'platform_autoconf.h')
            tmp_header_file = os.path.join(core_hearder_dir, '__tmp_conf.h')
            self.run_set_config(
                config_in = self.config_in,
                config_out = core_config_file,
                content = role_config
            )

            core_config = self.extract_lines(core_config_file, [self.general_section_name, core.upper()])
            with open(core_config_file_tmp, "w") as f:
                f.writelines(core_config)
            #NOTE: header MUST be generate use core_config_file_tmp but core_config_file
            #      for core_config_file would include other core-kconfig-specifiled config unexpectly
            self.run_gen_config(
                config_in=core_config_file_tmp,
                header_path=tmp_header_file
            )
            os.remove(core_config_file_tmp)

            skip_pattern = [f"_FOR_{c}" for c in self.projects.values() if c != core]
            skip_pattern.append(r"_MENU(=|\s).*$")
            self.process_section_suffix(core_config_file, core_config_file, skip_pattern, f"_FOR_{core}", core_config)

            #To avoid header_file regeneration even nothing changed, this bad case will break incremental build.
            #process_section_suffix will force .config_xxx.old generation each time, because some configs are removed.
            if os.path.exists(core_config_file_old):
                with open(core_config_file_old, 'r') as old, open(core_config_file, 'r') as file:
                    if (old.read() == file.read()):
                        os.remove(tmp_header_file)
                        continue

            self.process_section_suffix(tmp_header_file, core_header_file, skip_pattern, f"_FOR_{core}")
            os.remove(tmp_header_file)
        return 0
