import serial
import time
import sys

binary_file = 'ota_all.bin'
port = 'COM148'  # for Linux/Mac, may '/dev/ttyUSB0'
baudrate = 1500000

def main():
    try:
        with serial.Serial(port, baudrate, timeout=1, rtscts=True) as ser:
            while True:
                while True:
                    data = ser.read(5)
                    if data.decode('utf-8') == 'hello':
                        print(f'serial recv from {port}: {data.decode()}')
                        break
                    else:
                        time.sleep(0.1)
                total_bytes = 0
                with open(binary_file, 'rb') as fd:
                    while True:
                        data = fd.read(1)
                        if len(data) == 0:
                            break
                        while not ser.writable(): {}
                        size = ser.write(data)
                        ser.flush()
                        if size != len(data):
                            print("size: ", size)
                        
                        total_bytes += len(data)
                        if total_bytes % 1024 == 0:
                            print('.', end=' ')
                            sys.stdout.flush()
                        # time.sleep(0.1)
                        
                print(f'serial send to {port} finished, total {total_bytes} bytes')
    except serial.SerialException as e:
        print(f'serial error: {e}')

if __name__ == '__main__':
    main()
