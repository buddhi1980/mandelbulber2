REM @echo off

set _SCRIPT_DRIVE=%~d0
set _SCRIPT_FOLDER=%~dp0

ECHO Change Directory
%_SCRIPT_DRIVE%
cd %_SCRIPT_FOLDER%
set SRC=%CD%

Echo Mandelbulber2 Windoze Trial Bot

python %SRC%\trial-run-host.py
