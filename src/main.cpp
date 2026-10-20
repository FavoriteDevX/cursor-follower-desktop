#include <windows.h>
#include <commctrl.h>
#include <cmath>

int main() {
    // 1. Locate the window managing the desktop icons
    HWND hwndProgman = FindWindowW(L"Progman", NULL);
    HWND hwndShell = FindWindowExW(hwndProgman, NULL, L"SHELLDLL_DefView", NULL);
    HWND hwndLV = FindWindowExW(hwndShell, NULL, L"SysListView32", NULL);

    if (!hwndLV) {
        // Fallback for cases where Windows uses the WorkerW structure instead
        HWND hwndWorkerW = FindWindowExW(NULL, NULL, L"WorkerW", NULL);
        while (hwndWorkerW && !hwndLV) {
            hwndShell = FindWindowExW(hwndWorkerW, NULL, L"SHELLDLL_DefView", NULL);
            hwndLV = FindWindowExW(hwndShell, NULL, L"SysListView32", NULL);
            hwndWorkerW = FindWindowExW(NULL, hwndWorkerW, L"WorkerW", NULL);
        }
    }

    // If the desktop window listview isn't found, exit
    if (!hwndLV) return 1; 

    int itemCount = ListView_GetItemCount(hwndLV);
    double angle = 0;

    // Animation Loop
    while (true) {
        POINT mousePos;
        GetCursorPos(&mousePos); // Get current mouse cursor coordinates

        for (int i = 0; i < itemCount; i++) {
            // Mathematical wave calculation (sine and cosine displacement)
            int offsetX = static_cast<int>(sin(angle + i) * 60);
            int offsetY = static_cast<int>(cos(angle + i) * 60);

            // Update the icon position relative to the cursor
            ListView_SetItemPosition(hwndLV, i, mousePos.x + offsetX, mousePos.y + offsetY);
        }

        angle += 0.15; // Animation speed
        Sleep(16);     // Cap at roughly ~60 Frames Per Second
    }

    return 0;
}
