import os
import sys
import logging
import re
import struct
from functools import wraps
from typing import Callable, Union
import traceback
import hashlib

from ameba_enums import *

CONSOLES_COLOR_RESET='\033[0m'
CONSOLES_COLOR_RED='\033[1;31m'
CONSOLES_COLOR_GREEN='\033[1;32m'
CONSOLES_COLOR_YELLOW='\033[1;33m'
CONSOLES_COLOR_BLUE='\033[1;34m'
CONSOLES_COLOR_PURPLE='\033[1;35m'
CONSOLES_COLOR_CYAN='\033[1;36m'
CONSOLES_COLOR_WHITE='\033[1;37m'

def create_logger(name, level=logging.DEBUG, stream = sys.stdout):
    logger = logging.getLogger(name)
    if not logger.handlers:
        formatter = logging.Formatter(fmt = '[%(levelname)s][%(asctime)s.%(msecs)03d][%(filename)s|%(funcName)s|%(lineno)s] %(message)s', datefmt='%Y-%m-%d %H:%M:%S')

        logging.addLevelName(logging.DEBUG, '{}DBUG{}'.format(CONSOLES_COLOR_CYAN, CONSOLES_COLOR_RESET))
        logging.addLevelName(logging.INFO, '{}INFO{}'.format(CONSOLES_COLOR_GREEN, CONSOLES_COLOR_RESET))
        logging.addLevelName(logging.WARNING, '{}WARN{}'.format(CONSOLES_COLOR_YELLOW, CONSOLES_COLOR_RESET))
        logging.addLevelName(logging.ERROR, '{}EROR{}'.format(CONSOLES_COLOR_RED, CONSOLES_COLOR_RESET))
        logging.addLevelName(logging.FATAL, '{}FATL{}'.format(CONSOLES_COLOR_RED, CONSOLES_COLOR_RESET))
        consoleHandler = logging.StreamHandler(stream)
        consoleHandler.setFormatter(formatter)
        logger.addHandler(consoleHandler)

        logger.propagate = False # Prevent logging from propagating to the root logger
        logger.setLevel(level)
    return logger

default_logger = create_logger('default')

def exit_on_failure2(exit_on_failure:bool=True, catch_exception:bool=False):
    def decorator(func: Callable[..., Error]) -> Callable[..., Error]:
        @wraps(func)
        def wrapper(*args, **kwargs) -> Error:
            try:
                res = func(*args, **kwargs)
                if res and exit_on_failure:
                    sys.exit(res.error.value)
                return res
            except Exception as e:
                if not catch_exception:
                    raise
                if exit_on_failure:
                    default_logger.error(f"{func.__name__} failed: {str(e)}", exc_info=True)
                    sys.exit(ErrorType.EXCEPTION.value)
                else:
                    return Error(ErrorType.EXCEPTION, str(e))
        return wrapper
    return decorator

def exit_on_failure(exit_on_failure: bool = True, catch_exception: bool = False):
    def decorator(func: Callable[..., Error]) -> Callable[..., Error]:
        @wraps(func)
        def wrapper(*args, **kwargs) -> Error:
            if isinstance(func, staticmethod):
                actual_func = func.__func__
            elif isinstance(func, classmethod):
                actual_func = func.__func__
            else:
                actual_func = func
            # default_logger.debug(f"{actual_func.__name__}")
            try:
                res = func(*args, **kwargs)
                if res and exit_on_failure:
                    sys.exit(res.error.value)
                return res
            except Exception as e:
                # 获取实际的函数对象（处理静态方法和类方法的情况）
                if not catch_exception:
                    # 获取原始的跟踪信息
                    exc_type, exc_value, exc_traceback = sys.exc_info()
                    tb = traceback.extract_tb(exc_traceback)
                    # 找到被装饰函数的帧
                    for frame in tb:
                        if frame.filename == actual_func.__code__.co_filename and frame.name == actual_func.__name__:
                            line = frame.lineno
                            break
                    else:
                        line = tb[-1].lineno  # 如果没找到，就使用最后一帧的行号

                    # 构建新的异常信息
                    new_exc = type(e)(f"{str(e)} (in {actual_func.__name__} at line {line})")
                    raise new_exc.with_traceback(exc_traceback)

                if exit_on_failure:
                    # 获取被装饰函数的行号
                    tb = traceback.extract_tb(sys.exc_info()[2])
                    for frame in tb:
                        if frame.filename == actual_func.__code__.co_filename and frame.name == actual_func.__name__:
                            line = frame.lineno
                            break
                    else:
                        line = tb[-1].lineno

                    error_message = f"{actual_func.__name__} failed at line {line}: {str(e)}"
                    default_logger.error(error_message, exc_info=True)
                    sys.exit(ErrorType.EXCEPTION.value)
                else:
                    return Error(ErrorType.EXCEPTION, str(e))
        return wrapper
    return decorator

def merge_files(dst, *src, header = b''):
    """
    merge files

    :param header: bytes, only prepend_header and ota_prepend_header need, others fill ''
    :param dst: destination file
    :param *src: source files, (src1, src2, ...), cat to dst

    """
    with open(dst, "wb") as dst_fd:
        if header != b'':
            dst_fd.write(header)
        for s in src:
            if os.path.exists(s):
                with open(s, "rb") as f:
                    content = f.read()
                    dst_fd.write(content)

def append_files(dst, *src):
    """
    append files

    :param dst: destination file
    :param *src: source files, (src1, src2, ...), cat to dst

    """
    with open(dst, "ab") as dst_fd:
        for s in src:
            if os.path.exists(s):
                with open(s, "rb") as f:
                    content = f.read()
                    dst_fd.write(content)

def mcu_project_to_mcu_type(mcu_project:str, default:Union[str, None]='') -> Union[str, None]:
    mcu_same_list = ['km0', 'km4', 'ca32', 'kr4']
    if mcu_project in mcu_same_list:
        return mcu_project
    mcu_dicts = {'ap': 'ca32', 'hp': 'km4', 'lp': 'km0'}
    for t in mcu_dicts.values():
        if mcu_project.startswith(t):
            return t
    return mcu_dicts.get(mcu_project, default)

def parse_project_info(path:str) -> dict:
    if not os.path.isabs(path):
        path = os.path.abspath(path)
    path = os.path.normpath(path).replace("\\", "/") #Transform windows style to unix style to make re working
    #NOTE: support cases:
    #      1. /path/to/amebaxxx_gcc_project                     => soc_project: amebaxxx, mcu_project: empty
    #      2. /path/to/amebaxxx_gcc_project/build               => soc_project: amebaxxx, mcu_project: empty
    #      3. /path/to/amebaxxx_gcc_project/project_abc         => soc_project: amebaxxx, mcu_project: abc
    #      4. /path/to/amebaxxx_gcc_project/project_abc/build   => soc_project: amebaxxx, mcu_project: abc
    #      5. case 1~4 but "/path/to" contains utils/release_tool (release build)
    if "utils/release_tool" in path:
        pattern = r'(.*?/utils/release_tool/.*?/(\w+)_gcc_project)(?:/build)?(?:/project_(\w+))?(?:/|$)'
    else:
        pattern = r'(.*?/(\w+)_gcc_project)(?:/build)?(?:/project_(\w+))?(?:/|$)'
    match = re.search(pattern, path)

    if match:
        soc_dir = match.group(1)
        soc_project = match.group(2)
        mcu_project = match.group(3) if match.group(3) else ''
    else:
        soc_dir = ''
        soc_project = ''
        mcu_project = ''

    if path.endswith(".bin"):
        file_body = os.path.splitext(os.path.basename(path))[0]
        mcu_dicts = {'ap': 'ca32', 'hp': 'km4', 'lp': 'km0'}
        if mcu_project:
            #Avoid file project and location mismatch
            if 'all' in file_body and ('image2' in file_body or 'image3' in file_body):
                if mcu_project not in file_body:
                    if mcu_project in mcu_dicts.keys() and mcu_dicts[mcu_project] in file_body:
                        pass
                    else:
                        default_logger.warning(f"File maybe not in right location: file name: {os.path.basename(path)}, mcu project from path: {mcu_project}")
        else: # add for zephyr build
            mcu_same_list=['km0', 'km4', 'ca32', 'kr4']
            for mcu in mcu_same_list:
                if mcu in file_body:
                    mcu_project = mcu.lower()
                    break
            for key, value in mcu_dicts.items():
                if key in file_body:
                    mcu_project = key.lower()
                    break
            if mcu_project == '':
                default_logger.fatal(f"Failed to get mcu project from file name: {os.path.basename(path)}")

    # mcu_type = mcu_project_to_mcu_type(mcu_project, default='')
    # if not mcu_type:
    #     default_logger.error(f'Failed to get mcu type for {soc_project}:{mcu_project} from {path}')

    result = {
        "soc_dir": soc_dir,
        "soc_project": soc_project,
        "mcu_project": mcu_project,
    }

    return result

def parse_image_type(image_path:str) -> ImageType:
    image1_strs = ["image1", "boot", "ram_1"]
    image2_strs = ["image2", "sram_2", "ap_image_all"]
    image3_strs = ["image3", "img3"]
    app_all_strs = ["_app", "_app_ns", "_app_mp"]
    dsp_strs = ["dsp"]

    image_name = os.path.splitext(os.path.basename(image_path))[0]
    if any(x in image_name for x in image1_strs):
        return ImageType.IMAGE1
    elif any(x in image_name for x in image2_strs):
        return ImageType.IMAGE2
    elif any(x in image_name for x in image3_strs):
        return ImageType.IMAGE3
    elif any(image_name.endswith(x) for x in app_all_strs):
        return ImageType.APP_ALL
    elif any(x in image_name for x in dsp_strs):
        return ImageType.DSP
    else:
        default_logger.fatal(f"unknown image type for {image_name}")
        return ImageType.UNKNOWN

def parse_map_file(file_path:str, symbol:str) -> tuple:
    if os.path.exists(file_path):
        with open(file_path, 'r') as file:
            for line in file:
                columns = line.strip().split()
                if len(columns) >= 3 and columns[2] == symbol:
                    return tuple(columns)
    return ('0', '?', symbol)

def htonl_to_ntohl(value):
    # big-endian to little-endian
    packed_value = struct.pack('>I', value)
    return struct.unpack('<I', packed_value)[0]


def list_to_hex_str(lst):
    hex_data = ['%02X' % int(i) for i in lst]
    hex_str = ''
    # print(hex_data)
    for item in hex_data:
        hex_str += str(item)
    return hex_str

def get_layout_address(ld_file, section, info_type, offset = 0x20) -> str:
    with open(ld_file, 'r') as file:
        for line in file:
            line = line.strip()
            if line.startswith(section):
                if info_type == 'ORIGIN':
                    match = re.search(r'ORIGIN\s*=\s*(0x[0-9A-Fa-f]+)', line)
                    if match:
                        return hex(int(match.group(1), 16) - offset)
                elif info_type == 'LENGTH':
                    match = re.search(r'LENGTH\s*=\s*(0x[0-9A-Fa-f]+)\s*-\s*(0x[0-9A-Fa-f]+)', line)
                    if match:
                        end, start = int(match.group(1), 16), int(match.group(2), 16)
                        return hex(end - start)
    return ''

def snake_to_pascal(snake_str:str):
    return ''.join(word.capitalize() for word in snake_str.split('_'))

def pascal_to_snake(pascal_str:str):
    return re.sub(r'(?<!^)(?=[A-Z])', '_', pascal_str).lower()

def modify_file_path(original_path, body = None, prefix='', suffix='', new_directory=''):
    original_directory, filename = os.path.split(original_path)
    name, ext = os.path.splitext(filename)
    new_filename = f"{prefix}{name if body == None else body}{suffix}{ext}"
    output_directory = new_directory if new_directory else original_directory
    new_path = os.path.join(output_directory, new_filename)
    return new_path

def file_has_path(file_name):
    return '/' in file_name or '\\' in file_name

def get_file_md5sum(file_path):
    with open(file_path, "rb") as f:
        file_hash = hashlib.md5()
        file_hash.update(f.read())
    return file_hash.hexdigest()

def get_file_dir(file_path):
    return os.path.dirname(os.path.abspath(file_path))
