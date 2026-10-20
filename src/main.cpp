#include <windows.h>
#include <commctrl.h>
#include <cmath>

int main() {
    // 1. Locate the desktop window handles
    HWND hwndProgman = FindWindowW(L"Progman", NULL);
    HWND hwndShell = FindWindowExW(hwndProgman, NULL, L"SHELLDLL_DefView", NULL);
    HWND hwndLV = FindWindowExW(hwndShell, NULL, L"SysListView32", NULL);

    if (!hwndLV) {
        HWND hwndWorkerW = FindWindowExW(NULL, NULL, L"WorkerW", NULL);
        while (hwndWorkerW && !hwndLV) {
            hwndShell = FindWindowExW(hwndWorkerW, NULL, L"SHELLDLL_DefView", NULL);
            hwndLV = FindWindowExW(hwndShell, NULL, L"SysListView32", NULL);
            hwndWorkerW = FindWindowExW(NULL, hwndWorkerW, L"WorkerW", NULL);
        }
    }

    if (!hwndLV) return 1;

    // 2. Get Explorer process ID to allow memory operations
    DWORD processId;
    GetWindowThreadProcessId(hwndLV, &processId);
    HANDLE hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, processId);
    if (!hProcess) return 1;

    // Get total desktop icon count
    int itemCount = SendMessage(hwndLV, LVM_GETITEMCOUNT, 0, 0);
    double angle = 0;

    // Animation Loop
    while (true) {
        POINT mousePos;
        GetCursorPos(&mousePos);

        // Terminate if user presses Escape key for safety
        if (GetAsyncKeyState(VK_ESCAPE)) break;

        for (int i = 0; i < itemCount; i++) {
            // Wave displacement physics
            int offsetX = static_cast<int>(sin(angle + i) * 70);
            int offsetY = static_cast<int>(cos(angle + i) * 70);

            int targetX = mousePos.x + offsetX;
            int targetY = mousePos.y + offsetY;

            // Send standard Win32 message to move the icon index
            SendMessage(hwndLV, LVM_SETITEMPOSITION, i, MAKELPARAM(targetX, targetY));
        }

        // 3. Force Desktop Redraw to prevent graphical glitching/ghosting
        InvalidateRect(hwndLV, NULL, TRUE);

        angle += 0.15;
        Sleep(16); // ~60 FPS
    }

    CloseHandle(hProcess);
    return 0;
}
