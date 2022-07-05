:: echo PROJ_DIR %1

::;*****************************************************************************#
::;                      Select FreeRTOS version                                #
::;*****************************************************************************#
SET VERSION=10.2.0

set tooldir=%1\..\..\..\component\soc\realtek\8195b\misc\iar_utility
set link_path=%1\..\..\..\component\os\freertos\freertos
set src_path=%1\..\..\..\component\os\freertos\freertos_v%VERSION%

if [%1]==[] (
    call :local_exec
    echo PROJ_DIR is empty
) else (
    call :prebuild_exec
    echo PROJ_DIR is not empty
)
goto :end

:prebuild_exec
@echo off
SETLOCAL ENABLEDELAYEDEXPANSION

echo %src_path%
echo %link_path%
echo %tooldir%\junction
if not exist %link_path% %tooldir%\junction %link_path% %src_path%
EXIT /b

:local_exec
@echo off
SETLOCAL ENABLEDELAYEDEXPANSION
if not exist freertos junction freertos freertos_v%VERSION%
EXIT /b

:end
