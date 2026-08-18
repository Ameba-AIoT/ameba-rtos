# Ameba Example Pytest — Jenkins CI 集成

面向 **Jenkins Pipeline 开发人员**，说明环境准备、指令下发、硬件工位映射及输出产物。

---

## 环境准备

### 一次性配置：串口别名（由硬件管理员执行）

`setup_serial_alias.sh` 将 USB 串口设备的物理端口绑定到固定别名，使端口路径不随系统重启或 USB 枚举顺序变化。

脚本支持两种方式指定别名列表：

1. **`--config <file.json5>`（推荐）** — 从外部 JSON5 文件加载别名列表，可重复指定以加载多个配置
2. **不传 `--config`** — 使用脚本内置的默认别名列表（见脚本底部 `FIXED_ALIASES`）

#### 单独配置（单团队维护）

```bash
# RTOS peripheral_test 团队：仅配置 RTOS 设备别名
./setup_serial_alias.sh --config dev_aliases_rtos.json5
```

```bash
# Zephyr_test 团队：仅配置 Zephyr 设备别名
./setup_serial_alias.sh --config dev_aliases_zephyr.json5
```

#### 合并配置（多团队共用主机）

在同一台测试主机上为两个团队同时配置别名：

```bash
# 合并 RTOS + Zephyr 别名，一次完成
./setup_serial_alias.sh --config dev_aliases_rtos.json5 --config dev_aliases_zephyr.json5
```

> 也可以不传 `--config`，使用脚本内置的默认列表（包含下面两个表中的所有别名）。

#### 当前定义的固定别名

| 团队 | 别名 | 对应工位 | 用途 |
|------|------|---------|------|
| RTOS peripheral_test | `r_rtl8720f_dut0` | 普通测试工位（端口 1） | 单板、双板测试（主 DUT） |
| RTOS peripheral_test | `r_rtl8720f_dut1` | 普通测试工位（端口 2） | 双板测试（从 DUT） |
| RTOS peripheral_test | `r_rtl8720f_dut2` | GPIO 回环工位 | GPIO 回环测试 |
| Zephyr_test | `z_rtl872xda_evb_0` | AmebaDplus EVB | Zephyr 测试（ttyUSB0） |
| Zephyr_test | `z_rtl8721f_evb_0` | AmebaD EVB | Zephyr 测试 |
| Zephyr_test | `z_rtl8730e_evb_0` | AmebaSmart EVB | Zephyr 测试 |

#### 配置文件格式

`dev_aliases_rtos.json5`（RTOS 团队维护）：

```json5
{
  // 命名规范：r_<chip>_<role><序号>
  aliases: [
    "r_rtl8720f_dut0",    // 主 DUT（串口 0）
    "r_rtl8720f_dut1",    // 从 DUT（串口 1）
    "r_rtl8720f_dut2",    // loopback 专用（串口 2）
  ],
}
```

`dev_aliases_zephyr.json5`（Zephyr 团队维护）：

```json5
{
  // 命名规范：z_<完整board名>_<序号>
  aliases: [
    "z_rtl872xda_evb_0",   // AmebaDplus EVB
    "z_rtl8721f_evb_0",    // AmebaD EVB
    "z_rtl8730e_evb_0",    // AmebaSmart EVB
  ],
}
```

> 格式说明：使用 JSON5 格式，key 和字符串可用单/双引号或省略引号，支持 `//` 注释。每个文件只包含本团队维护的别名，互不干扰。可通过重复 `--config` 加载多个文件实现合并。

#### 注意事项

- **此脚本仅需执行一次**，后续增减工位或更换硬件时由管理员重新运行即可。**Jenkins Pipeline 中不需要执行此脚本。**
- 多个 `--config` 文件的别名会按文件顺序合并；如果不同配置文件中有同名别名，后者会覆盖前者（当前无此场景，但设计上已考虑）。
- 如果磁盘上已存在来自其他配置的旧别名，脚本会询问是否保留，默认保留（避免误删其他团队的别名）。

### Jenkins 端

`--auto-build` 方式下，pytest 会在执行时自动调用 `ameba.py build` 编译所需固件，Jenkins 端只需安装 Python 依赖即可：

```bash
# 唯一必需的步骤：安装 Python 依赖
cd example/peripheral/auto_test
pip install -r requirements.txt
```
---

## 执行测试

### 动态指令生成

Jenkins Pipeline **不硬编码**测试指令，而是解析 `pytest_cmds.json5`，按每个 environment 条目动态拼接 pytest 命令。管理员新增硬件工位时只需改 JSON5 文件，无需改 Jenkins 代码。

`pytest_cmds_demo.sh` 是**一个参考实现**，演示了如何解析 JSON5 并逐条生成与执行 pytest 指令，Jenkins 可直接调用它，也可按相同逻辑自行实现。参考脚本展开后等价于以下命令：

```bash
# 命令 1：常规测试项（单板 + 双板）
cd ~/workspace/peripheral_test/rtos_sdk/example/peripheral/auto_test && pytest \
    --log-dir ~/workspace/peripheral_test/pytest_logs \
    --auto-build \
    --target RTL8720F \
    --port '/dev/r_rtl8720f_dut0|/dev/r_rtl8720f_dut1' \
    -m generic

# 命令 2：GPIO 回环测试
cd ~/workspace/peripheral_test/rtos_sdk/example/peripheral/auto_test && pytest \
    --log-dir ~/workspace/peripheral_test/pytest_logs \
    --auto-build \
    --target RTL8720F \
    --port /dev/r_rtl8720f_dut2 \
    -m gpio_loopback
```

---

## 指令格式与参数说明

### 完整命令模板

```bash
cd example/peripheral/auto_test

pytest raw/ mbed/ \
    --target <SoC型号> \
    --port <串口路径> \
    -m '<mark表达式>' \
    --auto-build
```

### 参数含义

| 参数 | 是否必填 | 说明 |
|------|---------|------|
| `--target` | 是 | SoC 型号。双板测试时两个端口使用相同 SoC，用 `\|` 分隔写两次，如 `RTL8720F\|RTL8720F` |
| `--port` | 是 | USB 串口设备路径。固定别名如 `/dev/r_rtl8720f_dut0`，双板时用 `\|` 分隔，如 `/dev/r_rtl8720f_dut0\|/dev/r_rtl8720f_dut1` |
| `-m` | 是 | pytest mark 表达式，用于筛选测试用例。见下方「硬件工位映射」|
| `--auto-build` | 否（推荐） | 自动编译固件，pytest 执行时调用 `ameba.py build` 编译所需示例。**推荐 CI 始终加此参数**，免去手动编译步骤 |

---

## 硬件测试环境映射

测试环境的参数定义在 `pytest_cmds.json5` 中，这是**唯一需要维护**的配置文件。

### 文件位置

```
~/workspace/peripheral_test/rtos_sdk/example/peripheral/auto_test/pytest_cmds.json5
```

### 文件格式说明

```json5
{
  cwd: "~/workspace/peripheral_test/rtos_sdk/example/peripheral/auto_test",
  "--log-dir": "~/workspace/peripheral_test/pytest_logs",  // 共享参数：日志输出目录
  "--auto-build": true,                                     // 共享参数：自动编译固件

  environments: [
    {
      name: "常规测试项（单板 + 双板）",
      "--target": "RTL8720F",
      "--port":   "/dev/r_rtl8720f_dut0|/dev/r_rtl8720f_dut1",
      "-m":       "generic"
    },
    {
      name: "GPIO 回环测试",
      "--target": "RTL8720F",
      "--port":   "/dev/r_rtl8720f_dut2",
      "-m":       "gpio_loopback"
    }
  ]
}
```

> **格式说明**：使用 JSON5 格式，key 无特殊字符时可省略引号；CLI 参数型 key（如 `--target`）含连字符，**必须保留引号**。顶层参数（如 `"--auto-build": true`）自动继承到所有环境，环境内的同名字段会覆盖顶层值。

### 管理员维护指南

| 运维场景 | 操作 |
|---------|------|
| **串口别名变更** | 修改对应 environment 的 `"--port"` 字段值 |
| **新增硬件工位** | 在 `environments` 数组中追加一个 entry，填写 `"--target"`、`"--port"`、`"-m"` 等 pytest 参数 |
| **删除工位** | 删除对应的 entry |
| **更换芯片型号** | 修改对应 environment 的 `"--target"` 字段 |
| **执行目录变更** | 修改顶层 `cwd` |
| **新增公共参数** | 直接加在顶层，所有环境自动继承 |

> **原则**：`environments` 数组中有多少个 entry，就会生成多少条 pytest 命令（参考脚本 `pytest_cmds_demo.sh` 演示了具体做法）。**新增工位只需加一行 JSON5**，既有 pytest 参数可自由组合，无需改 Jenkins 代码。

---

## 输出产物

ameba_pytest 插件在执行时自动生成日志和报告，通过 `--log-dir` 控制输出根目录（默认为 SDK 根目录）。

### `--log-dir` 说明

| 参数 | 说明 | 示例 |
|------|------|------|
| `--log-dir` | 日志根目录。**实际产物路径为此值 + `/ameba-pytest/<timestamp>/`**。不传时默认 SDK 根目录 | `--log-dir /opt/jenkins/logs` → `/opt/jenkins/logs/ameba-pytest/...` |

### 产物目录结构

无论是否传 `--log-dir`，产物均按以下结构组织：

```
<root>/ameba-pytest/<YYYY-MM-DD_HH-MM-SS>/
├── report.html          # HTML 报告（自包含，含 Run Info 面板 + 三级分组）
├── report.xml           # JUnit XML（供 Jenkins junit 插件生成趋势图）
│
├── 001_<test_case_1>/   # 每个 test case 一个子目录，编号前缀保持顺序
│   ├── dut0_r_rtl8720f_dut0.txt # 单板：dut<N>_<别名>.txt
│   └── dut1_r_rtl8720f_dut1.txt # 双板：每个 DUT 各一个日志文件
│
├── 002_<test_case_2>/
│   └── dut0_r_rtl8720f_dut2.txt
│
└── ...
```

> `<root>` = `--log-dir` 的值（若指定）或 SDK 根目录（若未指定）。
> 例：`--log-dir /opt/jenkins/logs` → `/opt/jenkins/logs/ameba-pytest/2026-07-23_10-30-00/`

| 产物 | 说明 |
|------|------|
| `report.html` | 自包含 HTML：顶部 **Run Info** 面板显示 pytest 命令、target、port 等上下文；测试结果按 **文件 / 函数 / 参数** 三级可折叠分组；每行有日志文件超链接 |
| `report.xml` | JUnit XML 格式，Jenkins `junit` 插件自动读取后生成趋势曲线和 Flaky 统计 |
| `dut<N>_<port>.txt` | 串口原始日志，每个 test case 每个 DUT 各一份 |

---

## 与手动测试的互通性

CI 命令与开发者本地命令**完全一致**，仅设备路径的命名方式不同：

| 环境 | `--port` 示例 |
|------|-------------|
| Jenkins + 固定别名（Linux） | `--port /dev/r_rtl8720f_dut0\|/dev/r_rtl8720f_dut1` |
| 开发者本机（Windows） | `--port COM6\|COM7` |

pytest 参数、marker 过滤、配置文件均相同，确保 CI 失败可在本地重现调试。
