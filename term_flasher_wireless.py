#!/usr/init/env python3
import sys
import os
import time
import select
import serial
from xmodem import XMODEM

DEFAULT_PORT = "/dev/rfcomm0"
BAUD = 115200

def run_terminal_and_flash(port, baudrate, bin_path):
    # Open serial port (Bluetooth RFCOMM doesn't require DTR/RTS manipulation like CH340)
    ser = serial.Serial(port, baudrate, timeout=0.1)
    print(f"--- Connected wirelessly to {port} at {baudrate} baud ---")
    print("--- Automatic wireless flashing flow initialized ---")
    print("--- [Press Ctrl+C to exit] ---\n")

    def getc(size, timeout=1):
        ser.timeout = timeout
        return ser.read(size) or None

    def putc(data, timeout=1):
        ser.timeout = timeout
        return ser.write(data)

    try:
        while True:
            # Reset decoded text at the start of every iteration to prevent stale state retention
            decoded_text = ""

            # Check serial buffer
            if ser.in_waiting > 0:
                raw_data = ser.read(ser.in_waiting)
                decoded_text = raw_data.decode('utf-8', errors='ignore')

                # State 1: App is running and sending 'A' -> Request reset to bootloader
                if 'A' in decoded_text:
                    sys.stdout.write(decoded_text)
                    sys.stdout.flush()
                    print("\n[Wireless-Flasher] Detected application state ('A'). Requesting reset to bootloader...")
                    ser.write(b'reset\r\n')
                    ser.flush()
                    time.sleep(0.5)
                    continue

                # State 2: Bootloader is waiting and sending 'B' -> Send flash command
                elif 'B' in decoded_text:
                    sys.stdout.write(decoded_text)
                    sys.stdout.flush()
                    print("\n[Wireless-Flasher] Detected bootloader ready state ('B'). Sending flash request...")
                    ser.write(b'flash\r\n')
                    ser.flush()
                    time.sleep(0.2)
                    continue

                # State 3: XMODEM ready signal 'C' detected -> Start wireless upload
                elif 'C' in decoded_text and bin_path and os.path.exists(bin_path):
                    sys.stdout.write(decoded_text)
                    sys.stdout.flush()
                    
                    print("\n\n>>> XMODEM Start Signal 'C' Detected over Bluetooth! Starting Wireless Flash... <<<")
                    
                    file_size = os.path.getsize(bin_path)
                    total_packets = (file_size + 127) // 128

                    def progress_callback(total_packets_sent, success_count, error_count):
                        percent = min(100.0, (total_packets_sent / total_packets) * 100)
                        bar_length = 30
                        filled_length = int(bar_length * total_packets_sent // total_packets)
                        bar = '=' * filled_length + '-' * (bar_length - filled_length)
                        bytes_sent = min(file_size, total_packets_sent * 128)
                        
                        sys.stdout.write(f"\rProgress: [{bar}] {percent:5.1f}% ({bytes_sent}/{file_size} Bytes)")
                        sys.stdout.flush()

                    # Note: XMODEM over Bluetooth can occasionally benefit from slightly higher retry counts due to RF latency
                    modem = XMODEM(getc, putc, mode='xmodem', pad=b'\xFF')
                    
                    with open(bin_path, 'rb') as f:
                        success = modem.send(f, retry=32, callback=progress_callback)

                    print() # Newline after progress bar

                    if success:
                        print("\n>>> Wireless Firmware Upload Successful! <<<\n")
                    else:
                        print("\n>>> Wireless Firmware Upload Failed! <<<\n")

                    # Drain post-flash messages over Bluetooth for 1.5 seconds
                    end_time = time.time() + 1.5
                    while time.time() < end_time:
                        if ser.in_waiting:
                            post_data = ser.read(ser.in_waiting)
                            sys.stdout.write(post_data.decode('utf-8', errors='ignore').replace('\n', '\r\n'))
                            sys.stdout.flush()
                        time.sleep(0.05)

                    break  # Exit loop after flash attempt

                else:
                    # Standard UART text logging
                    sys.stdout.write(decoded_text.replace('\n', '\r\n'))
                    sys.stdout.flush()

            # Non-blocking read from Keyboard (Terminal Input -> Wireless MCU)
            if select.select([sys.stdin], [], [], 0)[0]:
                user_input = sys.stdin.read(1)
                ser.write(user_input.encode('utf-8'))

            # Check status and poll every 100ms
            time.sleep(0.1)

    except KeyboardInterrupt:
        print("\n--- Wireless Disconnected ---")
        ser.close()

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python3 term_flasher.py <app.bin> [port]")
        sys.exit(1)

    bin_file = sys.argv[1]
    serial_port = sys.argv[2] if len(sys.argv) >= 3 else DEFAULT_PORT
    
    import tty, termios
    fd = sys.stdin.fileno()
    old_settings = termios.tcgetattr(fd)
    try:
        tty.setcbreak(sys.stdin.fileno()) # Preserves standard \r\n formatting cleanly
        run_terminal_and_flash(serial_port, BAUD, bin_file)
    finally:
        termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
