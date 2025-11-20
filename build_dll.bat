@echo off
REM Build script for audiobackend.dll (64-bit)
REM This script must be run from x64 Native Tools Command Prompt for VS

echo Building audiobackend.dll...

REM Create output directory if it doesn't exist
if not exist "bin" mkdir bin

REM Compile and link the DLL
cl /LD /EHsc /O2 /W4 /D_USRDLL /D_WINDLL ^
   src\main.cpp ^
   /Fe:bin\audiobackend.dll ^
   /Fo:bin\ ^
   /link /DLL /MACHINE:X64

if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    exit /b 1
)

echo Build successful! DLL created at bin\audiobackend.dll
