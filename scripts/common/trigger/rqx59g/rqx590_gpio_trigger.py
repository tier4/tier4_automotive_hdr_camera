#!/usr/bin/env python3

import subprocess
import time
from threading import Timer
import signal
import sys
import argparse
import os
import psutil  # for CPU affinity
import ctypes  # for real-time scheduling
import ctypes.util

# Real-time scheduling parameters
SCHED_FIFO = 1
SCHED_RR = 2
SCHED_OTHER = 0

# Memory locking flags
MCL_CURRENT = 1
MCL_FUTURE = 2

class GPIOTrigger:
    def __init__(self, frequency=10, chip_name="gpiochip0", line_numbers=[92, 49, 139, 138], cpu_core=3):
        self.chip_name = chip_name
        self.line_numbers = line_numbers
        self.running = True
        self.frequency = frequency
        self.period = 1.0 / frequency
        self.cpu_core = cpu_core
        
        # Create gpioset command strings
        self.set_high_cmd = f"gpioset {self.chip_name} " + " ".join(f"{pin}=1" for pin in self.line_numbers)
        self.set_low_cmd = f"gpioset {self.chip_name} " + " ".join(f"{pin}=0" for pin in self.line_numbers)
        
        # Set up signal handlers
        signal.signal(signal.SIGINT, self.signal_handler)
        signal.signal(signal.SIGTERM, self.signal_handler)

    def lock_memory(self):
        """Lock process memory to prevent swapping"""
        try:
            libc = ctypes.CDLL(ctypes.util.find_library('c'), use_errno=True)
            result = libc.mlockall(MCL_CURRENT | MCL_FUTURE)
            
            if result != 0:
                error = ctypes.get_errno()
                raise OSError(error, f"mlockall failed: {os.strerror(error)}")
            print("Successfully locked memory")
            
        except Exception as e:
            print(f"Warning: Could not lock memory: {str(e)}")

    def set_realtime_priority(self):
        try:
            # Load the POSIX library
            libc = ctypes.CDLL('libc.so.6')
            
            # Define structure for scheduling parameters
            class sched_param(ctypes.Structure):
                _fields_ = [('sched_priority', ctypes.c_int)]
            
            # Set real-time scheduling policy (SCHED_FIFO)
            param = sched_param(99)  # Maximum real-time priority
            result = libc.sched_setscheduler(0, SCHED_FIFO, ctypes.byref(param))
            
            if result == 0:
                print("Successfully set real-time scheduling policy")
            else:
                print("Failed to set real-time scheduling policy. Are you running with sudo?")
                
        except Exception as e:
            print(f"Warning: Could not set real-time scheduling: {str(e)}")

    def set_cpu_affinity(self):
        try:
            # Get the process
            process = psutil.Process()
            
            # Set CPU affinity to specified core
            process.cpu_affinity([self.cpu_core])
            print(f"Successfully set CPU affinity to core {self.cpu_core}")
            
        except Exception as e:
            print(f"Warning: Could not set CPU affinity: {str(e)}")

    def setup_system_config(self):
        try:
            # Check if running as root
            if os.geteuid() != 0:
                raise PermissionError("This program must be run as root (sudo)")

            # Set real-time group (if not already in group)
            subprocess.run(["groupadd", "-f", "realtime"], check=True)
            subprocess.run(["usermod", "-a", "-G", "realtime", os.getenv("SUDO_USER", "root")], check=True)
            
            # Set system limits for real-time
            limits_file = "/etc/security/limits.d/99-realtime.conf"
            limits_content = """
@realtime soft rtprio 99
@realtime hard rtprio 99
@realtime soft memlock unlimited
@realtime hard memlock unlimited
"""
            try:
                with open(limits_file, "w") as f:
                    f.write(limits_content)
                print(f"Created {limits_file} with real-time limits")
            except Exception as e:
                print(f"Warning: Could not create limits file: {str(e)}")
                print("You may need to set up limits manually")
            
            print("Real-time system configuration set up successfully")
            
        except Exception as e:
            print(f"Warning: Could not set up real-time system configuration: {str(e)}")

    def check_gpio_tools(self):
        try:
            subprocess.run(["which", "gpioset"], check=True, capture_output=True)
        except subprocess.CalledProcessError:
            print("Error: gpioset command not found. Please install gpiod tools:")
            print("sudo apt-get install gpiod")
            sys.exit(1)

    def setup_gpio(self):
        try:
            # Check GPIO tools
            self.check_gpio_tools()

            # Configure system for real-time operation
            self.setup_system_config()
            
            # Set CPU affinity
            self.set_cpu_affinity()
            
            # Set real-time priority
            self.set_realtime_priority()
            
            # Lock memory
            self.lock_memory()

            # Test GPIO access
            subprocess.run(self.set_low_cmd, shell=True, check=True)
            print("GPIO setup successful")
            
        except subprocess.CalledProcessError as e:
            print(f"GPIO setup error: Command failed with return code {e.returncode}")
            print("Command output:", e.output if hasattr(e, 'output') else "No output")
            sys.exit(1)
        except Exception as e:
            print(f"GPIO setup error: {str(e)}")
            sys.exit(1)

    def trigger_pulse(self):
        if not self.running:
            return

        try:
            start_time = time.perf_counter()
            
            # Set pins high
            subprocess.run(self.set_high_cmd, shell=True, check=True)
            
            # Precise sleep for 5 milliseconds
            target_time = start_time + 0.005
            while time.perf_counter() < target_time:
                pass
            
            # Set pins low
            subprocess.run(self.set_low_cmd, shell=True, check=True)
            
            # Calculate next trigger time and remaining sleep
            end_time = time.perf_counter()
            elapsed = end_time - start_time
            sleep_time = max(0, self.period - elapsed)
            
            # Schedule next trigger
            if self.running:
                Timer(sleep_time, self.trigger_pulse).start()
                
        except subprocess.CalledProcessError as e:
            print(f"Error during trigger: Command failed with return code {e.returncode}")
            self.cleanup()
        except Exception as e:
            print(f"Error during trigger: {str(e)}")
            self.cleanup()

    def signal_handler(self, signum, frame):
        print("\nReceived termination signal, cleaning up...")
        self.cleanup()

    def cleanup(self):
        self.running = False
        try:
            # Ensure all pins are set to low
            subprocess.run(self.set_low_cmd, shell=True, check=True)
        except:
            pass
        sys.exit(0)

    def run(self):
        print(f"Starting GPIO trigger... (Frequency: {self.frequency} Hz)")
        print(f"Using GPIO lines: {self.line_numbers}")
        self.setup_gpio()
        self.trigger_pulse()

def parse_arguments():
    parser = argparse.ArgumentParser(description='Real-time GPIO Trigger Program')
    parser.add_argument('-f', '--frequency', 
                       type=float, 
                       default=10.0,
                       help='Trigger frequency in Hz (default: 10 Hz)')
    parser.add_argument('--chip', 
                       type=str,
                       default="gpiochip0",
                       help='GPIO chip name (default: gpiochip0)')
    parser.add_argument('--cpu-core',
                       type=int,
                       default=3,
                       help='CPU core to run on (default: 3)')
    parser.add_argument('--min-freq', 
                       type=float,
                       default=1,
                       help='Minimum allowed frequency in Hz (default: 1 Hz)')
    parser.add_argument('--max-freq', 
                       type=float,
                       default=30,
                       help='Maximum allowed frequency in Hz (default: 30 Hz)')
    return parser.parse_args()

if __name__ == "__main__":
    args = parse_arguments()
    
    # Check if frequency is within valid range
    if args.frequency < args.min_freq or args.frequency > args.max_freq:
        print(f"Error: Frequency must be between {args.min_freq} Hz and {args.max_freq} Hz")
        sys.exit(1)
        
    trigger = GPIOTrigger(frequency=args.frequency, chip_name=args.chip, cpu_core=args.cpu_core)
    trigger.run()
