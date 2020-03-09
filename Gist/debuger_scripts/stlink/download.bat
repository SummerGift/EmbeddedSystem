STM32_Programmer_CLI.exe -c port=swd -w rtthread.bin 0x08000000

:: STM32CubeProgrammer Utility flash script

REM @ECHO OFF
REM @setlocal
REM ::COLOR 0B

REM :: Current Directory
REM @SET CUR_DIR=%CD%

REM :: Chip Name
REM @SET CHIP_NAME=STM32H747I
REM :: Main Board
REM @SET MAIN_BOARD=-DISCO
REM :: Demo Name
REM @SET DEMO_NAME=STM32Cube_Demo
REM :: Demo Version
REM @SET DEMO_VER=V1.0.0
REM :: Hex filename
REM @SET HEX_FILE="%DEMO_NAME%-%CHIP_NAME%%MAIN_BOARD%-%DEMO_VER%.hex"
REM @IF NOT EXIST "%HEX_FILE%" @SET HEX_FILE=%DEMO_NAME%-%CHIP_NAME%%MAIN_BOARD%-%DEMO_VER%_FULL.hex
REM @IF NOT EXIST "%HEX_FILE%" @ECHO %HEX_FILE% Does not exist !! && GOTO goError

REM :: Board ID
REM @SET BOARD_ID=0
REM :: External Loader Name
REM @SET EXT_LOADER=MT25TL01G_%CHIP_NAME%%MAIN_BOARD%

REM @SET STM32_PROGRAMMER_PATH="%ProgramFiles(x86)%\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin"
REM @IF NOT EXIST %STM32_PROGRAMMER_PATH% @SET STM32_PROGRAMMER_PATH="%ProgramW6432%\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin"
REM @IF NOT EXIST %STM32_PROGRAMMER_PATH% @SET STM32_PROGRAMMER_PATH="%ProgramFiles%\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin"
REM @IF NOT EXIST %STM32_PROGRAMMER_PATH% @ECHO STM32CubeProgrammer is not installed !! && GOTO goError
REM @IF NOT EXIST %STM32_PROGRAMMER_PATH% @ECHO %STM32_PROGRAMMER_PATH% Does not exist !! && GOTO goError
REM @SET STM32_EXT_FLASH_LOADER=%STM32_PROGRAMMER_PATH%\ExternalLoader\%EXT_LOADER%.stldr
REM @IF NOT EXIST %STM32_EXT_FLASH_LOADER% @ECHO %STM32_EXT_FLASH_LOADER% Does not exist !! && GOTO goError

REM @SET STM32_EXT_FLASH_LOADER=%STM32_PROGRAMMER_PATH%\ExternalLoader\%EXT_LOADER%.stldr

REM TITLE STM32CubeProgrammer Utility for %CHIP_NAME%%MAIN_BOARD%

REM :: Add STM32CubeProgrammer to the PATH
REM @SET PATH=%STM32_PROGRAMMER_PATH%;%PATH%

REM @ECHO.
REM @ECHO =================================================
REM @ECHO Erase and Flash all memories and reboot the board
REM @ECHO =================================================
REM @ECHO. 
REM STM32_Programmer_CLI.exe -c port=SWD index=%BOARD_ID% reset=HWrst -el %STM32_EXT_FLASH_LOADER% -e all -d %HEX_FILE% -HardRst
REM @IF NOT ERRORLEVEL 0 (
REM   @GOTO goError
REM )

REM @GOTO goOut

REM :goError
REM @SET RETERROR=%ERRORLEVEL%
REM @COLOR 0C
REM @ECHO.
REM @ECHO Failure Reason Given is %RETERROR%
REM @PAUSE
REM @COLOR 07
REM @EXIT /b %RETERROR%

REM :goOut
