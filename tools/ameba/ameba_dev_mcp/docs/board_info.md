# `board_info.json5` 配置说明

`<SDK_ROOT>/board_info.json5` 描述测试床上的物理板：每块板一个 alias，记录串口 /
波特率 / 远程 host / 密码等。MCP 的 flash、串口、复位 tool **都只接收 alias**，
其余参数全部从这里读。

> 模板：[`board_info.template.json5`](board_info.template.json5) — MCP 在文件缺失时
> 自动生成的内容；想跳过自动建模板，直接 copy 到 `<SDK_ROOT>/board_info.json5` 也行。
>
> 第一次调用任意 flash / serial tool 时，如果文件不存在，MCP 会自动建一个空模板并提示路径。

---

## 1. 顶层结构

```json5
/*
 * board_info.json5 — test-bench config: which SoC is on which serial port.
 */
{
  "schema_version": 1,

  "defaults": {
    "baudrate": 1500000,
    "monitor_baudrate": 1500000,
    "chip_erase": false
  },

  "boards": {
    "RTL8721F_ttyUSB0": { ... },
    "RTL8730E_COM5_remote": { ... }
  }
}
```

| 字段 | 类型 | 说明 |
|---|---|---|
| `schema_version` | int | 当前 `1` |
| `defaults.baudrate` | int | 烧录与开端口默认速率（1.5 Mbps） |
| `defaults.monitor_baudrate` | int | 串口监视默认速率 |
| `defaults.chip_erase` | bool | 默认是否全片擦除 |
| `boards` | dict | key 即 alias，每个 board 一个条目 |

---

## 2. alias 命名建议

- 推荐格式：`<SOC>_<PORT>`，远程板加 `_remote` 后缀。
- 例：`RTL8721F_ttyUSB0`、`RTL8730E_COM5_remote`、`RTL8721F_ttyUSB1`、`RTL8721F_ttyUSB2`。
- 不强制；任意唯一字符串都接受。
- 双板互测：每块板独立 alias，避免错乱。

> AI 通过 MCP 资源 `board://list` 来发现可用 alias；用 `board://{alias}` 查详情（密码会被掩码）。

---

## 3. `BoardEntry` 字段

| 字段 | 必填 | 类型 | 默认 | 说明 |
|---|---|---|---|---|
| `soc` | ✓ | string | – | SoC 名（如 `RTL8721F`），用来在 `project_info.json5` 找烧录布局 |
| `transport` | – | `"local"` / `"remote"` | `"local"` | 本机串口 vs 远程 AmebaRemoteService |
| `port` | ✓ | string | – | `/dev/ttyUSB0` / `COM5` 等。**远程板填的是远端机器上看到的端口名** |
| `baudrate` | – | int | `defaults.baudrate` | 烧录与连接速率 |
| `monitor_baudrate` | – | int | `defaults.monitor_baudrate` | 串口监视速率 |
| `chip_erase` | – | bool | `defaults.chip_erase` | 烧录前是否全片擦除 |
| `remote` | 见下 | object | – | `transport=remote` 时必填 |
| `serial_log_record` | – | object | – | 串口日志落盘配置，见下文 |

`remote` 子对象：

| 字段 | 必填 | 类型 | 默认 | 说明 |
|---|---|---|---|---|
| `host` | ✓ | string | – | 远端 IP 或主机名 |
| `port` | – | int | `58916` | AmebaRemoteService 端口 |
| `password` | – | string | – | 可写入；建议用环境变量 `AMEBA_REMOTE_PWD` 覆盖 |

> Pydantic schema 严格 `extra="forbid"`：未声明的字段一律拒绝。

---

## 3.1 `serial_log_record`：串口日志落盘

配置后，**只要通过 MCP 打开该板的串口（任意 serial 工具触发的 open）就开始记录日志，关闭串口即停止**。
日志捕获的是**完整串口流**——即使 agent 调用带 `drain_first` 的工具清掉了接收缓冲，**日志文件依然保留全部内容**。
每行带 `HH:MM:SS.mmm` 时间戳；多核 AAG 输出复用与 serial 工具相同的解析逻辑（`[HP]/[LP]/[AP]` 标签）。

| 字段 | 必填 | 类型 | 默认 | 说明 |
|---|---|---|---|---|
| `enable` | – | bool | `false` | 是否开启日志捕获。关闭时无任何后台线程，串口读路径与历史完全一致 |
| `log_dir` | – | string | `PROJECT_ROOT/mcp_serial_log` | 日志目录。相对路径挂在 `PROJECT_ROOT` 下；目录自动创建 |
| `file_name` | – | string | 自动生成 | 见下方命名/翻天规则 |

**文件名与翻天规则**（生成模式 `<alias>_<YYYYMMDD>_<HHMMSS>.log`，如 `RTL8721F_COM23_20260605_141930.log`）：

- `file_name` 为空 → 按当前系统时间生成，并**回写** `board_info.json5`。
- `file_name` 命中生成模式且日期==今天 → **复用**该文件（追加），不回写。
- `file_name` 命中生成模式但日期≠今天 → 生成新名并回写（保证**每天至少一个新日志**；长会话跨午夜也会自动滚动）。
- `file_name` **不**命中生成模式（用户自定义名）→ **原样使用，永不翻天、永不回写**。

> 注意：模式是**秒级**（`HHMMSS`，6 位）。形如 `..._1419.log`（分钟级 4 位）会被当成"用户自定义名"，不会按天滚动。
>
> 回写会按模板重渲染整个 `board_info.json5`（仅保留模板头注释，用户行内注释不被保留——与既有 `save_board_info` 行为一致）。

示例：

```json5
"RTL8721F_COM23": {
  "soc": "RTL8721F",
  "transport": "remote",
  "port": "COM23",
  "remote": { "host": "127.0.0.1", "port": 58916 },
  "serial_log_record": {
    "enable": true
    /* log_dir 省略 → PROJECT_ROOT/mcp_serial_log；file_name 省略 → 首次打开自动生成并回写 */
  }
}
```

---

## 4. 本地板示例

```json5
"RTL8721F_ttyUSB0": {
  "soc": "RTL8721F",
  "transport": "local",
  "port": "/dev/ttyUSB0"
  /* baudrate / monitor_baudrate / chip_erase 全部继承 defaults */
}
```

最小写法就是 `soc + port` —— 其它字段都有合理默认。

---

## 5. 远程板示例

```json5
"RTL8730E_COM5_remote": {
  "soc": "RTL8730E",
  "transport": "remote",
  "port": "COM5",
  "remote": {
    "host": "192.168.1.100",
    "port": 58916,
    "password": "secret123"
  }
}
```

> **远程密码处理**：
> 1. 优先读环境变量 `AMEBA_REMOTE_PWD`。
> 2. 若环境变量未设置，使用 json5 中的 `password`。
> 3. 通过 MCP 资源 `board://{alias}` 查看时，密码恒被掩码为 `"***"`。
> 4. 写盘时 **不会** 自动掩码（`save_board_info` 写回原值），所以慎重 commit json5。

---

## 6. 双板互测示例

两块同型号 SoC 同时连机器上：

```json5
"boards": {
  "RTL8730E_ttyUSB1": { "soc": "RTL8730E", "transport": "local", "port": "/dev/ttyUSB1" },
  "RTL8730E_ttyUSB2": { "soc": "RTL8730E", "transport": "local", "port": "/dev/ttyUSB2" }
}
```

测试用两个 alias，AI 可以同时 `serial_connect_tool("RTL8730E_ttyUSB1")` 与
`serial_connect_tool("RTL8730E_ttyUSB2")`，互不串扰（每个 alias 一份独立的 AAG 解析器与连接）。

---

## 7. `chip_erase` 用法

- 默认 `false`：只擦写本次烧录涉及的扇区。
- 改成 `true`：每次 `flash_firmware_tool(alias)` 之前都会发 chip-erase 指令，整片擦后再烧。
- 适合"换工程 / 换 SoC 配置"或排查 OTA 残留时使用；常规迭代不要打开（速度慢）。
- 优先级：`boards.<alias>.chip_erase` > `defaults.chip_erase`。

---

## 8. 注释规范

- 仅 `/* … */` 块注释。
- 不要使用行尾 `//`。
- 自动生成的 header 注释由 `templates.render_board_info` 维护，回写时会被重写。

---

## 9. 错误码与引导

| 错误码 | 触发 | 排查 |
|---|---|---|
| `BOARD_CONFIG_MISSING` | 文件不存在 | MCP 会自动建模板，按返回值里的 `template_path` 编辑 |
| `BOARD_CONFIG_PARSE_ERROR` | json5 语法错 | 检查行尾 `//`、尾随逗号、不平衡的 `{}` |
| `BOARD_CONFIG_INVALID` | schema 校验失败 | 按 `field_path` 定位字段；常见：缺 `soc`/`port`，或 `transport=remote` 缺 `remote` |
| `ALIAS_NOT_FOUND` | flash / serial 传了错的 alias | 错误信息里会列出已配置 alias；查 `board://list` |
| `REMOTE_SPEC_MISSING` | `transport=remote` 缺 `remote` 块 | 加 `"remote": { "host": "..." }` |
| `REMOTE_HOST_EMPTY` | `remote.host` 是空字符串 | 填正确 IP/主机名 |
| `REMOTE_UNREACHABLE` | TCP 连不上远端 | 检查远端机器是否上电、`AmebaRemoteService.py` 是否在运行、host/port 是否正确 |
| `REMOTE_TCP_FAILED` | 其它 TCP 错 | 看 `message` 里的具体异常 |
| `REMOTE_AUTH_FAILED` | 密码错 | 改 `boards.<alias>.remote.password` 或 `export AMEBA_REMOTE_PWD=...` |
| `REMOTE_OPEN_FAILED` | 远端 open 失败 | 远端串口可能不存在或被占用 |
| `PORT_NOT_FOUND` | 本机串口不存在 | 检查 USB 线 / dmesg / 改 `port` |
| `PORT_BUSY` | 端口被占 / 权限不足 | 关掉占用进程；本地 Linux 检查用户是否在 `dialout` 组 |
| `PORT_OPEN_FAILED` | 其它本机打开失败 | 看 `message` |
| `FLASH_HW_ERROR` | 子进程烧录失败 | 检查 USB 线、板上电、boot mode；确认 `port` 与实际板匹配；看 `log_path` |

---

## 10. 工作流（典型场景）

1. **首次使用**
   ```
   ai > 帮我烧 RTL8721F
   mcp > flash_firmware_tool("RTL8721F_ttyUSB0")
       ← BOARD_CONFIG_MISSING + template_path
   user > 编辑 board_info.json5，填入板子 alias
   ai > flash_firmware_tool("RTL8721F_ttyUSB0")
       ← success
   ```

2. **新增一块板**
   - 编辑 `boards`，加新 alias，AI 通过 `board://list` 自动发现。

3. **远程开发板**
   - 远端机器跑 `AmebaRemoteService.py`，本地 `board_info.json5` 写 `transport: "remote"` + `remote.host`，
     密码用 `AMEBA_REMOTE_PWD` 环境变量更安全。

4. **同 SoC 多板互测**
   - 不同 alias 即可；AI 可同时 connect / read / write 互不冲突。

---

## 11. 与 `project_info.json5` 的关系

- `board_info.json5` 描述 **"哪块板"**（物理 / 链路）。
- [`project_info.json5`](project_info.md) 描述 **"烧什么"**（image / 地址）。
- `flash_firmware_tool(alias)`：alias → `board_info.json5`.boards[alias] →
  `board.soc` → `project_info.json5`.projects[soc] → image 列表 → 烧录子进程。
- 同一个 SoC 的 `ProjectEntry` 被多 alias 共享，是有意设计：换板不需要再写一份 layout。

---

## 12. 完整示例

```json5
/*
 * board_info.json5 — test-bench config.
 */
{
  "schema_version": 1,

  "defaults": {
    "baudrate": 1500000,
    "monitor_baudrate": 1500000,
    "chip_erase": false
  },

  "boards": {
    "RTL8721F_ttyUSB0": {
      "soc": "RTL8721F",
      "transport": "local",
      "port": "/dev/ttyUSB0"
    },
    "RTL8730E_ttyUSB1": {
      "soc": "RTL8730E",
      "transport": "local",
      "port": "/dev/ttyUSB1",
      "chip_erase": true
    },
    "RTL8730E_ttyUSB2": {
      "soc": "RTL8730E",
      "transport": "local",
      "port": "/dev/ttyUSB2"
    },
    "RTL8730E_COM5_remote": {
      "soc": "RTL8730E",
      "transport": "remote",
      "port": "COM5",
      "remote": {
        "host": "192.168.1.100",
        "port": 58916
      }
    }
  }
}
```
