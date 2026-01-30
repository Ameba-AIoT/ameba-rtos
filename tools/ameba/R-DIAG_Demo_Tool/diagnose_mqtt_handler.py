from diagnose_defines import *
import paho.mqtt.client as mqtt
import time
import uuid
import threading
from typing import Callable

class MQTTHandler:
    def __init__(self, broker: str, port: int, topic_pub: str, topic_sub: str):
        self.broker = broker
        self.port = port
        self.topic_pub = topic_pub
        self.topic_sub = topic_sub

        # MQTT客户端
        self.client: Optional[mqtt.Client] = None

        # 消息回调注册机制
        self._message_callbacks = {}

        # 状态管理
        # 可重入锁 同一个线程可多次获取同一个 RLock 而不会产生死锁。 但获取了多少次就必须释放多少次， 直到锁被完全释放后其他线程才能获取
        self._state_lock = threading.RLock()
        self._state = DiagState.DISCONNECTED
        self._connection_event = threading.Event()

        # 发送 diag command 相关属性
        self._current_command = None
        self._command_retries = 0
        self._response_received = threading.Event()
        self._last_process_success = False


    @property
    def state(self) -> DiagState:
        with self._state_lock:
            return self._state

    @state.setter
    def state(self, value: DiagState):
        with self._state_lock:
            old_state = self._state
            self._state = value
            if old_state != value:
                logger.info(f"状态变更: {old_state} -> {value}")

    @property
    def current_command(self) -> Optional[str]:
        with self._state_lock:
            return self._current_command

    @current_command.setter
    def current_command(self, value: Optional[str]):
        with self._state_lock:
            old_command = self._current_command
            self._current_command = value
            if old_command != None and value != None:
                logger.warning(f"command未完成的情况下变更: {old_command} -> {value}")

    def _on_connect(self, client, userdata, flags, reason_code, properties):
        """MQTT连接回调"""
        if reason_code == 0:
            logger.info("MQTT连接成功")
            self.state = DiagState.CONNECTED
            self.client.subscribe(self.topic_sub)
            self._connection_event.set()
        else:
            error_msg = f"连接失败，错误码：{reason_code}"
            logger.error(error_msg)
            self.state = DiagState.ERROR
            self._connection_event.set()

    def _on_disconnect(self, client, userdata, disconnect_flags, reason_code, properties):
        """MQTT断开连接回调"""
        logger.warning(f"MQTT 连接断开， 原因码: {reason_code}")
        self.state = DiagState.DISCONNECTED
        self._connection_event.clear()

    def _on_message(self, client, userdata, message):
        """MQTT消息回调"""
        try:
            if not self.current_command:
                #logger.debug("无当前命令，忽略消息")
                return
            #payload = message.payload.decode('utf-8', errors='ignore')
            # 处理消息
            if message.topic in self._message_callbacks:
                self._message_callbacks[message.topic](self.current_command, message.payload)
            else:
                logger.warning(f"未注册的回调 topic: {message.topic}")

        except Exception as e:
            logger.error(f"消息处理异常: {e}")

    def _safe_reconnect(self):
        """带指数退避的重连机制"""
        if self.state != DiagState.DISCONNECTED:
            return

        for attempt in range(MAX_RETRIES):
            try:
                logger.info(f"重连尝试 ({attempt+1}/{MAX_RETRIES})")
                with self._state_lock:
                    self.state = DiagState.CONNECTING

                self.client.reconnect()

                # 使用动态等待时间 (1, 2, 4, 8...秒)
                wait_time = min(DEFAULT_TIMEOUT ** attempt, 30)  # 上限30秒
                if self._connection_event.wait(wait_time):
                    if self.state == DiagState.CONNECTED:
                        logger.info("重连成功")
                        return True
            except Exception as e:
                logger.error(f"重连异常: {e}")
                time.sleep(1)  # 防止高频错误

            # 检查是否需要继续重试
            with self._state_lock:
                if self.state != DiagState.CONNECTING:
                    logger.info("外部状态变更，终止重连")
                    return False

        logger.error(f"达到最大重连次数 {MAX_RETRIES}")
        with self._state_lock:
            self.state = DiagState.ERROR
        return False


    def connect(self) -> bool:
        """连接到MQTT代理"""
        client_id = f"{uuid.uuid4().hex[:8]}"
        self.client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, client_id=client_id)
        self.client.on_connect = self._on_connect
        self.client.on_message = self._on_message
        self.client.on_disconnect = self._on_disconnect

        if self.state in [DiagState.CONNECTED, DiagState.CONNECTING]:
            logger.warning("已经连接或正在连接")
            return True
        for attempt in range(MAX_RETRIES):
            try:

                self.state = DiagState.CONNECTING
                self._connection_event.clear()

                logger.info(f"连接尝试{attempt+1}/{MAX_RETRIES}")
                self.client.connect(self.broker, self.port, 60)
                self.client.loop_start()

                # 等待连接建立
                # 计算动态等待时间（指数退避）
                wait_time = min(DEFAULT_TIMEOUT ** attempt, 30)  # 上限30秒
                if self._connection_event.wait(wait_time):
                    return self.state == DiagState.CONNECTED
                else:
                    logger.warning(f"尝试 {attempt+1} 连接超时")

            except Exception as e:
                logger.error(f"连接异常: {e}")
                self.state = DiagState.ERROR


        self.state = DiagState.ERROR
        return False


    def disconnect(self):
        """断开连接并清理资源"""
        if self.state == DiagState.DISCONNECTED or self.state == DiagState.ERROR:
            logger.info("已经断开连接")
            return
        logger.info("正在断开连接...")

        try:
            self.client.loop_stop()
            self.client.disconnect()
        except Exception as e:
            logger.error(f"断开连接异常: {e}")

        self.state = DiagState.DISCONNECTED
        logger.info("连接已断开")

    def _wait_for_response(self, start_time, timeout, wait_interval=0.1) -> bool:
        """带连接状态感知的响应等待"""
        total_timeout = timeout  # 记录原始超时时间
        time_used = 0

        while time_used < total_timeout:
            # 检查连接状态
            if self.state not in (DiagState.CONNECTED, DiagState.PROCESSING):
                logger.warning("检测到连接中断，尝试恢复...")
                reconnect_start = time.time()
                if not self._safe_reconnect():
                    return False
                # 计算重连耗时并补偿
                reconnect_duration = time.time() - reconnect_start
                time_used += reconnect_duration
                start_time += reconnect_duration  # 调整起始时间基准
                if time_used >= total_timeout:
                    break
                continue

            # 计算剩余等待时间
            remaining = total_timeout - time_used

            if self._response_received.wait(min(remaining, wait_interval)):
                if self._last_process_success:
                    return True
                else:
                    self._response_received.clear()
                    return False

            # 更新已用时间
            time_used = time.time() - start_time

        logger.debug(f"总等待时间 {time_used:.1f}s 超过 {total_timeout}s")
        return False

    def send_diag_command(self, cmd: str) -> SendCmdResult:

        send_cmd_result = SendCmdResult(success=False)

        if self.state != DiagState.CONNECTED and not self.connect():
            send_cmd_result.error = "连接失败"
            return send_cmd_result

        with self._state_lock:
            if self.current_command is not None:
                send_cmd_result.error = "已有命令在执行"
                return send_cmd_result

            self.current_command = cmd
            self.state = DiagState.PROCESSING

        try:
            self._response_received.clear()
            self._command_retries = 0
            self._last_process_success = False
            # 等待响应
            while self._command_retries < MAX_RETRIES:

                # 发送前二次确认连接状态
                if self.state != DiagState.CONNECTED and self.state != DiagState.PROCESSING:
                    send_cmd_result.error = "连接断开"
                    break

                self.client.publish(self.topic_pub, cmd)
                self._command_retries += 1
                logger.info(f"第 {self._command_retries} 次发送请求 {self.current_command}")
                start_time = time.time()
                if self._wait_for_response(start_time, DEFAULT_TIMEOUT):
                    send_cmd_result.success = True

                    break
                else:
                    logger.warning(f"第 {self._command_retries} 次发送请求 {self.current_command} 超时 ")
            # 检查超时
            if not send_cmd_result.success and self._command_retries == MAX_RETRIES:
                send_cmd_result.error = f"请求次数达到最大 {self._command_retries} 响应超时"


        except Exception as e:
            send_cmd_result.error = f"命令执行异常: {e}"

        finally:
            with self._state_lock:
                self.current_command = None
                if self.state == DiagState.PROCESSING:
                # 仍处于处理状态时才恢复连接状态
                    if self._connection_event.is_set():
                        self.state = DiagState.CONNECTED
                    else:
                        self.state = DiagState.DISCONNECTED

        return send_cmd_result

    def update_diag_command_state(self, success: bool):
        """更新命令执行状态"""
        with self._state_lock:
            if success and self.state == DiagState.PROCESSING:
                self.state = DiagState.CONNECTED

        self._response_received.set()
        self._last_process_success = success

    def register_message_callback(self, topic: str, callback: Callable[[str], None]):
        """注册消息处理回调"""
        self._message_callbacks[topic] = callback
