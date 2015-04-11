# XORLib - old school game library

XORLib is a simple multiplatform game library that is licensed under
MIT-license and developed at least for two platforms:

- PIC32 with NTSC and PAL TV
- DOS with CGA/EGA/VGA

PIC32 code based on "NTSC TV interface" examples from
http://hackaday.io/project/2032-pic32-oscilloscope
by Bruce Land (Cornell University) that is based on some ideas from
"Programming 32-bit Microcontrollers in C: Exploring the PIC32"
by Lucio Di Jasio.

DOS code doesn't exist yet...

GIF decoder was taken from NetSurf project (MIT-license):

http://www.netsurf-browser.org/projects/libnsgif/

JPG decoder is a public domain library called "picojpeg":

https://code.google.com/p/picojpeg/

Folder "examples" has some public domain apps for XORLib.

In order to build PIC32 application you need MPLAB X IDE from Microchip
with XC32 compiler. You need to create empty project for PIC32MX170256B
(or smaller device if your program will fit) and add header files:
- xorlib.h
- nedofont.h

and source files:
- xorlib.c
- nedofont.c
- yourapp.c (with function main - for example hello.c).

Then you need to add additional options for xc32-gcc (in Project Properties):
- option -DPIC32NTSCQ (if you have external crystal 14.31818 MHz) will set
28.63636MHz peripheral clock and 57.27272MHz CPU clock.
- option -DPIC32NTSC (if you don't have external crystal 14.31818 MHz and
internal 8 MHz oscillator will be used instead, but color modes will not
be available with this option) will set 32MHz peripheral clock and
64MHz CPU clock.

Also it is a good idea to power your device through the same PICkit3 that
you will use for programming (option "Power" for PICkit3 configuration of
Project Properties).

See http://xorlib.com for more info!

Shaos, March-April 2015
