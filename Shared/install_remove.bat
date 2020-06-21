@echo off

REM ------------------------------ UAC Boilerplate -----------------------------

:init
setlocal DisableDelayedExpansion
set "batchPath=%~0"
for %%k in (%0) do set batchName=%%~nk
set "vbsGetPrivileges=%temp%\OEgetPriv_%batchName%.vbs"
setlocal EnableDelayedExpansion

:checkPrivileges
NET FILE 1>NUL 2>NUL
if '%errorlevel%' == '0' ( goto gotPrivileges ) else ( goto getPrivileges )

:getPrivileges
if '%1'=='ELEV' (echo ELEV & shift /1 & goto gotPrivileges)
ECHO.
ECHO **************************************
ECHO Requesting privilege escalation
ECHO **************************************

ECHO Set UAC = CreateObject^("Shell.Application"^) > "%vbsGetPrivileges%"
ECHO args = "ELEV " >> "%vbsGetPrivileges%"
ECHO For Each strArg in WScript.Arguments >> "%vbsGetPrivileges%"
ECHO args = args ^& strArg ^& " "  >> "%vbsGetPrivileges%"
ECHO Next >> "%vbsGetPrivileges%"
ECHO UAC.ShellExecute "!batchPath!", args, "", "runas", 1 >> "%vbsGetPrivileges%"
"%SystemRoot%\System32\WScript.exe" "%vbsGetPrivileges%" %*
exit /B

:gotPrivileges
setlocal & pushd .
cd /d %~dp0
if '%1'=='ELEV' (del "%vbsGetPrivileges%" 1>nul 2>nul  &  shift /1)

REM -----------------------------------------------------------

reg query "HKLM\Hardware\Description\System\CentralProcessor\0" | find /i "x86" > NUL && set OS=x86 || set OS=x64
reg query "HKCR\CLSID" | find /i "{63424DF1-FAA8-4598-97E5-6E95D4A4ED67}" > NUL && goto REMOVE || goto INSTALL

:INSTALL
echo Installing Thumbnail Handler (%OS%)
regsvr32 Pso2SarThumbnailHandler.%OS%.dll

choice /c yn /t 30 /d n /m "Enable High Resolution Rendering?"
if %errorlevel% == 1 (
   reg add "HKCR\CLSID\{63424DF1-FAA8-4598-97E5-6E95D4A4ED67}" /v "HighDefinition" /t REG_DWORD /d 1 /f
) else (
   reg add "HKCR\CLSID\{63424DF1-FAA8-4598-97E5-6E95D4A4ED67}" /v "HighDefinition" /t REG_DWORD /d 0 /f
)


goto END
:REMOVE
echo Removing Thumbnail Handler (%OS%)
regsvr32 /u Pso2SarThumbnailHandler.%OS%.dll
goto END
:END