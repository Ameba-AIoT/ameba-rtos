#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0 

import argparse
import os
import sys

from tools.scripts import rtk_utils


LOONG_SDK_PIP_REQUIREMENTS_PATH = 'tools/requirements.txt'
CMD_INSTALL_LOONG_REQUIREMENTS = 'python -m pip install -r ' + LOONG_SDK_PIP_REQUIREMENTS_PATH


def main(argc, argv):
    parser = argparse.ArgumentParser(description=None)
    parser.add_argument('-g', '--update-git-hooks', action='store_true', help='update git hooks')
    parser.add_argument('-p', '--pristine', action='store_true', help='pristine build')

    args = parser.parse_args()

    print("Set up...")
    no_vcs_flag = not rtk_utils.is_repo_workspace() and not rtk_utils.is_git_workspace()
    if args.pristine:
        print("Clean workspace...")
        if rtk_utils.is_repo_workspace():
            os.system(rtk_utils.CMD_CLEAN_WORKSPACE)
        elif rtk_utils.is_git_workspace():
            os.system(rtk_utils.CMD_CLEAN_GIT_WORKSPACE)
        else:
            print("Warning: No VCS detected, the 'pristine' argument will be ignored.")
        print("Clean workspace done")
    else:
        pass

    # Update workspace
    workspace_update_return_code = 0
    if no_vcs_flag:
        print('Warning: workspace update will be skipped because no VCS was detected.')
    else:
        print('Update workspace...')
        result = rtk_utils.run_command(rtk_utils.CMD_REPO_UPDATE if rtk_utils.is_repo_workspace() else rtk_utils.CMD_GIT_UPDATE, show_output=True)
        workspace_update_return_code = result.returncode
        if result.returncode != 0:
            print('Error: Fail to update workspace')
        else:
            print('Update workspace done')

    # Install Python requirements
    print('Install Python requirements...')
    pip_install_result = rtk_utils.run_command(CMD_INSTALL_LOONG_REQUIREMENTS, show_output=True)
    print('Install Python requirements done')

    if not no_vcs_flag and workspace_update_return_code != 0:
        # when return code is not 0, result must be defined
        print(f"Error: Workspace update failed (code: {workspace_update_return_code}).")
        sys.exit(2)

    if pip_install_result.returncode != 0:
        print(f"Error: pip install failed (code: {pip_install_result.returncode}).")
        sys.exit(2)

    if args.update_git_hooks and rtk_utils.is_repo_workspace():
        print("Update Git hooks...")
        rtk_utils.update_git_hooks()
        print("Update Git hooks done")
    else:
        pass

    print("Set up Loong SDK done")


if __name__ == '__main__':
    main(len(sys.argv), sys.argv[1:])
