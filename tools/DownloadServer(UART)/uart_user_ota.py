import serial
import time
import sys
import signal
import os
import threading

binary_file = 'ota_all.bin'
port = 'COM6'
baudrate = 1500000

running = True
stop_event = threading.Event()

def signal_handler(sig, frame):
    """处理 Ctrl+C"""
    global running
    print('\n\n🛑 [CTRL+C] Stopping transfer...')
    running = False
    stop_event.set()  # ✅ 通知线程停止

def get_file_size(filename):
    try:
        return os.path.getsize(filename)
    except OSError:
        return 0

def print_progress(current, total):
    """打印进度"""
    if total == 0:
        return
    percent = int(current * 100 / total)
    bar_length = 50
    filled = int(bar_length * current / total)
    bar = '=' * filled + '>' + ' ' * (bar_length - filled - 1)

    sys.stdout.write(f'\r[{bar}] {percent}% | {current}/{total} bytes')
    sys.stdout.flush()

def transfer_worker(ser, file_path, file_size, stats):
    """传输工作线程"""
    global running

    try:
        with open(file_path, 'rb') as fd:
            chunk_size = 32  # 1KB chunks
            
            while running and not stop_event.is_set():
                # Read chunk
                data = fd.read(chunk_size)
                if len(data) == 0:
                    break  # EOF

                # Write with check
                try:
                    written = ser.write(data)
                    stats['total_bytes'] += written
                    stats['last_update'] = time.time()

                    # ✅ 检查停止标志
                    if stop_event.is_set():
                        break

                except serial.SerialException as e:
                    stats['error'] = str(e)
                    break

                # Small sleep to allow interruption
                time.sleep(0.001)

        stats['completed'] = (stats['total_bytes'] == file_size)

    except Exception as e:
        stats['error'] = str(e)

def main():
    global running

    # Register signal handler
    signal.signal(signal.SIGINT, signal_handler)

    file_size = get_file_size(binary_file)
    if file_size == 0:
        print(f'❌ [ERROR] File not found or empty: {binary_file}')
        return
    
    print(f'📡 [INFO] Starting OTA transfer')
    print(f'📡 [INFO] Port: {port} @ {baudrate} baud')
    print(f'📁 [INFO] File: {binary_file} ({file_size / 1024:.2f} KB)')
    print(f'⚡ [INFO] Press Ctrl+C to stop\n')

    ser = None
    transfer_thread = None

    try:
        ser = serial.Serial(
            port=port,
            baudrate=baudrate,
            timeout=0.5,
            write_timeout=1.0,
            rtscts=True
        )

        # Wait for handshake
        print('🤝 [1/2] Waiting for "hello"...')
        start_wait = time.time()

        while running and (time.time() - start_wait) < 30:
            if stop_event.is_set():
               break

            data = ser.read(5)
            if data == b'hello':
               print('✅ [OK] Handshake received\n')
               break
            else:
               time.sleep(0.1)

        if not running or stop_event.is_set():
            print('⚠️ [INFO] Stopped during handshake')
            return

        # Start transfer
        print('📤 [2/2] Transferring...')

        stats = {
            'total_bytes': 0,
            'last_update': time.time(),
            'completed': False,
            'error': None
        }

        start_time = time.time()

        # ✅ 启动传输线程
        transfer_thread = threading.Thread(
            target=transfer_worker,
            args=(ser, binary_file, file_size, stats),
            daemon=True
        )
        transfer_thread.start()

        # ✅ 主线程监控进度，可以立即响应 Ctrl+C
        last_print = time.time()
        while transfer_thread.is_alive() and running:
            current_time = time.time()

            # Update progress every 100ms
            if current_time - last_print >= 0.1:
                print_progress(stats['total_bytes'], file_size)
                last_print = current_time
            
            # ✅ 短暂休眠，让 Ctrl+C 能快速响应
            time.sleep(0.05)

            # Check if thread finished
            if stats['completed'] or stats['error']:
                break

        # Wait for thread to finish (with timeout)
        if transfer_thread.is_alive():
            transfer_thread.join(timeout=2.0)

        # Print final result
        elapsed = time.time() - start_time

        if stats['error']:
            print(f'\n\n❌ [ERROR] Transfer failed: {stats["error"]}')
        elif stats['completed'] and running:
            speed = stats['total_bytes'] / elapsed / 1024 if elapsed > 0 else 0
            print('\n\n✅ [OK] Transfer complete!')
            print(f'📊 Total: {stats["total_bytes"]} bytes ({stats["total_bytes"] / 1024:.2f} KB)')
            print(f'⏱️  Time: {elapsed:.2f} seconds')
            print(f'🚀 Speed: {speed:.2f} KB/s')
        else:
            print(f'\n\n⚠️ [INFO] Transfer interrupted')
            print(f'📊 Transferred: {stats["total_bytes"]}/{file_size} bytes')

    except serial.SerialException as e:
        print(f'\n❌ [ERROR] Serial error: {e}')
    except KeyboardInterrupt:
        print('\n⚠️ [INFO] KeyboardInterrupt in main')
    except Exception as e:
        print(f'\n❌ [ERROR] Unexpected error: {e}')
        import traceback
        traceback.print_exc()
    finally:
        running = False
        stop_event.set()
        
        if transfer_thread and transfer_thread.is_alive():
            transfer_thread.join(timeout=1.0)
        
        if ser and ser.is_open:
            try:
                ser.close()
                print('🔌 [INFO] Serial port closed')
            except:
                pass
        
        print('👋 [INFO] Exit\n')

if __name__ == '__main__':
    main()
