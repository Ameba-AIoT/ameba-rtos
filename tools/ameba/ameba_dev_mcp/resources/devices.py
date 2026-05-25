"""
Device profile resources for Ameba SoC.

These resources expose device profile information as MCP resources
that can be read by AI agents.
"""

import os
import sys
import json
from typing import Dict, Any, List

# Add parent directory to path for imports
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))

from mcp.server.fastmcp import FastMCP

# Import existing classes
from ameba.Flash.base.json_utils import JsonUtils
from ameba.Flash.base.device_profile import RtkDeviceProfile
from ameba.Flash.base.memory_info import MemoryInfo

# Profile directory
PROFILE_DIR = os.path.join(
    os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))),
    "ameba", "Flash", "Devices", "Profiles"
)


def _get_all_profile_files() -> List[Dict[str, str]]:
    """Get list of all profile files with their metadata."""
    profiles = []

    if not os.path.exists(PROFILE_DIR):
        return profiles

    for filename in sorted(os.listdir(PROFILE_DIR)):
        if filename.endswith(".rdev"):
            # Parse filename for device info
            base = filename[:-5]  # Remove .rdev
            parts = base.split("_")

            device_name = parts[0] if parts else base
            memory_type = "NOR"
            os_type = None

            for part in parts[1:]:
                if part.upper() in ["NOR", "NAND"]:
                    memory_type = part.upper()
                elif part.upper() in ["LINUX"]:
                    os_type = part.upper()

            profiles.append({
                "filename": filename,
                "device_name": device_name,
                "memory_type": memory_type,
                "os_type": os_type,
                "resource_uri": f"device://{device_name}/{memory_type.lower()}"
            })

    return profiles


def _load_profile_by_name(device_name: str, memory_type: str = "nor") -> Dict[str, Any]:
    """Load a device profile and return its information."""
    # Try different filename patterns
    patterns = [
        f"{device_name}.rdev",
        f"{device_name}_{memory_type.upper()}.rdev",
    ]

    # Also try with Linux suffix
    if memory_type.lower() == "nand":
        patterns.extend([
            f"{device_name}_Linux_NAND_128MB.rdev",
            f"{device_name}_Linux_NAND_256MB.rdev",
        ])
    elif memory_type.lower() == "nor":
        patterns.append(f"{device_name}_Linux_NOR_32MB.rdev")

    for pattern in patterns:
        profile_path = os.path.join(PROFILE_DIR, pattern)
        if os.path.exists(profile_path):
            try:
                profile_json = JsonUtils.load_from_file(profile_path, need_decrypt=True)
                profile = RtkDeviceProfile(**profile_json)

                # Build detailed info
                images = []
                for img in profile.images:
                    images.append({
                        "name": img.image_name,
                        "start_address": hex(img.start_address),
                        "end_address": hex(img.end_address),
                        "memory_type": ["RAM", "NOR", "NAND"][img.memory_type] if img.memory_type < 3 else "Unknown",
                        "mandatory": img.mandatory,
                        "full_erase": img.full_erase,
                        "description": img.description or ""
                    })

                return {
                    "device_name": profile.device_name,
                    "device_id": hex(profile.device_id),
                    "version": profile.version,
                    "memory_type": "NAND" if profile.memory_type == MemoryInfo.MEMORY_TYPE_NAND else "NOR",
                    "flash_start_address": hex(profile.flash_start_address),
                    "ram_start_address": hex(profile.ram_start_address),
                    "floader": profile.floader,
                    "floader_address": hex(profile.floader_address),
                    "handshake_baudrate": profile.handshake_baudrate,
                    "log_baudrate": profile.log_baudrate,
                    "support_usb_download": profile.support_usb_download,
                    "images": images,
                    "profile_file": pattern
                }
            except Exception as e:
                return {"error": str(e), "device_name": device_name}

    return {"error": f"Profile not found for {device_name}", "device_name": device_name}


def get_all_devices() -> Dict[str, Any]:
    """Get information about all supported devices."""
    profiles = _get_all_profile_files()

    # Group by device name
    devices = {}
    for profile in profiles:
        name = profile["device_name"]
        if name not in devices:
            devices[name] = {
                "device_name": name,
                "profiles": []
            }
        devices[name]["profiles"].append({
            "memory_type": profile["memory_type"],
            "os_type": profile["os_type"],
            "filename": profile["filename"]
        })

    return {
        "devices": list(devices.values()),
        "total_count": len(devices),
        "profile_directory": PROFILE_DIR
    }


def get_device_info(device_name: str, memory_type: str = "nor") -> Dict[str, Any]:
    """Get detailed information about a specific device."""
    return _load_profile_by_name(device_name, memory_type)


def register_device_resources(mcp: FastMCP) -> None:
    """
    Register device profile resources with the FastMCP server.

    Args:
        mcp: FastMCP server instance
    """

    @mcp.resource("device://profiles")
    def get_device_profiles() -> str:
        """
        List all available device profiles.

        Returns a JSON string containing information about all supported
        Ameba device profiles, grouped by device name.
        """
        data = get_all_devices()
        return json.dumps(data, indent=2)

    @mcp.resource("device://{device_name}/info")
    def get_device_profile(device_name: str) -> str:
        """
        Get detailed information about a specific device.

        Args:
            device_name: Device name (e.g., RTL8720E, RTL8730E)

        Returns:
            JSON string with device profile details
        """
        data = get_device_info(device_name, "nor")
        return json.dumps(data, indent=2)

    @mcp.resource("device://{device_name}/{memory_type}/info")
    def get_device_profile_by_memory(device_name: str, memory_type: str) -> str:
        """
        Get detailed information about a specific device and memory type.

        Args:
            device_name: Device name (e.g., RTL8720E, RTL8730E)
            memory_type: Memory type (nor or nand)

        Returns:
            JSON string with device profile details
        """
        data = get_device_info(device_name, memory_type)
        return json.dumps(data, indent=2)

