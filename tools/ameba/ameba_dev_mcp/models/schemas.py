"""
Pydantic models for MCP service validation.

These models define the API contracts for tools and resources.
"""

from typing import Optional, List, Dict, Any, Literal
from pydantic import BaseModel, ConfigDict, Field, field_validator, model_validator


class ImageSpec(BaseModel):
    """Specification for a firmware image to flash."""

    path: str = Field(..., description="Path to the firmware image file")
    start_addr: str = Field(
        ...,
        description="Start address in hex (e.g., '0x08000000')",
        pattern=r"^0x[0-9a-fA-F]+$"
    )
    end_addr: str = Field(
        ...,
        description="End address in hex (e.g., '0x08010000')",
        pattern=r"^0x[0-9a-fA-F]+$"
    )


class FlashConfig(BaseModel):
    """Configuration for firmware flashing operation."""

    device: str = Field(
        ...,
        description="Device name (e.g., RTL8720E, RTL8730E)"
    )
    port: str = Field(
        ...,
        description="Serial port (e.g., /dev/ttyUSB0, COM3)"
    )
    baudrate: int = Field(
        default=1500000,
        description="Serial baud rate",
        ge=9600,
        le=1500000
    )
    image_dir: str = Field(
        ...,
        description="Directory containing firmware images"
    )
    memory_type: str = Field(
        default="nor",
        description="Memory type: nor, nand, or ram"
    )
    chip_erase: bool = Field(
        default=False,
        description="Whether to erase entire chip before flashing"
    )
    remote_server: Optional[str] = Field(
        default=None,
        description="Remote serial server IP address (optional)"
    )
    remote_port: int = Field(
        default=58916,
        description="Remote serial server TCP port"
    )
    remote_password: Optional[str] = Field(
        default=None,
        description="Password for remote server authentication"
    )
    images: Optional[List[ImageSpec]] = Field(
        default=None,
        description="Custom images to flash (overrides device profile defaults)"
    )
    verify: bool = Field(
        default=True,
        description="Verify images before flashing"
    )


class EraseConfig(BaseModel):
    """Configuration for flash erase operation."""

    device: str = Field(..., description="Device name")
    port: str = Field(..., description="Serial port")
    baudrate: int = Field(default=1500000, description="Serial baud rate")
    start_addr: str = Field(
        ...,
        description="Start address to erase (hex)",
        pattern=r"^0x[0-9a-fA-F]+$"
    )
    end_addr: str = Field(
        ...,
        description="End address to erase (hex)",
        pattern=r"^0x[0-9a-fA-F]+$"
    )
    memory_type: str = Field(default="nor", description="Memory type: nor, nand, or ram")
    remote_server: Optional[str] = Field(default=None, description="Remote server IP")
    remote_port: int = Field(default=58916, description="Remote server port")
    remote_password: Optional[str] = Field(default=None, description="Remote server password")


class SerialConfig(BaseModel):
    """Configuration for serial port connection."""

    port: str = Field(..., description="Serial port (e.g., /dev/ttyUSB0, COM3)")
    baudrate: int = Field(default=115200, description="Serial baud rate", ge=9600, le=1500000)
    timeout: float = Field(default=1.0, description="Read timeout in seconds", ge=0.1, le=60.0)
    remote_server: Optional[str] = Field(default=None, description="Remote server IP")
    remote_port: int = Field(default=58916, description="Remote server port")
    remote_password: Optional[str] = Field(default=None, description="Remote server password")


class FlashResult(BaseModel):
    """Result of a firmware flashing operation."""

    success: bool = Field(..., description="Whether the operation succeeded")
    message: str = Field(..., description="Result message")
    device: str = Field(..., description="Device name")
    port: str = Field(..., description="Serial port used")
    images_flashed: List[str] = Field(default_factory=list, description="List of flashed image names")
    error_details: Optional[str] = Field(default=None, description="Error details if failed")


class SerialConnectionInfo(BaseModel):
    """Information about a serial connection."""

    success: bool = Field(..., description="Whether the connection was established successfully")
    connection_id: str = Field(..., description="Unique connection identifier (empty string on failure)")
    port: str = Field(..., description="Serial port name")
    baudrate: int = Field(..., description="Current baud rate")
    is_remote: bool = Field(..., description="Whether this is a remote connection")
    is_open: bool = Field(..., description="Whether the connection is open")
    remote_server: Optional[str] = Field(default=None, description="Remote server address if remote")


class DeviceProfileInfo(BaseModel):
    """Information about a device profile."""

    name: str = Field(..., description="Device name")
    profile_file: str = Field(..., description="Profile file name")
    device_id: int = Field(..., description="Device ID")
    memory_type: str = Field(..., description="Memory type (NOR/NAND)")
    flash_start_address: int = Field(..., description="Flash start address")
    ram_start_address: int = Field(..., description="RAM start address")
    images: List[Dict[str, Any]] = Field(default_factory=list, description="Default images")


class WriteResult(BaseModel):
    """Result of a serial write operation."""

    success: bool = Field(..., description="Whether the write succeeded")
    bytes_written: int = Field(..., description="Number of bytes written")
    message: str = Field(..., description="Result message")


class ReadResult(BaseModel):
    """Result of a serial read operation."""

    success: bool = Field(..., description="Whether the read succeeded")
    data: str = Field(..., description="Data read from serial (hex-encoded if binary)")
    bytes_read: int = Field(..., description="Number of bytes read")
    is_binary: bool = Field(default=False, description="Whether data is binary")
    message: str = Field(..., description="Result message")


class ResetResult(BaseModel):
    """Result of a device reset operation."""

    success: bool = Field(..., description="Whether the reset succeeded")
    message: str = Field(..., description="Result message")


# ===========================================================================
# project_info.json5 / board_info.json5 schema (Step 6.2)
# ===========================================================================

_HEX_ADDR_RE = r"^0x[0-9a-fA-F]{1,8}$"


class ProjectImageEntry(BaseModel):
    """One image inside `projects.<soc>.images[]`.

    Auto mode: type/path/start_addr/end_addr are all populated by build_firmware.
    Manual mode: only path + start_addr required; end_addr optional (used only
    for non-overlap checks); type optional.
    """
    model_config = ConfigDict(extra="forbid")

    type: Optional[str] = Field(default=None, description="Region type, e.g. IMG_BOOT")
    path: str = Field(..., description="Absolute path to the .bin file")
    start_addr: str = Field(..., description="Start flash address (hex)", pattern=_HEX_ADDR_RE)
    end_addr: Optional[str] = Field(default=None, description="End address inclusive (hex), optional in manual", pattern=_HEX_ADDR_RE)
    optional: bool = Field(default=False, description="If True, missing file is a warning, not an error (e.g. VFS)")


class ProjectEntry(BaseModel):
    model_config = ConfigDict(extra="forbid")

    flash_layout_setting_mode: Literal["auto", "manual"] = Field(...)
    build_dir: str = Field(..., description="Absolute path to build_<SOC>/")
    memory_type: Optional[Literal["nor", "nand", "ram"]] = Field(default=None)
    images: List[ProjectImageEntry] = Field(default_factory=list)


class ProjectInfoDefaults(BaseModel):
    model_config = ConfigDict(extra="forbid")

    memory_type: Literal["nor", "nand", "ram"] = "nor"
    chip_erase: bool = False


class ProjectInfo(BaseModel):
    model_config = ConfigDict(extra="forbid")

    schema_version: int = 1
    defaults: ProjectInfoDefaults = Field(default_factory=ProjectInfoDefaults)
    projects: Dict[str, ProjectEntry] = Field(default_factory=dict)


class RemoteSpec(BaseModel):
    model_config = ConfigDict(extra="forbid")

    host: str = Field(..., description="Remote serial server IP / hostname")
    port: int = Field(default=58916, ge=1, le=65535)
    password: Optional[str] = Field(default=None, description="Remote password (or set AMEBA_REMOTE_PWD env var)")


class SerialLogRecord(BaseModel):
    """Per-board serial-log capture config.

    When `enable` is true, opening the board's serial session via MCP
    starts a background reader that captures the FULL serial stream
    (timestamped, AAG-decoded) to a log file — independent of any
    `drain_first` the agent-facing read tools perform.

    `file_name` is auto-managed when it matches the generated pattern
    `<alias>_<YYYYMMDD>_<HHMMSS>.log`: a new file is created per day and
    the field is written back. A user-supplied custom name (not matching
    the pattern) is left untouched and never rotated.
    """
    model_config = ConfigDict(extra="forbid")

    enable: bool = Field(default=False, description="Capture serial output to a log file")
    log_dir: Optional[str] = Field(
        default=None,
        description="Log directory. Relative paths resolve under PROJECT_ROOT; "
                    "default is PROJECT_ROOT/mcp_serial_log.",
    )
    file_name: Optional[str] = Field(
        default=None,
        description="Log file name. Auto-generated/rotated when it matches "
                    "<alias>_<YYYYMMDD>_<HHMMSS>.log; a custom name is kept as-is.",
    )


class BoardEntry(BaseModel):
    model_config = ConfigDict(extra="forbid")

    soc: str = Field(..., description="SoC name, e.g. RTL8721F")
    transport: Literal["local", "remote"] = "local"
    port: str = Field(..., description="Serial port, e.g. /dev/ttyUSB0 or COM5")
    baudrate: Optional[int] = Field(default=None, description="Override default flash/connect baudrate")
    monitor_baudrate: Optional[int] = Field(default=None)
    chip_erase: Optional[bool] = Field(default=None)
    add_crlf: Optional[bool] = Field(default=None, description="Internal: serial_write trailing CRLF")
    remote: Optional[RemoteSpec] = Field(default=None)
    serial_log_record: Optional[SerialLogRecord] = Field(default=None)

    @model_validator(mode="after")
    def _validate_remote(self):
        if self.transport == "remote" and self.remote is None:
            raise ValueError("transport=remote requires `remote: { host: ... }`")
        if self.transport == "local" and self.remote is not None:
            raise ValueError("transport=local must not set `remote`")
        return self


class BoardInfoDefaults(BaseModel):
    model_config = ConfigDict(extra="forbid")

    baudrate: int = 1500000
    monitor_baudrate: int = 1500000
    chip_erase: bool = False
    add_crlf: bool = True


class BoardInfo(BaseModel):
    model_config = ConfigDict(extra="forbid")

    schema_version: int = 1
    defaults: BoardInfoDefaults = Field(default_factory=BoardInfoDefaults)
    default_alias: Optional[str] = Field(
        default=None,
        description=(
            "Optional fallback alias used when callers omit `alias` and "
            "boards has 2+ entries. When set, must be a key of `boards`. "
            "Tool envelopes report `selected_via=\"default\"` when this fires."
        ),
    )
    boards: Dict[str, BoardEntry] = Field(default_factory=dict)

    @model_validator(mode="after")
    def _check_default_alias(self):
        if self.default_alias is not None and self.default_alias not in self.boards:
            raise ValueError(
                f"default_alias '{self.default_alias}' is not in boards "
                f"({sorted(self.boards.keys())})"
            )
        return self


class ResolvedBoard(BaseModel):
    """A BoardEntry with all defaults filled in. Used by serial / flash tools."""
    model_config = ConfigDict(extra="forbid")

    alias: str
    soc: str
    transport: Literal["local", "remote"]
    port: str
    baudrate: int
    monitor_baudrate: int
    chip_erase: bool
    add_crlf: bool
    remote: Optional[RemoteSpec] = None
    serial_log_record: Optional[SerialLogRecord] = None


class ConfigError(BaseModel):
    """One validation error in a config file."""
    model_config = ConfigDict(extra="forbid")

    code: str
    field_path: str = ""
    message: str
    hint: Optional[str] = None

