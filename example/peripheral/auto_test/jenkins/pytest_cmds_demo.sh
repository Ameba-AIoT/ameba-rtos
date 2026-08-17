#!/bin/bash
# pytest_cmds_demo.sh — 解析 pytest_cmds.json5，按 entry 逐条生成 pytest 指令。
#
# 用法:
#   ./pytest_cmds_demo.sh                        # dry-run，只打印指令
#   ./pytest_cmds_demo.sh --run                  # 实际执行每条 pytest
#   ./pytest_cmds_demo.sh --run --exec generic   # 只跑 -m generic 的环境
#   ./pytest_cmds_demo.sh --file /path/to/custom.json5

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
DEFAULT_JSON5="${SCRIPT_DIR}/pytest_cmds.json5"

# --- 参数解析 ---
RUN_MODE="dryrun"
FILTER_MARK=""
JSON5_FILE="$DEFAULT_JSON5"

while [[ $# -gt 0 ]]; do
    case "$1" in
        --run)
            RUN_MODE="run"
            shift
            ;;
        --file)
            JSON5_FILE="$2"
            shift 2
            ;;
        --exec)
            RUN_MODE="run"
            FILTER_MARK="$2"
            shift 2
            ;;
        -h|--help)
            sed -n '2,8p' "$0"
            exit 0
            ;;
        *)
            echo "未知参数: $1" >&2
            echo "用法: $0 [--run] [--exec <mark>] [--file <path>]" >&2
            exit 1
            ;;
    esac
done

if [[ ! -f "$JSON5_FILE" ]]; then
    echo "错误: 文件不存在: $JSON5_FILE" >&2
    exit 1
fi

# --- 用 Python json5 解析，输出 TAB 分隔行：cwd<TAB>mark<TAB>args ---
ENV_DATA="$(python3 -c "
import json5, shlex

with open('$JSON5_FILE', 'r') as f:
    cfg = json5.load(f)

cwd = cfg.get('cwd', '')

# 收集公共参数（common 字段）
shared = cfg.get('common', {})

entries = cfg.get('environments', [])
if not entries:
    exit(0)

for env in entries:
    merged = dict(shared)
    for k, v in env.items():
        merged[k] = v

    # 提取 -m 的值给 shell 做 --exec 过滤
    mark = merged.get('-m', '')

    # 通用拼接：bool true→直接输出key，其他→key value
    parts = []
    for k, v in merged.items():
        if k == 'name':
            continue
        if isinstance(v, bool):
            if v:
                parts.append(k)
        else:
            parts.append(f'{k} {shlex.quote(v)}')
    args_str = ' '.join(parts)

    print(f'{cwd}\t{mark}\t{args_str}')
")"

if [[ -z "$ENV_DATA" ]]; then
    echo "错误: 未能从 $JSON5_FILE 解析到任何环境条目" >&2
    exit 1
fi

# --- 提取 SDK Root（从第一个环境的 cwd 推算）---
FIRST_CWD="$(echo "$ENV_DATA" | head -1 | cut -f1)"
if [[ "$FIRST_CWD" == \~/* ]]; then
    FIRST_CWD="$HOME${FIRST_CWD:1}"   # 把开头的 ~ 替换为 $HOME
fi
SDK_ROOT="$(cd "$FIRST_CWD" && cd ../../.. && pwd)"

echo "SDK Root: $SDK_ROOT"
if [[ "$RUN_MODE" == "run" ]]; then
    echo ">>> 执行 ameba.py cleansoc ..."
    (cd "$SDK_ROOT" && ./ameba.py cleansoc rtl8720f) || {
        echo "错误: ameba.py cleansoc 失败" >&2
        exit 1
    }
else
    echo ">>> 将执行: cd $SDK_ROOT && ./ameba.py cleansoc"
fi
echo ""

# --- 逐条执行 ---
CMD_INDEX=0

while IFS=$'\t' read -r cwd mark args; do
    if [[ -n "$FILTER_MARK" && "$mark" != "$FILTER_MARK" ]]; then
        continue
    fi

    CMD="cd $cwd && pytest $args"

    CMD_INDEX=$((CMD_INDEX + 1))
    echo "==================== 指令 $CMD_INDEX ===================="
    [[ -n "$mark" ]] && echo "环境: $mark"
    echo "指令: $CMD"
    echo ""

    if [[ "$RUN_MODE" == "run" ]]; then
        echo ">>> 开始执行..."

        if [[ "$CMD_INDEX" == 1 ]]; then
          eval "cd ./rtos_sdk && ./ameba.py cleansoc rtl8720f"   # cleansoc first
        fi

        (eval "cd $cwd && pytest $args") || {
            echo "警告: 指令 $CMD_INDEX 执行失败，继续下一个" >&2
        }
        echo ""
    fi
done <<< "$ENV_DATA"

if [[ $CMD_INDEX -eq 0 ]]; then
    if [[ -n "$FILTER_MARK" ]]; then
        echo "没有匹配 \"$FILTER_MARK\" 的环境条目。"
    else
        echo "没有可生成的环境条目。"
    fi
    exit 0
fi

echo "=========================================="
echo "共生成 $CMD_INDEX 条 pytest 指令。"
if [[ "$RUN_MODE" == "dryrun" ]]; then
    echo "添加 --run 参数来实际执行，或 --exec <mark> 指定仅跑某个环境。"
fi
