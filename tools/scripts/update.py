#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0 

import argparse
import os
import sys

from tools.scripts import rtk_utils


def main(argc, argv):
    parser = argparse.ArgumentParser(description=None)
    parser.add_argument('-p', '--pristine', action='store_true', help='pristine build')

    args = parser.parse_args()

    no_vcs_flag = not rtk_utils.is_repo_workspace() and not rtk_utils.is_git_workspace()
    if no_vcs_flag:
        print('Warning: Invalid repo or git workspace, ignored')
        sys.exit(1)
    else:
        pass

    print('Update...')
    if args.pristine:
        print("Clean workspace...")
        if rtk_utils.is_repo_workspace():
            os.system(rtk_utils.CMD_CLEAN_WORKSPACE)
        elif rtk_utils.is_git_workspace():
            os.system(rtk_utils.CMD_CLEAN_GIT_WORKSPACE)
        else:
            # Should not reach here, because of the 'no_vcs_flag' check
            print("Warning: No VCS detected, the 'pristine' argument will be ignored.")
        print("Clean workspace done")
    else:
        pass

    print('Update workspace...')
    os.system(rtk_utils.CMD_REPO_UPDATE if rtk_utils.is_repo_workspace() else rtk_utils.CMD_GIT_UPDATE)

    if rtk_utils.is_repo_workspace():
        print('Update Git hooks...')
        rtk_utils.update_git_hooks()

    print('Update done')


if __name__ == '__main__':
    main(len(sys.argv), sys.argv[1:])
