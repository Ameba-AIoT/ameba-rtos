#/** @file
# * Copyright (c) 2018, Arm Limited or its affiliates. All rights reserved.
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

# This is TBSA debugger script in Jython for DS5 debugger initialisation

import sys
from arm_ds.debugger_v1 import Debugger
from arm_ds.internal import Breakpoint, BreakpointService, MemoryService
from __builtin__ import len

#defines for sequences to be executed
SEQ_OPEN_STATE_READ = 0x10
SEQ_OPEN_STATE_WRITE = 0x20
SEQ_CLOSED_STATE_READ = 0x30
SEQ_CLOSED_STATE_WRITE = 0x40
SEQ_LOCKED_STATE_READ = 0x50
#SEQ_LOCKED_STATE_WRITE = 0x60
SEQ_NEXT_TEST = 0xE0
SEQ_TEST_END = 0xF0

#defines for indicating signature to PE
DBG_INIT_SIGNATURE = 0xDEED007
DBG_NEXT_SIGNATURE = 0xDEED008

#defines for status handshake between core and debugger
DBG_FLAG_TXFULL = 0x1
DBG_FLAG_RXFULL = 0x2
DBG_FLAG_READY = 0x4

if (len(sys.argv)) == 0:
    print "Please enter the flag and data address"
else:
    print "Printing the args"
    print "Flag address is: %s" %(sys.argv[1])
    print "Data address is: %s" %(sys.argv[2])

# Debugger object for accessing the debugger
debugger = Debugger()

# Ensure that the target is stopped
ec = debugger.getCurrentExecutionContext()
ec.getExecutionService().stop()

#Initialize the flag and data address
ec.getMemoryService().writeMemory32(sys.argv[1], DBG_FLAG_READY)
ec.getMemoryService().writeMemory32(sys.argv[2], DBG_INIT_SIGNATURE)

#Set the watchpoint on flag address
run_status = 0
wp = ec.getBreakpointService().setWriteWatchpoint(sys.argv[1])

#Resume the system
ec.getExecutionService().resume()

while (run_status != SEQ_TEST_END):
    #Wait until watchpoint is hit and system is halted
    ec.getExecutionService().waitForStop(0)
    flag_register = ec.getMemoryService().readMemory32(sys.argv[1])
    run_status = flag_register & 0xf0
    print "flag register value : 0x%x" %(flag_register)
    # Continue the loop is Txfull is 0
    if ((flag_register & 0xff) == 0x0):
        ec.getMemoryService().writeMemory32(sys.argv[1], DBG_FLAG_READY)
        ec.getExecutionService().resume()
        continue

    if run_status == SEQ_OPEN_STATE_READ:
        #Read and update the data register
        ec.getMemoryService().writeMemory32(sys.argv[1], 0x0)
        data = ec.getMemoryService().readMemory32(sys.argv[2])
        output = ec.getMemoryService().readMemory32(data)
        ec.getMemoryService().writeMemory32(sys.argv[2], output)
        #Set flag register appropriately so that CPU can proceed.
        ec.getMemoryService().writeMemory32(sys.argv[1], DBG_FLAG_READY|DBG_FLAG_RXFULL)
        ec.getExecutionService().resume()
    elif run_status == SEQ_OPEN_STATE_WRITE:
        #Read and update the data register
        ec.getMemoryService().writeMemory32(sys.argv[1], 0x0)
        addr = ec.getMemoryService().readMemory32(sys.argv[2])
        ec.getMemoryService().writeMemory32(sys.argv[1], DBG_FLAG_READY|DBG_FLAG_RXFULL)
        ec.getExecutionService().resume()
        ec.getExecutionService().waitForStop(0)
        ec.getMemoryService().writeMemory32(sys.argv[1], 0x0)
        data = ec.getMemoryService().readMemory32(sys.argv[2])
        ec.getMemoryService().writeMemory32(addr, data)
        ec.getMemoryService().writeMemory32(sys.argv[1], DBG_FLAG_READY|DBG_FLAG_RXFULL)
        ec.getExecutionService().resume()
    elif run_status == SEQ_CLOSED_STATE_READ:
        #Read the data register
        ec.getMemoryService().writeMemory32(sys.argv[1], 0x0)
        data = ec.getMemoryService().readMemory32(sys.argv[2])
        ec.getMemoryService().writeMemory32(sys.argv[1], DBG_FLAG_READY|DBG_FLAG_RXFULL)
        ec.getExecutionService().resume()
        # Add the code : wait for DAP error
        output = ec.getMemoryService().readMemory32(data) # this read must happen while still getting DAP error.
        # Add the code : check if still getting DAP error, to ensure that read access happened in closed state
    elif run_status == SEQ_CLOSED_STATE_WRITE:
        #Read the data register
        ec.getMemoryService().writeMemory32(sys.argv[1], 0x0)
        addr = ec.getMemoryService().readMemory32(sys.argv[2])
        ec.getMemoryService().writeMemory32(sys.argv[1], DBG_FLAG_READY|DBG_FLAG_RXFULL)
        ec.getExecutionService().resume()
        ec.getExecutionService().waitForStop(0)
        ec.getMemoryService().writeMemory32(sys.argv[1], 0x0)
        data = ec.getMemoryService().readMemory32(sys.argv[2])
        ec.getMemoryService().writeMemory32(addr, data)
        ec.getMemoryService().writeMemory32(sys.argv[1], DBG_FLAG_READY|DBG_FLAG_RXFULL)
        ec.getExecutionService().resume()
        # Add the code : wait for DAP error
        output = ec.getMemoryService().readMemory32(data)
        ec.getMemoryService().writeMemory32(addr, data) # this write must happen while still getting DAP error.
        # Add the code : check if still getting DAP error, to ensure that read access happened in closed state
    elif run_status == SEQ_LOCKED_STATE_READ:
        #Read and update the data register
        ec.getMemoryService().writeMemory32(sys.argv[1], 0x0)
        data = ec.getMemoryService().readMemory32(sys.argv[2])
        ec.getMemoryService().writeMemory32(sys.argv[1], DBG_FLAG_READY|DBG_FLAG_RXFULL)
        ec.getExecutionService().resume()
        # Add the code : wait for DAP error
        output = ec.getMemoryService().readMemory32(data) # this read must happen while still getting DAP error.
        # Add the code : check if still getting DAP error, to ensure that read access happened in closed state
        ec.getExecutionService().waitForStop(0)
        ec.getMemoryService().writeMemory32(sys.argv[1], 0x0)
        ec.getMemoryService().writeMemory32(sys.argv[2], output)
        #Set flag register appropriately so that CPU can proceed.
        ec.getMemoryService().writeMemory32(sys.argv[1], DBG_FLAG_READY|DBG_FLAG_RXFULL)
        ec.getExecutionService().resume()
    # This seq is to indicate to proceed to next test.
    elif run_status == SEQ_NEXT_TEST:
        ec.getMemoryService().writeMemory32(sys.argv[2], DBG_NEXT_SIGNATURE)
        ec.getMemoryService().writeMemory32(sys.argv[1], DBG_FLAG_READY)
        print "proceed to next debug test"
        ec.getExecutionService().resume()

wp.disable()
ec.getExecutionService().resume()
ec.executeDSCommand("quit")
print "debug tests finished"
