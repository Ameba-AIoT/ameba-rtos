#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

from distutils.version import LooseVersion

version = "2.0.2"
# latest_url = "https://aiot.realmcu.com/en/latest/tools/vscode/index.html#advanced-features"
latest_url = f"https://aiot.realmcu.com/download/misc/AmebaRemoteService_v{version}.exe"

def isVersionLatest(remote_version, local_version) -> bool:
    if LooseVersion(remote_version) < LooseVersion(local_version):
        print(f" {remote_version} < {local_version} !")
        return False
    return True
