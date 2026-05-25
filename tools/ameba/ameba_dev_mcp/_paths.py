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

# RemoteService helpers are imported from ameba.RemoteService.remote_serial,
# but remote_serial.py itself does `from version_manager import *`, so its
# directory must also be on sys.path.
REMOTE_SERVICE_DIR = os.path.join(AMEBA_TOOLS_ROOT, "RemoteService")
