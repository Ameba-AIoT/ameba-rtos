# Ameba Example Pytest 自动化测试

为 Ameba 外设**示例固件**（`example/peripheral/raw/`、`example/peripheral/mbed/`）提供板级自动化回归测试。

---

## 目录结构

```
example/peripheral/auto_test/
├── conftest.py          # pytest 插件配置、--auto-build、app_name/app_defines marker
├── pytest.ini           # pytest 配置（文件匹配、marker 注册）
├── README.md            # 本文件
│
├── raw/                 # raw（裸机）外设示例固件的测试
│   ├── pytest_raw_gpio.py
│   ├── pytest_raw_i2c.py
│   ├── pytest_raw_spi.py
│   └── pytest_raw_uart.py
│
├── mbed/                # mbed-OS 外设示例固件的测试
│   ├── pytest_mbed_gpio.py
│   ├── pytest_mbed_i2c.py
│   ├── pytest_mbed_spi.py
│   └── pytest_mbed_uart.py
│
└── tools/
    └── uart_tool/       # 双板测试辅助固件（替代 PC 终端 + USBtoTTL）
```

### raw vs mbed

| 分类 | 对应示例目录 | 测试文件 |
|------|-------------|---------|
| **raw**（裸机驱动） | `example/peripheral/raw/{GPIO,I2C,SPI,UART}/` | `pytest_raw_*.py` |
| **mbed**（mbed-OS 驱动） | `example/peripheral/mbed/{GPIO,I2C,SPI,UART}/` | `pytest_mbed_*.py` |

两个分类的测试结构、执行方式、fixture 使用完全一致。

---

## 前置条件

1. **Python 依赖安装**：

   ```bash
   cd example/peripheral/auto_test
   pip install -r requirements.txt
   ```

   > `ameba_pytest` 插件提供 `dut` fixture、串口通信、`--auto-build` 等核心能力。
   > `pexpect` 供 `dut.expect()` 匹配串口输出。
   > `pytest-html` 用于生成 HTML 报告。

2. **Ameba 开发板**（数量视测试要求）：
   - 单板测试：1 块板
   - 双板测试（`multi_board`）：2 块板，被测外设引脚一一对接，比如PA21 <--> PA21

3. **固件**：提前编译好示例固件（见下方「执行方法」），或使用 `--auto-build`

---

## 执行方法

### 1. 自动编译 + 运行（推荐）

添加 `--auto-build` 参数，pytest 会在 fixture setup 前自动编译固件：

```bash
cd example/peripheral/auto_test

pytest raw/pytest_raw_uart.py \
  --target RTL8720F \
  --port "COM6|COM7" \
  --auto-build
```

> `--auto-build` 依赖 `@pytest.mark.app_name(...)` 和 `image_dir` parametrize，二者缺一不可（conftest.py 会检查）。

### 2. 手动编译 + 运行

```bash
# 1. 切换到 SDK 根目录，初始化环境
cd <SDK_ROOT>
source env.sh

# 2. 选择 SoC
./ameba.py soc RTL8720F

# 3. 编译被测示例固件（以 raw_uart_poll_trx 为例）
./ameba.py build -a raw_uart_poll_trx -q \
  -D FINAL_IMAGE_DIR=example/peripheral/auto_test/raw/bin/raw_uart_poll_trx

# 4. 烧录固件到板子

# 5. 进入 auto_test 目录，运行 pytest
cd example/peripheral/auto_test

# 单板 GPIO 测试（loopback 跳线 PA_23 <-> PA_24）
pytest raw/pytest_raw_gpio.py \
  --target RTL8720F \
  --port COM6

# 双板 UART 测试
pytest raw/pytest_raw_uart.py \
  --target RTL8720F \
  --port "COM6|COM7"
```

### 3. 筛选特定测试

```bash
# 按 test case 名
pytest raw/pytest_raw_uart.py -k test_raw_uart_poll_trx --auto-build

# 按外设分类
pytest raw/pytest_raw_i2c.py mbed/pytest_mbed_i2c.py \
  --target RTL8720F --port "COM6|COM7"
```

### 4. 调试模式

```bash
# pytest.ini 已配置 -s -vv，串口日志实时打印
# 如需更详细输出，可加 --tb long
pytest raw/pytest_raw_gpio.py --target RTL8720F --port COM6 --tb long
```

### CLI 参数说明

| 参数 | 说明 | 示例 |
|------|------|------|
| `--target` | SoC 型号。双板用 `\|` 分隔 | `RTL8720F` / `RTL8720F\|RTL8720F` |
| `--port` | 串口。双板用 `\|` 分隔 | `COM6` / `COM6\|COM7` |
| `--auto-build` | 自动编译固件 | 无参数 |

### `--auto-build` 内部机制

`--auto-build` 在 fixture setup 前调用 `ameba.py build` 自动编译固件，编译产物输出到 `image_dir` 指定的目录，供后续烧录使用。

**自动检测 `prj.conf`**：部分示例固件目录下包含 `prj.conf`（如 `example/peripheral/raw/RDP/rdp_service/prj.conf`），其中定义 Kconfig 覆写。这些覆写仅在**纯净编译**（`-p`）时才生效。`conftest.py` 中 `_app_has_prj_conf()` 自动搜索该 app 目录下是否存在 `prj.conf`，存在则自动添加 `-p` 参数；不存在则使用增量编译，显著加快编译速度。

**强制纯净编译**：若某 app 因其他原因也需要纯净编译，可在 test case 上标记 `@pytest.mark.pristine_build`，强制传 `-p`。

**防止配置泄漏**：使用 `-p` 编译后，`build_<SOC>` 中残留了当前 app 的 `CONFIG_*` 变量。若下一个 app（无 `prj.conf`）在同一 SOC 目录增量编译，会误用这些残留配置。`conftest.py` 中 `_clean_build_state()` 在每次 `-p` 编译后自动清理 `build/` 和 `menuconfig/` 目录，下次编译时从头开始，消除泄漏风险。

| 场景 | 自动传 `-p`? | 清理构建目录? |
|------|:---:|:---:|
| 无 `prj.conf`，无 `@pristine_build` | ❌ | ❌ |
| 有 `prj.conf`（自动检测） | ✅ | ✅ |
| 无 `prj.conf`，有 `@pristine_build` | ✅ | ❌ |

---

## 核心机制

### Fixture 系统

测试框架基于 `ameba_pytest.plugin`，核心 fixture 为 **`dut`**（Device Under Test）。

```python
# 单板测试：dut 为一个 AmebaDut 对象
def test_raw_gpio_rw(dut):
    dut.expect("PASS", timeout=10)

# 双板测试（count=2）：dut 为二元组 (peer_board, test_board)
@pytest.mark.multi_board
@pytest.mark.parametrize("count", [2], ids=["dual"], indirect=True)
def test_raw_uart_poll_trx(dut):
    peer_board, test_board = dut
    peer_board.expect("Hello World!!!", timeout=20)
```

**重要**：示例固件无 shell，conftest.py 会 `patch` 掉 `_wait_for_prompt`（跳过 10 秒 shell 提示符等待，避免 fixture setup 超时）。

### Marker 系统

| Marker | 用途 | 示例 |
|--------|------|------|
| `multi_board` | 标记双板测试 | `@pytest.mark.multi_board` |
| `gpio_loopback` | GPIO 回环测试（需跳线） | `@pytest.mark.gpio_loopback` |
| `app_name(name)` | 示例应用名，`--auto-build` 使用 | `@pytest.mark.app_name("raw_uart_poll_trx")` |
| `app_defines(defines)` | 额外 CMake 定义 | `@pytest.mark.app_defines("GPIO_LOOPBACK=1")` |
| `pristine_build` | 强制纯净编译（`-p`） | `@pytest.mark.pristine_build` |

### Parametrize 参数

| 参数 | 类型 | 说明 |
|------|------|------|
| `count` | int | DUT 数量（单板=1，双板=2） |
| `image_dir` | str | 固件二进制目录路径（`--auto-build` 时使用） |

双板写法用 `|` 分隔两个板子的值：

```python
@pytest.mark.app_name("uart_tool|raw_uart_poll_trx")     # peer = uart_tool, DUT = raw_uart_poll_trx
@pytest.mark.parametrize("image_dir",
    ["../tools/uart_tool/bin|./bin/raw_uart_poll_trx"],   # peer image | DUT image
    indirect=True)
```

---

## 双板测试与 `uart_tool` 辅助固件

部分外设（如 UART）需要双板对测。对端板烧录 `uart_tool` 辅助固件，模拟 **PC 终端 + USB-to-TTL 适配器** 的角色：

```
┌──────────────┐    UART0 (38400 8N1)    ┌──────────────┐
│  对端板       │◄──────────────────────►│  被测板       │
│  uart_tool   │   TX◄──────►RX          │  被测示例     │
│              │   RX◄──────►TX          │              │
└──────┬───────┘                        └──────────────┘
       │ LOGUART (115200)
       │
  PC/pytest (发送 "uarttool <text>" 命令)
```

pytest 通过对端板的 LOGUART 发送 `uarttool <text>` 命令，对端板将文本经 UART0 转发给被测板，等待回显并逐字节对比，返回 PASS/FAIL。

详情见 [tools/uart_tool/README.md](tools/uart_tool/README.md)。

---

## 编写新的 Example Pytest

1. 在 `raw/` 或 `mbed/` 下创建 `pytest_<peripheral>.py`
2. 遵循以下模板：

```python
"""
Pytest tests for <peripheral> example firmware.

Test cases:
- test_xxx — description

Usage::

    cd example/peripheral/auto_test/
    pytest raw/pytest_<peripheral>.py \\
        --target RTL8720F \\
        --port COM6
"""

import pytest


@pytest.mark.app_name("example_app_name")
@pytest.mark.parametrize("image_dir", ["./bin/example_app_name"], indirect=True)
def test_example(dut):
    """Example test case description."""
    dut.expect("expected output", timeout=10)
```

3. 需要双板测试时，添加 `multi_board` marker 和 `count=2`：

```python
@pytest.mark.multi_board
@pytest.mark.app_name("peer_app|dut_app")
@pytest.mark.parametrize("count", [2], ids=["dual"], indirect=True)
@pytest.mark.parametrize("image_dir",
    ["path/to/peer/bin|path/to/dut/bin"], indirect=True)
def test_dual_board(dut):
    peer, target = dut
    # ...
```

### 固件输出约定

pytest 通过 `dut.expect()` 匹配串口输出来判定 PASS/FAIL。建议被测示例固件输出明确的 **PASS** / **FAIL** 标记，使测试断言简洁可靠。

---

## Jenkins CI 集成

详见 `jenkins/README.md`：面向 **Jenkins Pipeline 开发人员**，说明环境准备、指令下发、硬件工位映射及输出产物。
