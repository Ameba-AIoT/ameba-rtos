#!/usr/bin/python
#/** @file
# * Copyright (c) 2022 Arm Limited or its affiliates. All rights reserved.
# * SPDX-License-Identifier : Apache-2.0
# *
# * Licensed under the Apache License, Version 2.0 (the "License");
# * you may not use this file except in compliance with the License.
# * You may obtain a copy of the License at
# *
# *  http://www.apache.org/licenses/LICENSE-2.0
# *
# * Unless required by applicable law or agreed to in writing, software
# * distributed under the License is distributed on an "AS IS" BASIS,
# * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# * See the License for the specific language governing permissions and
# * limitations under the License.
#**/

# install pyOCD  --> pip install pyocd
# install libusb --> see https://github.com/pyocd/pyOCD#libusb-installation

import socket
import sys
import struct
import time
from pyocd.core.helpers import ConnectHelper
from pyocd.utility.conversion import byte_list_to_u32le_list

class ADAC_ACS():
    """! @brief Main class for the adac acs script, acting as bridge between acs testcases & platform"""

    def __init__(self, PORT=7777):
        self.port = PORT
        self.host = "0.0.0.0"
        self.target = None
        self.base = None
        self.reset = None

    def create_server_socket(self):
        try:
            print("Creating server socket ipaddress : %s, port : %d\n" % (self.host, self.port))
            self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            self.server_socket.bind((self.host, self.port))
        except socket.error as msg:
            print('Failed to create socket. Error Code : ' + str(msg[0]) + ' Message ' + msg[1])
            sys.exit()

    def parse_uint(self, s):
        try:
            return int(s, 16)
        except:
            print('self.parse_uint error : might be the number of arg and payload bytes mismatch\n')
        return

    def receive(self):
        while self.target.read32(self.base + 0x0) != 0xEFEFEFEF:
            time.sleep(0.1)
        hl = 8
        h = self.target.read_memory_block8(self.base + 0x10, hl)
        (_ign, head, words) = struct.unpack("<HHL", bytes(h))

        data = bytes([])
        if words > 0:
            data = bytearray(self.target.read_memory_block8(self.base + 0x10 + hl, words << 2))
        self.received += hl + len(data)
        return bytes(h), bytes(data)

    def connect(self):
        print("Reset device and request Connection")
        self.target.reset_and_halt()
        self.target.write32(self.base + 0x0, 0xFF00FF00)
        self.target.write32(self.base + 0x4, 0x00FF00FF)
        self.target.write32(self.base + 0x8, 0xFF00FF00)
        self.target.write32(self.base + 0xC, 0x00FF00FF)
        self.target.resume()

        acknowledged = False
        s = 0.1
        while not acknowledged:
            time.sleep(s)
            s *= 2

            m = [self.target.read32(self.base + 0x0), self.target.read32(self.base + 0x4),
                 self.target.read32(self.base + 0x8), self.target.read32(self.base + 0xC)]

            print("MUSCA_B1_SRAM_S_BASE[0]: 0x%08X (0x%08X)" % (m[0], 0x00FF00FF))
            print("MUSCA_B1_SRAM_S_BASE[1]: 0x%08X (0x%08X)" % (m[1], 0xFF00FF00))
            print("MUSCA_B1_SRAM_S_BASE[2]: 0x%08X (0x%08X)" % (m[2], 0x00FF00FF))
            print("MUSCA_B1_SRAM_S_BASE[3]: 0x%08X (0x%08X)" % (m[3], 0xFF00FF00))
            acknowledged = (m[0] == 0x00FF00FF) and (m[2] == 0x00FF00FF) and (m[3] == 0xFF00FF00)

            print("Acknowledged: %s" % ("True" if acknowledged else "False"))

        self.sent = 0
        self.received = 0

    def close(self):
        self.target.write32(self.base + 0x0, 0x00000000)
        self.target.write32(self.base + 0x4, 0x00000000)
        self.target.write32(self.base + 0x8, 0x00000000)
        self.target.write32(self.base + 0xC, 0x00000000)
        return self.sent, self.received

    def send(self, data):
        self.target.write_memory_block32(self.base + 0x10, byte_list_to_u32le_list(data))
        self.target.write32(self.base + 0x0, 0x12121212)
        self.sent += len(data)


if __name__ == "__main__":
    if len(sys.argv) > 1:
        adac_acs = ADAC_ACS(PORT=int(sys.argv[1]))
    else:
        adac_acs = ADAC_ACS()
    adac_acs.create_server_socket()
    session_args = {
        "unique_id": None,
        "frequency": 10000000,  # 10 MHz
    }

    adac_acs.base = 0x30000000
    session_args['connect_mode'] = "pre-reset"

    while True:
        with ConnectHelper.session_with_chosen_probe(**session_args) as session:
            adac_acs.target = session.board.target
            adac_acs.connect()
            while True:
                try:
                    data_received, address = adac_acs.server_socket.recvfrom(4096)
                    length = len(data_received)
                    print("Data received from adac acs : length : %d, data : %s"%(length, str(data_received.hex())))
                    adac_acs.send(data_received)
                    head, data = adac_acs.receive()
                    adac_acs.server_socket.sendto(head, address)
                    print("Header sent to adac acs : length : %d, data : %s"%(len(head), str(head.hex())))
                    adac_acs.server_socket.sendto(data, address)
                    print("Data sent to adac acs : length : %d, data : %s"%(len(data), str(data.hex())))
                except Exception as e:
                    continue

            adac_acs.close()
