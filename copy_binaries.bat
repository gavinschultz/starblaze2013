@echo on
set OUTPUTDIR=%~dp0\%1\
set RESOURCESDIR=%~dp0\resources\
set LIBDIR=%~dp0\lib\%2\
@echo on

if exist %LIBDIR% ( xcopy %LIBDIR%*.dll %OUTPUTDIR%* /R /Y /Q /D )
if exist %RESOURCESDIR% ( xcopy %RESOURCESDIR%*.* %OUTPUTDIR%* /R /Y /Q /D )