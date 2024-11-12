REM "c:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=amd64

REM Program
REM - Devices
cl /c /std:c89 ..\..\..\drivers\disk-c89-file\disk-c89-file.c /I ..\..\..\devices\disk\

REM - Infrastructural Program
cl /c launcher.c /I ..\..\..\devices\disk

REM Program
cl /c ..\program.c /I ..\..\..\devices\disk

REM - Combine the whole program
link /out:program.exe program.obj launcher.obj disk-c89-file.obj
