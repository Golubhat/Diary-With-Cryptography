@echo off

call "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat" -arch=x64

if errorlevel 1 (
    echo Failed to initialize MSVC environment.
    exit /b 1
)

if /I "%~x1"==".c" (
    cl /nologo /TC "%~1" /Fe:"%~dpn1.exe"
) else if /I "%~x1"==".cpp" (
    cl /nologo /EHsc /TP "%~1" /Fe:"%~dpn1.exe"
) else (
    echo Unsupported file type: %~x1
    exit /b 1
)