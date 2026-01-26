import sys
import json
import os
import threading
import functools
from http.server import ThreadingHTTPServer, SimpleHTTPRequestHandler
from typing import Dict, Any, List
from datetime import datetime
from diagnose_defines import *
from diagnose_mqtt_handler import MQTTHandler
from diagnose_diag_controller import DiagController

class WiFiDiagnoseServer:
    def __init__(self, mqtt_handler: MQTTHandler):
        # Initialize components
        self.mqtt_handler = mqtt_handler
        self.diag_controller = DiagController(self.mqtt_handler)
        # Create tools mapping
        self.tools = {
            "diag_read_events": self.diag_read_events,
        }

        # Try to connect to MQTT on startup
        try:
            self.mqtt_handler.connect()
            logger.info("MQTT connected on startup")
        except Exception as e:
            logger.warning(f"MQTT connection failed on startup: {e}. Use configure_mqtt to connect later.")

    def diag_read_events(self,
                         time_range: str,
                         custom_start_time: Optional[str] = None,
                         custom_end_time: Optional[str] = None) -> Dict[str, Any]:
        try:
            # 将字符串转换为枚举类型
            if isinstance(time_range, str):
                time_range = TimeRange(time_range)

            if time_range == TimeRange.CUSTOM_OPTIONS:
                if custom_start_time is None or custom_end_time is None:
                    raise ValueError("When time_range is CUSTOM, custom_start_time and custom_end_time must be provided")

            result = self.diag_controller.diag_read_backgroud_worker(time_range, custom_start_time, custom_end_time)
            if result:
                logger.info("获取 diag events 前置工作成功")
            else:
                return {
                "status": "fail",
                "timestamp": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
                }
            result = self.diag_controller.get_diag_event()
            if result:
                return {
                "status": "success",
                "timestamp": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
                }
            else:
                return {
                "status": "fail",
                "timestamp": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
                }

        except Exception as e:
            logger.error(f"{str(e)}")
            return {
                "status": "fail",
                "timestamp": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            }

def main():
    mqtt_handler = MQTTHandler(MQTT_BROKER, MQTT_PORT, MQTT_TOPIC_PUBLISH, MQTT_TOPIC_SUBSCRIBE)

    server = WiFiDiagnoseServer(mqtt_handler)
    server.diag_read_events(TimeRange.IN_A_DAY)

if __name__ == "__main__":
    main()