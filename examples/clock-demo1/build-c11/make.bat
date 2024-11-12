REM "c:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=amd64

REM Program
REM - Devices
cl /c /std:c11 ..\..\..\drivers\clock-c11\clock-c11.c /I ..\..\..\devices\clock\

REM - Infrastructural Program
cl /c launcher.c /I ..\..\..\devices\clock

REM Program
cl /c ..\program.c /I ..\..\..\devices\clock

REM - Combine the whole program
link /out:program.exe program.obj launcher.obj clock-c11.obj
