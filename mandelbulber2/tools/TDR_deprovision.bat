@echo off
Echo Mandelbulber2 Windoze Tools
echo Toggle Timeout Detection and Recovery (TDR) of Windows Display Driver Model (WDDM) 
REM https://docs.microsoft.com/en-us/windows-hardware/drivers/display/tdr-registry-keys

REM KeyPath   : HKEY_LOCAL_MACHINE\System\CurrentControlSet\Control\GraphicsDrivers
REM KeyValue  : TdrLevel
REM ValueType : REG_DWORD
REM ValueData : TdrLevelOff (0) - Detection disabled - This is the requested value.
REM ValueData : TdrLevelRecover (3) - Recover on timeout. This is the default value.


REM KeyPath   : HKEY_LOCAL_MACHINE\System\CurrentControlSet\Control\GraphicsDrivers
REM KeyValue  : TdrDelay
REM ValueType : REG_DWORD
REM ValueData : Number of seconds to delay. 2 seconds is the default value.


:checkadmin
net session >nul 2>&1
if %errorLevel% == 0 (
	echo Admin confirmed.
) else (
	goto notadmin
)

REM # Vars #
set _SCRIPT_DRIVE=%~d0
set _SCRIPT_FOLDER=%~dp0
set SRC=%_SCRIPT_FOLDER%\..\..\

:setTDR
Echo Modify registry to Enable TDR
echo [HKEY_LOCAL_MACHINE\System\CurrentControlSet\Control\GraphicsDrivers]
echo "TdrLevel"=dword:00000003
reg add "HKLM\System\CurrentControlSet\Control\GraphicsDrivers" /v TdrLevel /t REG_DWORD /d 3

Echo Modify registry to alter delay to 120 seconds
echo [HKEY_LOCAL_MACHINE\System\CurrentControlSet\Control\TdrDelay]
echo "TdrDelay"=dword:00000120
reg add "HKLM\System\CurrentControlSet\Control\GraphicsDrivers" /v TdrDelay /t REG_DWORD /d 120

REM --- exit ----
cd %_SCRIPT_FOLDER%
Echo Reboot Required
pause
GOTO:eof

:notadmin
cd %_SCRIPT_FOLDER%
echo Please run as admin, and retry
GOTO:eof
