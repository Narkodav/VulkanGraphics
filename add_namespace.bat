@echo off
setlocal enabledelayedexpansion

set "GRAPHICS_DIR=e:\unik\C_code\vulkan_projects\VulkanGraphics\Graphics"

:: Process .h files
for /r "%GRAPHICS_DIR%" %%f in (*.h) do (
    echo Processing %%f
    
    :: Check if file already has namespace
    findstr /c:"namespace Graphics" "%%f" >nul
    if errorlevel 1 (
        :: Create temp file
        set "tempfile=%%f.tmp"
        
        :: Add namespace at beginning (after includes)
        type nul > "!tempfile!"
        set "added=0"
        for /f "usebackq delims=" %%l in ("%%f") do (
            echo %%l >> "!tempfile!"
            if "!added!"=="0" (
                echo %%l | findstr /c:"#include" >nul
                if not errorlevel 1 (
                    echo. >> "!tempfile!"
                    echo namespace Graphics { >> "!tempfile!"
                    echo. >> "!tempfile!"
                    set "added=1"
                )
            )
        )
        
        :: Add closing namespace
        echo. >> "!tempfile!"
        echo } // namespace Graphics >> "!tempfile!"
        
        :: Replace original with temp
        move /y "!tempfile!" "%%f"
    )
)

:: Process .cpp files
for /r "%GRAPHICS_DIR%" %%f in (*.cpp) do (
    echo Processing %%f
    
    :: Check if file already has namespace
    findstr /c:"namespace Graphics" "%%f" >nul
    if errorlevel 1 (
        :: Create temp file
        set "tempfile=%%f.tmp"
        
        :: Add namespace at beginning (after includes)
        type nul > "!tempfile!"
        set "added=0"
        for /f "usebackq delims=" %%l in ("%%f") do (
            echo %%l >> "!tempfile!"
            if "!added!"=="0" (
                echo %%l | findstr /c:"#include" >nul
                if not errorlevel 1 (
                    echo. >> "!tempfile!"
                    echo namespace Graphics { >> "!tempfile!"
                    echo. >> "!tempfile!"
                    set "added=1"
                )
            )
        )
        
        :: Add closing namespace
        echo. >> "!tempfile!"
        echo } // namespace Graphics >> "!tempfile!"
        
        :: Replace original with temp
        move /y "!tempfile!" "%%f"
    )
)

echo Done!