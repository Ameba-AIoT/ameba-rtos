# Dev-loop 用例集 — `quick_test_tool` 与 expect/drain primitives

本文配合 `architecture.md` 与 `dev_loop_design.md` 阅读。

- **设计来源**：`dev_loop_design.md`（Option E）
- **落地范围**：`tools/dev_loop.py`、`tools/serial.py`（drain / expect / connect+reset+wait / write+expect）
- **不动**：`tools/ameba/RemoteService/remote_serial.py`

---

## 1. 一图概览：什么时候用哪个 tool

```
                       ┌─ 想一次跑完 build→flash→reset→等 boot ──→ quick_test_tool
                       │
edit code → build      ├─ 已 build，想烧+复位+抓 boot                  → quick_test_tool(build=False)
                       │
                       ├─ 已 boot，想发命令并拿响应                    → serial_write_tool(expect=[...])
                       │
                       ├─ 已 boot，想等某个特定 log line               → serial_expect_tool(patterns, timeout)
                       │
                       ├─ 想清掉 buffer 残留再发命令                   → serial_drain_tool
                       │
                       ├─ 只想"瞄一眼"当前 buffer，不想等              → serial_read_tool
                       │
                       └─ 测试完了                                       → serial_disconnect_tool ⚠ 必须！
```

> **唯一不需要手动 disconnect 的 tool 是 `quick_test_tool`**（它默认 `auto_close=True`）。
> 其它打开串口的 tool 都会保留 session 直到你显式 disconnect 或 30 分钟空闲被 reaper 收走。

---

## 2. `quick_test_tool` 典型用例

### 2.1 改了代码，要冒烟

```python
# 1. 编辑 main.c
# 2. 一次调用搞定
quick_test_tool(alias="RTL8720E_COM20")
# 默认行为：build=True, wait_for=["KM4 START SCHEDULER", "Crash Dump"],
#          timeout=20s, auto_close=True
```

返回值：
```json
{
  "success": true,
  "alias": "RTL8720E_COM20",
  "soc": "RTL8720E",
  "build":   { "summary": "Build done", "log_path": "...", "image_count": 3 },
  "flash":   { "images_flashed": ["boot.bin", "app.bin"],
               "info": ["Optional image(s) not found, skipped: ['vfs.bin']"] },
  "wait":    { "reason": "pattern", "matched": "KM4 START SCHEDULER",
               "match_index": 1832, "data": "<完整 boot 日志>",
               "elapsed_ms": 4317, "buffer_left": 0 },
  "closed_after": true
}
```

10 步 dev-loop → **1 步**。

### 2.2 复现 crash

```python
quick_test_tool(
    alias="RTL8720E_COM20",
    wait_for=["Crash Dump", "KM4 START SCHEDULER"],
    timeout=15.0,
)
```

如果 boot 撞 crash dump：
```json
{
  "success": false,
  "stage": "wait",
  "errors": [{"code": "BOOT_CRASHED", "message": "matched crash pattern: 'Crash Dump'", ...}],
  "wait": { "reason": "pattern", "matched": "Crash Dump", "data": "<含完整 register dump>", ... },
  "closed_after": true
}
```

> 注意：**boot 撞 crash 算 success=false**——便于 CI 直接判 PASS/FAIL。
> dump 仍在 `wait.data` 里，AI 可以直接拿来分析（addr2line 自动反查见
> `mcp_improvement.md` 中的 follow-up）。

### 2.3 测完想接着交互

```python
result = quick_test_tool(alias="RTL8720E_COM20", auto_close=False)
# wait 命中 SCHEDULER 后串口仍开着
serial_write_tool("RTL8720E_COM20", "?", expect=["MONITOR-I"], timeout=2.0)
serial_disconnect_tool("RTL8720E_COM20")    # ⚠ 自己收尾
```

### 2.4 不需要 build（已外部 build 过）

```python
quick_test_tool(alias="RTL8720E_COM20", build=False)
# 跳过 build 阶段，直接 flash → reset → wait
```

---

## 3. Primitive 用例

### 3.1 等某个特定关键字

```python
serial_expect_tool(
    alias="RTL8720E_COM20",
    patterns=["WIFI init OK", "wlan err"],
    timeout=10.0,
)
# 命中前者 → success boot 路径
# 命中后者 → 拿到错误现场，但 expect 本身仍 success=true，靠 `matched` 区分
```

### 3.2 长监视（无具体 marker，等空闲）

```python
serial_expect_tool(
    alias="RTL8720E_COM20",
    patterns=[],            # 空列表：纯时间停止
    timeout=30.0,
    idle=2.0,               # 2 秒没新数据就当"日志稳定"
)
# 注意：patterns 为空时，必须设 idle，否则会报 ARGUMENT_INVALID
```

### 3.3 发命令收响应

```python
serial_drain_tool("RTL8720E_COM20")             # 清掉之前的 boot 残留
serial_write_tool(
    alias="RTL8720E_COM20",
    data="DW 60001000 4",
    expect=["DW <HexAddr>"],   # shell 回显或下一次命令提示
    timeout=2.0,
)
```

### 3.4 attach to running board（不复位）

```python
serial_connect_tool(
    alias="RTL8720E_COM20",
    reset=False,             # 默认 True；改 False 表示"我只是来看日志的"
    drain_first=False,       # 默认 True；改 False 保留已积累的字节
)
serial_read_tool("RTL8720E_COM20")
serial_disconnect_tool("RTL8720E_COM20")
```

---

## 4. expect / wait_for 写法指南

### 停止条件优先级

`pattern → max_bytes → idle → timeout`，先到先返回。

| 字段 | 行为 |
|---|---|
| `patterns` 命中 | `reason="pattern"`、`matched=<原 pattern 字符串>`、`match_index=<在 data 中的偏移>` |
| 累积字节超 `max_bytes`（默认 64 KiB） | `reason="max_bytes"` — 防止野日志吞 token |
| `idle` 秒无新字节（仅在已收到至少 1 字节后） | `reason="idle"` |
| 触达 `timeout` | `reason="timeout"` — `matched=null`，`data` 仍含已读到的字节 |

### 正则要点

- 用 `re.MULTILINE` 编译；`^` `$` 匹配每一行的开始/结束
- 元字符要转义：例如 `[BOOT-I]` 实际匹配应该写 `\[BOOT-I\]`
- 多模式 OR-match：`patterns=["KM4 START SCHEDULER", "Crash Dump"]` 任一命中即返回，`matched` 字段指明是哪个

### 常用 pattern 速查

| 想等待 | pattern |
|---|---|
| RTL8720E boot 完成 | `KM4 START SCHEDULER` |
| RTL8721F boot 完成 | `Available heap after wifi init` |
| 任意 SoC crash | `Crash Dump` |
| Shell 提示符回显 | `MONITOR-I` 或具体命令名 |
| WiFi 关联成功 | `connected to AP` |

---

## 5. 与 primitive 的关系

```
                         build_firmware ─────┐
                                             │
flash_firmware_tool ─── (low-level)          │
                                             ▼
serial_connect_tool   ─── (drain+reset+wait inside)
serial_drain_tool     ─── (just drain)              quick_test_tool
serial_expect_tool    ─── (just wait)               (composes ALL above
serial_write_tool     ─── (write + optional wait)    + auto_close)
serial_read_tool      ─── (non-blocking peek)
serial_disconnect_tool─── (release port + drop AAG)
```

**`quick_test_tool` 不重新实现任何业务逻辑**——它只是把上面这些 primitive 串起来，并加 stage attribution。所以一旦你看到 `quick_test_tool` 的某个 stage 出错，可以直接在对应的 primitive 上重试调试。

---

## 6. 反模式（不要这么写）

| ❌ | ✅ |
|---|---|
| `serial_connect_tool(alias)` 后立刻 `serial_read_tool` 期望拿到 boot log（race） | `serial_connect_tool(alias, wait_for=["..."], wait_timeout=15.0)` 让 connect 自己等 |
| `serial_read_tool` 之后 `time.sleep` 再 `serial_read_tool` 拼字节 | `serial_expect_tool(patterns=[...], timeout=10.0)` 一次拿全 |
| `quick_test_tool(auto_close=False)` 之后忘了 `serial_disconnect_tool` | 测完显式 disconnect；或者干脆让 `auto_close` 保持默认 |
| `wait_for=[]` 不带 `idle` | 必须至少给一个 stop 条件：要么有 pattern，要么 `idle` 不为 None |
| 在 pattern 里直接写 `[BOOT-I]` | 写 `\[BOOT-I\]`（转义方括号）|

---

## 7. 与 `dev_loop_design.md` 的差异

设计文档原本计划：
- 改 `RemoteSerial` 加 ring buffer + lock + mark/read_since → **本迭代未做**，移入 `mcp_improvement.md`
- 加 `monitor_tool` 高层薄壳 → **本迭代砍掉**，`serial_expect_tool(patterns=[], idle=...)` 已等价
- Crash auto-trace（命中 dump 自动跑 addr2line）→ **本迭代不做**，仅记录到 `mcp_improvement.md`

实际落地的就是：drain / expect / 扩 connect/write / 删 reset_device / 加 quick_test 这五件事。
