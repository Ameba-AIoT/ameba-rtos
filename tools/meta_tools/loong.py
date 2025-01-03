#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0 

import os
import subprocess
import sys
import json

LOONG_SDK_ROOT_DIR = os.path.dirname(os.path.abspath(__file__))
LOONG_COMPONENT_DIR = os.path.join(LOONG_SDK_ROOT_DIR, 'component')
LOONG_META_TOOL_DIR = os.path.join('tools', 'meta_tools')
LOONG_CFG_FILE = os.path.join(LOONG_META_TOOL_DIR, 'loong.json')

LOONG_USAGE = '''\
Copyright(c) 2024, Realtek Semiconductor Corporation

Usage:
\tloong.py [-h] [-v] <command> [<options>]

Built-in options:
\t-h, --help\tprint usage information
\t-v, --version\tprint version information

Built-in commands:
\thelp:\t\tprint usage information for meta-tool or specific command

Extension commands:\
'''

def print_usage(cfg):
    print(cfg['brief'])
    print(LOONG_USAGE)
    for cmd in cfg['commands']:
        if len(cmd['name']) > 6:
            print('\t' + cmd['name'] + ':\t' + cmd['help'])
        else:
            print('\t' + cmd['name'] + ':\t\t' + cmd['help'])

def print_version(version):
    print('Realtek Loong SDK Meta Tool version: ' + version)

def run_command(script, argv):
    script_path = os.path.join(LOONG_META_TOOL_DIR, script)
    result = subprocess.run(['python', script_path] + argv)
    if result.returncode != 0:
        sys.exit(1)
    else:
        sys.exit(0)

def get_command_script(name, cfg):
    valid = False
    script = None
    for cmd in cfg['commands']:
        if cmd['name'] == name:
            valid = True
            script = cmd['script']
            break
        else:
            pass
    return script

def main(argc, argv):
    cfg = None
    help = False
    help_ext = False
    version = False
    command = None
    error = False
    script = None

    if os.path.exists(LOONG_COMPONENT_DIR):
        pass
    else:
        print('Error: Wrong directory to run meta tool, it is only allowed to run under SDK root directory')
        sys.exit(2)

    if os.path.exists(LOONG_CFG_FILE):
        try:
            with open(LOONG_CFG_FILE, 'r') as f:
                cfg = json.load(f)
        except:
            print('Error: Fail to load meta tool configuration file "' + LOONG_CFG_FILE + '"')
            sys.exit(2)
    else:
        print('Error: Meta tool configuration file "' + LOONG_CFG_FILE + '" does not exist')
        sys.exit(1)
    
    if argc == 1:
        print('Warning: Invalid arguments')
        print_usage(cfg);
        sys.exit(1)
    else:
        pass

    for arg in argv:
        if arg.startswith('-h'):
            help = True
        elif arg == '--help':
            help = True
        elif arg.startswith('-v'):
            version = True
        elif arg == '--version':
            version = True
        elif arg == 'help':
            help_ext = True
        elif arg.startswith('-'):
            error = True
            break
        else:
            command = arg
            script = get_command_script(command, cfg)
            if script == None:
                error = True
            else:
                pass
            break

    if help:
        print_usage(cfg)
        sys.exit(0)
    elif version:
        print_version(cfg['version'])
        sys.exit(0)
    elif help_ext:
        if command != None and script != None:
            run_command(script, ['-h'])
        else:
            error = True
    elif command != None and script != None:
        run_command(script, argv[1:])
    else:
        error = True
    
    if error:
        print('Warning: Invalid arguments')
        print_usage(cfg)
        sys.exit(1)
    else:
        pass

if __name__ == '__main__':
    main(len(sys.argv), sys.argv[1:])

