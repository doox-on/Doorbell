# Doorbell MVP (Raspberry Pi + Linux Driver + Userspace Audio)
Author: Jungwoo Moon
2025-9-4 released 

This project demonstrates a **Minimum Viable Product (MVP)** for a doorbell system on Raspberry Pi:
- A **Linux kernel module** (misc device + GPIO interrupt).
- A **userspace Python watcher** that polls `/dev/doorBell`.
- When the button is pressed, a **WAV sound file is played** via an I²S amplifier (MAX98357A).

---

In the scripts folder, run 
- chmod +x run_all.sh
- sudo ./run_all.sh
