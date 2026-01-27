import json
import re
import struct
from typing import Dict, Optional, Any
from datetime import datetime
import os
from diagnose_defines import *
from diagnose_mqtt_handler import MQTTHandler

class DiagController:
    def __init__(self, mqtt_handler: MQTTHandler):
        self.mqtt_handler = mqtt_handler
        # --> 注册消息处理到MQTT管理器
        self.mqtt_handler.register_message_callback(MQTT_TOPIC_SUBSCRIBE, self._process_raw_message)

        # 数据管理
        self.device_time = 0 #设备 timestamp
        self.real_time = 0 #真实时间转换的 timestamp ms，和 device_time 对应
        self.start_time = 0
        self.end_time = 0
        self.is_sync_time = False

        self._current_offset = 0
        self._current_timestamp = 0

        self._current_large_event = None
        self._received_large_event_info_length = 0
        self._current_read_sate = ReadState.READ_START_OR_COMPLETE

        self._rtk_events = []
        self._device_hash = ""
        self._format_json_path = None


        # 事件
        self.del_events_count = 0


    def _transfor_cmdstr_to_num(self, cmd: str) -> int:

        if cmd.startswith(DIAG_CMD_READ):
             return CMD_READ
        elif cmd.startswith(DIAG_CMD_OS_TIME):
            return CMD_OS_TIME
        elif cmd.startswith(DIAG_CMD_BUFFER):
            return CMD_BUFFER
        elif cmd.startswith(DIAG_CMD_VER):
            return CMD_VER
        elif cmd.startswith(DIAG_CMD_DEL_BF):
            return CMD_DEL_BF
        elif cmd.startswith(DIAG_CMD_DEL_AF):
            return CMD_DEL_AF
        elif cmd.startswith(DIAG_CMD_HEAP):
            return CMD_HEAP


    def _pre_process_message(self, payload: str) -> tuple[bytearray, str]:
        # 使用正则表达式提取方括号内的内容

        binary_data = bytearray(payload)
        return binary_data, ""

    def _validate_crc(self, data_with_crc: bytearray) -> bool:
        """
        验证带有 CRC 的数据

        :param data_with_crc: 包含 CRC 的字节数组
        :return: 如果 CRC 验证通过返回 True ，否则返回 False
        """
        # 在调用 _validate_crc 前已经对长度进行check 因此此处 check 长度不够完善
        if data_with_crc is None or len(data_with_crc) < 1:
            return False

        # 获取接收数据的 CRC 值（最后一个字节）
        received_crc = data_with_crc[-PKG_CRC_BYTES]

        # 计算数据的 CRC
        calculated_crc = data_with_crc[PKG_CMD_POS] #PKG_CMD_BYTES = 1
        for byte in data_with_crc[PKG_FIXED_HEAD_BYTES:(-PKG_CRC_BYTES)]:
            calculated_crc ^= byte

        return received_crc == calculated_crc

    def _process_raw_message(self, cmd:str, payload: str) -> ProcessResult:

        process_result = self._process_message(cmd, payload)

        if process_result.success:
            self._command_retries = 0
            self._last_process_success = True
        else:
            logger.error(process_result.error)
        if process_result.warn:
            logger.warning(process_result.warn)

        self.mqtt_handler.update_diag_command_state(process_result.success)

    def _process_message(self, cmd:str, payload: str) -> ProcessResult:

        """处理接收到的消息"""

        process_result = ProcessResult(success=False)

        try:
            pkg_bytes_array, err_log = self._pre_process_message(payload)
            logger.info(f"收到消息: {list(pkg_bytes_array)}")

            # 检查包的长度
            if pkg_bytes_array == bytearray():
                process_result.error = f"payload 为空 {err_log}"
                return process_result

            if len(pkg_bytes_array) < PKG_FIEXD_BYTES:
                process_result.error = f"无效响应, 包长小于固定开销 {PKG_FIEXD_BYTES} bytes"
                return process_result

            # 检查响应标识
            pkg_id = pkg_bytes_array[PKG_DIAG_ID_POS:(PKG_DIAG_ID_POS + PKG_DIAG_ID_BYTES)]
            if pkg_id != PKG_DIAG_ID:
                process_result.error = "响应标识符错误"
                return process_result

            # 检查 size 字段
            pkg_len = int.from_bytes(pkg_bytes_array[PKG_SIZE_POS : (PKG_SIZE_POS + PKG_SIZE_BYTES)], byteorder='little', signed=False)

            if pkg_len != len(pkg_bytes_array[PKG_CMD_POS:(-PKG_CRC_BYTES)]): # pkg size = cmd + payload 不包含crc
                process_result.error = "响应 size 字段和实际长度不匹配"
                return process_result

            # 检查 cmd 字段
            pkg_cmd = pkg_bytes_array[PKG_CMD_POS] #PKG_CMD_BYTES = 1
            cmd_type = pkg_cmd & PKG_CMD_TYPE_BIT
            if self._transfor_cmdstr_to_num(cmd) != cmd_type:
                process_result.error = "响应命令不匹配"
                return process_result
            if pkg_cmd & PKG_CMD_ERR_BIT:
                # payload = 2 byte offset + 1 byte 错误码
                if cmd_type == CMD_READ:
                    error_code = pkg_bytes_array[PKG_FIXED_HEAD_BYTES + PKG_OFFSET_BYTES]
                    process_result.error = f"事件读取失败, 错误码 {error_code}"
                elif cmd_type in (CMD_OS_TIME, CMD_VER):
                    process_result.error = f"响应无效: cmd {cmd_type} 不应该有错误标志"
                # payload = 1 byte 错误码
                else:
                    error_code = pkg_bytes_array[PKG_FIXED_HEAD_BYTES]
                    process_result.error = f"cmd {cmd_type} 错误码 {error_code}"
                return process_result

            # crc 校验
            if not self._validate_crc(pkg_bytes_array):
                process_result.error = "crc 校验错误"
                return process_result

            # 取 payload
            actual_pkg_bytes = pkg_bytes_array[PKG_FIXED_HEAD_BYTES:(-PKG_CRC_BYTES)]
            # 根据当前命令类型处理消息
            if cmd_type == CMD_READ:
                process_result = self._handle_get_event_response(actual_pkg_bytes, pkg_cmd & PKG_CMD_SEG_BIT)
            elif cmd_type == CMD_OS_TIME:
                process_result = self._handle_get_time_response(actual_pkg_bytes)
            elif cmd_type == CMD_BUFFER:
                process_result = self._handle_set_buffer_response(actual_pkg_bytes)
            elif cmd_type == CMD_VER:
                process_result = self._handle_get_hash_response(actual_pkg_bytes)
            elif cmd_type in (CMD_DEL_BF, CMD_DEL_AF):
                process_result = self._handle_del_event_response(actual_pkg_bytes)
            elif cmd_type == CMD_HEAP:
                process_result = self._handle_set_heap_response(actual_pkg_bytes)
            else:
                process_result.error = "未知 cmd"

        except Exception as e:
            process_result.success = False
            process_result.error = str(e)

        return process_result

    def _handle_del_event_response(self, response: bytearray) -> ProcessResult:
        process_result = ProcessResult(success=True)
        try:
            if len(response) != DEL_COUNT_LEN:
                process_result.success = False
                process_result.error = f"无效响应: 删除事件个数字段长度为 {len(response)}, 应为 {DEL_COUNT_LEN}"
                return process_result
            self.del_events_count = int.from_bytes(response, byteorder='little', signed=False)

        except Exception as e:
            process_result.success = False
            process_result.error = str(e)

        return process_result

    def _handle_get_hash_response(self, response: bytearray) -> ProcessResult:
        process_result = ProcessResult(success=True)
        try:
            if len(response) != VER_HASH_LEN:
                process_result.success = False
                process_result.error = f"无效响应: hash 版本字段长度为 {len(self._device_hash)}, 应为 {VER_HASH_LEN}"
                return process_result
            self._device_hash = response.decode('ascii')

        except Exception as e:
            process_result.success = False
            process_result.error = str(e)

        return process_result

    def _handle_set_heap_response(self, response: bytearray) -> ProcessResult:
        process_result = ProcessResult(success=True)
        try:
            if len(response) != HEAP_SIZE_LEN:
                process_result.success = False
                process_result.error = f"无效响应: heap size 字段长度为 {len(response)}, 应为 {HEAP_SIZE_LEN}"
                return process_result

            device_heap_size = int.from_bytes(response, byteorder='little', signed=False)

            if device_heap_size != DIAG_STORE_HEAP_SIZE:
                process_result.warn = f"heap size 设置为 {device_heap_size} 与用户配置不同"

            if device_heap_size == 0:
                process_result.success = False
                process_result.error = "响应显示 heap size 配置为 0"
                return process_result

        except Exception as e:
            process_result.success = False
            process_result.error = str(e)

        return process_result

    def _handle_set_buffer_response(self, response: bytearray) -> ProcessResult:
        process_result = ProcessResult(success=True)
        try:
            if len(response) != BUF_SIZE_LEN:
                process_result.success = False
                process_result.error = f"无效响应: buffer size 字段长度为 {len(response)}, 应为 {BUF_SIZE_LEN}"
                return process_result

            device_buffer_size = int.from_bytes(response, byteorder='little', signed=False)

            if device_buffer_size != DIAG_READ_BUFFER_SIZE:
                process_result.warn = f"buffer size 设置为 {device_buffer_size} 与用户配置不同"

            if device_buffer_size == 0:
                process_result.success = False
                process_result.error = "响应显示 buffer size 配置为 0"
                return process_result

        except Exception as e:
            process_result.success = False
            process_result.error = str(e)

        return process_result

    def _handle_get_time_response(self, response: bytearray) -> ProcessResult:
        """处理时间响应"""
        process_result = ProcessResult(success=True)
        try:
            if len(response) != SYS_TIME_LEN:
                process_result.success = False
                process_result.error = f"无效响应: 时间戳字段长度为 {len(response)}, 应为 {SYS_TIME_LEN}"
                return process_result

            self.real_time = int(datetime.now().timestamp() * 1000)  # 会有一定的时间差 包传播的时间 解析的时间
            self.device_time = int.from_bytes(response, byteorder='little', signed=False)
            self.is_sync_time = True

        except Exception as e:
            process_result.success = False
            process_result.error = str(e)

        return process_result

    def _handle_get_event_response(self, response: bytearray, seg_type: int) -> ProcessResult:

        process_result = ProcessResult(success=True)

        try:
            result_correct = True
            result_info = ""

            # 空包表示请求的 time 后再无事件
            if self._check_is_empty_package(response):
                if self._current_read_sate in (ReadState.READ_BIG_FIRST_SEG, ReadState.READ_BIG_MIDDLE_SEG):
                    self._clear_large_event_info()
                    process_result.warn = "大事件片段丢失: 没有末段"
                self._finalize_get_diag_events()
                return process_result

            if seg_type == PKG_EVT_START:
                if self._current_read_sate in (ReadState.READ_BIG_FIRST_SEG, ReadState.READ_BIG_MIDDLE_SEG):
                    process_result.warn = "大事件片段丢失: 没有末段"
                    self._clear_large_event_info()

                check_result = self._check_is_all_complete_events(response)
                if check_result == 1:
                    result_correct, result_info = self._process_complete_events(response)
                    self._current_read_sate = ReadState.READ_START_OR_COMPLETE
                elif check_result == 0:
                    result_correct, result_info = self._process_large_event_start(response)
                    self._current_read_sate = ReadState.READ_BIG_FIRST_SEG
                else:
                    process_result.success = False
                    process_result.error = "同时包含了片段和完整事件"
                    return process_result

            elif seg_type in (PKG_EVT_MID, PKG_EVT_END):
                if self._current_read_sate == ReadState.READ_START_OR_COMPLETE:
                    process_result.success = False
                    process_result.error = "收到大事件片段但之前并未收到首段"
                    return process_result
                result_correct, result_info = self._process_large_event_segment(response, seg_type == PKG_EVT_MID)
                self._current_read_sate = ReadState.READ_BIG_MIDDLE_SEG if seg_type == PKG_EVT_MID else ReadState.READ_START_OR_COMPLETE

            # 收到的 event 的时间已经超过了用户设置的时间
            if self._current_timestamp >= self.end_time:
                self._finalize_get_diag_events()

            process_result.success = result_correct
            if result_correct and result_info:
                if process_result.warn:
                    process_result.warn = f"{process_result.warn} {result_info}"
                else:
                    process_result.warn = result_info
            if not result_correct:
                process_result.error = result_info

        except Exception as e:
            process_result.success = False
            process_result.error = str(e)

        return process_result

    def _check_is_all_complete_events(self, response: bytearray) -> int:
        """
        return: 0  大事件片段
                1  完整事件
                -1 片段 + 完整事件 (错误的情况, 不应该存在)
        """
        try:
            total_data_len =  len(response) - PKG_OFFSET_BYTES
            if total_data_len < EVT_FIXED_BYTES:
                return 0
            event_data = response[PKG_OFFSET_BYTES:] #可能会有多个事件

            position = 0
            has_complete_events = False
            has_partial_events = False

            # 事件包解析循环
            while position < total_data_len:
                header = self._parse_event_header(event_data, position)
                if header is None:
                    has_partial_events = True
                    break
                full_packet_size = header['event_length'] + EVT_FIXED_BYTES
                # 包体完整性检查
                if position + full_packet_size <= len(event_data):
                    has_complete_events = True
                    position += full_packet_size  # 移动读取指针
                else:
                    has_partial_events = True
                    break

            if has_complete_events and has_partial_events:
                logger.error("同时包含完整事件和事件片段")
                return -1
            elif has_partial_events:
                return 0
            elif has_complete_events:
                return 1

        except Exception as e:
            logger.error(f"数据解析异常: {str(e)}")
            return False

    def _check_is_empty_package(self, response: bytearray) -> bool:

        return response == bytearray()

    def _parse_event_header(self, buf: bytearray, pos: int) -> Optional[Dict[str, Any]]:
        try:
            # 基本边界检查：至少要有固定头
            if pos + EVT_FIXED_BYTES > len(buf):
                return None
            event_type = int.from_bytes(buf[pos:pos + EVT_TYPE_BYTES], byteorder='little', signed=False)
            event_length = int.from_bytes(buf[(pos + EVT_LEN_POS):(pos + EVT_LEN_POS + EVT_LEN_BYTES)], byteorder='little', signed=False)
            event_time = int.from_bytes(buf[(pos + EVT_TIME_POS):(pos + EVT_TIME_POS + EVT_TIME_BYTES)], byteorder='little', signed=False)
            event_level = buf[pos + EVT_LEV_POS]
            reserved = buf[(pos + EVT_RSV_POS):(pos + EVT_RSV_POS + EVT_RSV_BYTES)]

            return {
                'event_type': event_type,
                'event_length': event_length,
                'event_time': event_time,
                'event_level': event_level,
                'reserved': reserved,
            }
        except Exception:
            return None

    def _process_complete_events(self, response: bytearray) -> tuple[bool, str]:

        if len(response) <= EVT_FIXED_BYTES + PKG_OFFSET_BYTES:
            return False, "无效响应: 包长小于等于固定部分"

        pkg_offset = int.from_bytes(response[:PKG_OFFSET_BYTES], byteorder='little', signed=False)
        event_data = response[PKG_OFFSET_BYTES:]

        result_info = ""
        position = 0

        current_timestamp = self._current_timestamp
        current_offset =  pkg_offset  # 收到的 resp 中的 offset

        while position < len(event_data):
            header = self._parse_event_header(event_data, position)
            if header is None:
                break

            # 第一笔事件
            if position == 0:
                # 如果 time 不变, 检查 offset 是否和发送的 req 一致
                if header['event_time'] == current_timestamp and self._current_offset != pkg_offset:
                    result_info = "事件丢失: 收到的响应中的 offset 和 发出的 cmd 中的 offset 不一致"

            if header['event_length'] > 0:
                event_info = event_data[(position + EVT_FIXED_BYTES): (position + EVT_FIXED_BYTES + header['event_length'])]
            else:
                event_info = bytearray()

            if  header['event_time'] < self.end_time:
                # 添加事件
                self._rtk_events.append(RtkEvent(
                    event_type=header['event_type'],
                    event_length=header['event_length'],
                    event_time=header['event_time'],
                    event_level=header['event_level'],
                    reserved=header['reserved'],
                    event_info=event_info
                ))
                full_packet_size = header['event_length'] + EVT_FIXED_BYTES
            # 更新 offset 和 timestamp
            if header['event_time'] != current_timestamp:
                current_timestamp = header['event_time']
                current_offset = full_packet_size # 重置为新 time 的 event 长度
            else:
                current_offset += full_packet_size

            # 移动到下一个事件
            position += full_packet_size

        if position != len(event_data):
            return False, "无效响应: 包含了除 event 外的未知字段"

        self._current_offset = current_offset
        self._current_timestamp = current_timestamp

        return True, result_info


    def _process_large_event_start(self, response: bytearray) -> tuple[bool, str]:

        if len(response) < EVT_FIXED_BYTES + PKG_OFFSET_BYTES:
            return False, "无效响应: 包长小于固定部分"

        event_data = response[PKG_OFFSET_BYTES:]
        pkg_offset = int.from_bytes(response[:PKG_OFFSET_BYTES], byteorder='little', signed=False)
        result_info = ""

        header = self._parse_event_header(event_data, 0)
        if header is None:
            return False, "无效响应: 包长小于固定部分或不足以构成完整事件"

        # 超出了期望时间的事件不记录
        if  header['event_time'] < self.end_time:
            self._current_large_event = RtkEvent(
                event_type=header['event_type'],
                event_length=header['event_length'],
                event_time=header['event_time'],
                event_level=header['event_level'],
                reserved=header['reserved'],
                event_info=bytearray()
            )
            if len(event_data) - EVT_FIXED_BYTES != 0: # 开头的判断已经避免了 < 0 的 case
                self._current_large_event.event_info.extend(event_data[EVT_FIXED_BYTES:])
                self._received_large_event_info_length = len(self._current_large_event.event_info)

            # 更新 offset 和 timestamp
            if header['event_time'] != self._current_timestamp:
                self._current_timestamp = header['event_time']
                self._current_offset = len(event_data)  # 重置为当前大事件的片段长而不是 event info 长度
            else:
                if self._current_offset != pkg_offset:  #req 的 offset 和 response 的 offset 不一致
                    result_info = "事件丢失: 收到的响应中的 offset 和 发出的 cmd 中的 offset 不一致"
                self._current_offset = pkg_offset + len(event_data)

        return True, result_info

    def _process_large_event_segment(self, response: bytearray, is_middle_seg: bool) -> tuple[bool, str]:

        # 检查响应长度和当前大型事件是否存在
        if len(response) <= PKG_OFFSET_BYTES:
            return False, "无效响应: 包长小于等于 offset 长度"
        if  self._current_large_event is None:
            return False, "无效响应: 此前没有收到大事件首段"

        pkg_offset = int.from_bytes(response[:PKG_OFFSET_BYTES], byteorder='little', signed=False)
        if self._current_offset != pkg_offset:  #req 的 offset 和 response 的 offset 不一致
            return False, "大事件片段的 offset 和 发出的 cmd 的 offset 不一致"

        event_data = response[PKG_OFFSET_BYTES:]

        if is_middle_seg:
            if self._received_large_event_info_length + len(event_data) > self._current_large_event.event_length:
                return False, " 大事件片段长度超出了期望的长度"
        else:
            if self._received_large_event_info_length + len(event_data) != self._current_large_event.event_length:
                return False, " 收到的大事件片段长度不符合期望的长度"

        self._current_large_event.event_info.extend(event_data)
        # 更新偏移量
        self._current_offset += len(event_data)
        self._received_large_event_info_length += len(event_data)

        if not is_middle_seg:
            # 添加完整的大事件
            self._rtk_events.append(self._current_large_event)
            self._clear_large_event_info()

        return True, ""

    def _clear_large_event_info(self):

        self._current_large_event = None
        self._received_large_event_info_length = 0


    def _get_format_info_from_json(self) -> Dict:
        """
        从JSON文件中获取指定hash的格式信息

        return: 包含格式信息的字典
        """
        try:
            format_info = {}
            with open(self._format_json_path, 'r', encoding='utf-8') as f:
                json_data = json.load(f)

            hash_object = json_data.get("hash", {})

            # 1. 直接匹配目标hash
            if self._device_hash in hash_object:
                format_info = hash_object[self._device_hash].get("format")
                if format_info:
                    return format_info

            # 2. 在compatible列表中查找
            for _, hash_value in hash_object.items():
                compatible_list = hash_value.get("compatible", [])
                if self._device_hash in compatible_list:
                    format_info = hash_value.get("format", {})
                    if format_info:
                        return format_info

            # 3. 使用latest_hash
            latest_hash = json_data.get("latest_hash")
            if latest_hash and latest_hash in hash_object:
                return hash_object[latest_hash].get("format")

            return {}

        except Exception as e:
            logger.error(f"Error processing JSON file: {str(e)}")
            return {}

    def _parse_event_types(self, json_config: dict) -> Dict[str, int]:
        """
        解析事件类型映射关系 (仅处理全大写键名的 section)
        :param json_config: JSON配置字典
        :return: 全大写的事件类型名称到值的映射字典
        """
        # 初始化基础类型映射（保持原始键名）
        base_types = json_config.get("rtk_event_type", {})
        event_type_map = dict(base_types)

        # 处理所有以 diag_evt_ 开头的配置项
        for section_key, section_value in json_config.items():
            # 检查键是否以 diag_evt_ 开头，且值是否为字典类型。
            if section_key.startswith("diag_evt_") and isinstance(section_value, dict):
                # 检查整个 section 是否所有事件名称都是全大写，只有枚举定义才是全大写，用全大写来区分结构体和枚举
                if not all(evt_name.isupper() for evt_name in section_value.keys()):
                    continue

                # 处理当前 section 中的所有事件
                for evt_name, expr in section_value.items():
                    if isinstance(expr, str):
                        try:
                            # 表达式预处理（保留原始表达式）
                            substituted_expr = expr

                            # 使用正则表达式替换所有基础类型变量
                            for base_name, base_value in event_type_map.items():
                                substituted_expr = re.sub(
                                    r'\b' + re.escape(base_name) + r'\b',
                                    str(base_value),
                                    substituted_expr
                                )

                            calculated_value = eval(substituted_expr)
                            # 保存事件名称和计算结果
                            event_type_map[evt_name] = int(calculated_value)
                        except Exception as e:
                            logger.error(f"无法解析表达式 '{expr}' (处理后: {substituted_expr}): {str(e)}")
                    else:
                        logger.error(f"忽略非字符串表达式 {evt_name} -> {expr}")

        return event_type_map

    def _get_event_type_name(self, event_value: int, event_type_map: Dict[str, int]) -> str:
        """
        根据事件值查找对应的事件类型名称
        :param event_value: 事件数值
        :param event_type_map: 事件类型映射字典
        :return: 匹配的事件类型名称或默认值
        """
        # 精确匹配优先
        # 创建反向映射
        reverse_map = {v: k for k, v in event_type_map.items()}

        if event_value in reverse_map:
            return reverse_map[event_value] # 保持原始命名风格

        # 模糊匹配（处理可能的数值类型差异）
        for value, name in reverse_map.items():
            if int(value) == event_value:
                return name

        return f"UNKNOWN_EVENT_TYPE_{event_value}"

    def _get_event_level_name(self, event_level_value: int, json_config: dict) -> str:
        event_level_map = json_config["rtk_event_level"]
        for name, value in event_level_map.items():
            if value == event_level_value:
                evt_level_name = name
                if evt_level_name.startswith("RTK_EVENT_LEVEL_"):
                    evt_level_name = evt_level_name[len("RTK_EVENT_LEVEL_"):]
                return evt_level_name

        return f"UNKNOWN_LEVEL_{event_level_value}"

    def _parse_event_info(self, event_type: int, event_info: bytearray, str_evt_type: str, json_config: dict) -> Optional[dict]:
        """
        主解析函数
        """
        if not event_info:
            return None

        try:
            struct_def = self._get_event_struct_definition(event_type, str_evt_type, json_config)
            if not struct_def:
                logger.error(f"查找 {str_evt_type} 定义失败")
                return None

            reader = StructReader(event_info)
            info_parse_result = self._parse_event_struct(reader, struct_def, json_config)
            return info_parse_result

        except Exception as e:
            return None


    def _get_event_struct_definition(self, event_type: int, str_evt_type: str, json_config: dict) -> Optional[dict]:
        """
        获取事件结构定义
        """
        if not (0 <= event_type <= 60001):
            logger.error(f"event type 值 {event_type} 超出范围 (0-60001)")
            return None

        # 特殊事件类型处理
        if event_type == 1:
            return json_config.get("diag_evt_demo_1")
        if event_type == 2:
            return json_config.get("diag_evt_demo_2")

        # 通用类型查找 结构体名为全小写
        return json_config.get(str_evt_type.lower())

    def _parse_event_struct(self, reader: StructReader, struct_def: dict, json_config: dict) -> Dict[str, Any]:
        """
        结构体解析（支持递归嵌套）
        """
        result = {}
        for field_name, field_type in struct_def.items():
            try:
                # 递归解析字段
                result[field_name] = self._parse_struct_field(reader, field_type, json_config)
            except Exception as e:
                result[field_name] = f"ERROR: {str(e)}"
        return result

    def _convert_to_enum_string(self, value: int, enum_name: str, json_config: dict) -> str:
        enum_def = json_config.get(enum_name)
        if enum_def:
            for enum_desc, enum_value in enum_def.items():
                if int(enum_value) == value:
                    return f"{enum_desc} ({value})"
        return str(value)

    def _parse_struct_field(self, reader: StructReader, field_def: Any, json_config: dict) -> Any:
        """
        字段解析（支持结构体引用和嵌套数组）
        """

        # 处理结构体直接定义
        # 1.处理 "reason": "u16" 这种类型
        if isinstance(field_def, str):
            # 处理数组
            if '[' in field_def and ']' in field_def:
                type_name, size = field_def.split('[')
                size = int(size.rstrip(']'))
                type_name = type_name.strip()

                # 处理基础类型数组（如 "char[32]" 或 "u8[32]"）
                if type_name in BASIC_TYPE_MAP:
                    if type_name == 'char':
                        """
                        读取字符数组并转换为字符串，去除尾部的空字符
                        """
                        data = reader.read_bytes(size)
                        # 移除空字节并解码为字符串
                        return data.split(b'\x00')[0].decode('latin-1')
                    return [self._read_basic_type(reader, type_name) for _ in range(size)]

                # 处理结构体数组（如 "diag_evt_wifi_rxbd_t[5]"）
                struct_def = json_config.get(type_name.strip())
                if struct_def:
                    return [self._parse_event_struct(reader, struct_def, json_config) for _ in range(size)]

            # 处理基础类型
            if field_def.split(':')[0] in BASIC_TYPE_MAP:
                return self._read_basic_type(reader, field_def)

            # 处理结构体（如 "struct diag_evt_demo_1" 或 "diag_evt_demo_1"）
            struct_name = field_def.replace("struct ", "").strip()
            struct_def = json_config.get(struct_name)
            if struct_def:
                return self._parse_event_struct(reader, struct_def, json_config)


        if isinstance(field_def, dict):

            '''
             2.处理 "reason": {
             "type": "u16",
             "enum_ref": "rtw_disconn_reason",
             "comment": "enum rtw_disconn_reason in wifi_api_types.h"
             },
            '''
            if "type" in field_def:
                if "enum_ref" in field_def:
                    value = self._read_basic_type(reader, field_def["type"])
                    return self._convert_to_enum_string(value, field_def["enum_ref"], json_config)
                else:
                    return self._parse_struct_field(reader, field_def["type"], json_config)

            # 3.处理嵌套结构体定义
            else:
                return self._parse_event_struct(reader, field_def, json_config)

        raise NotImplementedError(f"不支持的字段类型: {type(field_def)}")


    def _read_basic_type(self, reader: StructReader, type_str: str) -> Any:
        """
        读取基本数据类型
        """
        # 处理位域（如u8:4）
        if ':' in type_str:
            base_type, bits = type_str.split(':')
            return reader.read_bits(base_type.strip(), int(bits))

        fmt, size = BASIC_TYPE_MAP.get(type_str.lower(), (None, None))
        if not fmt:
            raise NotImplementedError(f"不支持的类型: {type_str}")

        # 读取数据
        data = reader.read_bytes(size)

        # 特殊处理 char 类型
        if fmt == 'c':
            return data.decode('latin-1')

        # 解析其他类型的数据
        value = struct.unpack(fmt, data)[0]

        # 特殊处理浮点数
        if fmt[-1] in ('f', 'd'):  # fmt 可能为 <f <d 因此取最后一位
            return round(value, 6)  # 截断到小数点后六位

        return value


    def _convert_rtk_events_to_diag_events(self):
        json_config = self._get_format_info_from_json()
        event_type_map = self._parse_event_types(json_config)
        current_time = datetime.now().strftime("%Y%m%d_%H%M%S")
        # 输出文件
        output_file = os.path.join(DIAG_JSON_FOLDER, f"DiagnoseInfo_{current_time}.json")

        try:
            # 读取已存在的事件（如果文件存在）
            existing_events = []
            if os.path.exists(output_file):
                try:
                    with open(output_file, 'r', encoding='utf-8') as f:
                        content = f.read().strip()
                        if content and content != '[]':
                            existing_events = json.loads(content)
                except (json.JSONDecodeError, Exception) as e:
                    logger.warning(f"读取现有事件文件失败，将创建新文件: {e}")
                    existing_events = []

            # 转换新事件
            new_events = []
            for rtk_evt in self._rtk_events:
                event_type_name = self._get_event_type_name(rtk_evt.event_type, event_type_map)
                dt = datetime.fromtimestamp(((self.real_time) - (self.device_time - rtk_evt.event_time))/1000)
                formatted_str = dt.strftime('%Y-%m-%d %H:%M:%S.%f')[:-3]  # 保留毫秒部分 %f 默认保留6位，这里截取前3位

                diag_event = {
                    'device_time_stamp': rtk_evt.event_time,
                    'time': formatted_str,
                    'type': event_type_name.replace("DIAG_EVT_", ""),
                    'level': self._get_event_level_name(rtk_evt.event_level, json_config),
                    'info': self._parse_event_info(rtk_evt.event_type, rtk_evt.event_info, event_type_name, json_config)
                }

                # 记录日志
                logger.info(diag_event)
                new_events.append(diag_event)

            # 合并事件并去重
            all_events = existing_events + new_events

            seen = {}
            unique_events = []
            for event in all_events:
                # 创建事件的唯一标识, 不能使用 time 因为 time 每次会有秒级别的变化  会有一定的时间差 包传播的时间 解析的时间
                event_key = (event['device_time_stamp'], event['type'], event['level'])

                if event_key not in seen:
                    seen[event_key] = []
                    seen[event_key].append(event['info'])
                    unique_events.append(event)
                else:
                    if event['info'] not in seen[event_key]:
                        seen[event_key].append(event['info'])
                        unique_events.append(event)

             #按时间排序
            unique_events.sort(key=lambda x: x['device_time_stamp'])

            # 写入文件
            with open(output_file, 'w', encoding='utf-8') as f:
                f.write('[\n')  # 开始 JSON 数组
                for i, event in enumerate(unique_events):
                    # 将每个事件转换为一行 JSON 字符串
                    event_str = json.dumps(event, ensure_ascii=False)
                    if i < len(unique_events) - 1:
                        f.write(f'  {event_str},\n')  # 除了最后一个事件，后面都加逗号
                    else:
                        f.write(f'  {event_str}\n')   # 最后一个事件不加逗号
                f.write(']')  # 结束 JSON 数组

        except Exception as e:
            logger.error(f"处理诊断事件文件失败: {e}")

    def _finalize_get_diag_events(self):

        self.start_time = 0
        self.end_time = 0

        self._current_offset = 0
        self._current_timestamp = 0
        self._current_read_sate = ReadState.READ_START_OR_COMPLETE

        self._convert_rtk_events_to_diag_events()
        self._rtk_events = []


    def check_hash_and_get_format_json(self, json_folder_path: str) -> bool:
        try:
            send_cmd_result = self.mqtt_handler.send_diag_command("AT+DIAG=VER")
            if not send_cmd_result.success:
                logger.error(f"获取 device hash 失败: {send_cmd_result.error}")
                return False

            match_hash = False

            # 获取文件夹中所有的 JSON 文件
            json_files = [f for f in os.listdir(json_folder_path) if f.endswith('.json')]

            for file in json_files:
                file_path = os.path.join(json_folder_path, file)

                with open(file_path, 'r') as f:
                    json_data = json.load(f)

                if 'hash' in json_data and isinstance(json_data['hash'], dict):
                    hash_object = json_data['hash']
                    for key, value in hash_object.items():
                        if key == self._device_hash:
                            match_hash = True
                            self._format_json_path = file_path
                            break

                        if isinstance(value, dict) and 'compatible' in value:
                            compatible_array = value['compatible']
                            if self._device_hash in compatible_array:
                                match_hash = True
                                self._format_json_path = file_path
                                break

            # 如果没有找到匹配的 Hash，使用默认文件
            if not match_hash:
                logger.error("Current diagnose profile is not compatible. Please update the diagnose profile and try again.")
                return False

            return True

        except Exception as e:
            logger.error(f"处理 JSON 文件时出错: {str(e)}")
            return False

    def _date_string_to_timestamp(self, datetime_str:str) -> int:
        """
        将日期字符串转换为 ms 时间戳
        :param datetime_str: 日期字符串，格式为 "YYYY-MM-DD HH:MM:SS"
        :return 对应的时间戳 ms
        """

        # 解析为 datetime 对象
        dt_object = datetime.strptime(datetime_str, "%Y-%m-%d %H:%M:%S")

        # 转换为时间戳
        timestamp = int(dt_object.timestamp() * 1000) # ms

        return timestamp

    def set_diag_event_time_period(self,
                                   time_range: TimeRange,
                                   custom_start_time: Optional[str] = None,
                                   custom_end_time: Optional[str] = None) -> bool:

        send_cmd_result = self.mqtt_handler.send_diag_command("AT+DIAG=TM")
        if not send_cmd_result.success:
            logger.error(f"获取 device 时间失败: {send_cmd_result.error}")
            return False

        if time_range == TimeRange.IN_A_DAY:
            self.start_time = max(0, self.device_time - 86400 * 1000)
            self.end_time = self.device_time
        elif time_range == TimeRange.IN_A_WEEK:
            self.start_time = max(0, self.device_time - 7 * 86400 * 1000)
            self.end_time = self.device_time
        elif time_range == TimeRange.IN_A_MONTH:
            logger.info(f"time_period {time_range}")
            self.start_time = max(0, self.device_time - 30 * 86400 * 1000)
            self.end_time = self.device_time
        elif time_range == TimeRange.CUSTOM_OPTIONS:
            custom_start_timestamp = self._date_string_to_timestamp(custom_start_time)
            custom_end_timestamp = self._date_string_to_timestamp(custom_end_time)
            if custom_start_timestamp > custom_end_timestamp:
                logger.error(f"自定义时间范围错误: {custom_start_time} > {custom_end_time}")
                return False
            self.start_time = max(0, self.device_time - (self.real_time - custom_start_timestamp))
            self.end_time = max(self.start_time, self.device_time - (self.real_time - custom_end_timestamp))
        elif time_range == TimeRange.ALL:
            self.start_time = 0
            self.end_time = self.device_time
        else:
            logger.error(f"不支持的时间范围: {time_range}")
            return False

        return True


    def set_diag_send_buffer(self) -> bool:

        send_cmd_result = self.mqtt_handler.send_diag_command(f"AT+DIAG=BUF_COM,{DIAG_READ_BUFFER_SIZE}")
        if not send_cmd_result.success:
            logger.error(f"获取 diag event 事件失败: {send_cmd_result.error}")
            return False
        return True

    def diag_read_backgroud_worker(self,
                                   time_range: TimeRange,
                                   custom_start_time: Optional[str] = None,
                                   custom_end_time: Optional[str] = None) -> bool:

        result = self.check_hash_and_get_format_json(DIAG_JSON_FOLDER)
        if result:
            logger.info("hash 校验成功")
        else:
            logger.error(f"hash 校验失败")
            return False

        result = self.set_diag_event_time_period(time_range, custom_start_time, custom_end_time)
        if result:
            logger.info("event 时间范围设置成功")
        else:
            logger.error(f"event 时间范围设置失败")
            return False

        result = self.set_diag_send_buffer()
        if result:
            logger.info("buffer 设置成功")
        else:
            logger.error(f"buffer 设置失败")
            return False

        return True

    def _check_need_continue_get_diag_event(self, last_result:SendCmdResult) -> bool:
        if last_result.success == False:
            return False
        if self._current_timestamp or self._current_offset:
            return True
        return False

    def get_diag_event(self) -> bool:

        send_cmd_result = self.mqtt_handler.send_diag_command(f"AT+DIAG=GET_EVT,{self.start_time},0")
        if not send_cmd_result.success:
            logger.error(f"获取 diag event 事件失败: {send_cmd_result.error}")
            return False

        while self._check_need_continue_get_diag_event(send_cmd_result):
            send_cmd_result = self.mqtt_handler.send_diag_command(f"AT+DIAG=GET_EVT,{self._current_timestamp},{self._current_offset}")
            if not send_cmd_result.success:
                logger.error(f"获取 diag event 事件失败: {send_cmd_result.error}")
                return False
        logger.info("获取 diag event 事件结束")
        return True

    def del_diag_events(self, datetime_str:str, before = True) -> tuple[bool, int]:

        if not self.is_sync_time:
            send_cmd_result = self.mqtt_handler.send_diag_command("AT+DIAG=TM")
            if not send_cmd_result.success:
                logger.error(f"获取 device 时间失败: {send_cmd_result.error}")
                return False, 0

        time_stamp = self.device_time - (self.real_time - self._date_string_to_timestamp(datetime_str))
        if time_stamp <= 0 or time_stamp >= self.device_time:
            logger.warning("超出设备记录的时间")
            return True, 0

        if before:
            send_cmd_result = self.mqtt_handler.send_diag_command(f"AT+DIAG=DEL_EVT_BF,{time_stamp}")
        else:
            send_cmd_result = self.mqtt_handler.send_diag_command(f"AT+DIAG=DEL_EVT_AF,{time_stamp}")
        if send_cmd_result.success:
            return True, self.del_events_count
        else:
            logger.error(f"删除事件失败: {send_cmd_result.error}")
            return False, 0

