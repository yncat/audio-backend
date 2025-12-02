@echo off
REM Build script for audiobackend_examples.exe (64-bit)
REM This script must be run from x64 Native Tools Command Prompt for VS

echo Building audiobackend_examples.exe...

REM Create output directory if it doesn't exist
if not exist "bin" mkdir bin

REM Compile and link the executable
cl /EHsc /O2 /W4 ^
   examples\main.cpp ^
   examples\helper.cpp ^
   examples\test_core_init_free.cpp ^
   examples\test_bgm_functions.cpp ^
   examples\test_loop_point.cpp ^
   examples\test_sample_oneshot.cpp ^
   examples\test_vr_initialization.cpp ^
   examples\test_plugin_inspector.cpp ^
   examples\test_3d_oneshot.cpp ^
   examples\test_vr_player_position.cpp ^
   examples\test_vr_room_effects.cpp ^
   /Fe:bin\audiobackend_examples.exe ^
   /Fo:bin\ ^
   /link /MACHINE:X64

if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    exit /b 1
)

echo Build successful! Executable created at bin\audiobackend_examples.exe
