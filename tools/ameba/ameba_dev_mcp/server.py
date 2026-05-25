#!/usr/bin/env python3
"""
FastMCP Server for Ameba SoC Development.

This is the main entry point for the MCP service that provides
AI agents with tools for Realtek Ameba SoC firmware flashing
and serial communication.

Usage:
    # Run with stdio transport (default, for Claude Desktop)
    python -m ameba_dev_mcp.server

    # Run with HTTP transport
    python -m ameba_dev_mcp.server --transport http --port 8080

    # Run with SSE transport
    python -m ameba_dev_mcp.server --transport sse --port 8080

    # Or using FastMCP CLI
    fastmcp dev ameba_dev_mcp.server:mcp
"""

import os
import sys
import logging
import argparse
from contextlib import asynccontextmanager
from typing import AsyncIterator

# Ensure parent directory is in path for imports
# We need to go up two levels from ameba_dev_mcp/ to get to tools/ameba/
# then up one more level to tools/ so that 'import ameba' works
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))

from mcp.server.fastmcp import FastMCP

# Import tools and resources
from ameba_dev_mcp.tools.flash import register_flash_tools
from ameba_dev_mcp.tools.serial import register_serial_tools, cleanup_all_connections
from ameba_dev_mcp.tools.project import register_project_tools
from ameba_dev_mcp.tools.kconfig import register_kconfig_tools
from ameba_dev_mcp.tools.dev_loop import register_dev_loop_tools
from ameba_dev_mcp.tools.env_check import register_env_check_tools
from ameba_dev_mcp.prompts import register_prompts
from ameba_dev_mcp.resources.devices import register_device_resources
from ameba_dev_mcp.resources.boards import register_board_resources
from ameba_dev_mcp.resources.config import register_config_resources
from ameba_dev_mcp.resources.debug import register_debug_resources


# Configure logging
def setup_logging(level: int = logging.INFO) -> None:
    """Configure logging for the server."""
    logging.basicConfig(
        level=level,
        format="%(asctime)s - %(name)s - %(levelname)s - %(message)s",
        handlers=[
            logging.StreamHandler(sys.stderr)
        ]
    )


# Server state
class ServerState:
    """Server state container."""

    def __init__(self):
        self.serial_connections = {}
        self.monitor_threads = {}


@asynccontextmanager
async def app_lifespan(server: FastMCP) -> AsyncIterator[ServerState]:
    """
    Manage server lifecycle with resource cleanup.

    This context manager handles:
    - Initialization of server state
    - Cleanup of serial connections on shutdown
    """
    state = ServerState()
    logging.info("Ameba MCP Server starting up...")

    yield state

    # Cleanup on shutdown
    logging.info("Ameba MCP Server shutting down...")
    cleanup_all_connections()
    logging.info("Cleanup complete")


# Create FastMCP server instance
mcp = FastMCP(
    name="Ameba SoC Development Server",
    lifespan=app_lifespan,
)


def register_all() -> None:
    """Register all tools and resources with the server."""
    # Register flash tools
    register_flash_tools(mcp)
    logging.info("Registered flash tools")

    # Register serial tools
    register_serial_tools(mcp)
    logging.info("Registered serial tools")

    # Register project tools (build, kconfig, …)
    register_project_tools(mcp)
    logging.info("Registered project tools")

    # Register kconfig tools (get, set, search)
    register_kconfig_tools(mcp)
    logging.info("Registered kconfig tools")

    # Dev-loop module currently registers no MCP tools (quick_test_tool was
    # removed; quick_test() remains as a Python function for shell scripts).
    register_dev_loop_tools(mcp)
    logging.info("Registered dev-loop tools (none)")

    # Register environment pre-check tool (env_pre_check_tool)
    register_env_check_tools(mcp)
    logging.info("Registered env-check tools")

    # Register device resources
    register_device_resources(mcp)
    logging.info("Registered device resources")

    # Register board / config resources (alias discovery + json5 inspection)
    register_board_resources(mcp)
    register_config_resources(mcp)
    logging.info("Registered board/config resources")

    # Register hardware-debug reference (debug://hardware) — fetched on demand
    register_debug_resources(mcp)
    logging.info("Registered debug resources")

    # Register MCP prompts (slash-style workflow shortcuts)
    register_prompts(mcp)
    logging.info("Registered prompts")


def main() -> None:
    """Main entry point."""
    parser = argparse.ArgumentParser(
        description="FastMCP Server for Ameba SoC Development"
    )
    parser.add_argument(
        "--transport",
        choices=["stdio", "http", "sse"],
        default="stdio",
        help="Transport protocol (default: stdio)"
    )
    parser.add_argument(
        "--port",
        type=int,
        default=8080,
        help="Port for HTTP/SSE transport (default: 8080)"
    )
    parser.add_argument(
        "--host",
        default="0.0.0.0",
        help="Host for HTTP/SSE transport (default: 0.0.0.0)"
    )
    parser.add_argument(
        "--debug",
        action="store_true",
        help="Enable debug logging"
    )

    args = parser.parse_args()

    # Setup logging
    log_level = logging.DEBUG if args.debug else logging.INFO
    setup_logging(log_level)

    # Register all tools and resources
    register_all()

    # Run server
    if args.transport == "stdio":
        mcp.run()
    elif args.transport == "http":
        mcp.run(transport="http", host=args.host, port=args.port)
    elif args.transport == "sse":
        mcp.run(transport="sse", host=args.host, port=args.port)


if __name__ == "__main__":
    main()
