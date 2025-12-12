import os
import re
import sys
from typing import List, Tuple, Optional, Dict, Any

soc_to_drv_file = {
    'amebadplus':     'autoconf_8721da.h',
    'amebad':         'autoconf_8721d.h',
    'amebalite':      'autoconf_8720e.h',
    'amebasmartplus': 'autoconf_8730e.h',
    'amebasmart':     'autoconf_8730e.h',
    'amebagreen2':    'autoconf_8721f.h',
    'amebaL2':        'autoconf_amebax.h',
    'RTL8720F':       'autoconf_8720f.h',
    'amebapro3':      'autoconf_8721f.h'
}

def extract_version_from_file(file_path: str, check_marco_name: str) -> Tuple[int, List[Dict[str, Any]]]:
    """
    仅支持以下两种形式的宏定义，并提取所有出现的值和 since 信息：
      #define <MACRO> <NUM> [/* since <VER> */]
      //#define <MACRO> <NUM> [/* since <VER> */]

    返回:
      - active_value: 最后一个未注释（非 // 前缀）的定义值；若无则为 1
      - all_defs: 列表，包含所有匹配项：
          {
            "value": int,           # 支持十进制或 0x 十六进制
            "since": Optional[str], # 来自 /* since <VER> */，没有则为 None
            "active": bool          # True=未注释定义；False=行注释定义
          }
    """

    if not os.path.exists(file_path):
        print(f"error: {file_path} not found")
        return None, []

    with open(file_path, 'r', encoding='utf-8') as f:
        content = f.read()

    lines = content.splitlines(keepends=False) # 行级遍历，确保不处理块注释包裹的情况

    # 定义匹配：捕获是否为行注释（通过是否以 // 开头判断）
    # 示例：
    #   #define MACRO 1 /* since 1.1.15 */
    #   //#define MACRO 2 /* since 1.1.19 */
    define_pattern = re.compile(
        rf'^\s*(//)?\s*#\s*define\s+{re.escape(check_marco_name)}\s+(0x[0-9A-Fa-f]+|\d+)\b',
        re.ASCII
    )
    since_pattern = re.compile(r'/\*\s*since\s+([0-9]+(?:\.[0-9]+){0,3})\s*\*/', re.IGNORECASE)

    all_defs = []
    active_value = 1  # for backward compatibility, old SDK/tool without VER macro default to Version 1.

    for line in lines:
        macro_match = define_pattern.search(line)
        if not macro_match:
            continue

        is_line_commented = macro_match.group(1) is not None  # 有 // 则为注释
        val_str = macro_match.group(2)

        # 解析数值
        try:
            value = int(val_str, 16) if val_str.lower().startswith('0x') else int(val_str)
        except ValueError:
            # 非法数值，跳过
            continue

        # 提取 since 版本
        since_match = since_pattern.search(line)
        since_ver = since_match.group(1) if since_match else None

        entry = {
            "value": value,
            "since": since_ver,
            "active": not is_line_commented
        }
        all_defs.append(entry)

        if not is_line_commented:
            # 采用最后一个未注释定义为 active_value
            active_value = value

    return active_value, all_defs

def find_since_by_value(tool_verion_list: List[Dict], value: int) -> Optional[str]:
    """
    在 tool_verion_list 中查找指定 value 对应的 since 字符串。
    不区分 active 与否，只要存在该 value 且有 since 就返回。
    """
    for item in tool_verion_list:
        # 兼容 0x 写法与十进制：统一用 int() 比较
        if int(item.get("value")) == int(value):
            return item.get("since")  # 若无 since 或 None，直接返回 None
    return None

def version_compare(drv_record_version, tool_record_version, drv_version_list, tool_verion_list, fname) -> bool:
    """
    比较驱动记录版本与当前激活的工具版本，并打印合理的版本区间提示。
    规则：
      - 两者均不为 -1 时才比较。
      - 相等返回 True。
      - tool > drv：提示工具过新，区间基于 drv 及 drv+1 的 since。
      - tool < drv：提示工具过旧，区间基于 drv 及 drv+1 的 since。
    """
    if drv_record_version != -1 and tool_record_version != -1:
        if tool_record_version == drv_record_version:
            return True

        log_str = f" Replace the correct version of the tool and regenerate {fname}"
        # 工具过新：tool > drv
        if tool_record_version > drv_record_version:
            status = "new"
            lower_since = find_since_by_value(tool_verion_list, drv_record_version)
            upper_since = find_since_by_value(tool_verion_list, drv_record_version + 1)
        # 工具过旧：tool < drv
        elif tool_record_version < drv_record_version:
            status = "old"
            lower_since = find_since_by_value(drv_version_list, drv_record_version)
            upper_since = find_since_by_value(drv_version_list, drv_record_version + 1)

        if lower_since and upper_since:
            print(f"\n[ERROR] GenTxPowerTool version too {status}, should be [{lower_since}, {upper_since})." + log_str)
        elif lower_since and not upper_since:
            print(f"\n[ERROR] GenTxPowerTool version too {status}, should be >= {lower_since}." + log_str)
        elif not lower_since and upper_since:
            print(f"\n[ERROR] GenTxPowerTool version too {status}, should be < {upper_since}." + log_str)
        else:
            print(f"\n[ERROR] GenTxPowerTool version too {status}." + log_str)

    return False

def version_check(soc_type, cmpt_wifi_dir, cmpt_usrcfg_dir, driver_marco_name, tool_marco_name) -> bool:

    drv_record_file_name = soc_to_drv_file.get(soc_type, None)
    if not drv_record_file_name:
        print(f'error: autoconf h file of {soc_type} does not exist')
        return False

    drv_record_file = os.path.join(cmpt_wifi_dir, drv_record_file_name)
    drv_record_version, drv_version_list = extract_version_from_file(drv_record_file, driver_marco_name)

    tool_files = ['ameba_wifi_power_table_usrcfg.c']
    if soc_type == 'amebadplus':
        tool_files += [
            'ameba_wifi_power_table_usrcfg_1v8.c',
            'ameba_wifi_power_table_usrcfg_3v3_1v25.c'
        ]
    elif soc_type == 'amebad':
        tool_files += [
            'ameba_wifi_power_table_usrcfg_1v8.c'
        ]

    for fname in tool_files:
        tool_record_file = os.path.join(cmpt_usrcfg_dir, fname)
        tool_record_version, tool_verion_list = extract_version_from_file(tool_record_file, tool_marco_name)
        if not version_compare(drv_record_version, tool_record_version, drv_version_list, tool_verion_list, fname):
            return False
    return True

def main():

    if len(sys.argv) != 6:
        print("Usage: python script.py <SOC_TYPE> <CMPT_WIFI_DIR> <CMPT_USRCFG_DIR> <DRIVER_VERSION_MACRO_NAME> <TOOL_VERSION_MACRO_NAME>")
        sys.exit(1)

    soc_type = sys.argv[1]     # SOC type
    cmpt_wifi_dir = sys.argv[2]  # CMPT_WIFI_DIR
    cmpt_usrcfg_dir = sys.argv[3]  # CMPT_USRCFG_DIR
    driver_marco_name = sys.argv[4]  # driver macro name for GenTxPowerTool ver
    tool_marco_name = sys.argv[5]  # tool macro name for GenTxPowerTool ver

    if not version_check(soc_type, cmpt_wifi_dir, cmpt_usrcfg_dir, driver_marco_name, tool_marco_name):
        sys.exit(1)

if __name__ == '__main__':

    main()

