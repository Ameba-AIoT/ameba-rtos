#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2025 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0 

import glob
import shutil
import subprocess
import os
import sys


LOONG_SDK_GIT_HOOKS_SRC_DIR = 'tools/scripts/git_hooks'
LOONG_SDK_GIT_HOOKS_TGT_DIR = '.repo/repo/hooks'

CMD_CLEAN_GIT_WORKSPACE="git reset --hard && git clean -fd"
CMD_GIT_UPDATE = 'git pull'
CMD_CLEAN_WORKSPACE = "repo forall -c 'git reset --hard && git clean -fd'"
CMD_REPO_UPDATE = 'repo sync'


CMD_RM_PRE_COMMIT_HOOKS = 'find .repo/ -name "pre-commit" | grep -v ".repo/repo/hooks/" | xargs rm'
CMD_RM_COMMIT_MSG_HOOKS = 'find .repo/ -name "commit-msg" | grep -v ".repo/repo/hooks/" | xargs rm'
GIT_HOOKS_SYNC_FAIL_MSG = 'Not replacing locally modified'

def run_command(command, show_output=False):
    if show_output:
        result = subprocess.run(command, shell=True, text=True)
    else:
        result = subprocess.run(command, shell=True, capture_output=True, text=True)
    return result


def is_repo_workspace():
    return os.path.exists('.repo') and os.path.isdir('.repo')


def is_git_workspace():
    return os.path.exists('.git') and os.path.isdir('.git')

def get_current_vcs():
    if is_repo_workspace():
        return "repo"
    if is_git_workspace():
        return "git"
    return "none"

def get_abs_path(path):
    abs_path = os.path.normcase(path)
    sdkroot = os.getcwd()
    if not os.path.isabs(abs_path):
        abs_path = os.path.join(sdkroot, abs_path)
    else:
        pass
    return abs_path

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
    rc = run_command(CMD_REPO_UPDATE + ' 2>&1')
    if rc.returncode != 0:
        print('Error: Fail to sync Git hooks to repositories')
        sys.exit(2)
    elif GIT_HOOKS_SYNC_FAIL_MSG in rc.stdout:
        print('Warning: Fail to sync Git hooks, retry')
        run_command(CMD_RM_PRE_COMMIT_HOOKS, True)
        run_command(CMD_RM_COMMIT_MSG_HOOKS, True)
        rc = run_command(CMD_REPO_UPDATE + ' 2>&1')
        if rc.returncode != 0:
            print('Error: Fail to sync Git hooks to repositories')
            print(rc.stdout)
            sys.exit(2)
        else:
            pass
    else:
        pass