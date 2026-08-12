# UART Tool — Dual-Board Pytest 辅助固件

## 概述

UART Tool 是一个专为 Ameba 外设 pytest 自动化测试框架设计的**辅助固件**，运行在双板测试中的"对端板"(Peer Board)上。它不演示任何外设功能，而是作为测试基础设施：

- 通过 **Console (LOGUART)** 接收 `uarttool <text>` 命令
- 将文本通过 **Test UART (UART0)** 转发给被测板
- 等待被测板的回显并逐字节对比，报告 PASS/FAIL

> **角色说明**：在典型的双板 UART 测试中，两块 Ameba 板通过 UART0 直连：
> - **对端板 (Peer Board)** — 烧录 `uart_tool` 固件，负责主动发送数据
> - **被测板 (DUT)** — 烧录被测的 UART 示例固件（如 `raw_uart_poll_trx`、`raw_uart_dma_stream` 等）

### 与单板示例的关系：模拟 "USBtoTTL 适配器 + PC 终端"

各 UART 示例的 README（如 [raw_uart_poll_trx/README.md](../../../../raw/UART/raw_uart_poll_trx/README.md)）中描述的使用方式为：

```
Ameba 板  ──UART0──  USBtoTTL 适配器  ──USB──  PC（超级终端）
```

用户通过 PC 上的终端软件输入字符，字符经 USBtoTTL 发送到 Ameba 板 UART0，板子收到后回显到终端。

在双板自动化测试中，**`uart_tool` 固件精确地扮演了 "USBtoTTL 适配器 + PC 终端" 的角色**：

| 单板手动测试 | 双板自动化测试 |
|-------------|---------------|
| PC 终端（人输入字符） | `uarttool <text>` 命令（pytest 自动输入） |
| USBtoTTL 适配器（USB↔UART 电平转换） | 对端板 UART0 直连（板对板电器连接） |
| 人眼对比发送与回显 | `uart_tool_verify()` 逐字节自动对比 |
| 每次输入需手动操作 | 多个测试用例全自动循环运行 |

简言之，对端板上的 `uart_tool` 固件将一个带终端的 PC 及其 USBtoTTL 适配器**完整替换为一块可编程的 Ameba 板**，使得整个测试流程可由 pytest 全自动编排，无需人工干预。

## 架构

```
┌──────────────┐    UART0 (38400 8N1)    ┌──────────────┐
│  对端板       │◄──────────────────────►│  被测板       │
│  uart_tool   │   TX◄──────►RX          │  被测示例     │
│              │   RX◄──────►TX          │              │
│              │   RTS◄─────►CTS         │              │
│              │   CTS◄─────►RTS         │              │
└──────┬───────┘                        └──────────────┘
       │ LOGUART (115200)
       │
  PC/pytest (发送 "uarttool <text>" 命令)
```

### 内部任务结构

固件启动后创建两个并发上下文：

| 组件 | 角色 | 详情 |
|------|------|------|
| **Shell 命令** `uarttool` | 命令处理器 | 在 shell 任务上下文中运行，拼接 argv→send_buf，置位 `send_pending`，等待结果 |
| **Monitor 任务** `uart_tool_monitor` | Test-UART 所有者 | 唯一轮询 UART0 RX FIFO 的任务：持续转发回显到 Console；检测到 `send_pending` 时执行 `uart_tool_verify()` |

**线程安全约定**：
- `send_pending` 和 `monitor_ready` 为 `volatile` 标记的简单标志，shell 按顺序处理命令，无需原子保护
- `monitor_ready` 确保 monitor 任务完成初始化后 shell 命令才可执行，防止死锁
- shell 命令处理中忙等 `send_pending` 清零，保证结果在 shell 提示符返回前上报

## 硬件接线

### 双板直连（UART0）

| 对端板 (uart_tool) | 被测板 | 说明 |
|--------------------|--------|------|
| UART_TX | UART_RX | 对端发→被测收 |
| UART_RX | UART_TX | 被测发→对端收 |
| GND | GND | 共地 |

部分测试（如 `raw_uart_poll_autoctrl`）还需要连接 RTS/CTS 流控线。

> **引脚定义**见 `example_uart_ext.h`，不同 SoC 引脚不同。

**注意**：对端板还需一根 USB-to-TTL 连接 LOGUART 到 PC，pytest 通过该串口发送 `uarttool` 命令。

## 软件配置

在 `example_uart_tool.c` 中可配置：

```c
#define UART_DEV    UART0_DEV   // 测试用 UART 设备
#define UART_BAUD   38400       // 波特率
#define SEND_BUF_SZ 256         // 发送缓冲区大小
#define RECV_BUF_SZ 256         // 接收缓冲区大小
#define ECHO_TIMEOUT_MS 2000    // 回显等待超时 (ms)
```

> 如需更换测试 UART（如 UART1），还需同步修改 `example_uart_ext.h` 中的 TX/RX 引脚定义，并在 `uart_get_idx()` 返回值正确的前提下初始化对应时钟。

## 构建与烧录

### 命令行构建

```bash
# 切换到 SDK 根目录
cd <SDK_ROOT>

# 初始化环境
source env.sh

# 选择 SoC
./ameba.py soc RTL8720F

# 构建 uart_tool
./ameba.py build -a uart_tool -q -D FINAL_IMAGE_DIR=<path>/uart_tool/bin

# 烧录（首次需通过 Ameba Image Tool 下载 images）
./ameba.py flash
```

### pytest 自动构建

添加 `--auto-build` 参数后，pytest 会自动调用 `ameba.py build`：

```bash
cd example/peripheral/auto_test/
pytest pytest_raw_uart.py -k test_uart_raw_poll_trx \
  --target RTL8720F --port "COM6|COM7" --auto-build
```

## 使用方式

### 手动测试

```bash
# 打开串口终端 (115200 8N1) 连接对端板 LOGUART
# 等待固件启动，看到提示：
========== UART Tool Ready ==========
Test UART: UART0 @ 38400 8N1
Type on console: uarttool <text>
======================================

# 在终端输入命令：
uarttool Hello_UART

# 输出示例：
Hello_UART
PASS: 11/11 bytes matched
```

### pytest 自动化

uart_tool 被以下 pytest 用例作为对端板固件使用（通过 `app_name` marker 指定）：

| 测试函数 | 被测固件 | 测试内容 |
|----------|----------|---------|
| `test_uart_raw_poll_trx` | `raw_uart_poll_trx` | 轮询方式双板回环 |
| `test_uart_raw_int_trx` | `raw_uart_int_trx` | 中断方式双板回环 |
| `test_uart_raw_int_stream` | `raw_uart_int_stream` | 中断流式 8 字节回环 |
| `test_uart_raw_int_rx_timeout` | `raw_uart_int_rx_timeout` | 中断超时接收（13B/超时） |
| `test_uart_raw_dma_stream` | `raw_uart_dma_stream` | DMA 变长回环（2~33 字节） |
| `test_uart_raw_dma_rx_fc` | `raw_uart_dma_rx_fc` | DMA 流控回环 |

```bash
# 运行所有依赖 uart_tool 的测试
cd example/peripheral/auto_test/
pytest pytest_raw_uart.py \
  --target RTL8720F --port "COM6|COM7" \
  --auto-build
```

## PASS/FAIL 判定逻辑

```
发送 "Hello" (5 字节)
  ├── 2 秒内未收到任何字节 → [TIMEOUT] 打印超时日志
  ├── 收到字节数 ≠ 发送字节数 → FAIL: sent 5 bytes, got N bytes
  └── 收到字节数 = 发送字节数
        ├── 逐字节匹配 → PASS: 5/5 bytes matched
        └── 存在不匹配 → FAIL: 打印 sent/recv 内容
```

## 关键技术决策

### 避免轮询 LOGUART RX（跨核兼容）

`uart_tool_cmd()` 通过注册 `CMD_TABLE` 命令回调获取用户输入，**不直接轮询 LOGUART RX**。

在多核 SoC（如 RTL8720F）上，LOGUART RX FIFO 被两个内核的 shell ISR 共享。如果示例代码同时在 APP 核轮询该 FIFO，会导致两核 ISR 互相抢占、系统崩溃。将用户输入获取统一交由 shell 任务处理，避免了这一冲突。

### 初始化顺序保证

`monitor_ready` 标志确保 shell `uarttool` 命令不会在 monitor 任务完成 `uart_tool_init()` 前被执行，防止 `send_pending` 被置位后无人清零导致的 shell 永久死锁。

## 支持的 SoC

- RTL8721F
- RTL8720F
