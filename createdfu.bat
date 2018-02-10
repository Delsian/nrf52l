#@echo off
setlocal

set GIT_DESCRIBE="git describe --tags --abbrev=0"
set DFU_ZIP=dfu.zip
set KEY_PEM=..\config\private.pem

for /f %%i in ('%GIT_DESCRIBE%') do set DESCRIBE=%%i
nrfutil pkg generate --hw-version 52 --application Debug/nrf52l.hex --key-file %KEY_PEM% --sd-req 0x9D --application-version-string %DESCRIBE% %DFU_ZIP%

exit /b 0
