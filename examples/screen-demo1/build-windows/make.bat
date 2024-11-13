REM Program
REM - Devices
cl /c ..\..\..\drivers\screen-windows-winapi\screen-windows-winapi.c /I ..\..\..\devices\screen\ /I ..\..\..\devices\keyboard\

REM - Infrastructural Program
cl /c launcher.c /I ..\..\..\devices\screen

REM Program
cl /c ..\program.c /I ..\..\..\devices\screen

REM - Combine the whole program
link /out:program.exe program.obj launcher.obj screen-windows-winapi.obj user32.lib gdi32.lib
