#!/usr/bin/env python3
import os, subprocess, time

DEV  = "/dev/doorBell"
WAV  = "/home/jw/doorbell-mvp/userspace/sample.wav"
ALSA = "plughw:0,0"
MIN_INTERVAL = 0.30  #debouncing

fd = os.open(DEV, os.O_RDONLY)  # Blocking method
last = 0.0

print(f"blocking on {DEV} -> {WAV}")
while True:
    # sleep until the kernel's my_read() is active
    n = os.read(fd, 1)  # consume 1 event
    
    now = time.monotonic()  #debouncing 0.3s
    if now - last >= MIN_INTERVAL:
        last = now
        subprocess.Popen(["aplay", "-q", "-D", ALSA, WAV]) #play WAV file