# XORLib - old school game library

XORLib is a simple multiplatform game library that is licensed under
MIT-license and developed at least for two platforms:

- PIC32 with NTSC TV and may be PAL TV and VGA
- DOS32 with CGA and partially EGA/VGA

Version 0.x means that library is in active developement right now and
far from completion, but usable. Check progress on our
[wiki](https://github.com/shaos/xorlib/wiki).

Folder "tools" has some tools licensed under MIT-license.

Folder "tests" has some random test programs - some public domain, but
some licensed under MIT-license.

Folder "examples" has some public domain apps for XORLib - you can use
them as a base for your application!

![](http://nedopc.org/xorya/colors.jpg "colors.c")

## Acknowledgements

PIC32 code based on "NTSC TV interface" examples from
http://hackaday.io/project/2032-pic32-oscilloscope
by Bruce Land (Cornell University) that is based on examples from
"Programming 32-bit Microcontrollers in C: Exploring the PIC32"
by Lucio Di Jasio.

GIF decoder was taken from NetSurf project "libnsgif" (MIT-license):

http://www.netsurf-browser.org/projects/libnsgif/

Authors: Richard Wilson, Sean Fox and James Bursa (decode_gif.c)

JPG decoder is a public domain library called "picojpeg":

https://code.google.com/p/picojpeg/

Author: Rich Geldreich

## Usage

In order to build PIC32 application you need MPLAB X IDE from Microchip
with XC32 compiler. You need to create empty project for PIC32MX170F256B
(or smaller device if your program will fit) and add header files:
- xorlib.h
- nedofont.h

and source files:
- xorlib.c
- nedofont.c
- yourapp.c (with function main - for example hello.c).

Then you need to add additional options for xc32-gcc (in Project Properties):

- option -DPIC32NTSC will use internal 8 MHz oscillator with
setting 30MHz as peripheral clock and 60MHz as CPU clock
(black and white modes only):

![](http://nedopc.org/xorya/pic32ntsc.gif "PIC32NTSC")

- option -DPIC32NTSCQ will use external crystal 14.31818 MHz with
setting 28.63636MHz as peripheral clock and 57.27272MHz as CPU clock
(this option enables NTSC colors similar to CGA "composite" mode):

![](http://nedopc.org/xorya/pic32ntscQ.gif "PIC32NTSCQ")

Also it is a good idea to power your device through the same PICkit3 that
you will use for programming (option "Power" for PICkit3 configuration of
Project Properties in MPLAB X) - this is PICKit3 pinout:

1. MCLR (pin 1)
2. +3.3V (pin 13)
3. GND (pin 8)
4. PGED1 (pin 4)
5. PGEC1 (pin 5)
6. Not connected

See http://xorlib.com for more info!

Shaos, March-April 2015

