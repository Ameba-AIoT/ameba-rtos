import os
import subprocess
import re
import shutil
from typing import List, Dict
from difflib import unified_diff

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

        self.general_configs = []

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
        invalid_pattern = rf'^CONFIG_CORE_AS_(.)P=y$'
        for line in lines:
            stripped_line = line.strip()
            if not stripped_line:
                continue
            match = re.match(invalid_pattern, line)
            if match:
                print("FATAL ERROR! Please do not select the configs in last line which signs <DO NOT ENTER IN THIS BUTTON> ")
                return 1
        return 0

    def apply_manual_config(self) -> int:
        if os.path.exists(self.config_default_old):
            os.remove(self.config_default_old)
        if not os.path.exists(self.config_in):
            self.apply_default_config()
        return self.run_command('menuconfig.py', self.top_kconfig)

    def apply_default_config(self) ->int:
        if os.path.exists(self.config_in):
            os.remove(self.config_in)
        file_list = []
        # if os.path.exists(self.config_prj):
        #     file_list.append(self.config_prj)
        return self.apply_files_config(file_list)

    def apply_check_config(self) ->int:

        if not os.path.exists(self.config_in):  # a pristine project, empty menuconfig floder, need generate .config
            # print("INFO: no .config exists, create with default configurations")
            file_list = []
            if os.path.exists(self.prj_conf): file_list.append(self.prj_conf)
            if self.apply_files_config(file_list): return 1

        if self.apply_files_config(files = [], output_config_default = True): return 2 #generate .config_default each time

        if os.path.exists(self.config_default_old): # differece exists in .config_default, imply that Kconfig is out of date
            print(f"{YELLOW}WARNING: .config file is out of date.{RESET}")

            print(f"{MAGENTA}Difference between: menuconfig/.config_default.old and menuconfig/.config_default{RESET}")
            self.unified_file_diff(self.config_default_old, self.config_default)

            print("按下[回车] 继续: 忽略Kconfig变更差异，使用当前的.config配置")
            print("按下[Ctrl+C] 退出: 退出后请运行 menuconfig.py 脚本以重新配置")
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
        if output_config_default:
            config_in  = "__empty__"
            config_out = self.config_default
        else:
            config_in = None
            config_out = None
            if os.path.exists(self.config_root_dir):
                shutil.rmtree(self.config_root_dir)
                os.makedirs(self.config_root_dir)


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
                    confs.append(stripped_line)
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
        return self.run_command('saveconfig.py --kconfig ', self.top_kconfig, ' --out ', file)

    def extract_core_role(self, core:str, lines:List[str] = []) -> List[str]:
        pattern = rf'^CONFIG_CORE_AS_(.*)(_FOR_{core.upper()})=(.*)$'
        configs = []
        for line in lines if lines else self.general_configs:
            stripped_line = line.strip()
            if not stripped_line:
                continue

            match = re.match(pattern, line)
            if match:
                configs.append(f"CORE_AS_{match.group(1)}={match.group(3)}")

        return configs

    def extract_lines(self, filename, sections:List[str]) -> List[str]:
        results = []
        for section in sections:
            begin_marker = f'{self.section_begin_marker_prefix}{section}{self.section_begin_marker_suffix}'
            end_marker = f'{self.section_end_marker_prefix}{section}{self.section_end_marker_suffix}'

            with open(filename, 'r') as file:
                lines = file.readlines()
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
            self.run_gen_config(
                config_in=core_config_file,
                header_path=tmp_header_file
            )
            core_config = self.extract_lines(core_config_file, [self.general_section_name, core.upper()])

            skip_pattern = [f"_FOR_{c}" for c in self.projects.values() if c != core]
            skip_pattern.append("_MENU(=|\s).*$")
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
