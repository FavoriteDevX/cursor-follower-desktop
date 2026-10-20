## Wavy Snake Desktop Icons 🐍

<img width="501" height="570" alt="image" src="https://github.com/user-attachments/assets/871ae9f0-fc85-45a8-b354-fcc8246722e3" />

A lightweight, zero-dependency Windows automation project that transforms all your real desktop icons into a fluid, snake-like trailing animation that follows your mouse cursor with smooth, undulating slither movements.

No Python, Node.js/npm, or Git installations are required for the end-user. It runs natively using pure Windows sub-systems.

---

## ✨ Features

- **Real App Extraction:** Automatically reads both your personal desktop folder and the Windows Public Desktop folder to compile and extract icons for all your actual apps (e.g., Chrome, Roblox, Recycle Bin, Word, folders, etc.).
- **Anti-Shake Logic:** Icons remain completely still, firm, and frozen when the mouse is stationary. The slithering wave animation only advances dynamically as you move the cursor across the screen.
- **Ultra-Smooth Trail:** Uses Linear Interpolation (Lerp) vector mathematics and high-frequency update loops (~100Hz) to deliver a seamless cascading snake effect without graphical ghosting.
- **Panic Exit:** Instantly restores your desktop back to 100% normal the millisecond you press the `ESC` key.

---

## 🚀 How to Run (Direct Execution via Terminal)

Since this script interacts with native UI rendering boundaries blocked by recent Windows security patches on traditional desktop handles, running it inside a high-performance shell like PowerShell is the most reliable method.

1. Minimize all windows so you can see your desktop wallpaper.
2. Click your Windows Start Menu, search for **PowerShell**, and launch it.
3. Copy the complete script block below:

```powershell
$code = @"
using System;
using System.IO;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;
using System.Runtime.InteropServices;

public class PerfectMovingSnake {
    [DllImport("user32.dll")] public static extern IntPtr FindWindow(string lpClassName, string lpWindowName);
    [DllImport("user32.dll")] public static extern IntPtr FindWindowEx(IntPtr hwndParent, IntPtr hwndChildAfter, string lpszClass, string lpszWindowName);
    [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);
    [DllImport("user32.dll")] public static extern short GetAsyncKeyState(int vKey);

    private const int SW_HIDE = 0;
    private const int SW_SHOW = 5;

    public static void Launch() {
        IntPtr progman = FindWindow("Progman", null);
        IntPtr shellView = FindWindowEx(progman, IntPtr.Zero, "SHELLDLL_DefView", null);
        IntPtr listView = FindWindowEx(shellView, IntPtr.Zero, "SysListView32", null);
        if (listView == IntPtr.Zero) {
            IntPtr workerW = IntPtr.Zero;
            while ((workerW = FindWindowEx(IntPtr.Zero, workerW, "WorkerW", null)) != IntPtr.Zero) {
                shellView = FindWindowEx(workerW, IntPtr.Zero, "SHELLDLL_DefView", null);
                listView = FindWindowEx(shellView, IntPtr.Zero, "SysListView32", null);
                if (listView != IntPtr.Zero) break;
            }
        }
        if (listView != IntPtr.Zero) ShowWindow(listView, SW_HIDE);

        Form overlay = new Form();
        overlay.FormBorderStyle = FormBorderStyle.None;
        overlay.ShowInTaskbar = false;
        overlay.TopMost = true;
        overlay.WindowState = FormWindowState.Maximized;
        overlay.BackColor = Color.Lime;
        overlay.TransparencyKey = Color.Lime;

        List<string> allFiles = new List<string>();
        string userDesktop = Environment.GetFolderPath(Environment.SpecialFolder.Desktop);
        if (Directory.Exists(userDesktop)) allFiles.AddRange(Directory.GetFiles(userDesktop));
        string publicDesktop = Path.Combine(Environment.GetEnvironmentVariable("PUBLIC"), "Desktop");
        if (Directory.Exists(publicDesktop)) allFiles.AddRange(Directory.GetFiles(publicDesktop));
        
        List<Panel> appSegments = new List<Panel>();
        List<PointF> positions = new List<PointF>();
        HashSet<string> processedNames = new HashSet<string>(StringComparer.OrdinalIgnoreCase);

        foreach (string file in allFiles) {
            string name = Path.GetFileNameWithoutExtension(file);
            if (string.IsNullOrEmpty(name) || name.Equals("desktop", StringComparison.OrdinalIgnoreCase)) continue;
            if (processedNames.Contains(name)) continue;
            
            try {
                Icon icon = Icon.ExtractAssociatedIcon(file);
                processedNames.Add(name);
                if (name.Length > 11) name = name.Substring(0, 9) + "...";

                Panel panel = new Panel();
                panel.Width = 85; panel.Height = 85;
                panel.BackColor = Color.Transparent;

                PictureBox pb = new PictureBox { Image = icon.ToBitmap(), Width = 32, Height = 32, SizeMode = PictureBoxSizeMode.StretchImage, Left = 26, Top = 10 };
                Label lbl = new Label { Text = name, ForeColor = Color.White, Font = new Font("Segoe UI", 8), TextAlign = ContentAlignment.MiddleCenter, Width = 85, Height = 30, Top = 45 };

                panel.Controls.Add(pb); panel.Controls.Add(lbl);
                overlay.Controls.Add(panel);
                appSegments.Add(panel);
                positions.Add(new PointF(Cursor.Position.X, Cursor.Position.Y));
            } catch { continue; }
        }

        overlay.Show();

        Timer timer = new Timer();
        timer.Interval = 10; 
        
        double waveAngle = 0;
        float speed = 0.35f; 
        Point lastMousePos = Cursor.Position;

        timer.Tick += (s, e) => {
            if (GetAsyncKeyState(0x1B) != 0) { 
                timer.Stop(); overlay.Close();
                if (listView != IntPtr.Zero) ShowWindow(listView, SW_SHOW);
                return;
            }

            Point mouse = Cursor.Position;
            float deltaX = mouse.X - lastMousePos.X;
            float deltaY = mouse.Y - lastMousePos.Y;
            float distanceMoved = (float)Math.Sqrt(deltaX * deltaX + deltaY * deltaY);

            if (distanceMoved > 1.0f) {
                waveAngle += (distanceMoved * 0.04); 
            }

            for (int i = 0; i < appSegments.Count; i++) {
                PointF current = positions[i];
                PointF target = (i == 0) ? new PointF(mouse.X, mouse.Y) : positions[i - 1];

                float nx = current.X + (target.X - current.X) * speed;
                float ny = current.Y + (target.Y - current.Y) * speed;
                positions[i] = new PointF(nx, ny);

                int wave = 0;
                if (distanceMoved > 0.5f || (i > 0 && Math.Abs(positions[i].X - positions[i-1].X) > 2)) {
                    wave = (int)(Math.Sin(waveAngle + (i * 0.40)) * 25);
                }

                appSegments[i].Left = (int)nx + wave - (appSegments[i].Width / 2);
                appSegments[i].Top = (int)ny - (appSegments[i].Height / 2);
            }

            lastMousePos = mouse;
        };

        timer.Start();
        Application.Run(overlay);
    }
}
"@

Add-Type -TypeDefinition $code -ReferencedAssemblies "System.Windows.Forms", "System.Drawing" -ErrorAction SilentlyContinue
[PerfectMovingSnake]::Launch()
```

4. Paste it inside the PowerShell terminal window and press **Enter**.

<img width="1143" height="646" alt="image" src="https://github.com/user-attachments/assets/624f376f-67be-410b-9671-1b0c6ba5d911" />

5. Move your mouse cursor around the screen to see the snake slither. 

<img width="1129" height="330" alt="Screenshot 2026-10-20 175037" src="https://github.com/user-attachments/assets/35dafe98-a9cb-4d4e-8dfe-ca0e85ba394d" />

---

## 🛑 How to Stop & Restore

- Click back onto the PowerShell window and hit the **`ESC` (Escape)** key on your physical keyboard. 
- The animation will immediately terminate, the script will safely unpack, and your default Windows desktop interface will automatically reveal itself exactly where you left it.

## 🛠️ How it Works under the Hood

1. **System Abstraction:** To bypass heavy Windows OS position rendering locks, the script invokes Win32 API calls (`FindWindowEx`) to securely toggle the visibility state of the system's stock background listview layer (`SysListView32`).
2. **Transparent Layering:** A completely frameless, top-most, transparent Windows Form canvas overlay is mounted over the screen bounds using a hardware-accelerated chroma key.
3. **Cascading Linear Interpolation (Lerp):** Position tracking coordinates are evaluated sequentially; segment `[i]` uses delta math calculations relative to segment `[i-1]`, tethering the lead head directly to the physical system hardware mouse cursor.
