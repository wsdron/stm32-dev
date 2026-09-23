#!/usr/bin/env python3
import sys
import os
import time
import select
import serial
from xmodem import XMODEM

PORT = "/dev/ttyUSB0"  # Or /dev/rfcomm0 for Bluetooth
BAUD = 115200

def run_terminal_and_flash(port, baudrate, bin_path):
    ser = serial.Serial(port, baudrate, timeout=0.1)
    print(f"--- Connected to {port} at {baudrate} baud ---")
    print("--- [Press Ctrl+C to exit] ---\n")

    # Callbacks for python-xmodem module
    def getc(size, timeout=1):
        ser.timeout = timeout
        return ser.read(size) or None

    def putc(data, timeout=1):
        ser.timeout = timeout
        return ser.write(data)

    try:
        while True:
            # 1. Non-blocking read from Serial (MCU Output -> Screen)
            if ser.in_waiting > 0:
                raw_data = ser.read(ser.in_waiting)
                
                # Detect XMODEM ready signal ('C' / 0x43) from bootloader
                if b'C' in raw_data and bin_path and os.path.exists(bin_path):
                    sys.stdout.write(raw_data.decode('utf-8', errors='ignore'))
                    sys.stdout.flush()
                    
                    print("\n\n>>> XMODEM Start Signal 'C' Detected! Starting Flashing... <<<")
                    
                    # Calculate total packets (XMODEM transfers in 128-byte blocks)
                    file_size = os.path.getsize(bin_path)
                    total_packets = (file_size + 127) // 128

                    # Callback function invoked by modem.send() after each packet
                    def progress_callback(total_packets_sent, success_count, error_count):
                        percent = min(100.0, (total_packets_sent / total_packets) * 100)
                        bar_length = 30
                        filled_length = int(bar_length * total_packets_sent // total_packets)
                        bar = '=' * filled_length + '-' * (bar_length - filled_length)
                        
                        bytes_sent = min(file_size, total_packets_sent * 128)
                        
                        # Print progress bar inline using \r
                        sys.stdout.write(f"\rProgress: [{bar}] {percent:5.1f}% ({bytes_sent}/{file_size} Bytes)")
                        sys.stdout.flush()

                    modem = XMODEM(getc, putc, mode='xmodem', pad=b'\xFF')
                    
                    with open(bin_path, 'rb') as f:
                        success = modem.send(f, retry=16, callback=progress_callback)

                    print() # Newline after progress bar completes

                    if success:
                        print("\n>>> Firmware Upload Successful! <<<\n")
                    else:
                        print("\n>>> Firmware Upload Failed! <<<\n")

                    # Read and display serial output (e.g. "Jumping to application...") for 1 second
                    end_time = time.time() + 1.0
                    while time.time() < end_time:
                        if ser.in_waiting:
                            post_data = ser.read(ser.in_waiting)
                            sys.stdout.write(post_data.decode('utf-8', errors='ignore'))
                            sys.stdout.flush()
                        time.sleep(0.05)

                    break  # Exit loop

                else:
                    # Standard UART text logging (print flash status / debug text to terminal)
                    sys.stdout.write(raw_data.decode('utf-8', errors='ignore'))
                    sys.stdout.flush()

            # 2. Non-blocking read from Keyboard (Terminal Input -> MCU)
            if select.select([sys.stdin], [], [], 0)[0]:
                user_input = sys.stdin.read(1)
                ser.write(user_input.encode('utf-8'))

            time.sleep(0.01)

    except KeyboardInterrupt:
        print("\n--- Disconnected ---")
        ser.close()

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python3 term_flasher.py <app.bin> [port]")
        print("Example: python3 term_flasher.py firmware.bin /dev/ttyUSB0")
        sys.exit(1)

    bin_file = sys.argv[1]
    serial_port = sys.argv[2] if len(sys.argv) >= 3 else PORT
    
    # Configure stdin to read character-by-character without pressing Enter
    import tty, termios
    fd = sys.stdin.fileno()
    old_settings = termios.tcgetattr(fd)
    try:
        tty.setraw(sys.stdin.fileno())
        run_terminal_and_flash(serial_port, BAUD, bin_file)
    finally:
        termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
