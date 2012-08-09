@cd "%~dp0"
@SET PATH=%PATH%;.\Bin;.\Deps\Bin;
@if exist "%MSVCDIR%" SET PATH=.\Deps\Bin\MSVC;%PATH%;
@if not exist "%MSVCDIR%" SET PATH=.\Deps\Bin\MinGW;%PATH%;
@if exist gamelog.txt del /f gamelog.txt
@if exist Bin\main.exe start Bin\main.exe %*
