SET WATCOM=C:\SYSTEM\WATCOM19
SET INCLUDE=%WATCOM%\H
SET WATBIN=%WATCOM%\BINW
SET PATH=%WATBIN%;%PATH%
IF EXIST xorlib.obj GOTO L1
wpp386 /onatx /oh /oi+ /ei /3 /fp3 /dDOS32 xorlib.c
:L1
IF EXIST nedofont.obj GOTO L2
wpp386 /onatx /oh /oi+ /ei /3 /fp3 /dDOS32 nedofont.c
:L2
wpp386 /onatx /oh /oi+ /ei /3 /fp3 /dDOS32 %1.c
wlink system pmodew option stack=64k file %1,xorlib,nedofont name %1
