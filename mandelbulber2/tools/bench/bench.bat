@echo off
Echo Mandelbulber2 Windoze Bench Bot

REM # Timestamp #
for /f "tokens=1-7 delims=:/-, " %%i in ('echo exit^|cmd /q /k"prompt $d $t"') do (
   for /f "tokens=2-4 delims=/-,() skip=1" %%a in ('echo.^|date') do (
      set dow=%%i
      set %%a=%%j
      set %%b=%%k
      set %%c=%%l
      set hh=%%m
      set min=%%n
      set ss=%%o
   )
)

@echo on
REM # Vars #
set _SCRIPT_DRIVE=%~d0
set _SCRIPT_FOLDER=%~dp0
set timestamp=%yy%%mm%%dd%%hh%%min%%ss%_%dow%
set SRC=%_SCRIPT_FOLDER%\..\..\..\
set IntDir=%SRC%\build-win\Bench\%timestamp%

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

REM # Clean IntDir #
if exist %IntDir%\ rd /s /q %IntDir%
mkdir %IntDir%
mkdir %IntDir%\ocl

REM # Bench #
%_SCRIPT_DRIVE%
cd %OutDir%
%bulb% --nogui --version
%bulb% --benchmark 1 -o %IntDir% --logfilepath %IntDir%\bulb_bench.txt
REM %bulb% -O 'opencl_enabled=1' --benchmark 1 -o %IntDir%\ocl

REM --- exit ----
cd %_SCRIPT_FOLDER%
GOTO:eof
