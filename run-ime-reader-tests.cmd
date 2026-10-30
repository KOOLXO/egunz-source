@echo off
where cl >nul 2>nul
if errorlevel 1 (
  echo Run from an x86 Visual Studio Developer Command Prompt.
  exit /b 1
)
cl /nologo /EHsc /std:c++14 /W4 /WX /Od /RTC1 ime-reader-tests.cpp /Fe:ime-reader-tests.exe /Fo:ime-reader-tests.obj
if errorlevel 1 exit /b 1
ime-reader-tests.exe
