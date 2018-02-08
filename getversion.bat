@echo off
setlocal

set GIT_DESCRIBE=git describe --tags
set VERSION_H=..\config\fw_version.h

for /F "usebackq" %%i in (`%GIT_DESCRIBE%`) do (
  set DESCRIBE=%%i
)

echo #define FW_VERSION_STRING "%DESCRIBE%" > %VERSION_H%

exit /b 0