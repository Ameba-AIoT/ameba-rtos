#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0 

import argparse
import glob
import os
import shutil
import subprocess
import sys

LOONG_SDK_GIT_HOOKS_SRC_DIR = 'tools/meta_tools/git_hooks'
LOONG_SDK_GIT_HOOKS_TGT_DIR = '.repo/repo/hooks'

CMD_CLEAN_WORKSPACE = "repo forall -c 'git reset --hard && git clean -fd'"
CMD_REPO_UPDATE = 'repo sync'
CMD_REPO_LIST = "repo list | awk '{print $1}'"
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


def is_repo_workspace():
    return os.path.exists('.repo') and os.path.isdir('.repo')


def main(argc, argv):
    parser = argparse.ArgumentParser(description=None)
    parser.add_argument('-p', '--pristine', action='store_true', help='pristine build')

    args = parser.parse_args()

    rc = is_repo_workspace()
    if rc:
        pass
    else:
        print('Warning: Invalid repo workspace, ignored')
        sys.exit(1)

    print('Update...')

    if args.pristine:
        print('Clean workspace...')
        os.system(CMD_CLEAN_WORKSPACE)
    else:
        pass

    print('Update workspace...')
    os.system(CMD_REPO_UPDATE)

    print('Update Git hooks...')
    update_git_hooks()

    print('Update done')


if __name__ == '__main__':
    main(len(sys.argv), sys.argv[1:])
