"""MCP prompts for Ameba dev workflow.

Prompts are deliberately terse — they only encode multi-step procedures
the agent would otherwise have to derive on its own. Detailed schemas
and error codes still live in tool docstrings.
"""

from mcp.server.fastmcp import FastMCP


def register_prompts(mcp: FastMCP) -> None:
    @mcp.prompt(
        name="ameba-setup-boards",
        description="Bootstrap board_info.json5 in one round-trip.",
    )
    def ameba_setup() -> str:
        return (
            "Bootstrap an Ameba test bench. Drive the conversation in TWO\n"
            "AskUserQuestion calls (plus a free-text prompt for SoCs), so that\n"
            "users on a local bench are NOT shown remote-only fields. Ask the\n"
            "structural questions (board count, transport) FIRST, then the\n"
            "open-ended SoC list — once the user has told us how many boards,\n"
            "listing their SoCs is more concrete:\n"
            "\n"
            "Step 1 (FIRST AskUserQuestion — required for everyone):\n"
            "First, check if board_info.json5 already exists in the SDK root directory. Then determine whether it was added or modified.\n"
            "  Gather in one call:\n"
            "    - board_count (int >= 1)\n"
            "    - transport, with EXACTLY these two options as a fixed list:\n"
            "          'local'  — boards plugged directly into this machine\n"
            "          'remote' — any serial port forwarded via AmebaRemoteService\n"
            "                     (including ports inside WSL, or on another host)\n"
            "      Both options are equally valid; do NOT tag either with\n"
            "      '(Recommended)'.\n"
            "\n"
            "Step 2 (SECOND AskUserQuestion — ONLY if transport=='remote'):\n"
            "  Ask:\n"
            "    - remote_host (default 127.0.0.1)\n"
            "    - remote_password (default empty string)\n"
            "  remote_port is fixed at 58916; do not ask.\n"
            "  When transport=='local', SKIP this step entirely.\n"
            "\n"
            "Step 3 (FREE TEXT, plain question — NOT AskUserQuestion):\n"
            "  Ask the user for their SoCs list (one per board, matching\n"
            "  the board_count from step 1). Show a few examples (e.g.\n"
            "  RTL8721F / RTL8720E / RTL8730E / RTL8726E / RTL8720F / ...) and\n"
            "  make clear any SoC name supported by this SDK is fine. The SoCs\n"
            "  catalog is open-ended.\n"
            "\n"
            "Step 4 (REQUIRED: pair every SoC with its serial port):\n"
            "  apply_board_config_tool needs both `soc` AND `port` for every\n"
            "  board. board_count >= 2 means the agent MUST explicitly ask\n"
            "  the user which port corresponds to which SoC — never assume\n"
            "  the order in step 3 matches the physical order.\n"
            "\n"
            "  Port discovery options:\n"
            "    a) The user already knows the port names (e.g. 'RTL8721F is\n"
            "       on COM23, RTL8720E is on ttyUSB0'). Use those directly.\n"
            "    b) The user is unsure of the port names. Then:\n"
            "       - transport=='local'  → call list_serial_ports_tool()\n"
            "       - transport=='remote' → call scan_remote_ports_tool(host, password)\n"
            "       Show the discovered ports back to the user and ask them\n"
            "       to map each port to a SoC from step 3 (a single round of\n"
            "       AskUserQuestion is fine; for board_count >= 4 a free-text\n"
            "       'list one mapping per line' is also acceptable).\n"
            "\n"
            "  Goal: produce a complete `boards` array shaped like\n"
            "    [ {soc: 'RTL8721F', port: 'COM23'},\n"
            "      {soc: 'RTL8720E', port: 'ttyUSB0'}, ... ]\n"
            "  with one entry per board, ready to hand to apply_board_config_tool.\n"
            "\n"
            "Step 5 (Persist):\n"
            "  Write all entries with apply_board_config_tool, then run\n"
            "  env_pre_check_tool to verify. After success, relay\n"
            "  apply_board_config_tool's `remind_user` text to the user."
        )
