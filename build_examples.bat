@echo off
REM Build script for audiobackend_examples.exe (64-bit)
REM This script must be run from x64 Native Tools Command Prompt for VS

echo Building audiobackend_examples.exe...

REM Create output directory if it doesn't exist
if not exist "bin" mkdir bin

REM Compile and link the executable
cl /EHsc /O2 /W4 ^
   examples\main.cpp ^
   /Fe:bin\audiobackend_examples.exe ^
   /Fo:bin\ ^
   /link /MACHINE:X64

if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    exit /b 1
)

echo Build successful! Executable created at bin\audiobackend_examples.exe
