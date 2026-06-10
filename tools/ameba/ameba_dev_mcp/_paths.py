"""
Centralised path constants for the ameba_dev_mcp package.

Import from here instead of recomputing dirname chains in every tool file.
"""

import os

# This file lives at: tools/ameba/ameba_dev_mcp/_paths.py
_MCPSCRIPTS_ROOT = os.path.dirname(os.path.abspath(__file__))
AMEBA_TOOLS_ROOT = os.path.dirname(_MCPSCRIPTS_ROOT)       # tools/ameba/
TOOLS_ROOT       = os.path.dirname(AMEBA_TOOLS_ROOT)        # tools/
SDK_ROOT         = os.path.dirname(TOOLS_ROOT)              # SDK root (env_test/)

# PROJECT_ROOT is the *working directory* the build/flash/config files live
# under — build_<SOC>/, build/.config, soc_info.json, board_info.json5 and
# project_info.json5. It defaults to SDK_ROOT (in-SDK development, zero
# behaviour change), but can be pointed at an out-of-tree project consumed by
# the SDK (e.g. ~/ameba_claw_proj) via the AMEBA_PROJECT_ROOT env var, which
# launcher.sh exports from its `--project-root` argument.
#
# IMPORTANT: this is distinct from SDK_ROOT. The SDK still owns ameba.py, the
# toolchain, the Flash tools/profiles, and the flash-layout headers
# (component/soc/usrcfg/*/ameba_flashcfg.{c,h}). Only the per-project build
# artifacts and json5 config move with PROJECT_ROOT. See flashcfg_parser
# .parse_project(..., build_base=...) for the split on the flash side.
PROJECT_ROOT = os.path.abspath(os.environ.get("AMEBA_PROJECT_ROOT") or SDK_ROOT)

# RemoteService helpers are imported from ameba.RemoteService.remote_serial,
# but remote_serial.py itself does `from version_manager import *`, so its
# directory must also be on sys.path.
REMOTE_SERVICE_DIR = os.path.join(AMEBA_TOOLS_ROOT, "RemoteService")
