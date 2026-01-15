import os
import sys
import json
import subprocess
from typing import List, Dict, Any, Optional
import base64
import zlib

BLACK = '\033[30m'
RED = '\033[31m'
GREEN = '\033[32m'
YELLOW = '\033[33m'
BLUE = '\033[34m'
MAGENTA = '\033[35m'
CYAN = '\033[36m'
WHITE = '\033[37m'
RESET = '\033[0m'

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
SDK_ROOT= os.path.abspath(os.path.join(SCRIPT_DIR, '../..'))

def run_script(target_dir, script_name, args) -> bool:
    script_path = os.path.join(target_dir, script_name)
    if not os.path.exists(script_path):
        print(f"{RED}ERROR: Script not found: {script_path}{RESET}", file=sys.stderr)
        return False

    cmd = [sys.executable, script_path] + args
    result = subprocess.run(cmd, check=False)
    return result.returncode == 0

class SocManager:
    def __init__(self, cwd=os.getcwd()):
        current_dir = os.getcwd()
        if current_dir == SDK_ROOT or os.path.dirname(current_dir) == SDK_ROOT:
            self.sdk_root = SDK_ROOT
        else:
            self.sdk_root = os.environ.get('AMEBA_SDK')
        if not self.sdk_root or not os.path.isdir(self.sdk_root):
            raise ValueError(f"SDK root path does not exist or is not a directory: {self.sdk_root}")

        self.info_file_name = "soc_info.json"
        self.info_file = os.path.join(cwd, self.info_file_name)

        self.raw_soc_map = self._load_soc_map()
        if self.raw_soc_map is None:
            raise FileNotFoundError("The SOC map file could not be loaded. Cannot proceed.")

        self.soc_map = self._check_soc_map()
        self.socs_lower = {key.lower(): key for key in self.soc_map}

    def get_available_socs(self) -> List[str]:
        return sorted(self.soc_map.keys())

    def parse_soc_info(self) -> Dict:
        if not self.info_file or not os.path.exists(self.info_file):
            return None
        soc_info = None
        try:
            with open(self.info_file, 'r') as jsonfile:
                config = json.load(jsonfile)
                soc_name = config.get('soc', {}).get('name')
                soc_dir = self._parse_project_path(soc_name)
                if soc_dir:
                    soc_info = {
                        "name": soc_name,
                        "project": self.soc_map.get(str(soc_name)),
                        "dir": soc_dir
                    }
                else:
                    print(f"{YELLOW}WARNING: Invalid SOC name: '{soc_name}', read from '{self.info_file}'.{RESET}")
            return soc_info
        except IOError as e:
            print(f"{RED}ERROR: Failed to read file '{self.info_file}': {e}{RESET}", file=sys.stderr)
            return None

    def save_soc_info(self, soc_info_set: dict) -> bool:
        config = {
            'soc': {
                'name': soc_info_set['name'],
            }
        }
        try:
            with open(self.info_file, 'w') as jsonfile:
                json.dump(config, jsonfile, indent=4)
            return True
        except IOError as e:
            print(f"{RED}ERROR: Failed to write the file '{self.info_file}': {e}{RESET}", file=sys.stderr)
            return False

    def _parse_project_path(self, soc_name: str) -> Optional[str]:
        if not soc_name:
            return None

        project_alias = self.raw_soc_map.get(soc_name)
        if project_alias:
            path = os.path.join(self.sdk_root, f"{project_alias}_gcc_project")
            if os.path.isdir(path):
                return path
        return None

    def _check_soc_map(self) -> Dict[str, Any]:
        if not self.raw_soc_map:
            return {}

        valid_map = {}
        for soc, alias in self.raw_soc_map.items():
            if self._parse_project_path(soc):
                valid_map[soc] = alias
        return valid_map


    def _load_soc_map(self) -> dict:
        def _process_stream(data: bytes) -> bytes:
            buffer = bytearray(data)
            for i, byte_val in enumerate(buffer):
                buffer[i] = ((byte_val - 37 + 256) % 256) ^ (i & 0xFF)

            n = len(buffer) // 2
            return buffer[n:] + buffer[:n]

        MAP_FILE = os.path.join(self.sdk_root, 'tools/scripts/soc_map.dat')
        try:
            with open(MAP_FILE, 'rb') as f:
                data = f.read()

            data_b64 = base64.b64decode(data)
            data_process = _process_stream(data_b64)
            final_payload = zlib.decompress(data_process)

            return json.loads(final_payload)

        except FileNotFoundError:
            print(f"{RED}ERROR: File not found: '{MAP_FILE}'{RESET}")
            return None
        except Exception as e:
            print(f"{RED}ERROR: Failed to parse '{MAP_FILE}': {e}{RESET}")
            return None
