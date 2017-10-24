@echo off
Echo Mandelbulber2 Windoze Bench Bot

@echo on
REM # Vars #
set _SCRIPT_DRIVE=%~d0
set _SCRIPT_FOLDER=%~dp0
set SRC=%_SCRIPT_FOLDER%\..\..\..\
set IntDir=%SRC%\build-win\Bench

REM # Support build configs #
set ChkDir=%SRC%\build-win\AnalysisRelease\Win32
if exist %ChkDir%\mandelbulber2.exe set OutDir=%ChkDir%
set ChkDir=%SRC%\build-win\Debug\Win32
if exist %ChkDir%\mandelbulber2.exe set OutDir=%ChkDir%
set ChkDir=%SRC%\build-win\Release\Win32
if exist %ChkDir%\mandelbulber2.exe set OutDir=%ChkDir%
set ChkDir=%SRC%\build-win\AnalysisRelease\x64
if exist %ChkDir%\mandelbulber2.exe set OutDir=%ChkDir%
set ChkDir=%SRC%\build-win\Debug\x64
if exist %ChkDir%\mandelbulber2.exe set OutDir=%ChkDir%
set ChkDir=%SRC%\build-win\Release\x64
if exist %ChkDir%\mandelbulber2.exe set OutDir=%ChkDir%
set bulb=%OutDir%\mandelbulber2.exe

REM # Make Bench Directory #
if not exist %IntDir% mkdir %IntDir%

REM # Bench #
%_SCRIPT_DRIVE%
cd %OutDir%
%bulb% --nogui --version
%bulb% --benchmark 200 -o %IntDir%

REM --- exit ----
cd %_SCRIPT_FOLDER%
GOTO:eof
