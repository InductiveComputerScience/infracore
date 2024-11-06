REM "c:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=amd64

REM Program
REM - Devices
cl /c ..\..\..\drivers\screen-windows-winapi\screen-windows-winapi.c /I ..\..\..\devices\screen\

REM - Infrastructural Program
cl /c launcher.c /I ..\..\..\devices\screen

REM Program
ml64 /I ..\..\..\devices\screen\ /c ../program-masm.asm 

REM - Combine the whole program
link /out:program.exe program-masm.obj launcher.obj screen-windows-winapi.obj user32.lib gdi32.lib
