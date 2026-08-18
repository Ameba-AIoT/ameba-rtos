#!/usr/bin/env bash
#
# setup_serial_alias.sh
#
# 功能：
#   为自动化测试环境创建固定串口别名，例如：
#     /dev/dut0 -> /dev/ttyACM0
#     /dev/dut1 -> /dev/ttyACM1
#
# 特点：
#   1. 别名由固定列表提供，使用 --config <file> 从外部 JSON5 文件加载，
#      或使用脚本内置的默认列表（详见脚本底部的 FIXED_ALIASES）。
#   2. 用户只需要选择实际串口设备绑定到哪个固定别名。
#   3. 已经绑定过的设备不会在后续选择列表中显示。
#   4. 剩余设备序号会重新从 1 开始。
#   5. 使用 udev 的 ENV{ID_PATH} 绑定 USB 物理路径。
#   6. 适合自动化测试脚本稳定访问固定设备名。
#
# 绑定方式说明：
#   本脚本使用 udev 的 ENV{ID_PATH} 创建固定串口别名。
#   ID_PATH 表示 USB 设备连接到主机时的物理路径，例如：
#     pci-0000:00:14.0-usb-0:9.4:1.0
#
#   因此，本脚本绑定的是“USB 物理端口位置”，不是下面这些动态信息：
#     1. 不是绑定 /dev/ttyACM0、/dev/ttyACM1、/dev/ttyUSB0 等临时设备名
#     2. 不是绑定设备插入后的枚举顺序
#     3. 不是主要依赖设备序列号 Serial
#
# 需要重新配置的情况：
#   1. 串口设备换到了另一个 USB 口
#   2. 串口设备换到了另一个 Hub 口
#   3. 更换了 USB Hub，导致 USB 拓扑路径变化
#   4. 换了一台测试主机
#   5. 想修改 /dev/dut0、/dev/dut1 与实际设备之间的对应关系
#   6. 通过 --config <file> 传入的 JSON5 配置文件里增删了别名
#   7. 设备类型或接口发生变化，导致 ID_PATH、接口号或端口号变化
#
# 不需要重新配置的情况：
#   1. 系统重启后，设备仍然插在原来的 USB 口或 Hub 口
#   2. 设备拔掉后重新插回原来的 USB 口或 Hub 口
#   3. /dev/ttyACM0、/dev/ttyACM1、/dev/ttyUSB0 等动态编号发生变化
#   4. 同一个 USB 物理位置上的设备枚举顺序发生变化
#   5. udev 规则文件没有改变，只是需要重新触发规则
#
# 如果别名没有立即出现：
#   可以尝试拔插设备，或执行：
#     sudo udevadm trigger --action=add --subsystem-match=tty
#


set -Eeuo pipefail

# ============================================================
# 固定别名列表
# ============================================================
# 自动化测试脚本应该固定使用这些名字。
# 用户不能自定义别名，避免脚本抓不到正确设备。
#
# 本脚本支持两种方式指定别名列表：
#   1. 传 --config <file.json5>（推荐），从外部 JSON5 文件加载。
#      可重复指定以加载多个配置（例如：--config rtos.json5 --config zephyr.json5）。
#   2. 不传 --config 时，使用下方 FIXED_ALIASES 内置默认列表。
#
# JSON5 配置文件格式示例：
#   {
#     aliases: [
#       "r_rtl8720f_dut0",   // RTOS 主 DUT
#       "z_rtl872xda_evb_0", // Zephyr EVB
#     ]
#   }
#
# 示例：
#   /dev/dut0
#   /dev/dut1
#
# 如需修改内置默认列表，可以改成：
#   FIXED_ALIASES=("dut0" "dut1" "log_uart" "cmd_uart")
#
# 注意：
#   这里不要写 /dev/ 前缀。
#
# r_rtl8720f_dut0 / r_rtl8720f_dut1  — rtos peripheral_test 团队使用，请勿改名/挪用。
# z_rtl872xda_evb_0  — zephyr_test 团队使用（本机 ttyUSB0，AmebaDplus）。
#                       新增其他板子用 z_ + 完整 board 名 + 序号，不要用
#                       872xd/872xda 这类容易看错的缩写。
FIXED_ALIASES=("r_rtl8720f_dut0" "r_rtl8720f_dut1" "r_rtl8720f_dut2" "z_rtl872xda_evb_0" "z_rtl8721f_evb_0" "z_rtl8730e_evb_0")

# udev 规则文件路径
RULE_FILE="/etc/udev/rules.d/99-fixed-serial-alias.rules"

# 串口权限
PERM_PART='MODE="0666"'

# 规则文件备份保留份数：只保留最近 N 份，更早的备份自动删除，
# 避免 /etc/udev/rules.d/ 里 .bak 文件无限堆积。
BACKUP_KEEP=1

SCRIPT_NAME="$(basename "$0")"
TMP_RULE="$(mktemp)"

trap 'rm -f "$TMP_RULE"' EXIT

# ============================================================
# 全局数组
# ============================================================

# 当前检测到的串口设备列表
SERIAL_DEVICES=()

# 当前可选择的串口设备列表
# 已绑定设备会被过滤掉
AVAILABLE_DEVICES=()

# 用户选择后的绑定关系
BIND_ALIASES=()
BIND_DEVICES=()
BIND_ID_PATHS=()
BIND_IFACES=()
BIND_PORTS=()

# ============================================================
# 基础工具函数
# ============================================================

# 从 JSON5 配置文件中读取别名列表并输出（每行一个）。
# 使用 Python 解析，与 pytest_cmds.json5 共用 json5 库。
read_aliases_from_json5() {
    local file="$1"

    if [[ ! -f "$file" ]]; then
        echo "错误：配置文件不存在：$file"
        exit 1
    fi

    python3 -c "
import json5, sys
try:
    with open(sys.argv[1]) as f:
        data = json5.load(f)
except Exception as e:
    print(f'解析配置文件失败: {e}', file=sys.stderr)
    sys.exit(1)
aliases = data.get('aliases', [])
for alias in aliases:
    print(alias)
sys.stdout.flush()
sys.exit(0)

" "$file"
}

require_cmd() {
    local cmd="$1"

    if ! command -v "$cmd" >/dev/null 2>&1; then
        echo "错误：缺少命令：$cmd"
        exit 1
    fi
}

ask_yes_no() {
    local prompt="$1"
    local default="${2:-N}"
    local ans

    if [[ "$default" == "Y" || "$default" == "y" ]]; then
        read -r -p "$prompt [Y/n]: " ans
        ans="${ans:-Y}"
    else
        read -r -p "$prompt [y/N]: " ans
        ans="${ans:-N}"
    fi

    case "$ans" in
        y|Y|yes|YES|Yes)
            return 0
            ;;
        *)
            return 1
            ;;
    esac
}

get_prop() {
    local dev="$1"
    local key="$2"

    udevadm info -q property -n "$dev" 2>/dev/null \
        | awk -F= -v k="$key" '$1 == k {print substr($0, index($0, "=") + 1); exit}'
}

escape_udev_value() {
    local s="$1"

    s="${s//\\/\\\\}"
    s="${s//\"/\\\"}"

    printf '%s' "$s"
}

validate_fixed_aliases() {
    local alias
    local i
    local j

    if [[ "${#FIXED_ALIASES[@]}" -eq 0 ]]; then
        echo "错误：FIXED_ALIASES 不能为空。"
        exit 1
    fi

    for alias in "${FIXED_ALIASES[@]}"; do
        if [[ -z "$alias" ]]; then
            echo "错误：FIXED_ALIASES 中存在空别名。"
            exit 1
        fi

        if [[ "$alias" == /* ]]; then
            echo "错误：固定别名不要带 /dev/ 前缀：$alias"
            echo "请写成 dut0，而不是 /dev/dut0。"
            exit 1
        fi

        if ! [[ "$alias" =~ ^[A-Za-z0-9._-]+$ ]]; then
            echo "错误：固定别名包含非法字符：$alias"
            echo "只允许使用字母、数字、点号、下划线和短横线。"
            exit 1
        fi
    done

    for i in "${!FIXED_ALIASES[@]}"; do
        for j in "${!FIXED_ALIASES[@]}"; do
            if (( i < j )) && [[ "${FIXED_ALIASES[$i]}" == "${FIXED_ALIASES[$j]}" ]]; then
                echo "错误：FIXED_ALIASES 中存在重复别名：${FIXED_ALIASES[$i]}"
                exit 1
            fi
        done
    done
}

# ============================================================
# 串口设备扫描和显示
# ============================================================

find_serial_devices() {
    local d

    SERIAL_DEVICES=()

    shopt -s nullglob

    for d in /dev/ttyUSB* /dev/ttyACM*; do
        if [[ -e "$d" ]]; then
            SERIAL_DEVICES+=("$d")
        fi
    done

    shopt -u nullglob

    if [[ "${#SERIAL_DEVICES[@]}" -gt 0 ]]; then
        mapfile -t SERIAL_DEVICES < <(printf '%s\n' "${SERIAL_DEVICES[@]}" | sort -V)
    fi
}

get_by_path_link() {
    local dev="$1"
    local resolved_dev
    local link

    resolved_dev="$(readlink -f "$dev" 2>/dev/null || true)"

    shopt -s nullglob

    for link in /dev/serial/by-path/*; do
        if [[ "$(readlink -f "$link" 2>/dev/null || true)" == "$resolved_dev" ]]; then
            basename "$link"
            shopt -u nullglob
            return 0
        fi
    done

    shopt -u nullglob

    echo "-"
}

show_devices() {
    local i
    local dev
    local id_path
    local model
    local vendor_id
    local product_id
    local serial
    local by_path

    echo
    echo "当前可选择的串口设备："
    echo "--------------------------------------------------------------------------------"
    printf "%-4s %-14s %-22s %-12s %-16s %s\n" \
        "序号" "设备" "型号" "VID:PID" "Serial" "by-path"
    echo "--------------------------------------------------------------------------------"

    if [[ "${#AVAILABLE_DEVICES[@]}" -eq 0 ]]; then
        echo "没有可选择的串口设备。"
        echo "可能原因："
        echo "  1. 没有检测到 /dev/ttyUSB* 或 /dev/ttyACM* 设备"
        echo "  2. 所有串口设备都已经被绑定"
        echo "--------------------------------------------------------------------------------"
    else
        for i in "${!AVAILABLE_DEVICES[@]}"; do
            dev="${AVAILABLE_DEVICES[$i]}"

            id_path="$(get_prop "$dev" "ID_PATH")"
            model="$(get_prop "$dev" "ID_MODEL")"
            vendor_id="$(get_prop "$dev" "ID_VENDOR_ID")"
            product_id="$(get_prop "$dev" "ID_MODEL_ID")"
            serial="$(get_prop "$dev" "ID_SERIAL_SHORT")"
            by_path="$(get_by_path_link "$dev")"

            [[ -z "$model" ]] && model="-"
            [[ -z "$vendor_id" ]] && vendor_id="----"
            [[ -z "$product_id" ]] && product_id="----"
            [[ -z "$serial" ]] && serial="-"
            [[ -z "$id_path" ]] && id_path="-"
            [[ -z "$by_path" ]] && by_path="-"

            printf "%-4s %-14s %-22s %-12s %-16s %s\n" \
                "$((i + 1))" \
                "$dev" \
                "$model" \
                "${vendor_id}:${product_id}" \
                "$serial" \
                "$by_path"
        done

        echo "--------------------------------------------------------------------------------"
    fi

    show_bound_devices
}

# 按 ID_PATH(+接口号/端口号) 在当前实际串口设备里反查设备名。
# 动态名字（/dev/ttyACM*）会变，绑定信息里的名字可能是历史值，
# 所以显示时用稳定的 ID_PATH 反查当前真实名字。
find_dev_by_idpath() {
    local id_path="$1"
    local iface="$2"
    local port="$3"
    local d

    for d in "${SERIAL_DEVICES[@]}"; do
        if [[ "$(get_prop "$d" "ID_PATH")" == "$id_path" \
            && "$(get_prop "$d" "ID_USB_INTERFACE_NUM")" == "$iface" \
            && "$(get_prop "$d" "ID_PORT")" == "$port" ]]; then
            printf '%s' "$d"
            return 0
        fi
    done

    return 1
}

# 显示已经绑定、因而被从可选列表中过滤掉的端口，避免"设备静默消失"。
show_bound_devices() {
    local i
    local cur

    if [[ "${#BIND_ALIASES[@]}" -eq 0 ]]; then
        return 0
    fi

    echo
    echo "已绑定端口（不会出现在上面的可选列表中）："
    echo "--------------------------------------------------------------------------------"
    printf "%-20s %-14s %s\n" "固定别名" "当前设备" "ID_PATH"
    echo "--------------------------------------------------------------------------------"

    for i in "${!BIND_ALIASES[@]}"; do
        cur="$(find_dev_by_idpath "${BIND_ID_PATHS[$i]}" "${BIND_IFACES[$i]}" "${BIND_PORTS[$i]}" || true)"
        [[ -z "$cur" ]] && cur="(未插入)"

        printf "%-20s %-14s %s\n" \
            "/dev/${BIND_ALIASES[$i]}" \
            "$cur" \
            "${BIND_ID_PATHS[$i]:--}"
    done

    echo "--------------------------------------------------------------------------------"
}

show_device_detail() {
    local dev="$1"
    local id_path
    local iface
    local port
    local model
    local vendor_id
    local product_id
    local serial
    local by_path

    id_path="$(get_prop "$dev" "ID_PATH")"
    iface="$(get_prop "$dev" "ID_USB_INTERFACE_NUM")"
    port="$(get_prop "$dev" "ID_PORT")"
    model="$(get_prop "$dev" "ID_MODEL")"
    vendor_id="$(get_prop "$dev" "ID_VENDOR_ID")"
    product_id="$(get_prop "$dev" "ID_MODEL_ID")"
    serial="$(get_prop "$dev" "ID_SERIAL_SHORT")"
    by_path="$(get_by_path_link "$dev")"

    echo
    echo "设备详情：$dev"
    echo "----------------------------------------"
    echo "ID_PATH              : ${id_path:-无}"
    echo "ID_USB_INTERFACE_NUM : ${iface:-无}"
    echo "ID_PORT              : ${port:-无}"
    echo "ID_MODEL             : ${model:-无}"
    echo "ID_VENDOR_ID         : ${vendor_id:-无}"
    echo "ID_MODEL_ID          : ${product_id:-无}"
    echo "ID_SERIAL_SHORT      : ${serial:-无}"
    echo "by-path              : ${by_path:-无}"
    echo "----------------------------------------"
}

# ============================================================
# 绑定状态判断
# ============================================================

device_already_bound() {
    local dev="$1"
    local i

    for i in "${!BIND_DEVICES[@]}"; do
        if [[ "${BIND_DEVICES[$i]}" == "$dev" ]]; then
            return 0
        fi
    done

    return 1
}

alias_already_bound() {
    local alias="$1"
    local i

    for i in "${!BIND_ALIASES[@]}"; do
        if [[ "${BIND_ALIASES[$i]}" == "$alias" ]]; then
            return 0
        fi
    done

    return 1
}

# 判断某个物理路径（ID_PATH + 接口号 + 端口号）是否已经被绑定。
# 绑定的稳定标识是 ID_PATH，而不是动态的 /dev/ttyACM* 名字，
# 所以过滤可选设备时必须按 ID_PATH 比较，否则同一物理端口会因为
# 动态名字变化（例如重启后 ttyACM0 变成 ttyACM1）被重复绑定到多个别名。
idpath_already_bound() {
    local id_path="$1"
    local iface="$2"
    local port="$3"
    local i

    for i in "${!BIND_ID_PATHS[@]}"; do
        if [[ "${BIND_ID_PATHS[$i]}" == "$id_path" \
            && "${BIND_IFACES[$i]}" == "$iface" \
            && "${BIND_PORTS[$i]}" == "$port" ]]; then
            return 0
        fi
    done

    return 1
}

build_available_devices() {
    local dev
    local id_path
    local iface
    local port

    AVAILABLE_DEVICES=()

    for dev in "${SERIAL_DEVICES[@]}"; do
        if device_already_bound "$dev"; then
            continue
        fi

        id_path="$(get_prop "$dev" "ID_PATH")"
        iface="$(get_prop "$dev" "ID_USB_INTERFACE_NUM")"
        port="$(get_prop "$dev" "ID_PORT")"

        # 即使动态名字已经变化，只要物理路径已被绑定就不再列出。
        if [[ -n "$id_path" ]] && idpath_already_bound "$id_path" "$iface" "$port"; then
            continue
        fi

        AVAILABLE_DEVICES+=("$dev")
    done
}

# 读取磁盘上已有的规则文件，把其他团队/其他别名已经配置好的绑定
# 预先加载进 BIND_* 数组。避免本次运行只处理自己的别名时，把别人已经
# 配置好的绑定从新生成的规则文件里漏掉。
load_existing_bindings() {
    local line
    local dev=""
    local alias
    local id_path
    local iface
    local port

    [[ -e "$RULE_FILE" ]] || return 0

    while IFS= read -r line; do
        if [[ "$line" =~ ^#\ (.+)\ -\>\ /dev/(.+)$ ]]; then
            dev="${BASH_REMATCH[1]}"
            alias="${BASH_REMATCH[2]}"
            continue
        fi

        if [[ -n "$dev" && "$line" == SUBSYSTEM==* ]]; then
            id_path="$(sed -n 's/.*ENV{ID_PATH}=="\([^"]*\)".*/\1/p' <<<"$line")"
            iface="$(sed -n 's/.*ENV{ID_USB_INTERFACE_NUM}=="\([^"]*\)".*/\1/p' <<<"$line")"
            port="$(sed -n 's/.*ENV{ID_PORT}=="\([^"]*\)".*/\1/p' <<<"$line")"

            BIND_ALIASES+=("$alias")
            BIND_DEVICES+=("$dev")
            BIND_ID_PATHS+=("$id_path")
            BIND_IFACES+=("$iface")
            BIND_PORTS+=("$port")
            dev=""
        fi
    done < "$RULE_FILE"
}

# 判断别名是否在固定别名列表 FIXED_ALIASES 中。
is_fixed_alias() {
    local target="$1"
    local a

    for a in "${FIXED_ALIASES[@]}"; do
        if [[ "$a" == "$target" ]]; then
            return 0
        fi
    done

    return 1
}

# 清理"孤儿别名"：磁盘规则文件里存在、但已不在 FIXED_ALIASES 中的别名
# （通常是改名/下线后遗留的旧别名）。逐个询问是否删除，默认保留，
# 避免误删其他团队新增的别名。被删除的别名不会写回新规则文件。
prune_unknown_aliases() {
    local i
    local cur
    local found_unknown=0
    local keep_aliases=()
    local keep_devices=()
    local keep_id_paths=()
    local keep_ifaces=()
    local keep_ports=()

    for i in "${!BIND_ALIASES[@]}"; do
        if is_fixed_alias "${BIND_ALIASES[$i]}"; then
            keep_aliases+=("${BIND_ALIASES[$i]}")
            keep_devices+=("${BIND_DEVICES[$i]}")
            keep_id_paths+=("${BIND_ID_PATHS[$i]}")
            keep_ifaces+=("${BIND_IFACES[$i]}")
            keep_ports+=("${BIND_PORTS[$i]}")
            continue
        fi

        found_unknown=1
        cur="$(find_dev_by_idpath "${BIND_ID_PATHS[$i]}" "${BIND_IFACES[$i]}" "${BIND_PORTS[$i]}" || true)"
        [[ -z "$cur" ]] && cur="(未插入)"

        echo
        echo "发现未知别名（不在固定别名列表 FIXED_ALIASES 中）："
        echo "  固定别名 : /dev/${BIND_ALIASES[$i]}"
        echo "  当前设备 : ${cur}"
        echo "  ID_PATH  : ${BIND_ID_PATHS[$i]:--}"
        echo "  它可能是改名/下线后遗留的旧别名，也可能是其他团队新增的别名。"

        if ask_yes_no "删除该别名（不再写回规则文件）？" "N"; then
            echo "将删除：/dev/${BIND_ALIASES[$i]}"
            continue
        fi

        echo "保留：/dev/${BIND_ALIASES[$i]}"
        keep_aliases+=("${BIND_ALIASES[$i]}")
        keep_devices+=("${BIND_DEVICES[$i]}")
        keep_id_paths+=("${BIND_ID_PATHS[$i]}")
        keep_ifaces+=("${BIND_IFACES[$i]}")
        keep_ports+=("${BIND_PORTS[$i]}")
    done

    # 没有孤儿别名时不动数组（同时规避空数组在 set -u 下的边界情况）。
    if [[ "$found_unknown" -eq 0 ]]; then
        return 0
    fi

    BIND_ALIASES=("${keep_aliases[@]}")
    BIND_DEVICES=("${keep_devices[@]}")
    BIND_ID_PATHS=("${keep_id_paths[@]}")
    BIND_IFACES=("${keep_ifaces[@]}")
    BIND_PORTS=("${keep_ports[@]}")
}

# ============================================================
# 固定别名绑定逻辑
# ============================================================

bind_fixed_alias() {
    local alias="$1"
    local input
    local idx
    local dev
    local id_path
    local iface
    local port

    echo
    echo "============================================================"
    echo "配置固定别名：/dev/${alias}"
    echo "============================================================"

    if alias_already_bound "$alias"; then
        for idx in "${!BIND_ALIASES[@]}"; do
            [[ "${BIND_ALIASES[$idx]}" == "$alias" ]] || continue

            echo
            echo "检测到 /dev/${alias} 已有绑定（来自磁盘上已有的规则文件）："
            echo "  实际设备 : ${BIND_DEVICES[$idx]}"
            echo "  ID_PATH  : ${BIND_ID_PATHS[$idx]}"

            if ask_yes_no "保留现有绑定，不重新选择设备？" "Y"; then
                return 0
            fi

            # 用户选择重新配置：摘除旧绑定，走正常选择流程。
            BIND_ALIASES=("${BIND_ALIASES[@]:0:$idx}" "${BIND_ALIASES[@]:$((idx + 1))}")
            BIND_DEVICES=("${BIND_DEVICES[@]:0:$idx}" "${BIND_DEVICES[@]:$((idx + 1))}")
            BIND_ID_PATHS=("${BIND_ID_PATHS[@]:0:$idx}" "${BIND_ID_PATHS[@]:$((idx + 1))}")
            BIND_IFACES=("${BIND_IFACES[@]:0:$idx}" "${BIND_IFACES[@]:$((idx + 1))}")
            BIND_PORTS=("${BIND_PORTS[@]:0:$idx}" "${BIND_PORTS[@]:$((idx + 1))}")
            break
        done
    fi

    # 每次配置固定别名前，重新生成可选设备列表。
    # 已经绑定过的设备不会再显示。
    build_available_devices

    show_devices

    if [[ "${#AVAILABLE_DEVICES[@]}" -eq 0 ]]; then
        echo
        echo "没有可用于绑定 /dev/${alias} 的设备，跳过。"
        return 0
    fi

    echo
    echo "请为固定别名 /dev/${alias} 选择实际串口设备。"
    echo "已绑定给其他固定别名的设备不会出现在列表中。"
    echo "如果该别名当前不需要配置，直接回车跳过。"
    echo

    while true; do
        read -r -p "请输入设备序号，或回车跳过 /dev/${alias}: " input

        if [[ -z "$input" ]]; then
            echo "跳过：/dev/${alias}"
            return 0
        fi

        if ! [[ "$input" =~ ^[0-9]+$ ]]; then
            echo "错误：请输入数字序号。"
            continue
        fi

        if (( input < 1 || input > ${#AVAILABLE_DEVICES[@]} )); then
            echo "错误：序号超出范围。"
            continue
        fi

        idx="$((input - 1))"
        dev="${AVAILABLE_DEVICES[$idx]}"

        if alias_already_bound "$alias"; then
            echo "错误：/dev/${alias} 已经绑定过。"
            return 1
        fi

        if device_already_bound "$dev"; then
            echo "错误：$dev 已经绑定到其他固定别名，请选择其他设备。"
            continue
        fi

        id_path="$(get_prop "$dev" "ID_PATH")"
        iface="$(get_prop "$dev" "ID_USB_INTERFACE_NUM")"
        port="$(get_prop "$dev" "ID_PORT")"

        if [[ -z "$id_path" ]]; then
            echo "错误：$dev 没有 ID_PATH，无法按 ID_PATH 方式绑定。"
            echo "建议检查：udevadm info -q property -n $dev"
            continue
        fi

        show_device_detail "$dev"

        echo
        echo "即将添加绑定："
        echo "  固定别名 : /dev/${alias}"
        echo "  实际设备 : ${dev}"
        echo "  ID_PATH  : ${id_path}"

        if [[ -n "$iface" ]]; then
            echo "  接口号   : ${iface}"
        else
            echo "  接口号   : 无"
        fi

        if [[ -n "$port" ]]; then
            echo "  端口号   : ${port}"
        else
            echo "  端口号   : 无"
        fi

        echo "  权限     : ${PERM_PART}"

        if ask_yes_no "确认将 ${dev} 绑定为 /dev/${alias}？" "Y"; then
            BIND_ALIASES+=("$alias")
            BIND_DEVICES+=("$dev")
            BIND_ID_PATHS+=("$id_path")
            BIND_IFACES+=("$iface")
            BIND_PORTS+=("$port")

            echo "已添加：${dev} -> /dev/${alias}"
            return 0
        else
            echo "已取消，请重新选择。"
        fi
    done
}

show_bindings() {
    local i

    echo
    echo "最终绑定关系："
    echo "--------------------------------------------------------------------------------"

    if [[ "${#BIND_ALIASES[@]}" -eq 0 ]]; then
        echo "暂无绑定。"
    else
        printf "%-4s %-20s %-14s %s\n" \
            "序号" "固定别名" "实际设备" "ID_PATH"
        echo "--------------------------------------------------------------------------------"

        for i in "${!BIND_ALIASES[@]}"; do
            printf "%-4s %-20s %-14s %s\n" \
                "$((i + 1))" \
                "/dev/${BIND_ALIASES[$i]}" \
                "${BIND_DEVICES[$i]}" \
                "${BIND_ID_PATHS[$i]}"
        done
    fi

    echo "--------------------------------------------------------------------------------"
}

# ============================================================
# udev 规则生成
# ============================================================

build_rule_line() {
    local i="$1"
    local alias
    local id_path
    local iface
    local port
    local line

    alias="$(escape_udev_value "${BIND_ALIASES[$i]}")"
    id_path="$(escape_udev_value "${BIND_ID_PATHS[$i]}")"
    iface="$(escape_udev_value "${BIND_IFACES[$i]}")"
    port="$(escape_udev_value "${BIND_PORTS[$i]}")"

    line="SUBSYSTEM==\"tty\", ENV{ID_PATH}==\"${id_path}\""

    # 对于复合 USB 设备，增加接口号限制，避免误匹配。
    if [[ -n "$iface" ]]; then
        line+=", ENV{ID_USB_INTERFACE_NUM}==\"${iface}\""
    fi

    # 对于多端口 USB 串口，增加 ID_PORT 限制。
    if [[ -n "$port" ]]; then
        line+=", ENV{ID_PORT}==\"${port}\""
    fi

    line+=", SYMLINK+=\"${alias}\", ${PERM_PART}"

    printf '%s' "$line"
}

generate_rule_file() {
    local i

    {
        echo "# This file is generated by ${SCRIPT_NAME}"
        echo "#"
        echo "# 功能："
        echo "#   为自动化测试创建固定串口别名。"
        echo "#"
        echo "# 说明："
        echo "#   1. 别名来自脚本内置固定列表，用户不能自由输入。"
        echo "#   2. 自动化测试脚本应使用这些固定别名，例如 /dev/dut0、/dev/dut1。"
        echo "#   3. 本规则使用 ENV{ID_PATH} 绑定 USB 物理路径。"
        echo "#   4. 如果更换 USB 口、Hub 口或测试主机，需要重新运行脚本。"
        echo "#   5. 本脚本固定设置 MODE=\"0666\"。"
        echo "#"

        for i in "${!BIND_ALIASES[@]}"; do
            echo
            echo "# ${BIND_DEVICES[$i]} -> /dev/${BIND_ALIASES[$i]}"
            build_rule_line "$i"
            echo
        done
    } > "$TMP_RULE"
}

# 只保留最近 BACKUP_KEEP 份备份，删除更早的 .bak 文件。
# runner 为空表示当前已是 root，否则传 "sudo" 以便有权限删除。
prune_old_backups() {
    local runner="$1"
    local files
    local f
    local idx=0

    # 备份文件名带时间戳（%Y%m%d_%H%M%S），字典序即时间序，倒序取最新在前。
    mapfile -t files < <(ls -1 "${RULE_FILE}.bak."* 2>/dev/null | sort -r)

    for f in "${files[@]}"; do
        idx=$((idx + 1))
        if (( idx > BACKUP_KEEP )); then
            echo "删除旧备份：${f}"
            $runner rm -f "$f"
        fi
    done
}

write_rule_file() {
    local ts
    local backup_file

    ts="$(date +%Y%m%d_%H%M%S)"
    backup_file="${RULE_FILE}.bak.${ts}"

    echo
    echo "准备写入规则文件："
    echo "  ${RULE_FILE}"

    if [[ "$EUID" -eq 0 ]]; then
        if [[ -e "$RULE_FILE" ]]; then
            echo "备份旧规则文件到："
            echo "  ${backup_file}"
            cp -a "$RULE_FILE" "$backup_file"
            prune_old_backups ""
        fi

        install -m 0644 "$TMP_RULE" "$RULE_FILE"

        udevadm control --reload-rules
        udevadm trigger --action=add --subsystem-match=tty || true
        udevadm settle || true
    else
        if sudo test -e "$RULE_FILE"; then
            echo "备份旧规则文件到："
            echo "  ${backup_file}"
            sudo cp -a "$RULE_FILE" "$backup_file"
            prune_old_backups "sudo"
        fi

        sudo install -m 0644 "$TMP_RULE" "$RULE_FILE"

        sudo udevadm control --reload-rules
        sudo udevadm trigger --action=add --subsystem-match=tty || true
        sudo udevadm settle || true
    fi

    echo
    echo "udev 规则已写入并重新加载。"
}

# ============================================================
# 结果检查
# ============================================================

show_result() {
    local alias

    echo
    echo "检查固定别名："
    echo "--------------------------------------------------------------------------------"

    for alias in "${FIXED_ALIASES[@]}"; do
        if [[ -e "/dev/${alias}" ]]; then
            ls -l "/dev/${alias}"
        else
            echo "未生成或未配置：/dev/${alias}"
        fi
    done

    echo "--------------------------------------------------------------------------------"

    echo
    echo "如果别名未立即生成，可以尝试："
    echo "  1. 拔插对应 USB 串口设备"
    echo "  2. 或执行：sudo udevadm trigger --action=add --subsystem-match=tty"
    echo
}

print_header() {
    local alias

    echo "============================================================"
    echo " 固定串口别名配置工具"
    echo "============================================================"
    echo
    echo "固定别名列表："

    for alias in "${FIXED_ALIASES[@]}"; do
        echo "  /dev/${alias}"
    done

    echo
    echo "说明："
    echo "  用户只能为以上固定别名选择实际串口设备。"
    echo "  不能自定义别名，避免自动化测试脚本找不到设备。"
    echo "  已经绑定过的设备不会在后续列表中显示。"
    echo
    echo "绑定方式："
    echo "  本脚本按 USB 物理路径 ID_PATH 绑定。"
    echo "  不依赖 /dev/ttyACM0、/dev/ttyACM1、/dev/ttyUSB0 等动态编号。"
    echo
    echo "需要重新配置："
    echo "  更换 USB 口、Hub 口、USB Hub、测试主机，或想修改 dut 对应关系。"
    echo
    echo "不需要重新配置："
    echo "  系统重启、设备拔插回原 USB 口，或 ttyACM/ttyUSB 动态编号变化。"
    echo
}

check_runtime_env() {
    require_cmd udevadm
    require_cmd readlink
    require_cmd awk
    require_cmd sed
    require_cmd sort
    require_cmd install
    require_cmd mktemp
    require_cmd basename
    require_cmd date

    if [[ "$EUID" -ne 0 ]]; then
        require_cmd sudo
    fi
}

# ============================================================
# 主流程
# ============================================================

main() {
    local alias
    local config_files=()

    # ============================================================
    # 命令行参数解析
    # ============================================================
    # --config <file>  从 JSON5 配置文件读取别名列表（可重复指定）
    #                   不传此参数时使用脚本内置的默认别名列表
    # ============================================================
    while [[ $# -gt 0 ]]; do
        case "$1" in
            --config|-c)
                if [[ -z "${2:-}" ]]; then
                    echo "错误：--config/-c 需要参数"
                    exit 1
                fi
                config_files+=("$2")
                shift 2
                ;;
            --help|-h)
                echo "用法：$SCRIPT_NAME [--config <file> ...]"
                echo
                echo "   --config <file>, -c <file>"
                echo "          从 JSON5 配置文件读取别名列表，可重复指定以加载多个文件。"
                echo "          不传此参数时使用脚本内置的默认别名列表。"
                echo
                echo "   --help, -h"
                echo "          显示此帮助信息。"
                echo
                echo "内置默认别名列表："
                for alias in "${FIXED_ALIASES[@]}"; do
                    echo "  /dev/${alias}"
                done
                exit 0
                ;;
            *)
                echo "错误：未知参数：$1"
                echo "用法：$SCRIPT_NAME [--config <file> ...]"
                exit 1
                ;;
        esac
    done

    # 如果传了 --config，从 JSON5 文件加载别名，覆盖内置默认列表。
    if [[ ${#config_files[@]} -gt 0 ]]; then
        FIXED_ALIASES=()
        for cf in "${config_files[@]}"; do
            while IFS= read -r line; do
                FIXED_ALIASES+=("$line")
            done < <(read_aliases_from_json5 "$cf")
        done
    fi

    check_runtime_env
    validate_fixed_aliases
    print_header

    load_existing_bindings

    find_serial_devices
    build_available_devices

    if [[ "${#SERIAL_DEVICES[@]}" -eq 0 ]]; then
        show_devices
        echo
        echo "错误：未检测到 /dev/ttyUSB* 或 /dev/ttyACM* 设备。"
        echo "请确认设备已插入，并且系统已经识别。"
        exit 1
    fi

    # 先清理磁盘上遗留的孤儿别名（改名/下线的旧别名），
    # 再进入固定别名的正常绑定流程。
    prune_unknown_aliases

    for alias in "${FIXED_ALIASES[@]}"; do
        bind_fixed_alias "$alias"
    done

    show_bindings

    if [[ "${#BIND_ALIASES[@]}" -eq 0 ]]; then
        echo
        echo "没有任何绑定关系，退出。"
        exit 0
    fi

    generate_rule_file

    echo
    echo "即将生成的 udev 规则内容如下："
    echo "============================================================"
    cat "$TMP_RULE"
    echo "============================================================"

    if ! ask_yes_no "确认写入 ${RULE_FILE}？" "Y"; then
        echo "用户取消，未写入规则。"
        exit 0
    fi

    write_rule_file
    show_result

    echo
    echo "完成。"
}

main "$@"
