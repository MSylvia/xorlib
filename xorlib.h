/*

XORLib - old school game library for anyone
===========================================

Copyright (c) 2015 A.A.Shabarshin <me@shaos.net>

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom
the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#ifndef XORLIB_H
#define XORLIB_H

#define XORLIB_VERSION 0x0001 /* v0.1 */

/* NTSC compatible modes */
#define XOMODE_TEXT         0 /* ASCII mode with any dimensions */
#define XOMODE_1BIT_256x200 1 /* Black and white 256x200 */
#define XOMODE_1BIT_640x200 2 /* Black and white 640x200 */
#define XOMODE_2BIT_320x200 3 /* Grayscale or black/magenta/cyan/white */
#define XOMODE_4BIT_256x200 4 /* Grayscale with 16 shades of gray */
#define XOMODE_4BIT_320x200 5 /* EGA-like mode with standatd 16-color palette */
#define XOMODE_4BIT_640x200 6 /* EGA-like mode with standard 16-color palette */
#define XOMODE_8BIT_320x200 7 /* Predefined 256-color RGB-palette */
/* VGA compatible modes */
#define XOMODE_4BIT_640x350 8 /* EGA-like mode with standard 16-color palette */
#define XOMODE_4BIT_640x480 9 /* VGA-like mode with standard 16-color palette */

/* Graphics mode options */
#define XOMODE_OPTION_DBLEBUFFER 1 /* Double buffer option */
#define XOMODE_OPTION_GRAYSCALE  2 /* Grayscale mode (to treat 16 colors as shades of gray) */
#define XOMODE_OPTION_HIGHLIGHTS 4 /* Highlight segmensts of some lines of video */

/* Standard colors */
#define XOCOLOR_BLACK           0
#define XOCOLOR_BLUE            1
#define XOCOLOR_GREEN           2
#define XOCOLOR_CYAN            3
#define XOCOLOR_RED             4
#define XOCOLOR_MAGENTA         5
#define XOCOLOR_BROWN           6
#define XOCOLOR_WHITE           7
#define XOCOLOR_GRAY            8
#define XOCOLOR_BRIGHT_BLUE     9
#define XOCOLOR_BRIGHT_GREEN   10
#define XOCOLOR_BRIGHT_CYAN    11
#define XOCOLOR_BRIGHT_RED     12
#define XOCOLOR_BRIGHT_MAGENTA 13
#define XOCOLOR_BRIGHT_YELLOW  14
#define XOCOLOR_BRIGHT_WHITE   15

#define XOCOLOR_INVERT         -1

/* Trick for 5 shades grayscale mode over black&white (xopixel2x2) */
#define XOCOLOR_BRIGHT_GRAY    XOCOLOR_BRIGHT_CYAN

/* Configuration flags (bits 0..15 for supported graphics modes) */
#define XOCONFIG_BIGENDIAN 0x00010000 /* bit 16 */
#define XOCONFIG_32BITINT  0x00020000 /* bit 17 */
#define XOCONFIG_NTSCTV    0x00040000 /* bit 18 */
#define XOCONFIG_SHOULDERS 0x00080000 /* bit 19 */
#define XOCONFIG_NETWORK   0x00100000 /* bit 20 */
#define XOCONFIG_KEYBOARD  0x00200000 /* bit 21 */
#define XOCONFIG_SDCARD    0x00400000 /* bit 22 */
#define XOCONFIG_CDROM     0x00800000 /* bit 23 */

/* Control bits */
#define XOCONTROL_LEFT1  0x0001 /* bit 0 */
#define XOCONTROL_RIGHT1 0x0002 /* bit 1 */
#define XOCONTROL_UP1    0x0004 /* bit 2 */
#define XOCONTROL_DOWN1  0x0008 /* bit 3 */
#define XOCONTROL_LEFT2  0x0010 /* bit 4 */
#define XOCONTROL_RIGHT2 0x0020 /* bit 5 */
#define XOCONTROL_UP2    0x0040 /* bit 6 */
#define XOCONTROL_DOWN2  0x0080 /* bit 7 */

/* Some control aliases */
#define XOCONTROL_SELECT XOCONTROL_LEFT2
#define XOCONTROL_X      XOCONTROL_UP2
#define XOCONTROL_Y      XOCONTROL_LEFT2
#define XOCONTROL_A      XOCONTROL_RIGHT2
#define XOCONTROL_B      XOCONTROL_DOWN2

/* Optional control bits */
#define XOCONTROL_LSHOULDER 0x0100 /* bit 8 */
#define XOCONTROL_RSHOULDER 0x0200 /* bit 9 */
#define XOCONTROL_KEYREADY  0x0400 /* bit 10 - keyboard buffer is not empty */

struct xocontext
{
 unsigned long config;    /* configuration bits (see above) */
 unsigned long seconds;   /* counter of seconds */
 unsigned long frames;    /* counter of frames */
 unsigned short line;     /* current video line (0 is the 1st line of the screen) */
 unsigned short controls; /* current state of controls */
};

struct xoimage
{
 short width, height;     /* width and height of the image in pixels */
 unsigned char pitch;     /* pitch in bytes (to calculate vertical offset) */
 signed char bpp;         /* bits per pixel (negative if grayscale) */
 unsigned short omask;    /* offset to the 1-bit mask in data (optional) */
 int data[1];             /* array of the image data (with or without mask) */
};

/* Some macros */
#define TEXTNORMAL xotextattr(XOCOLOR_WHITE,XOCOLOR_BLACK)
#define TEXTINVERT xotextattr(XOCOLOR_BLACK,XOCOLOR_WHITE)

#ifdef __cplusplus
extern "C" {
#endif

/*
 User must define function xogame and it will be called periodically
 by the system from the main loop. Normally function should return 0,
 but if user wants the system to wait video blank before calling
 the function next time, then it should return 1 (also if double buffer
 is enabled then system will switch buffers before next call in this case).
*/

int xogame(struct xocontext* context);

/* System functions */

int xofirst(struct xocontext* context);                 /* check if it's a 1st call of xogame function */
int xosupported(struct xocontext* context, int m);      /* check if graphics mode is supported */

/* Graphics functions */

int xoinit(int m, int o);                               /* set graphics mode with options -> 0 if failed */
int xopixel(int x, int y, char c);                      /* draw a pixel (-1 means inversion) -> 0 if invalid args */
int xopixel2x2(int x, int y, char c);                   /* draw a double pixel (inversion is not applicable) -> 0 if invalid args */
int xoget(int x, int y);                                /* get state of the pixel (actual value) -> -1 if not supported */
int xoline(int x1, int y1, int x2, int y2, char c);     /* draw a line with a color (-1 means inversion) */
int xorect(int x, int y, int w, int h, char c);         /* draw a rectangular with a color (-1 means inversion) */
int xobar(int x, int y, int w, int h, char c);          /* draw a solid rectangular with a color (-1 means inversion) */
int xocircle(int x, int y, int r, char c);              /* draw a circle with a color (-1 means inversion) */
int xopie(int x, int y, int r, int a1, int a2, char c); /* draw a solid piece of circle (-1 means inversion) */
int xofill(int x, int y, char c);                       /* fill a region with a color (inversion is not applicable) */
int xotextattr(char i, char p);                         /* set text color attributes (ink, paper) */
int xochar(int x, int y, char c);                       /* print character using text location */
int xostring(int x, int y, char* s);                    /* print string using text location */
int xoprintf(char* s, ...);                             /* print string to current position with a possible scroll */
int xouserchar(int i, unsigned char* p);                /* add user character with code 0...31 using 8 bytes -> 0 if error */
int xotextwidth(void)                                   /* return text screen width */
int xotextheight(void)                                  /* return text screen height */
char xogray5(int i);                                    /* 5 shades of gray function (0,1,2,3,4) returns color */
char xogray5a(int i);                                   /* 5 shades of gray function (0,1,2,3,4) returns ASCII code */
struct xoimage* xoscreen(void);                         /* get pointer to the screen as an image */
int xocopy(struct xoimage* d, struct xoimage* s);       /* copy one image into another with mask if presented */
int xorcopy(struct xoimage* d, struct xoimage* s);      /* copy one image into another with xor -> 0 if not supported */
int xopcopy(struct xoimage* d, struct xoimage* s, int x, int y, int w, int h); /* partial copy without mask */
int xopcopyk(struct xoimage* d, struct xoimage* s, int x, int y, int w, int h, char c); /* partial copy with a key color */
int xohighlight(int y, int l, int r);                   /* highlight segment of line y (-1 means all) from l to r -> 0 if not supported */

#ifdef __cplusplus
}
#endif

#endif
