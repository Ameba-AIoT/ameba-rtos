from enum import Enum, IntEnum
from dataclasses import dataclass, field
from typing import Optional
import logging
import sys
import os

BASE_DIR = os.path.dirname(os.path.abspath(__file__))

# 配置参数
MQTT_BROKER = "192.168.1.190"
MQTT_PORT = 1883
MQTT_TOPIC_PUBLISH = "request_diag_events"
MQTT_TOPIC_SUBSCRIBE = "response_diag_events"

DIAG_JSON_FOLDER      = os.path.join(BASE_DIR, 'DiagnoseProfiles')
DIAG_LOG_PATH         = os.path.join(BASE_DIR, 'diagnostic_tool.log')

DIAG_READ_BUFFER_SIZE = 1024
DIAG_STORE_HEAP_SIZE = 10000
DEFAULT_TIMEOUT = 5
#  在 get event 期间断线会尝试 MAX_RETRIES 重连
MAX_RETRIES = 3

AUTO_RECONN = True

# 配置日志
# 输出到 stderr
logging.basicConfig(level=logging.INFO,
                    format='%(asctime)s - %(levelname)s - %(message)s',
                    handlers=[logging.FileHandler(f"{DIAG_LOG_PATH}"),logging.StreamHandler(sys.stderr)])
logger = logging.getLogger(__name__)


# 固定宏
# 诊断命令常量
DIAG_CMD_OS_TIME = "AT+DIAG=TM"
DIAG_CMD_BUFFER = "AT+DIAG=BUF_COM,"
DIAG_CMD_READ = "AT+DIAG=GET_EVT,"
DIAG_CMD_VER = "AT+DIAG=VER"
DIAG_CMD_DEL_BF = "AT+DIAG=DEL_EVT_BF,"
DIAG_CMD_DEL_AF = "AT+DIAG=DEL_EVT_AF,"
DIAG_CMD_HEAP = "AT+DIAG=BUF_EVT,"

# 命令ID常量
CMD_OS_TIME  = 0x01
CMD_BUFFER   = 0x02
CMD_READ     = 0x03

CMD_HEAP     = 0x04
CMD_VER      = 0x05
CMD_GET_DEL  = 0x06
CMD_CLR_DEL  = 0x07
CMD_DEL_BF   = 0x08
CMD_DEL_AF   = 0x09

PKG_FIEXD_BYTES = 6 #head(2)+size(2)+cmd(1)+crc(1)
# identifier(2)
PKG_DIAG_ID = bytearray([0x72, 0x74]) # ASCII r,t
PKG_DIAG_ID_BYTES = 2
PKG_DIAG_ID_POS = 0
# size(2)
PKG_SIZE_BYTES = 2
PKG_SIZE_POS = PKG_DIAG_ID_BYTES
# cmd(1) bit 7 err, bit[5:4] seg type, bit [3:0] cmd type
PKG_CMD_BYTES = 1
PKG_FIXED_HEAD_BYTES = PKG_DIAG_ID_BYTES + PKG_SIZE_BYTES + PKG_CMD_BYTES # identifier(2)+size(2)+cmd(1)
PKG_CMD_POS = PKG_DIAG_ID_BYTES + PKG_SIZE_BYTES
PKG_CMD_TYPE_BIT = 0x0f
PKG_CMD_SEG_BIT =  0x30
PKG_CMD_ERR_BIT = 0x80
PKG_EVT_START = 0 << 4 # 00
PKG_EVT_MID = 1 << 4  # 01
PKG_EVT_END = 2 << 4 # 10

PKG_OFFSET_BYTES = 2
PKG_CRC_BYTES = 1

EVT_FIXED_BYTES = 12 # evt_type(2) + evt_len(2)+evt_time(4)+level(1)+rsvd(3)

EVT_TYPE_BYTES = 2
EVT_LEN_POS = EVT_TYPE_BYTES
EVT_LEN_BYTES = 2
EVT_TIME_POS = EVT_TYPE_BYTES + EVT_LEN_BYTES
EVT_TIME_BYTES = 4
EVT_LEV_POS = EVT_TYPE_BYTES + EVT_LEN_BYTES + EVT_TIME_BYTES
EVT_LEV_BYTES = 1

EVT_RSV_POS = EVT_TYPE_BYTES + EVT_LEN_BYTES + EVT_TIME_BYTES + EVT_LEV_BYTES
EVT_RSV_BYTES = 3

SYS_TIME_LEN = 4
VER_HASH_LEN = 8
BUF_SIZE_LEN = 2
DEL_COUNT_LEN = 2
HEAP_SIZE_LEN = 2

BASIC_TYPE_MAP = {
    # 无符号整型
    'u8':           ('B', 1),  # 无符号字节 (0~255)
    'uint8_t':      ('B', 1),  # C标准8位无符号整型
    'unsigned char':('B', 1),  # C风格无符号字符

    'u16':          ('<H', 2), # 小端无符号短整型 (0~65535)
    'uint16_t':     ('<H', 2), # C标准16位无符号整型
    'u32':          ('<I', 4), # 小端无符号长整型 (0~4294967295)
    'uint32_t':     ('<I', 4), # C标准32位无符号整型

    # 有符号整型
    's8':           ('b', 1),  # 有符号字节 (-128~127)
    'int8_t':       ('b', 1),  # C标准8位有符号整型
    's16':          ('<h', 2), # 小端有符号短整型 (-32768~32767)
    'int16_t':      ('<h', 2), # C标准16位有符号整型
    's32':          ('<i', 4), # 小端有符号长整型 (-2147483648~2147483647)
    'int32_t':      ('<i', 4), # C标准32位有符号整型

    # 浮点型
    'float':        ('<f', 4), # 小端单精度浮点 (IEEE 754)
    'f32':          ('<f', 4), # 32位浮点别名
    'double':       ('<d', 8), # 小端双精度浮点 (IEEE 754)
    'f64':          ('<d', 8), # 64位浮点别名

    # 字符型
    'char':         ('c', 1)   # ASCII字符 (单字节)
}


class DiagState(Enum):
    DISCONNECTED = "disconnected"
    CONNECTING = "connecting"
    CONNECTED = "connected"
    PROCESSING = "processing"
    ERROR = "error"

class TimeRange(Enum):
    IN_A_DAY = "IN_A_DAY"
    IN_A_WEEK = "IN_A_WEEK"
    IN_A_MONTH = "IN_A_MONTH"
    CUSTOM_OPTIONS = "CUSTOM_OPTIONS"  # YYYY-MM-DD HH:MM:SS
    ALL = "ALL"

class ReadState(IntEnum):
    READ_START_OR_COMPLETE = 0  # 默认状态：最开始，还没处理事件，或者刚处理的是完整事件/末段
    READ_BIG_FIRST_SEG = 1  # 刚处理的片段是首段
    READ_BIG_MIDDLE_SEG = 2 # 刚处理的是中间段

@dataclass
class ProcessResult:
    success: bool
    error: Optional[str] = None
    warn: Optional[str] = None

@dataclass
class SendCmdResult:
    success: bool
    error: Optional[str] = None

@dataclass
class RtkEvent:
    event_type: int
    event_length: int
    event_time: int
    event_level: int
    reserved: bytes = field(default_factory=lambda: bytes(3))  # 初始化赋值以后不能被改变
    event_info: bytearray = field(default_factory=bytearray)  # 默认空 bytearray


class StructReader:
    """
    基于bytearray的二进制读取器（小端序）
    """
    def __init__(self, data: bytearray):
        self.data = data
        self.pos = 0
        self._bit_buffer = 0
        self._bit_count = 0

    def read_bytes(self, size: int) -> bytes:
        """读取指定字节数"""
        if self.pos + size > len(self.data):
            raise ValueError(f"缓冲区溢出 需求:{size} 剩余:{len(self.data)-self.pos}")

        result = bytes(self.data[self.pos : self.pos+size])
        self.pos += size
        return result

    def read_bits(self, type_str: str, bits: int) -> int:
        """
        读取位域数据（小端序） 即 u32 0x12345678 存储为 0x78 0x56 0x34 0x12

        以 "diag_evt_wifi_rxbd_t": {
          "sw_index": "u8:4",
          "hw_index": "u8:4",
          "bb_cca_cck": "u32:9",
          "bb_cca_ofdm": "u32:9",
          "rsvd": "u32:14"
        }, 为例
        - sw_index 为2, hw_index 为0, 因此diag_evt_wifi_rxbd_t 第一个byte为2
        - bb_cca_cck 为311, bb_cca_ofdm 为273, rsvd 为0, 因此这三个变量占的u32为 b100010001100110111,
          按小端序存储, 因此u32的第一个Byte为0, 第二个为b10即2. 第三个为b0010 0011即35 第四个为 b0011 0111即 55, 因此存储为 55 35 2 0
        - 因此输入的bytearray 为 [2, 55, 35, 2, 0]

        以 "diag_evt_wifi_rxbd_t": {
          "sw_index": "u8:3",
          "hw_index": "u8:4",
          "bb_cca_cck": "u32:9",
          "bb_cca_ofdm": "u32:9",
          "bb_rx_cck": "u16:7",
          "bb_rx_ofdm": "u16:8"
        }, 为例
        - sw_index 为2, hw_index 为0, bb_cca_cck为300, bb_cca_ofdm为360, bb_rx_cck为0, bb_rx_ofdm为32
        - 由于结构体是 __PACKED 定义的, 所以整个结构体占5bytes 分别为 [2, 150, 104, 1, 32]
        """
        type_str = type_str.lower()
        if type_str not in ['u8', 'uint8_t', 'u16', 'uint16_t', 'u32', 'uint32_t']:
            raise NotImplementedError("仅支持u8、u16和u32类型的位域")

        # 确定类型的最大位数
        max_bits = {'u8': 8, 'uint8_t': 8, 'u16': 16, 'uint16_t': 16, 'u32': 32, 'uint32_t': 32}[type_str]

        if bits > max_bits:
            raise ValueError(f"{type_str}类型最多支持{max_bits}位")

        # 确保缓冲区有足够的位
        while self._bit_count < bits:
            if self.pos >= len(self.data):
                raise ValueError("数据不足以读取请求的位数")
            self._bit_buffer |= self.data[self.pos] << self._bit_count
            self._bit_count += 8
            self.pos += 1

        # 从低位到高位提取
        value = self._bit_buffer & ((1 << bits) - 1)
        self._bit_buffer >>= bits
        self._bit_count -= bits

        return value

