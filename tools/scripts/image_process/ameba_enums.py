from enum import Enum
from dataclasses import dataclass

class ImageType(Enum):
    IMAGE1   = 1
    IMAGE2   = 2
    IMAGE3   = 3

    APP_ALL  = 5
    CERT     = 6
    DSP      = 7
    VBMETA   = 8

    # IMAGE1_SUB    = 10 # like: fullmac_ram_1.bin, ram_1.bin
    # IMAGE2_SUB    = 11 # like: fullmac_sram_2.bin

    UNKNOWN  = 255

class ErrorType(Enum):
    SUCCESS         = 0

    INVALID_INPUT   = 1
    FILE_NOT_FOUND  = 2
    INVALID_ARGS    = 3

    EXCEPTION       = 20

    UNKNOWN_ERROR   = 99

class EncryptType(Enum):
    NONE    = 0

    SBOOT   = 1
    RSIP    = 2
    RDP     = 3


@dataclass
class Error:
    error: ErrorType
    message: str

    def __init__(self, error:ErrorType = ErrorType.SUCCESS, message:str = '') -> None:
        self.error = error
        self.message = message

    def __bool__(self):
        return self.error != ErrorType.SUCCESS

    def __int__(self):
        return self.error.value

    def __str__(self):
        return f"{self.error.name}: {self.message}"

    @staticmethod
    def success(message = ''):
        return Error(ErrorType.SUCCESS, message)