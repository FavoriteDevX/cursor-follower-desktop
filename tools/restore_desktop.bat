@echo off
echo Resetting desktop layout...
taskkill /f /im explorer.exe
start explorer.exe
echo Done! Your desktop icons should be back to their default alignment.
pause
