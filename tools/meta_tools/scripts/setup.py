#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0 

import argparse
import glob
import json
import os
import shutil
import subprocess
import sys

LOONG_SDK_VENV_DIR = '.venv'
LOONG_SDK_PIP_REQUIREMENTS_PATH = 'tools/requirements.txt'
LOONG_SDK_GIT_HOOKS_SRC_DIR = 'tools/meta_tools/git_hooks'
LOONG_SDK_GIT_HOOKS_TGT_DIR = '.repo/repo/hooks'

CMD_REPO_UPDATE = 'repo sync'
CMD_REPO_LIST = "repo list | awk '{print $1}'"
CMD_INSTALL_LOONG_REQUIREMENTS = 'pip install -r ' + LOONG_SDK_PIP_REQUIREMENTS_PATH
CMD_CLEAN_WORKSPACE = "repo forall -c 'git reset --hard && git clean -fd'"
CMD_RM_PRE_COMMIT_HOOKS = 'find .repo/ -name "pre-commit" | grep -v ".repo/repo/hooks/" | xargs rm'
CMD_RM_COMMIT_MSG_HOOKS = 'find .repo/ -name "commit-msg" | grep -v ".repo/repo/hooks/" | xargs rm'

GIT_HOOKS_SYNC_FAIL_MSG = 'Not replacing locally modified'


def run_shell_cmd_with_output(cmd):
    return subprocess.run(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)


def update_git_hooks():
    if os.path.exists(LOONG_SDK_GIT_HOOKS_TGT_DIR):
        hooks = glob.glob(LOONG_SDK_GIT_HOOKS_SRC_DIR + '/*')
        for hook in hooks:
            if os.path.isdir(hook):
                pass
            elif os.path.isfile(hook):
                shutil.copy(hook, LOONG_SDK_GIT_HOOKS_TGT_DIR)
                os.system("chmod a+x " + os.path.join(LOONG_SDK_GIT_HOOKS_TGT_DIR, os.path.basename(hook)))
            else:
                pass
    else:
        print('Error: Repo damaged, no hooks directory found')
        sys.exit(2)

    print('Sync Git hooks to repositories...')
    rc = run_shell_cmd_with_output(CMD_REPO_UPDATE + ' 2>&1')
    if rc.returncode != 0:
        print('Error: Fail to sync Git hooks to repositories')
        sys.exit(2)
    elif GIT_HOOKS_SYNC_FAIL_MSG in rc.stdout:
        print('Warning: Fail to sync Git hooks, retry')
        run_shell_cmd_with_output(CMD_RM_PRE_COMMIT_HOOKS)
        run_shell_cmd_with_output(CMD_RM_COMMIT_MSG_HOOKS)
        rc = run_shell_cmd_with_output(CMD_REPO_UPDATE + ' 2>&1')
        if rc.returncode != 0:
            print('Error: Fail to sync Git hooks to repositories')
            print(rc.stdout)
            sys.exit(2)
        else:
            pass
    else:
        pass


def check_venv():
    print("Check Python virtual environment...")
    if os.path.exists(LOONG_SDK_VENV_DIR):
        print("Python virtual environment exists")
    else:
        # Create virtual environment if it does not exist
        try:
            print("Python virtual environment does not exist")
            venv.create(LOONG_SDK_VENV_DIR)
            print("Python virtual environment created")
        except:
            print("Error: Fail to create Python virtual environment")
            sys.exit(2)


def is_repo_workspace():
    return os.path.exists('.repo') and os.path.isdir('.repo')


def main(argc, argv):
    parser = argparse.ArgumentParser(description=None)
    parser.add_argument('-g', '--update-git-hooks', action='store_true', help='update git hooks')
    parser.add_argument('-p', '--pristine', action='store_true', help='pristine build')
    parser.add_argument('-v', '--venv', action='store_true', help='process in Python virtual enviroment')

    args = parser.parse_args()

    print("Set up...")

    if args.pristine:
        print("Clean workspace...")
        os.system(CMD_CLEAN_WORKSPACE)
        print("Clean workspace done")
    else:
        pass

    # Check virtual environment
    venv = False
    if args.venv:
        venv = True
        check_venv()
    else:
        pass

    cmd = ''
    if venv:
        cmd_activate_venv = None
        cmd_deactivate_venv = None
        if os.name == 'nt':
            cmd_activate_venv = os.path.join(LOONG_SDK_VENV_DIR, 'Scripts', 'activate.bat')
            cmd_deactivate_venv = os.path.join(LOONG_SDK_VENV_DIR, 'Scripts', 'deactivate.bat')
        else:
            cmd_activate_venv = 'source ' + os.path.join(LOONG_SDK_VENV_DIR, 'bin', 'activate')
            cmd_deactivate_venv = 'deactivate'
        cmd += cmd_activate_venv + ' && '
    else:
        pass
    cmd += 'echo "Update workspace..." && ' + CMD_REPO_UPDATE + ' && echo "Update workspace done" && '
    cmd += 'echo "Install Python requirements..." && ' + CMD_INSTALL_LOONG_REQUIREMENTS + ' && echo "Install Python requirements done"'

    rc = 0
    try:
        rc = os.system(cmd)
    except:
        rc = 1

    if venv:
        run_shell_cmd_with_output(cmd_deactivate_venv)
    else:
        pass

    if rc != 0:
        print("Error: Set up Loong SDK failed (" + str(rc) + ")")
        sys.exit(2)
    else:
        pass

    rc = is_repo_workspace()
    if args.update_git_hooks and rc:
        print("Update Git hooks...")
        update_git_hooks()
        print("Update Git hooks done")
    else:
        pass

    print("Set up Loong SDK done")

if __name__ == '__main__':
    main(len(sys.argv), sys.argv[1:])
