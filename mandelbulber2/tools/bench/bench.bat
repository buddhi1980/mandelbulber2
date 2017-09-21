REM @echo off
@echo on
Echo Mandelbulber2 Windoze Bench Bot

REM # Vars #
set _SCRIPT_DRIVE=%~d0
set _SCRIPT_FOLDER=%~dp0
set SRC=%_SCRIPT_FOLDER%\..\..\..\
set PATH=%SRC%;%PATH%
set OutDir=%SRC%\build-win\Release\x64\

REM # Bench #



REM --- exit ----
cd %_SCRIPT_FOLDER%
GOTO:eof
