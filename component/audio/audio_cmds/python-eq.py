#!/usr/bin/env python
# -*- coding: utf-8 -*-

import serial
import time
import json
from datetime import datetime
import threading

msg_query = '{"type":"query"}'
msg_set_freq = '{"type":"setfrequency", "id":0, "centerfrequency":47}'
msg_set_gain = '{"type":"setgain", "id":5, "gain":15}'
msg_set_qfactor = '{"type":"setqfactor", "id":0, "qfactor":0.96}'
msg_set_filtertype = '{"type":"setfiltertype", "id":0, "filtertype":0}'

class SerialPort:
    def __init__(self, port, buand):
        self.data_bytes=bytearray()
        self.is_exit = False
        self.port = serial.Serial(port, buand)
        self.port.close()
        if not self.port.isOpen():
            self.port.open()

    def port_open(self):
        if not self.port.isOpen():
            self.port.open()

    def port_close(self):
        self.port.close()

    def send_data(self,data):
        self.port.write(data.encode())

    def read_data(self):
        while not self.is_exit:
            count = self.port.inWaiting()
            if count > 0:
                rec_str = self.port.read(count)
                self.data_bytes=self.data_bytes+rec_str
                print("get %s"%self.data_bytes)

serialPort = 'COM40'
baudRate = 1500000

if __name__ == '__main__':

    mSerial = SerialPort(serialPort, baudRate)

    t1 = threading.Thread(target=mSerial.read_data)
    t1.setDaemon(True)
    t1.start()
    frame_cnt = 0
    frame_error_cnt = 0

    query_obj = json.loads(msg_query)
    query_json_str = json.dumps(query_obj, indent=4)
    print(query_json_str)
    mSerial.send_data(query_json_str)

    #sleep 10ms
    time.sleep(0.05)

    set_freq_obj = json.loads(msg_set_freq)
    set_freq_json_str = json.dumps(set_freq_obj, indent=4)
    print(set_freq_json_str)
    mSerial.send_data(set_freq_json_str)

    time.sleep(0.05)

    set_gain_obj = json.loads(msg_set_gain)
    set_gain_json_str = json.dumps(set_gain_obj, indent=4)
    print(set_gain_json_str)
    mSerial.send_data(set_gain_json_str)

    time.sleep(0.05)

    set_qfactor_obj = json.loads(msg_set_qfactor)
    set_qfactor_json_str = json.dumps(set_qfactor_obj, indent=4)
    print(set_qfactor_json_str)
    mSerial.send_data(set_qfactor_json_str)

    time.sleep(0.05)

    set_filtertype_obj = json.loads(msg_set_filtertype)
    set_filtertype_json_str = json.dumps(set_filtertype_obj, indent=4)
    print(set_filtertype_json_str)
    mSerial.send_data(set_filtertype_json_str)

    try:
        while True:
            time.sleep(5)

    except KeyboardInterrupt:
        if mSerial.port_open():
            mSerial.port_close()