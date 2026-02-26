## Krypton Bootkit 🔒💀

![Krypton](https://img.shields.io/badge/Krypton-Bootkit-red?style=for-the-badge&logo=github&logoColor=white)
[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Windows](https://img.shields.io/badge/Target-Windows%20x64-green.svg)](https://github.com/topics/windows)
[![Pentest](https://img.shields.io/badge/Purpose-Pentesting-orange.svg)](https://owasp.org/www-project-top-ten/)

**Advanced MBR Bootkit for Authorized Penetration Testing**  
*Demonstrates persistence, BSoD triggers, and auto-restart capabilities*

## 🚨 Authorization Notice
This tool is developed for authorized penetration testing only. The author has explicit written permission to test target systems. Use on unauthorized systems is illegal under CFAA 18 U.S.C. § 1030.





## Features

| Capability | Status | Description |
|------------|--------|-------------|
| **MBR Infection** | ✅ | Overwrites Master Boot Record with custom payload |
| **BIOS Persistence** | ✅ | Survives OS reinstalls, displays "HACKED BY JASON" |
| **Auto-Restart** | ✅ | Forces reboot 3s after infection |
| **BSoD Trigger** | ✅ | Instant Blue Screen of Death |
| **Admin Detection** | ✅ | Validates Administrator privileges |
| **Static Linking** | ✅ | 15KB standalone EXE, no dependencies |

## Attack Chain
krypton.exe (Admin) → MBR Overwritten
[Optional] → BSoD Trigger
3s Delay → Forced Restart
BIOS → "HACKED BY JASON!!" (Unbootable)




## Quick Deploy (Kali Linux)

# 1. Cross-compile
x86_64-w64-mingw32-g++ -static -O2 krypton.cpp -o krypton.exe

# 2. Transfer & Execute (Windows Target)
# krypton.exe --bsod  # Full chain
# krypton.exe         # MBR + Auto-restart
Usage
powershell



# Standard deployment (MBR + Auto-restart)
krypton.exe

# Maximum disruption
krypton.exe --bsod

# MBR only (manual reboot)
krypton.exe --no-restart
Technical Breakdown
MBR Payload (x86 Real Mode)
asm



; 512-byte boot sector
SET VIDEO MODE 13h (320x200)
PRINT "HACKED BY JASON!!" (INT 10h/AH=0Eh)
INFINITE LOOP
SIGNATURE: 55AA
Defensive Evasion
BitLocker: Auto-detects & warns
AV: Static analysis evades most signatures
Size: 15KB (highly portable)
Recovery Procedures
cmd



# Windows Recovery Environment (Boot USB)
bootrec /fixmbr
bootrec /fixboot
bootrec /rebuildbcd
chkdsk C: /f /r
Build Matrix


Platform	Command	Output
Kali Linux	make	krypton.exe
Windows	cl krypton.cpp	krypton.exe
WSL	x86_64-w64-mingw32-g++	krypton.exe
Prerequisites
bash



# Kali/WSL
sudo apt install mingw-w64

# Windows
Visual Studio (C++ Build Tools)


# Pentest Methodology
Recon: Verify admin access, BitLocker status
Deploy: krypton.exe via SMB/USB/PSExec
Observe: MBR overwrite + restart
Impact: BIOS persistence demonstrated
Report: Persistence bypass capability
Cleanup: bootrec /fixmbr

## Legal & Ethical Use
✅ Authorized pentesting
✅ Red team exercises
✅ Security research
❌ Unauthorized systems
❌ Malicious deployment
