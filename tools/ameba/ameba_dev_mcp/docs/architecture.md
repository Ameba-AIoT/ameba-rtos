# Ameba MCP — 架构说明

本文描述 `tools/ameba/ameba_dev_mcp/` 下 MCP 服务的内部结构、工具/资源清单、数据流以及错误码体系。
配置文件字段细节见 [`project_info.md`](project_info.md) 与 [`board_info.md`](board_info.md)。

---

## 1. 设计目标

1. **AI 调用极简**：烧录与串口 tool 的入参收敛到 **`alias` 一个字符串**，所有连接 / 镜像 / 地址参数从两份 json5 自动解析。
2. **配置即数据**：板卡（测试床）配置与工程（烧录布局）配置分离：
   - `<SDK_ROOT>/board_info.json5`：物理板与串口 / 远程 / 速率参数。
   - `<SDK_ROOT>/project_info.json5`：每个 SoC 的 flash layout（auto / manual 两模式）。
3. **避免地址漂移**：用户既可能用 CLI 编译、也可能用 MCP 烧录；auto 模式在 **每次 flash 前都会再实时解析一次** SDK 头文件并与 json5 对账。
4. **结构化错误**：所有失败都返回 `{success: false, errors: [{code, field_path, message, hint}]}`，AI 可直接转述给用户、引导改 json5 或排查硬件。
5. **不向后兼容**：旧 tool 签名（带 `device / port / image_dir / connection_id` 的多参版本）已整体废弃，不再支持。

---

## 2. 目录与文件

```
tools/ameba/ameba_dev_mcp/
├── server.py                    MCP 启动入口，注册所有 tool / resource
├── _paths.py                    SDK_ROOT / TOOLS_ROOT / REMOTE_SERVICE_DIR 常量
├── models/
│   └── schemas.py               Pydantic 模型：ProjectInfo / BoardInfo / ResolvedBoard / ConfigError
├── config/
│   ├── flashcfg_parser.py       移植自 amebaFlash.ts，解析 ameba_flashcfg.{c,h}
│   ├── loader.py                json5 读 / 写 / 校验 / 增量更新
│   ├── templates.py             空模板与回写渲染（仅 /* */ 注释）
│   └── board_session.py         alias → ActiveSession 进程内单例
├── tools/
│   ├── flash.py                 flash_firmware_tool / list_serial_ports_tool（含占用诊断）
│   ├── serial.py                connect/disconnect/read/write/drain/expect — alias 驱动的串口 primitive
│   ├── dev_loop.py              quick_test_tool 薄壳（build → flash → reset → wait → auto_close）
│   ├── project.py               set_target / build_firmware（成功后自动同步 json5）
│   └── kconfig.py               kconfig_get / kconfig_set / kconfig_search / kconfig_apply_file
├── resources/
│   ├── boards.py                board://list、board://{alias}
│   ├── config.py                config://project_info、config://board_info
│   └── devices.py               .rdev profile 元数据资源
├── tests/                       pytest（loader / board_session / 等）
└── docs/                        本目录
    ├── architecture.md
    ├── project_info.md
    ├── board_info.md
    ├── dev_loop_design.md       Step 3 设计提案与备选方案分析
    └── dev_loop.md              quick_test 用例集 + expect/drain 用法
```

---

## 3. MCP 工具清单（AI 直接调）

| Tool | 签名 | 模块 | 说明 |
|---|---|---|---|
| `set_target` | `(soc_name: str)` | `tools/project.py` | 切换当前编译目标 SoC |
| `build_firmware` | `()` | `tools/project.py` | 构建当前 SoC；**成功时自动增量同步 `project_info.json5`** |
| `kconfig_get` | `(symbols: list[str], soc?: str)` | `tools/kconfig.py` | 查 Kconfig 当前值 |
| `kconfig_set` | `(assignments: list[str], soc?: str)` | `tools/kconfig.py` | 设 Kconfig（依赖不满足整体拒绝） |
| `kconfig_search` | `(pattern: str, soc?: str)` | `tools/kconfig.py` | 按正则搜符号 / prompt |
| `kconfig_apply_file` | `(files: list[str], soc?: str)` | `tools/kconfig.py` | 应用 `prj.conf` 等配置档（从 `default.conf`+files **重生成** `.config`，会重置 `kconfig_set` 改动）；`build_firmware(config_files=...)` 可在构建前一次性应用 |
| `flash_firmware_tool` | `(alias?: str)` | `tools/flash.py` | 烧录。**`alias` 单板时可省**；多板必传，未传返回 `ALIAS_REQUIRED` 列出可选 |
| `list_serial_ports_tool` | `(alias?: str)` | `tools/flash.py` | 不传 alias → 本机串口；传 alias 且远程 → 远端枚举。返回里每个 port 含 `busy / held_by_self / held_by_self_alias / holder` 占用诊断字段 |
| `env_pre_check_tool` | `(with_reset=False, soc_filter?: str)` | `tools/env_check.py` | **环境预检**：JSON 完整性 + 所有 alias 端口可见性 + 远端 TCP 可达性 + 可选 reset 烟测。失败结构化为 `BOARD_CONFIG_*` / `PORT_NOT_VISIBLE` / `REMOTE_UNREACHABLE` / `RESET_TEST_FAILED`，附带 `next_steps` 与 `debug://hardware` 指路 |
| `serial_connect_tool` | `(alias?, reset=True, drain_first=True, wait_for?, wait_timeout=15.0, idle?)` | `tools/serial.py` | 打开串口；默认在 open 后 drain + reset；`wait_for` 非空则阻塞等 pattern OR-match。`alias` 单板时可省 |
| `serial_disconnect_tool` | `(alias?)` | `tools/serial.py` | 关闭 + drop AAG parser。`alias` 单板时可省 |
| `serial_drain_tool` | `(alias?)` | `tools/serial.py` | 丢弃当前 buffer + reset AAG line state；下次 expect/read 不会被旧字节污染 |
| `serial_expect_tool` | `(alias?, patterns: list[str], *, timeout, idle?, max_bytes=65536)` | `tools/serial.py` | 阻塞读直到 pattern OR-match / max_bytes / idle / timeout 之一 |
| `serial_write_tool` | `(alias?, *, data, expect?, timeout=5.0, idle?)` | `tools/serial.py` | 写串口；`expect` 非空则写完阻塞等 pattern |
| `serial_read_tool` | `(alias?, size?: int=1024, timeout?: float=1.0)` | `tools/serial.py` | 非阻塞 peek；自动识别 AAG 多核帧 `[HP]/[LP]/[AP]` |
| `quick_test_tool` | `(alias?, build=True, wait_for?, timeout=10.0, idle?, auto_close=True)` | `tools/dev_loop.py` | **薄壳**：组合 build → flash → connect(reset+drain+wait) →（可选 disconnect）；测完自动关串口。`alias` 单板时可省 |

> **alias 单板可省语义**：当 `board_info.json5` 仅含一个 alias 时，所有 alias-driven 工具的 `alias` 参数可省略（自动选中唯一 alias）；2 个及以上时必传，未传返回 `ALIAS_REQUIRED` 错误码并在 hint 中列出全部可选 alias。`list_serial_ports_tool` 的 `alias=None` 是历史例外语义（无 alias = 列本机端口），不受单板自动选影响。

> **隐藏内部接口**（仅 MCP 自身调用，不向 AI 暴露）：
> - `validate_project_config(sdk_root, soc?)`
> - `validate_board_config(sdk_root, alias?)`
> - `ensure_project_info_template(sdk_root)` / `ensure_board_info_template(sdk_root)`
> - `update_project_for_soc(sdk_root, parsed_project)`

---

## 4. MCP 资源清单（AI 用来发现配置）

| URI | 内容 |
|---|---|
| `board://list` | `{count, aliases, boards: [{alias, soc, transport, port, remote_host}, …]}` |
| `board://{alias}` | 单板 `ResolvedBoard`（默认值已合并，密码掩码为 `***`） |
| `config://project_info` | 完整 `project_info.json5`（解析后） |
| `config://board_info` | 完整 `board_info.json5`（密码掩码） |
| `device://list` / `device://{name}` | `Devices/Profiles/*.rdev` 元数据 |
| `debug://hardware` | **硬件侧排查文档**（PL2303 5.2.x 驱动要求、UMH3N 自动下载电路、RTS/DTR 时序、错误码 → 排查路径索引）。**默认不进 system prompt**，仅在错误 envelope 通过 `resource_url` 字段指路时按需 fetch |

---

## 5. 核心数据流

### 5.1 `build_firmware` 自动同步 `project_info.json5`

```
ameba.py build -q
    │
    ▼
build 子进程 → 成功
    │
    ▼  (project.py:_sync_project_info)
flashcfg_parser.parse_project(SDK_ROOT, soc)
    │
    ▼
loader.update_project_for_soc(...)
    ├── SoC 不存在        → 新建 auto 条目
    ├── SoC 存在 & auto   → 覆盖 build_dir + images（generated 区）
    └── SoC 存在 & manual → 不动（用户拥有所有权）
    │
    ▼
templates.render_project_info(...)  ← 仅 /* */ 注释
    │
    ▼
write to <SDK_ROOT>/project_info.json5
```

返回值附 `project_info: {updated, soc, mode, image_count}`。

### 5.2 `flash_firmware_tool(alias)` 流程

```
flash_firmware_tool(alias)
    │
    ▼
load_board_info  ──── 不存在 ──→ ensure_board_info_template
    │                            返回 BOARD_CONFIG_MISSING + template_path
    ▼
alias 在 boards 中？─ 否 ──→ ALIAS_NOT_FOUND（含已配置 alias 列表）
    │
    ▼
validate_board_config(alias)（隐藏内部校验）
    │
    ▼
resolve_board(...)  →  ResolvedBoard（默认值已合并）
    │
    ▼
load_project_info → 找到 board.soc 的 ProjectEntry
    │            └─ 没有 → PROJECT_NOT_CONFIGURED ("先 build_firmware")
    ▼
validate_project_config(soc)  （AUTO_IMAGES_MISSING 不致命，其它致命）
    │
    ▼
_resolve_images_for_flash:
    ├── manual           → 直接用 json5 images[]
    └── auto:
        flashcfg_parser.parse_project(SDK_ROOT, soc)
        ├── 解析失败                  → FLASH_CFG_PARSE_FAILED
        ├── json5 images 为空          → 用解析值 + info（建议 build 同步）
        ├── 解析值 == json5            → 用 json5
        └── 解析值 ≠  json5            → FLASH_LAYOUT_OUT_OF_SYNC
    │
    ▼
所有 mandatory 镜像文件存在？→ 否 → IMAGE_FILE_NOT_FOUND
    │
    ▼
找 .rdev profile（Devices/Profiles/<SOC>[_NOR].rdev）→ 没有 → PROFILE_NOT_FOUND
    │
    ▼
释放 alias 上的 monitor session（避免端口冲突）
    │
    ▼
构造 AmebaFlash.py 子进程命令：
    --download --profile X.rdev --port .. --baudrate ..
    --memory-type ... [--remote-server H --remote-password ...]
    [--chip-erase] --partition-table <base64 JSON>
    │
    ▼
解析 stdout：'Finished PASS' / 'Finished FAIL'
    ├── 失败 → FLASH_HW_ERROR + 引导查硬件
    └── 成功 → {success, alias, soc, images_flashed, log_path, info, message}
```

### 5.3 串口 / 复位流程

`serial_*` 与 `reset_device_tool` 都走相同的 `_resolve_or_error(alias)`：
1. `load_board_info` → 缺失自动建模板 + `BOARD_CONFIG_MISSING`。
2. alias 不存在 → `ALIAS_NOT_FOUND`。
3. `validate_board_config(alias)` → `REMOTE_SPEC_MISSING` 等。
4. `resolve_board` → `ResolvedBoard`。
5. `BoardSessionManager.connect(board, monitor_mode=True)`：
   - local：`pyserial.Serial`（错误映射到 `PORT_NOT_FOUND` / `PORT_BUSY` / `PORT_OPEN_FAILED`）。
   - remote：`RemoteSerial`，先 TCP 连接 → `validate(password)` → `open()`；失败映射到 `REMOTE_UNREACHABLE` / `REMOTE_TCP_FAILED` / `REMOTE_AUTH_FAILED` / `REMOTE_OPEN_FAILED`。
6. AAG 帧解析器是 **per-alias 持久** 的（`_aag_parsers[alias]`），保证多次 read 之间 buffer 状态不丢。flash 前会同时 disconnect session **+** drop AAG parser，避免重烧后多核标签错位。

### 5.4 `expect / drain / reset` 时序

```
caller: serial_connect_tool(alias, reset=True, drain_first=True, wait_for=[…], wait_timeout=15)
                    │
                    ▼
session_manager.connect(board, monitor_mode=True)   ← 幂等
                    │
                    ▼
drain_first?  ── yes ──→ _drain_serial(sess)
                          ├── flushInput() / reset_input_buffer()
                          └── AAGParser.reset()       ← 清 _line_start
                    │
                    ▼
reset?        ── yes ──→ _apply_reset(sess)
                          ├── local:  setDTR/RTS pulse + 100ms ×2
                          └── remote: RemoteSerial.reset_device(timeout=10)
                    │
                    ▼
wait_for 非空? ─ yes ──→ _wait_loop(...)
                          while True:
                              ▽ inWaiting()/in_waiting → conn.read(N)
                              ▽ AAGParser.parse(chunk) → 累加到 accum
                              ▽ check max_bytes → reason="max_bytes"
                              ▽ for cp,orig in patterns: re.search(cp,accum) → reason="pattern"
                              ▽ if elapsed >= timeout → reason="timeout"
                              ▽ if idle and silent for idle s and got bytes → reason="idle"
                              sleep 50ms
                    │
                    ▼
return { reset_applied, dropped_bytes, wait: {reason, matched, data, …} }
```

`serial_expect_tool` / `serial_write_tool(expect=…)` 复用 `_wait_loop` 的同一份语义。

---

## 6. `BoardSessionManager`

**位置**：`config/board_session.py`，进程级单例 `manager`。

- 维护 `alias -> ActiveSession`，幂等 connect：已 open 的 session 直接 touch 后返回。
- `monitor_mode` 决定使用 `monitor_baudrate` 还是 `baudrate`；后者用于烧录。
- 后台 reaper 线程每 60s 扫一次，**30 分钟空闲** session 自动关闭（沿用旧 connection_id 的策略）。
- `atexit` 注册全局 `disconnect_all`，避免远程 `AmebaRemoteService.py` 10 路连接耗尽。
- flash 时由 `flash.py` 显式 `manager.disconnect(alias)`，让出端口给 `AmebaFlash.py` 子进程，烧录完后下次串口操作会自动重连。

---

## 7. `flashcfg_parser`（amebaFlash.ts → Python）

**位置**：`config/flashcfg_parser.py`。对应 TS 实现：`vscode_plugin/cline/src/core/controller/ameba/amebaFlash.ts`。

| 关键函数 | 职责 |
|---|---|
| `parse_project(sdk_root, soc)` | 入口：返回 `ParsedProject{soc, build_dir, images}` |
| `parse_flash_layout(c_path)` | 取 `Flash_Layout[]` 最后一个定义 |
| `parse_macros(h_path, dot_config)` | 处理 `if(CONFIG_X)…else()…endif()` 与 `#ifdef CONFIG_X…#else…#endif` |
| `eval_macro_expr` | 受限算术求值，**用 `ast.parse(mode="eval")` 白名单**，不调 `eval()` |
| `resolve_address` | 替换标识符 → 立即数 → 计算 |
| `get_boot_image_name` / `get_app_image_name` | 解析 CMakeLists 中 `ameba_set_if` / `ameba_firmware_package` |
| `validate_layout` | 4K 对齐 + 区段不重叠 |

SoC↔IC 映射读 `tools/scripts/soc_map.dat`（解码 `tools/scripts/ameba_soc_utils.py:120-145`），不硬编码。

---

## 8. JSON5 加载与缓存策略

- **加载**：`load_*_info(sdk_root)` 每次都从磁盘读，不做内存缓存（json5 文件改了立即生效；MCP 调用频率低）。
- **写入**：`save_*_info` 调 `templates.render_*` 重新生成文本，**保留固定 header 注释**；写完整文件，不做局部 patch。
- **注释规范**：仅使用 `/* … */` 块注释；**不允许行尾 `//`**（json5 库支持但项目规范禁止；测试断言不出现 `//`）。
- **路径**：所有 `path` / `build_dir` 都必须是绝对路径，validators 拒绝相对路径。

---

## 9. flash 前的双重对账（auto 模式）

为什么需要：用户可能用 CLI（`./ameba.py build`）编译，但用 MCP 烧录。CLI 路径 **不会** 自动同步 json5（同步逻辑在 MCP 的 `build_firmware` 里），所以 json5 的 `images[]` 可能落后于实际 SDK 头。

机制（`tools/flash.py:_resolve_images_for_flash`）：

| 状态 | 处理 |
|---|---|
| `flash_layout_setting_mode == "manual"` | 直接用 json5 |
| auto + 实时解析失败 | `FLASH_CFG_PARSE_FAILED`，停止烧录 |
| auto + json5 `images[]` 为空 | 用解析值烧 + info 提示 "建议 build_firmware 同步" |
| auto + 解析值 == json5 | 用 json5 |
| auto + 解析值 ≠ json5 | `FLASH_LAYOUT_OUT_OF_SYNC`，提示 "先 mcp build_firmware 同步" |

比较项：`(type, start_addr, end_addr)`（小写 hex 后比较），忽略 `path` 与 `optional`。

---

## 10. 错误码总表

| Code | 触发位置 | 含义 / 引导 |
|---|---|---|
| `BOARD_CONFIG_MISSING` | `loader.load_board_info` | 文件不存在；MCP 自动建模板，附 `template_path` |
| `BOARD_CONFIG_PARSE_ERROR` | `loader._read_json5` | json5 语法错（含行尾 `//` 之类） |
| `BOARD_CONFIG_INVALID` | Pydantic 校验 | 字段不符合 schema（type / range / extra） |
| `ALIAS_NOT_FOUND` | flash / serial | alias 不在 `boards` 中，附已配置 alias 列表 |
| `REMOTE_SPEC_MISSING` | validators / connect | `transport=remote` 但缺 `remote.host` |
| `REMOTE_HOST_EMPTY` | validators | `remote.host` 为空字符串 |
| `REMOTE_UNREACHABLE` | board_session | TCP 连不上远程 host |
| `REMOTE_TCP_FAILED` | board_session | 其他 TCP 错误 |
| `REMOTE_AUTH_FAILED` | board_session | 密码错；建议改 json5 或设 `AMEBA_REMOTE_PWD` |
| `REMOTE_OPEN_FAILED` | board_session | 远端串口打不开（杂项） |
| `PORT_NOT_FOUND` | board_session | 串口设备文件不存在；查 USB 或改 `boards.<alias>.port` |
| `PORT_BUSY` | board_session | 端口被占用 / 权限不足（dialout） |
| `PORT_OPEN_FAILED` | board_session | 其它打开失败 |
| `PROJECT_CONFIG_MISSING` | `loader.load_project_info` | `project_info.json5` 缺失（建议 build_firmware） |
| `PROJECT_CONFIG_PARSE_ERROR` | loader | json5 语法错 |
| `PROJECT_CONFIG_INVALID` | Pydantic 校验 | schema 不合 |
| `PROJECT_NOT_CONFIGURED` | flash | json5 中缺该 SoC 条目 |
| `PROJECT_PATH_NOT_ABSOLUTE` | validate | `build_dir` / `path` 不是绝对路径 |
| `BUILD_DIR_MISSING` | validate | auto 模式 `build_dir` 不存在；先 build |
| `AUTO_IMAGES_MISSING` | validate | auto 模式 `images[]` 为空（非致命；flash 会尝试 live 解析） |
| `MANUAL_IMAGES_EMPTY` | validate | manual 模式必须至少 1 个 image |
| `IMAGE_PATH_NOT_ABSOLUTE` | validate | image `path` 非绝对 |
| `IMAGE_FILE_NOT_FOUND` | validate / flash | image 文件不存在且非 optional |
| `IMAGE_RANGE_OVERLAP` | validate | manual 区段地址重叠 |
| `FLASH_CFG_PARSE_FAILED` | flash | live 解析 `ameba_flashcfg.{c,h}` 失败 |
| `FLASH_LAYOUT_OUT_OF_SYNC` | flash | json5 与实时解析 layout 不一致；先 build_firmware |
| `PROFILE_NOT_FOUND` | flash | `Devices/Profiles/` 找不到对应 `.rdev` |
| `FLASH_HW_ERROR` | flash | 子进程烧录失败，附 `log_path` |

---

## 11. 测试

```
tools/ameba/ameba_dev_mcp/tests/
├── test_config_loader.py       16 tests：模板 / update / 校验 / 掩码 / schema 拒绝
└── test_board_session.py        4 tests：本地 PORT_NOT_FOUND / 远程 unreachable / 簿记
```

跑法：
```
cd tools/ameba && python -m pytest ameba_dev_mcp/tests -v
```

集成测试（板上）见计划 §八，需要真实硬件。

---

## 12. 与 vscode-cline 插件的关系

`vscode_plugin/cline/src/core/controller/ameba/amebaFlash.ts` 是 TS 版的 flash 解析；本 MCP 把核心逻辑移植到 Python，但**不共享代码**。两边对同一份 SDK 的解析必须保持一致——单测用 cline 的输出做 ground truth。

---

## 13. 不再支持

- 旧 `flash_firmware_tool(device, port, image_dir, baudrate, memory_type, ...)` 等多参签名 → 已删除。
- 旧基于 `connection_id` 的串口模型 → 已替换为 alias。
- json5 文件中的行尾 `//` 注释 → 项目规范禁止；测试会校验。
