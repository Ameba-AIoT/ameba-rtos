#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import argparse
import json
import os
import uuid
import xml.dom.minidom

import base.rtk_utils


LOONG_SDK_MANIFEST_FILE = os.path.join('.repo', 'manifest.xml')
CMD_GET_MANIFEST_URL = "cat .repo/manifests.git/config | grep -m 1 url | cut -d\"=\" -f 2"

def do_query_app(sdkroot, cfg):
    include_path_list = [os.path.normpath(os.path.join(sdkroot, x)) for x in cfg.get("apps", {}).get("include", [])]
    exclude_path_list = [os.path.normpath(os.path.join(sdkroot, x)) for x in cfg.get("apps", {}).get("exclude", [])]

    class Node():
        def __init__(self, path, sdkroot):
            self.path = path
            self.sdkroot = sdkroot
            self.name = os.path.basename(path)
            self.parent_path = os.path.dirname(path)
            self.parent = None
            self.children = []

        @property
        def json_prepare(self):
            if self.children:
                children_list = []
                for child in self.children:
                    children_list.append(child.json_prepare)
                return {self.name: children_list}
            else:
                return {self.name: os.path.relpath(self.path, os.path.basename(self.sdkroot))}

        def __repr__(self):
            return f"Node({self.name} {self.parent_path})"

    nodes_dict = {}
    for include_path in include_path_list:
        # use os.walk can run faster than path rglob
        if os.path.exists(include_path):
            for root, dirs, files in os.walk(include_path):
                for file in files:
                    if file == "app_example.c":
                        app = root
                        for exclude_path in exclude_path_list:
                            if app.startswith(exclude_path):
                                break
                        else:
                            rel_path = os.path.relpath(app, os.path.dirname(sdkroot))
                            node = Node(rel_path, sdkroot)
                            nodes_dict.update({rel_path: node})
        else:
            raise FileNotFoundError(f"Not exists {include_path}!")

    # restructure
    node_paths = list(nodes_dict.keys())
    for node_path in node_paths:
        node = nodes_dict[node_path]
        while node.parent_path:
            parent = nodes_dict.get(node.parent_path)
            if parent is None:
                parent = Node(node.parent_path, sdkroot)
                nodes_dict.update({parent.path: parent})
            if node not in parent.children:
                parent.children.append(node)
            node.parent = node
            node = parent

    root_node = nodes_dict.get(os.path.basename(sdkroot))
    if root_node:
        ret_dict = root_node.json_prepare
        ret_dict = {"apps": ret_dict[os.path.basename(sdkroot)]}
        ret = json.dumps(ret_dict, indent=4)
    else:
        raise Exception('Can not get any apps!')
    print(ret)


def do_query_device(cfg):
    # Query against configuration file
    devices = dict()
    for key, value in cfg['devices'].items():
        device_path = os.path.normcase(value['path'])
        chip = value['chip']
        chip_cfg = cfg['chips'][value['chip']]
        value['core'] = chip_cfg['core']
        value['target'] = chip_cfg['target']
        value['toolchain'] = chip_cfg['toolchain']
        value['jlinkScript'] = os.path.join(device_path, os.path.normcase(chip_cfg['jlinkScript']))
        value['path'] = device_path
        devices[key] = value
    ret = json.dumps(devices, indent=4)
    print(ret)


def do_query_info(sdkroot, cfg):
    repo_info = cfg['info']
    info = dict()
    info['uuid'] = str(uuid.uuid4())
    info['brief'] = repo_info['brief']
    info['details'] = None
    info['soc'] = repo_info['soc']
    info['os'] = repo_info['os']
    info['server'] = None
    info['vcs'] = base.rtk_utils.get_current_vcs()
    info['metaTool'] = repo_info['meta']
    info['metaToolPath'] = os.path.join('tools', 'meta_tools', repo_info['meta'])
    info['repoUrl'] = None
    info['toolchainUrl'] = None
    info['revision'] = None
    info['worktree'] = False

    rc = base.rtk_utils.is_repo_workspace()
    if rc:
        dom = xml.dom.minidom.parse(LOONG_SDK_MANIFEST_FILE)
        root = dom.documentElement
        item_list = root.getElementsByTagName('include')
        item = item_list[0]
        info['manifest'] = item.getAttribute('name')
        info['manifestDir'] = None

        rc = base.rtk_utils.run_command(CMD_GET_MANIFEST_URL)
        if rc.returncode == 0:
            info['url'] = rc.stdout.strip()
        else:
            raise RuntimeError('Fail to get manifest url: ' + rc.stderr)

        if 'github' in info['url']:
            info['internal'] = False
        else:
            info['internal'] = True
    else:
        info['manifest'] = None
        info['manifestDir'] = None
        info['url'] = None
        info['internal'] = False

    info['debuggerArgs'] = repo_info['debuggerArgs']
    info['overrideLaunchCommands'] = repo_info['overrideLaunchCommands']

    print(json.dumps(info, indent=4))

def do_query_compatibility(cfg):
    # Query against configuration file
    ret = json.dumps(cfg['compatibilityInfo'], indent=4)
    print(ret)


def main():
    parser = argparse.ArgumentParser(description=None)
    parser.add_argument('-a', '--app', action='store_true', help='show application information')
    parser.add_argument('-d', '--device', action='store_true', help='show device information')
    parser.add_argument('-i', '--info', action='store_true', help='show repository information')
    parser.add_argument('-c', '--compatibility', action='store_true', help='show compatibility information')

    args = parser.parse_args()

    cfg = None
    if os.path.exists(base.rtk_utils.LOONG_SDK_QUERY_CFG_FILE):
        try:
            with open(base.rtk_utils.LOONG_SDK_QUERY_CFG_FILE, 'r') as f:
                cfg = json.load(f)
        except:
            raise RuntimeError('Error: Fail to load query configuration file "' + base.rtk_utils.LOONG_SDK_QUERY_CFG_FILE + '"')
    else:
        raise RuntimeError('Error: Query configuration file "' + base.rtk_utils.LOONG_SDK_QUERY_CFG_FILE + '" does not exist')

    # Meta tool should only run under SDK root directory
    sdkroot = os.getcwd()

    if args.app:
        do_query_app(sdkroot, cfg)
    elif args.device:
        do_query_device(cfg)
    elif args.info:
        do_query_info(sdkroot, cfg)
    elif args.compatibility:
        do_query_compatibility(cfg)
    else:
        parser.print_usage()
        raise RuntimeError('Warning: Invalid arguments')


if __name__ == '__main__':
    main()
