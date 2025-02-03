@ECHO OFF

PUSHD %~dp0

cmake -G "Visual Studio 17 2022" -A x64 -Wno-dev "%~dp0"

IF %ERRORLEVEL% NEQ 0 (
    PAUSE
) ELSE (
    START Gallus.sln
)

POPD

:Exit

POPD
