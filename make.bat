@echo off
cls

rem for foenix hardware
wdc816as -DUSING_816 -DLARGE -V -L fxloader.asm -O fxloader.obj

wdc816cc -ML -Ic:\wdc\projects\foenixLibrary\fatfs main.c
wdc816cc -ML -Ic:\wdc\projects\foenixLibrary\fatfs header.c
wdc816cc -ML -Ic:\wdc\projects\foenixLibrary\fatfs stack.c
wdc816cc -ML -Ic:\wdc\projects\foenixLibrary\fatfs callStack.c
wdc816cc -ML -Ic:\wdc\projects\foenixLibrary\fatfs object.c
wdc816cc -ML -Ic:\wdc\projects\foenixLibrary\fatfs text.c
wdc816cc -ML -Ic:\wdc\projects\foenixLibrary\fatfs data.c

wdcln -HIE -T  -P00 main.obj header.obj stack.obj callStack.obj object.obj text.obj data.obj fxloader.obj -L../foenixLibrary/FMX -LML -LCL -O zmachine.hex -C10000  -D20000

