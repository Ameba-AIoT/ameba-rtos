# Ameba MCP Server

让 Claude Code 等 AI Agent 通过 MCP 协议直接操作 Ameba 开发板：build / flash /
烧录 / 串口 / quick_test 一站完成。支持的 IC：除了AmebaD 之外的所有芯片。

> 本文只讲**怎么装、怎么跑、怎么把硬件先准备好**。详细 tool / resource
> 行为见 `docs/architecture.md`，板级排错见 `docs/hardware_debug.md`
> （MCP resource `debug://hardware`）。

## 1. 环境准备：

### 1.1 硬件前提：

1. 必须使能开发板的自动下载电路。默认开发板都没有使能。需要找硬件加电阻。
2. 可以将 tools/ameba/Flash/Settings.json 中的 AutoSwitchToDownloadModeWithDtrRtsFirst 置1，直接使用硬件电路进入下载模式。不使用使用软件进入下载模式。

### 1.2 软件前提：先让 .venv 就绪

MCP server 依赖 SDK 的 `.venv` 环境。**如果 `.venv` 尚未建立，server 无法启动。**
最简单的办法是先编译一次 SDK，编译流程会自动检查并安装所有依赖：

```bash
cd <SDK_ROOT>
source env.sh           # 创建 .venv、安装依赖、注册 ameba-mcp 到 venv
ameba.py build -q       # 编译任意 SoC（触发环境完整性检查） -q 安静编译
```

> 环境就绪后，MCP server 才能被 Claude Code 正常拉起。如果你在注册 MCP 时 `.venv` 还没建好，Claude Code 会显示 server 连接失败。等 `source env.sh` + 首次编译完成后，重新启动 Claude Code，即可自动连上。

### 1.3 远程串口转发：

使用远程服务器或者本地 wsl 开发时，板子接在 windows 系统上，需要运行 `AmebaRemoteService.exe` 进行串口转发。

## 2. 注册到 Claude Code


将 Ameba-Dev MCP 注册到当前项目的工作区，配置写入 `~/.claude.json` 中的 workspace 层级。

- 优点：多个 SDK checkout 各自独立注册，互不干扰。不同的 sdk 使用不同的mcp server 和配置文件。
- 限制：如果新建一个工作区，则需要重新注册一次 MCP 服务。

安装：

**Linux / macOS / WSL2**

```bash
# <SDK_ROOT> 替换为 SDK 根目录的绝对路径
claude mcp add ameba-dev -- <SDK_ROOT>/tools/ameba/ameba_dev_mcp/launcher.sh
```

**Windows**

```bat
:: 简洁写法（如 claude mcp list 显示 ✓ Connected 即可）
claude mcp add ameba-dev -- <SDK_ROOT>\tools\ameba\ameba_dev_mcp\launcher.bat

:: 若上一条连不上，改用保险写法
claude mcp add ameba-dev -- cmd.exe /c <SDK_ROOT>\tools\ameba\ameba_dev_mcp\launcher.bat
```

验证：

```bash
claude mcp list     # 应出现 ameba-dev: ✓ Connected
```

卸载：

```bash
claude mcp remove ameba-dev
```

> **launcher 做了什么**：定位 prebuilts 工具链、把 `cmake/ninja/make` 加进 PATH、激活 `.venv`，最后启动 venv 里的 `ameba-mcp`。这样无论 Claude Code 是从终端、桌面图标还是 IDE 插件启动，MCP server 都拿得到完整的工具链 + Python 依赖，不依赖系统级 `cmake` / `python` 是否齐备。
>
> launcher 依赖 `.venv` 已就绪（由 `source env.sh` / `env.bat` 时的 `pip install -e tools/ameba` 自动生成）；缺则会以 stderr 报错并退出。

## 3. 初始化硬件测试环境

Claude Code 启动后第一次让 AI 用 ameba-dev 时，按以下顺序走：
在 Claude Code 里输入命令：

```
/ameba-setup-boards
```

AI 会引导用户进行开发版配置。并把 `board_info.json5` 写好并跑完只读环境自检，全程无需
用户手动编辑 JSON。引导完成后会生成两个文件：

- `<SDK_ROOT>/board_info.json5`——板子 / 端口 / 远端连接
- `<SDK_ROOT>/project_info.json5`——按 SoC 区分的烧录镜像表，**首次
  build 时由 build_firmware 自动回填**，无需手填

之后板子有增减、换端口，重新跑一次 `/ameba-setup-boards` 即可，或者直接编辑 `board_info.json5` 里的 `boards` 节点。

## 4. 手动启动 / 冒烟测试

正常情况下 Claude Code 会自动拉起 server，不需要手动跑。怀疑 server 本身
有问题（import 错、依赖缺失、PATH 不对）时，直接手动跑一次 launcher：
（`<SDK_ROOT>` 替换为SDK 根目录的绝对路径）

```bash
# Linux / macOS / WSL2
<SDK_ROOT>/tools/ameba/ameba_dev_mcp/launcher.sh
# Windows
<SDK_ROOT>\tools\ameba\ameba_dev_mcp\launcher.bat
# Ctrl+C 退出
```

server 阻塞等 stdio 输入；启动时 stderr 会打 8 行 `Registered ...`：

```
Registered flash tools
Registered serial tools
Registered project tools
Registered kconfig tools
Registered dev-loop tools
Registered env-check tools
Registered device resources
Registered board/config resources
Registered debug resources
Ameba MCP Server starting up...
```

8 行齐 + `starting up` 出现 = 健康。缺哪行就是哪个模块 import 出问题，照
stderr 里的 traceback 修。

## 5. 硬件准备

### PC 侧

| 项 | 要求 |
|---|---|
| 操作系统 | Linux（一线支持）/ Windows / WSL2 |
| Python | 3.10+ |
| PL2303 驱动 | **必须 ≥ 5.2.x**。低版本 RTS/DTR 信号有 bug，自动下载会失败 |
| USB 线 | 支持数据传输（不是仅充电线） |

**PL2303 驱动版本检查**

- Linux：`modinfo pl2303 | grep version`
- Windows：设备管理器 → 端口 (COM & LPT) → Prolific USB-to-Serial → 属性 → 驱动程序

### 板侧（自动下载）

如果你希望工具能**自动复位 + 进入下载模式**（无需手按 BOOT/RST），开发板需要：

- PL2303 → UMH3N(U20) → MCU 的标准电路（R175/R176/R7/R8/C145）
- **R7、R8 默认未焊接**，要补焊上才能让 CHIP_EN / UART_LOG_TX 正常被电路控制

电路细节、信号定义、5 步 DTR/RTS 时序、定制板手动按键流程：见
`docs/hardware_debug.md` 或 MCP resource `debug://hardware`。

## 6. 错误排查

claude code 中，按 ctrl + o 展开详细的 mcp 调用参数和返回值。查看错误码： 

| 现象 | 第一步看 |
|---|---|
| 报 `BOARD_CONFIG_MISSING` / `PROJECT_CONFIG_MISSING` | 重跑 `/ameba-setup-boards` 一键生成；或按 envelope 里 `template_path` 手填，字段参考 `docs/board_info.md` |
| 报 `PORT_NOT_VISIBLE` | 重插 USB / 换数据线 / 查 PL2303 驱动版本 |
| 报 `REMOTE_UNREACHABLE` | 远端机器起 AmebaRemoteService、查 58916 防火墙 |
| 报 `RESET_FAILED` / 烧录卡在 boot mode | 看 `debug://hardware` §1（驱动）+ §2（电路）|
| 报 `ALIAS_REQUIRED` / `ALIAS_NOT_FOUND` | hint 已列出可选 alias，挑一个传 |
| AI 调工具完全没反应 | `claude mcp list` 看 ameba-dev 是否 ✓ Connected；不在就照 §2 重新 add |
| server 启动崩 | 按 §4 手动跑一次 `<SDK_ROOT>/tools/ameba/ameba_dev_mcp/launcher.sh`（Windows 同名 .bat）看完整 stderr |



### 附：用户level 注册 MCP 的方法：

注册到当前用户的所有项目，配置写入 `~/.claude.json` 的用户层级。
- 优点：适合长期只用一个 SDK、或希望在任意目录都能调用的场景。
- 限制：因为这个board.json project.json 是在sdk根目录，因此多份sdk 情况下注册文件不会在当前sdk根目录。比如：如果你在user level 注册 sdk1 （比如开发lite）里面的 mcp；
那么你在sdk2 （开发g2）去set board时候，这个配置文件还是会进到 sdk1 下面。若确实需要用户level 注册且用多份sdk，可以考虑注册的时候修改 下面注册命令中的 ameba-dev MCP 名称，改为 ameba-dev-xxx 来区分不同的 MCP server （不建议这么用）。

注册（路径换成 launcher，与项目 level 一致）：

```bash
# Linux / macOS / WSL2
claude mcp add ameba-dev --scope user -- <SDK_ROOT>/tools/ameba/ameba_dev_mcp/launcher.sh
# Windows（如不能 Connected 则前面加 `cmd.exe /c`）
claude mcp add ameba-dev --scope user -- <SDK_ROOT>\tools\ameba\ameba_dev_mcp\launcher.bat
```

卸载：

```bash
claude mcp remove ameba-dev --scope user
```
