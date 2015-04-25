SET WATCOM=C:\SYSTEM\WATCOM19
SET INCLUDE=%WATCOM%\H
SET WATBIN=%WATCOM%\BINW
SET PATH=%WATBIN%;%PATH%
%WATBIN%\wpp386 /onatx /oh /oi+ /ei /3 /fp3 /dDOS32 %1.c xorlib.c nedofont.c
%WATBIN%\wlink system pmodew option stack=64k file %1,xorlib,nedofont name %1
