# SDK 内 vs 外部工程：路径解析与配置逻辑

> 速查：MCP server 如何区分「SDK 根」和「工程根」，以及哪些东西取自哪边。

## 两个根

| 概念 | 来源 | 拥有的东西 |
|---|---|---|
| **SDK_ROOT** | 由包自身位置回推（`tools/ameba/ameba_dev_mcp/_paths.py` 上溯 3 级），**永远不变** | `ameba.py`、工具链/`.venv`、Flash 工具与 `.rdev` profile、`tools/scripts`、`soc_map.dat`、flash layout 头文件 `component/soc/usrcfg/<ic>/ameba_flashcfg.{c,h}` |
| **PROJECT_ROOT** | `AMEBA_PROJECT_ROOT` 环境变量，未设则 `= SDK_ROOT` | `build_<soc>/`、`build/.config`、`soc_info.json`、`board_info.json5`、`project_info.json5`、build 子进程 cwd |

`_paths.py`：

```python
PROJECT_ROOT = os.path.abspath(os.environ.get("AMEBA_PROJECT_ROOT") or SDK_ROOT)
```

## 怎么切换

`AMEBA_PROJECT_ROOT` 由 `launcher.sh` / `launcher.bat` 的 `--project-root` 参数导出（参数会被 launcher **消费**，不透传给 `ameba-mcp`，支持 `~` 展开并转绝对路径）。

```bash
# SDK 内开发（默认，行为不变）
claude mcp add ameba-dev -- <SDK>/tools/ameba/ameba_dev_mcp/launcher.sh

# 外部工程（产物/配置全部落工程根）
claude mcp add ameba-dev -- <SDK>/tools/ameba/ameba_dev_mcp/launcher.sh \
    --project-root /home/miles_wang/ameba_claw_proj
```

按 **workspace 级** 注册，两个工作区可同名 `ameba-dev` 互不干扰。

## 为什么能行

`ameba.py` 本身就是 cwd 相对的：`ameba_manager.py` 用 `os.getcwd()` 定位工程、`os.getcwd()/build_<soc>` 放产物，而 `soc_map.dat` 等按脚本自身位置（SDK）读。SDK 早已把「SDK 位置」和「工程 cwd」分开——MCP 只需把构建子进程的 `cwd` 与配置文件目录指向 PROJECT_ROOT 即可。外部工程不 fork SoC 配置（无 `component/`），所以 flash layout 永远取自 SDK。

## flash layout 的「双根」拆分（关键）

`flashcfg_parser.parse_project(sdk_root, soc, build_base=None)`：

- **`sdk_root`**：Flash_Layout 表、地址宏、SoC→IC 映射、boot/app 镜像**名**模板 → 全来自 SDK。
- **`build_base`**（= PROJECT_ROOT）：`build_<soc>/build/.config`（既驱动地址宏解析，也决定 app_name 分支）+ 实际 bin 路径 → 来自工程。
- `build_base` 缺省回退 `sdk_root`，旧调用零变化。

即：**布局算法跑在 SDK 的 flashcfg 上，但喂给它的 `.config` 和产出的 bin 都来自工程。**

## 两次解析必须一致

| 时机 | 位置 | 调用 |
|---|---|---|
| 编译后回填 `project_info.json5` | `project.py::_sync_project_info` | `parse_project(SDK_ROOT, soc, build_base=PROJECT_ROOT)` |
| 烧录前重解析 + 比对 | `flash.py::_resolve_images_for_flash` | `parse_project(SDK_ROOT, soc, build_base=PROJECT_ROOT)` |

两处传**同一对** `(SDK_ROOT, PROJECT_ROOT)`，用同一份工程 `.config` + 同一份 SDK flashcfg → 地址必然一致；`_images_equal` 只比 type/addr（忽略 path），bin 又都在工程 build 目录 → 不会误报 `FLASH_LAYOUT_OUT_OF_SYNC`。

## 不随工程走的东西（仍锚 SDK）

`ameba.py` 路径、`AmebaFlash.py` / `.rdev` profile / `RemoteService`、工具链、`soc_map.dat`、flashcfg 头文件。改造时**只动** board/project json5 的根 + `parse_project` 的 `build_base`，这些一律不碰。`log/flash.log` 在包目录下（SDK），多工程共享。
