#!/usr/bin/env python

import os
import argparse


parser = argparse.ArgumentParser()
parser.add_argument("ns_target_name")
parser.add_argument("s_target_name")

args = parser.parse_args()

ns_target_name = args.ns_target_name
s_target_name = args.s_target_name


def AddSpace(total_len, str_len):
    space = ''
    SpaceNum = total_len - str_len
    space = ' ' * SpaceNum
    return space


def read_rom_size(target_name):
    contents = []

    if os.path.exists(target_name):
        with open(target_name, 'r', encoding='utf-8') as save_f:
            source_lines = save_f.read().splitlines()
        # os.remove(target_name)

        for line in source_lines:
            content = []
            datas = line.split(' ')
            if datas[-1] == '(TOTALS)':
                for data in datas:
                    if data != '':
                        content.append(data)
                contents.append(content)
            elif datas[-1] == '(AUTO_TOTALS)':
                return 0

        sum0 = str(sum(int(num[0]) for num in contents))
        sum1 = str(sum(int(num[1]) for num in contents))
        sum2 = str(sum(int(num[2]) for num in contents))
        sum3 = str(sum(int(num[3]) for num in contents))

        with open(target_name, 'a', encoding='utf-8') as w_f:

            w_f.write('=============================================================')

            w_f.write('\n')
            w_f.write('%s%s%s%s%s%s%s%s (AUTO_TOTALS)' % (
                AddSpace(10, len(sum0)), sum0, AddSpace(11, len(sum1)), sum1, AddSpace(11, len(sum2)), sum2,
                AddSpace(11, len(sum3)), sum3))

        w_f.close()
    else:
        return 0

#print("total_rom_size.py")
read_rom_size(ns_target_name)
read_rom_size(s_target_name)






























