#!/bin/bash
set -e

DRV=/home/jw/doorbell-mvp/drivers/button
USR=/home/jw/doorbell-mvp/userspace
MOD=rpi_button
DEV=/dev/doorBell

cd "$DRV"
make

# 0) if the file is running, kill.
pkill -f "button_watcher.py" 2>/dev/null || true
sleep 0.2

# 1) Module unload just in case 
sudo rmmod "$MOD" 2>/dev/null || true

# 2) New module upload
sudo insmod ./rpi_button.ko
dmesg | tail -n 10

# 3) give permission to read and write
sudo chmod 666 "$DEV" || true

# 4) run python
exec python3 "$USR/button_watcher.py"