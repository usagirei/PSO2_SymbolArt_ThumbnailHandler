@echo off
reg query "HKLM\Hardware\Description\System\CentralProcessor\0" | find /i "x86" > NUL && set OS=x86 || set OS=x64
reg query "HKCR\CLSID" | find /i "{63424DF1-FAA8-4598-97E5-6E95D4A4ED67}" > NUL && goto REMOVE || goto INSTALL

:INSTALL
echo Installing Thumbnail Handler (%OS%)
regsvr32 Pso2SarThumbnailHandler.%OS%.dll
goto END
:REMOVE
echo Installing Thumbnail Handler (%OS%)
regsvr32 /u Pso2SarThumbnailHandler.%OS%.dll
goto END
:END