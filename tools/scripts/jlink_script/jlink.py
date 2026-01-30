import os
import sys
import subprocess
import platform
import shutil
import argparse
import json5
import time
import jlinkscript_generate as util

SCRIPT_DIR = os.path.join(os.path.dirname(__file__))
PLATFORM = platform.system()

if PLATFORM == "Windows":
    JLINK_EXE_NAME = "JLink.exe"
    JLINK_GDB_EXE_NAME = "JLinkGDBServer.exe"
    JLINK_DIR = r"C:\Program Files (x86)\SEGGER\JLink"
else: # Linux
    JLINK_EXE_NAME = "JLinkExe"
    JLINK_GDB_EXE_NAME = "JLinkGDBServer"
    JLINK_DIR = "/opt/SEGGER/JLink"

#=========================== Utils =========================
def load_soc_config(json_file):
    if not os.path.exists(json_file):
        print(f"Jlink Error: jlink config file not found: {json_file}")
        sys.exit(1)

    try:
        with open(json_file, 'r', encoding='utf-8') as f:
            raw_data = json5.load(f)
        return raw_data
    except Exception as e:
        print(f"Jlink Error: Failed to parse JSON: {e}")
        sys.exit(1)

def jlink_connect(config, core=None):
    if not core:
        core = config.get('DEVICE').keys()

    for c in core:
        # Generate Script
        script_file = os.path.join(config['script_dir'], f"AutoGen_{c.upper()}.JLinkScript")
        util.gen_jlink_script(c, config, script_file)

        args = [
            "-device", config.get('DEVICE').get(c),
            "-if", "SWD",
            "-jlinkscriptfile", script_file
        ]
        run_jlink(args, title = f"JLink Console - {c.upper()}")
        time.sleep(1.5)


def jlink_gdb_connect(config, core=None):
    if not core:
        core = config.get('DEVICE').keys()

    for c in core:
        # Generate Script
        script_file = os.path.join(config['script_dir'], f"AutoGen_{c.upper()}.JLinkScript")
        util.gen_jlink_script(c, config, script_file)

        args = [
            "-device", config.get('DEVICE').get(c),
            "-if", "SWD",
            "-select", "USB",
            "-port", config.get('GDB_PORTS').get(c),
            "-scriptfile", script_file
        ]
        if PLATFORM == "Windows":
            args += ["-nolocalhostonly"]

        run_jlink_gdb(args)
        time.sleep(1.5)


def swdpwd_user(config):
    # Generate Script
    script_file = os.path.join(config['script_dir'], f"AutoGen_SWDPWD_User.JLinkScript")
    util.gen_swd_script(config, script_file)
    log_file = os.path.join(config['script_dir'], "swdpwd_password.log")

    args = [
        "-select", "USB",
        "-device", "Cortex-M33",
        "-if", "SWD",
        "-scriptfile", script_file,
        "-port", config.get('GDB_PORTS').get('swd'),
        "-log", log_file
    ]

    run_jlink_gdb(args)

def swdpwd_verify():
    # Generate Script
    script_file = os.path.join(SCRIPT_DIR, "SWDPWD_Verify.JLinkScript")
    if not os.path.exists(script_file):
        print(f"Jlink Error: script not found: {script_file}")
        sys.exit(1)

    args = [
        "-select", "USB",
        "-device", "Cortex-M33",
        "-if", "SWD",
        "-scriptfile", script_file,
        "-port", 2340,
        "-log", "swdpwd_verification.log"
    ]

    run_jlink_gdb(args)

def gen_ozone_script(config, core=None):
    if not core:
        core = config.get('DEVICE').keys()
    for c in core:
        script_file = os.path.join(config['script_dir'], f"AutoGen_Ozone_{c.upper()}_Debug.JLinkScript")
        util.gen_ozone_script(c, config, script_file)


#=========================== main =========================
parser = argparse.ArgumentParser(description="Realtek Ameba J-Link Tool")
parser.add_argument("-t", "--type", choices=["jlink", "gdb", "swd", "ozone"],
                    default="jlink",
                    help="Specify the type of operation.")
parser.add_argument("-d", "--jlink-dir", help="Specify custom J-Link installation path.")
parser.add_argument("--chip", required=True, help=argparse.SUPPRESS)
parser.add_argument("-k", "--core", nargs="+",
                    help="Specify the core(s) to operate on (e.g., km0, km4).")
parser.add_argument("-sd", "--script-dir",
                    help="Specify the directory of generated J-Link scripts.")

args = parser.parse_args()

if not args.chip:
    print("Error: Please specify the chip type using --chip.")
    sys.exit(1)

if args.jlink_dir:
    JLINK_DIR = args.jlink_dir

def run_jlink(args, title="J-Link"):
    executable = os.path.join(JLINK_DIR, JLINK_EXE_NAME)
    if not os.path.exists(executable):
        print(f"\033[33m[ERROR] '{JLINK_EXE_NAME}' not found at {JLINK_DIR}\033[0m")
        print(f"\033[33mPlease use '-d' to specify the correct path.\033[0m")
        sys.exit(1)

    args = [str(x) for x in args]
    args_str = " ".join([f'"{a}"' if " " in a else a for a in args])

    if PLATFORM == "Windows":
        # start a new cmd window
        cmd_str = f'start "{title}" "{executable}" {args_str}'
        print(f"[POPUP] {title}")
        subprocess.Popen(cmd_str, shell=True)
    else:
        print(f"[EXEC] {executable} {args_str}")
        subprocess.Popen([executable] + args)

def run_jlink_gdb(args):
    executable = os.path.join(JLINK_DIR, JLINK_GDB_EXE_NAME)

    if not os.path.exists(executable):
        print(f"\033[33m[ERROR] '{JLINK_GDB_EXE_NAME}' not found at {JLINK_DIR}\033[0m")
        print(f"\033[33mPlease use '-d' to specify the correct path.\033[0m")
        sys.exit(1)

    cmd = [executable] + args
    cmd = [str(x) for x in cmd]
    print(f"\n[EXEC] {' '.join(cmd)}")
    print("-" * 60)
    subprocess.Popen(cmd)

#============== 1. load jlink config ==============
SOC_CONFIG = load_soc_config(os.path.join(SCRIPT_DIR, 'ameba_jlink_config.json5'))
CONFIG = SOC_CONFIG.get(args.chip, {})
if not CONFIG:
    print(f"Jlink Error: No configuration found for chip: {args.chip}")
    sys.exit(1)

if args.script_dir:
    CONFIG['script_dir'] = os.path.abspath(args.script_dir)
    if not os.path.isdir(CONFIG['script_dir']):
        os.makedirs(CONFIG['script_dir'])
else:
    CONFIG['script_dir'] = SCRIPT_DIR

#============== 2. operation ==============
# J-Link connect with GDB Server
if args.type== "gdb":
    jlink_gdb_connect(CONFIG, core=args.core)
# J-Link connect
elif args.type== "jlink":
    jlink_connect(CONFIG, core=args.core)
# J-Link connect with SWD password.
elif args.type== "swd":
    swdpwd_user(CONFIG)
elif args.type== "ozone":
    gen_ozone_script(CONFIG, core=args.core)
# SWD password verification: internal only
elif args.type== "swd_verify":
    swdpwd_verify()
else:
    print("Jlink Error: Unsupported operation.")
    sys.exit(1)







