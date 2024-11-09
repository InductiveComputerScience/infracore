REM Program
REM - Devices
cl /c ..\..\..\drivers\audio-windows-waveform\audio-windows-waveform.c /I ..\..\..\devices\audio\

REM - Infrastructural Program
cl /c launcher.c /I ..\..\..\devices\audio

REM Program
cl /c ..\program.c /I ..\..\..\devices\audio

REM - Combine the whole program
link /out:program.exe program.obj launcher.obj audio-windows-waveform.obj winmm.lib

