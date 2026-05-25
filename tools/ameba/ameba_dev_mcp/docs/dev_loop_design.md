# Ameba MCP Dev-Loop —— 整体分析与优化设计

> 范围：build → flash → reset → boot log → 交互这一典型嵌入式调试闭环。
> 目标：在不牺牲灵活性与可观测性的前提下，**减少 AI ↔ MCP 往返次数、解决串口缓冲区与 boot 完成判定的潜在风险**。
> 本文是设计提案 + 现状剖析；不直接落代码。落地节奏见 §10。

---

## 1. 现状：一次完整 dev-loop 的 MCP 调用图谱

刚才 RTL8720E "插桩复现 crash → 修代码 → 再验证" 的真实流：

```
1. Edit                     main.c:138（删 HAL_READ32(0x34000000)）
2. build_firmware()         ~5–60s（增量 5s；首次 + 工具链下载 30 min）
3. flash_firmware_tool      ~7s   (boot+app via remote)
4. serial_connect_tool      ~0.3s
5. reset_device_tool        ~0.5s
6. Bash sleep 3             人为对齐 boot 完成
7. serial_read_tool size=16k  → boot log
8. serial_write_tool "?"
9. Bash sleep 1
10. serial_read_tool()         → shell help
4. serial_close_tool      ~0.3s
```

**10 次 tool/外部调用，4 次串口 read，2 次硬编码 sleep。**

成本拆解：

| 维度 | 数字 |
|---|---|
| AI 上下文消耗 | 每个 tool 描述 + 入参 + 返回值 ≈ 200–600 token；本流约 **3 k–5 k tokens** |
| 时延 | tool 调用 RTT 各 50–200 ms × 10 ≈ **2 s 控制面开销** |
| 决策点 | 4 次 read 之间需要 AI"等多久"的判断（刚才用 sleep 1/3）；判错就拿不到 boot |
| 错误分支 | 每步都可能失败，AI 要分别处理；缺乏跨步骤"恢复"语义 |

主要痛点不是"调用次数太多"本身，而是：
- **boot 完成判定靠人肉 sleep**，没有结构化等待。
- **串口缓冲区是无界的字节流**，AI 要靠读出来的字节自己解析阶段（boot vs shell 输出）。
- **重复编辑-验证迭代**时，缓冲区里残留旧 boot 日志会造成数据混淆。

把这些核心问题先挑出来，比单纯堆"组合 tool"重要得多。

---

## 2. 串口管线现状（必须先理顺）

```
local 板:     pyserial.Serial               ──┐
remote 板:    RemoteSerial (TCP + bg thread) ──┤── ActiveSession (per alias) ──┐
                                                                                ├── BoardSessionManager (singleton)
                                              AAGParser (per alias, lazy)     ──┘
```

**关键事实**：

| 组件 | 行为 | 风险 |
|---|---|---|
| `RemoteSerial.receive_buffer` | 后台线程持续 `+=` 累加；调 `read(size)` 切片 + 移除前缀 | **无界增长**：监视长开 + 设备日志多 → OOM；mcp_improvement.md §1 已记 |
| `serial_read_tool` | 立即返回当前缓冲区前 `size` 字节，buffer 空就 `No data available` | **非阻塞**：AI 必须靠 sleep 等数据；时序依赖外部判断 |
| AAGParser | per-alias 持久；多核帧自动打 `[HP]/[LP]/[AP]` 标 | 与 reset 不联动；reset 后若 buffer 里还残留前一次 boot 的多核帧，标签可能错位 |
| reset_device_tool | local 走 DTR/RTS pulse；remote 走 RemoteSerial.reset_device | **不 drain buffer**：reset 之后立即 read 可能拿到的是 reset *之前* 的字节 |
| BoardSessionManager.reaper | 30 min 空闲自动断开 | 长时间监视的会话自动断；buffer 累计上限只受 reaper 限制 |

**真正的设计漏洞**：
1. **read 是非阻塞 + 缓冲区无 drain 操作** → AI 没法做"清干净 → reset → 收 boot log → 看到 SCHEDULER 就停"这种 deterministic 的等待。
2. **buffer 在 reset 前后是连续的同一段字节** → 没有"分段标记"或者"reset 后 only"的语义。
3. **boot 完成 / crash 检测全部丢给 AI 自己处理** → 浪费上下文，且 AI 不一定每次都用同样的 pattern。

---

## 3. 真实场景下的 8 种典型 dev 流

抽象出来，AI 实际想表达的"意图"远比 10 次 raw 调用少：

| 场景 | 意图 |
|---|---|
| A. 改代码后冒烟 | build → flash → reset → 等到 SCHEDULER → 报告是否 boot OK |
| B. 复现一次 crash | flash 已编译 → reset → 等到 `Crash Dump` 或 boot 完成 → 返回 dump + addr2line |
| C. 抓启动日志 | reset → 等到稳定（SCHEDULER + N 秒静默）→ 整段 boot log |
| D. 单步发命令 | 写一行 → 等回显（提示符 / 关键词） → 返回响应 |
| E. 长时监视 | 持续读 N 秒，按行/按 pattern 切片返回 |
| F. 配置变更回归 | menuconfig 改 → build → flash → 多板并行 reset & monitor → 比对日志 |
| G. ATS 对接 | 把 alias 当 testbed 节点，喂 case |
| H. 多板互测 | 板 A 发包，板 B 收包，时间对齐 |

A/B/C/D 是 80% 用例，全都需要**结构化等待 + 缓冲区分段**。

---

## 4. 设计方案空间（5 选 1）

### Option A：单一巨型 tool `dev_loop`

```python
dev_loop(alias, *, build=True, flash=True, reset=True,
         read_boot_until="START SCHEDULER", boot_timeout=15) -> dict
```

**优点**：1 次调用、上下文最省（~700 → ~200 token）。

**缺点**：
- 单步任意一阶段失败，错误归因/恢复都得自己塞返回里 → 分支变复杂。
- 一次返回里塞 build output、flash log、boot log、超时状态，**AI 难以选择性消化**。
- 长时阻塞：build 30 min、boot 15s 串行 → 单 tool 调用近 30 min，其它 tool 排队。
- AI 失去中途介入能力（比如 build warning 多想停下检查）。
- "组合"是死的：场景 G/H 一旦不是 build+flash+reset+read，就得加更多 boolean 开关或第二个巨型 tool。

### Option B：场景化薄壳 (`flash_and_monitor`、`build_and_flash`、`smoke_test`、`reproduce_crash`…)

**优点**：每个 tool 表达明确意图，AI 选粒度。
**缺点**：tool 数迅速膨胀；维护两套 API（粗粒度 + 细粒度）；薄壳内部还是要解决 §2 的 buffer / boot-detect 问题，不解决就是把痛点搬家。

### Option C：状态化 Session/Script DSL

```python
session = open_session(alias)
session.run("""
  build
  flash
  reset
  expect "KM4 START SCHEDULER" within 15s
  send "?"
  expect "MONITOR-I"
""")
```

**优点**：表达力极强，单调用搞定复杂时序。
**缺点**：
- 在 MCP 里发明新语言/新状态机 → AI 学习成本高、出错难调。
- 把 AI 决策能力替换成脚本，反而压缩了它在调试中的判断空间（debug 的本质就是分支）。
- 与 ATS 的角色重叠。

### Option D：底层 Primitive 增强（drain / expect / wait_pattern）

不增加组合 tool，而是让现有 primitive 更**确定性**：

```
serial_drain(alias)                     # 清空 buffer 与 AAG 状态
serial_expect(alias, pattern,           # 阻塞读，命中 / 超时 / 静默  （正则）
              timeout=15, idle=2)
reset_device_tool(alias, drain_first=True,
                  wait_for=None)         # reset_device 内置 drain + 可选等待
```

**优点**：最小语义增量；AI 仍主导编排但每步确定性强；任何 §3 场景都能用 3-5 个 primitive 表达。
**缺点**：调用次数没真的减少（仍 5-7 次）；token 节省靠返回值精简（boot log 切片返回，不返回原始大字节流）。

### Option E（推荐）：D + 一个薄壳 `quick_flash`

把 D 的 primitive 做扎实，再在它上面做**一个**面向最高频场景的 alias：

```
quick_flash(alias, *, build=False,
            wait_for="START SCHEDULER",
            timeout=15) -> {boot_log, matched, elapsed_ms, ...}
```

**80% 场景一次搞定**（场景 A/B/C），剩下 20% 用 primitive 拼。同时 primitive 是公开 API，不藏在 quick_flash 里。

**这是本文推荐路线。**

---

## 5. 推荐方案的 Tool Surface（增量）

### 5.1 串口 primitive 增强

| Tool | 签名 | 行为 |
|---|---|---|
| `serial_drain` | `(alias)` | 丢弃当前 buffer 全部数据；reset AAG 解析器状态；返回 `{dropped_bytes}` |
| `serial_expect` | `(alias, patterns: list[str], timeout=15.0, idle=None, max_bytes=64*1024)` | 阻塞地从串口读，**命中任一正则** / **idle 秒无新字节** / **timeout** / **max_bytes** 任一条件即返回 |
| `reset_device_tool` | `(alias, drain_first=True, wait_for=None, wait_timeout=15.0)` | 已有；新增三个参数 |
| `serial_write_tool` | `(alias, data, expect=None, timeout=5.0)` | 已有；可选直接发 + 等回显 |
| `serial_read_tool` | 不变 | 仍为最原始非阻塞读；保留向后兼容 |

`serial_expect` 返回结构（建议）：

```json
{
  "success": true,
  "alias": "RTL8720E_COM20",
  "matched": "START SCHEDULER",
  "match_index": 4,                 // 在返回 text 中字节偏移
  "reason": "pattern" | "idle" | "timeout" | "max_bytes",
  "data": "<整段从读起到命中行末的 text>",
  "elapsed_ms": 1342,
  "buffer_left": 88,                // 命中后剩余在 buffer 里的字节数
  "trace": [...]                    // 若检测到 Crash Dump，自动附 addr2line 解析
}
```

### 5.2 高层薄壳

| Tool | 签名 | 内部 |
|---|---|---|
| `quick_flash` | `(alias, build=False, wait_for="START SCHEDULER", timeout=15.0, idle=None)` | (opt build) → flash → drain → reset → expect |
| `monitor` | `(alias, duration=10.0, until=None)` | drain → 持续读 duration 秒或直到 pattern；返回带行号的 chunked text |

注意：**只新增 2 个薄壳**，避免 API 膨胀。F/G/H 等小众场景仍走 primitive。

### 5.3 自动行为

| 场景 | 自动化 |
|---|---|
| `expect` 命中 `========== Crash Dump ==========` | 自动 collect register dump + 调本机 `arm-none-eabi-addr2line` 把 PC/LR/栈帧反查源码 → 塞进 `trace` |
| `quick_flash` 检测到 Crash Dump | 不视为成功 boot；返回 `success=false, reason="crash"` 但带完整 trace |
| `expect` 超时 | 不视为致命；返回 `reason="timeout"` + 已读 buffer，让 AI 决策（可能本来就是要观察"什么都没打印"）|

---

## 6. 串口缓冲区的彻底治理（避免在更高层留坑）

§4-D 增强串口 primitive 之前，**RemoteSerial.receive_buffer 必须先治**：

| 改 | 现状 | 目标 |
|---|---|---|
| 大小限制 | 无界 `bytearray +=` | 环形缓冲，默认 1 MiB，超出丢最旧并 emit `BUFFER_OVERFLOW_DROP` 事件 |
| 阻塞读 | 没有 | 加 `read_blocking(size, timeout)` 给 expect 用，不影响现有非阻塞 `read(size)` |
| 序列号 / mark | 没有 | 加 `mark()` 返回当前写入位置 token；`read_since(mark)` 拿 mark 之后的字节 → drain 之后 reset 拿到的就是干净 boot 段 |
| 线程安全 | GIL 兜底 | `threading.Lock` 包 `receive_buffer` 写入与读出 |
| AAG 状态 | 与 buffer 共生 | drain 时同步重置（否则前后 boot 的核标会粘连） |

**buffer 治理是先决条件**——`serial_expect` 如果不能"从某一位置开始读"，永远会被 stale 数据污染。

---

## 7. 优缺点矩阵

| 维度 | 现状 | Option A 巨型 tool | Option E 推荐 |
|---|---|---|---|
| 单流 token | ~3-5 k | ~0.5-1 k | ~1.5-2.5 k |
| 单流耗时 | =真耗时+控制面 | 同 | 同（控制面省一半） |
| AI 中途介入 | 强 | 几乎不可能 | 强 |
| 错误归因 | 每步独立 | 单返回值塞所有阶段 | 每 primitive 独立 + 薄壳返回 stage 字段 |
| 与 ATS 关系 | 共存 | 重叠 | 共存 |
| 多板时序 | AI 编排 | 巨型 tool 不擅长 | primitive 直接支持 |
| 学习成本 | 中 | 低（但黑盒） | 中（多 2 个 primitive） |
| 实现成本 | 0 | ★★ (大) | ★★ (中，主要在 buffer 治理) |
| 长跑稳定性 | 差（buffer 无界） | 同 | **彻底治** |
| 文档复杂度 | 中 | 低 | 中（多 2 个 tool 的页） |

---

## 8. 风险

1. **`serial_expect` 阻塞期间整条 MCP 长占用**
   - 默认 timeout 15s，最大可调；若超过 MCP 工具自身上限会被切。
   - 缓解：tool 内部按 ~250 ms 切片读 + 周期性回触 keepalive（如 MCP 协议支持），或者在文档里明示"长 expect 请先 split"。
2. **正则注入**
   - AI 给 pattern 是字符串。要求 `serial_expect` 调 `re.compile`，捕获 `re.error` → 返回 `INVALID_PATTERN`，不让 AI 把 buffer 里的内容意外当 regex 元字符。
3. **drain 误清未消费数据**
   - 在 quick_flash 内 drain 是显式动作；在 reset_device_tool 默认 `drain_first=True`，**对老脚本是行为变更** → 加显式参数让旧调用方能关掉。
4. **Crash auto-trace 调 addr2line 是同步外部进程**
   - 找不到 toolchain / 找不到 .axf 时只能给文字 dump；`trace` 字段允许为 null。
5. **薄壳与 primitive 行为不一致风险**
   - 通过严格用 primitive 实现 quick_flash 来规避（不复制粘贴 logic）。
6. **多 alias 同时 expect 的并发竞争**
   - per-alias 锁保护 buffer + AAG 状态；不同 alias 互不影响，验证一下 `BoardSessionManager` 当前已分别持有 `ActiveSession`，应该天然并发安全。

---

## 9. 机会（顺手能拿的二阶价值）

| 机会 | 价值 |
|---|---|
| **结构化 boot 阶段标记** | 把"已知 SoC 的 boot 关键字"做成内置 pattern map（`{RTL8720E: ["KM4 MAIN","START SCHEDULER","Available heap after wifi init"]}`）→ AI 不用记 → `quick_flash` 默认值更准 |
| **Crash auto-trace** | 已有 `addr2line` 路径在 dump 末尾；MCP 抢先解析能省一次 AI 调用 + 一次 bash |
| **boot-perf 数据** | `quick_flash` 返回里带 `boot_to_main_ms`、`boot_to_scheduler_ms` → 给以后 perf regression 跟踪打地基 |
| **多板 fixture** | `monitor(alias)` 每个返回里带 wall-clock；多板比对 trivial |
| **ATS 对接** | `serial_expect` 是 ATS case 的 minimal primitive，未来如果 ATS 想走 MCP 也无缝 |
| **streaming**（mid-term） | MCP 协议支持流式后，`serial_expect` 可以走流，AI 做"边读边判断"，目前先按一次性返回 |

---

## 10. 落地节奏（建议）

### Phase 1 — buffer 治理（必须）

| 改动 | 文件 |
|---|---|
| `RemoteSerial`: ring buffer + lock + `mark()` + `read_blocking` | `tools/ameba/RemoteService/remote_serial.py` |
| `pyserial` 本地侧也加 `read_blocking` 包装 | `ameba_dev_mcp/config/board_session.py` |
| AAG 解析器加 `reset()` | 现成 |
| 单测：边界 + 溢出丢弃 + 多线程并发读写 | `ameba_dev_mcp/tests/test_remote_serial_buffer.py`（新） |

### Phase 2 — primitive 增强

| 改动 | 文件 |
|---|---|
| `serial_drain(alias)` | `ameba_dev_mcp/tools/serial.py` |
| `serial_expect(alias, patterns, timeout, idle, max_bytes)` | 同上 |
| `reset_device_tool(... drain_first, wait_for)` | 同上 |
| `serial_write_tool(... expect, timeout)` | 同上 |
| 文档：docs/architecture.md tool 表更新 | docs/ |

### Phase 3 — Crash auto-trace

| 改动 | 文件 |
|---|---|
| 抽 `crash_parser.py`：识别 dump、抓 PC/LR/栈、调 addr2line | `ameba_dev_mcp/tools/crash.py`（新） |
| `serial_expect` 命中 dump pattern 时自动调用 | `tools/serial.py` |

### Phase 4 — 薄壳

| 改动 | 文件 |
|---|---|
| `quick_flash(alias, build, wait_for, timeout, idle)` | `ameba_dev_mcp/tools/dev_loop.py`（新） |
| `monitor(alias, duration, until)` | 同上 |
| 文档：新增 `docs/dev_loop.md` 用例集 | docs/ |

### Phase 5 — 弃用决策

不弃用任何 primitive。`quick_flash` 与 `serial_*` 是不同抽象层级，长期共存。

**总工时估算**：Phase 1 + 2 各 1 天，Phase 3 半天，Phase 4 半天，Phase 5 0；总 3 个工作日左右，含单测与文档。

---

## 11. 推荐结论

> **不要直接做巨型 dev_loop。先把缓冲区与 expect 这层确定性补齐，再加薄壳。**

具体建议接下来按 Phase 1 → 2 推进，Phase 3 视真实使用频率决定（crash auto-trace 是利器但不紧急）。

走完 Phase 1 + 2，前面那个真实 RTL8720E 流程会从 10 步压成：

```
1. Edit
2. build_firmware()
3. quick_flash(alias, wait_for="START SCHEDULER")     ← 内部：flash + drain + reset + expect
4. serial_write(alias, "?", expect="MONITOR-I")       ← 内部：write + expect
```

**4 步、零 sleep、boot 完成自动检测、crash 自动 trace；同时 primitive 仍可用，灵活性不掉。**

---

## 12. 附录：当前与未来 tool 对照

| 当前 | 未来（增量） |
|---|---|
| `set_target / build_firmware / kconfig_*` | 不变 |
| `flash_firmware_tool(alias)` | 不变 |
| `serial_connect / disconnect_tool(alias)` | 不变（薄壳内部仍幂等连接） |
| `serial_read_tool(alias, size, timeout)` | 不变（保底） |
| `serial_write_tool(alias, data)` | + `expect`、`timeout` 可选 |
| `reset_device_tool(alias)` | + `drain_first=True`、`wait_for`、`wait_timeout` |
| `list_serial_ports_tool(alias?)` | 不变 |
| — | `serial_drain(alias)` |
| — | `serial_expect(alias, patterns, timeout, idle, max_bytes)` |
| — | `quick_flash(alias, build, wait_for, timeout)` |
| — | `monitor(alias, duration, until)` |

新增 4 个、改动 2 个、其它不动。
