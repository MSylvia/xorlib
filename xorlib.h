/*

XORLib - old school game library licenced under the MIT License
===============================================================

See for more info: http://www.xorlib.com

*/

#ifndef _XORLIB_H_
#define _XORLIB_H_

#define XORLIB_VERSION 0x0001 /* v0.1 */

/* Available external macros:

   PIC32NTSC - for PIC32MX with 8 MHz crystal and NTSC timings (black and white modes - better for 256x200)
   PIC32NTSCQ - for PIC32MX with 14.31818 MHz crystal (color burst mode - better for 320x200 and 640x200)
   PIC32PAL - for PIC32MX with 8 MHz crystal and PAL timings (black and white modes)
   PIC32VGA - experimental mode for PIC32MX connected to VGA monitor (future)
   DOS32 - for 32-bit DOS
   DOS16 - for 16-bit DOS

   DECODEGIF - compiled with GIF decoder (xdec_gif.c)
   DECODEJPG - compiled with JPG decoder (xdec_jpg.c)
*/

/* NTSC compatible video modes - 256 bits per line (original) */
#define XOMODE_256x200_MONO    0 /* Black and white 256x200 */
#define XOMODE_128x100_GRAY5   1 /* Pseudo mode above mode 0 with 5 shades of gray */
/* NTSC compatible video modes - 320 bits per line */
#define XOMODE_320x200_MONO    2 /* Black and white 320x200 */
#define XOMODE_160x100_GRAY5   3 /* Pseudo mode above mode 2 with 5 shades of gray */
/* NTSC compatible video modes - 640 bits per line */
#define XOMODE_640x200_MONO    4 /* Black and white 640x200 */
#define XOMODE_320x100_GRAY5   5 /* Pseudo mode above mode 4 with 5 shades of gray */
#define XOMODE_160x200_COL16   6 /* Color burst mode over 640x200 with 16 predefined colors (4 pallets) */
#define XOMODE_160x100_COL256  7 /* Pseudo mode above mode 6 with configurable pallette of approximated colors */
/* NTSC compatible video modes with additional hardware - 640 bits per line */
#define XOMODE_320x200_COL4    8 /* CGA-like mode with configurable 4-color palette */
/* NTSC compatible video modes with additional hardware - 1280 bits per line */
#define XOMODE_320x200_COL16   9 /* EGA-like mode with configurable 16-color palette */
#define XOMODE_160x200_COL256 10 /* Predefined 256-color RGB-palette (including 64 shades of gray) */
/* NTSC compatible video modes with additional hardware - 2560 bits per line */
#define XOMODE_640x200_COL16  11 /* EGA-like mode with configurable 16-color palette */
#define XOMODE_320x200_COL256 12 /* Predefined 256-color RGB-palette (including 64 shades of gray) */
/* VGA compatible video modes (just a placeholder for future) */
#define XOMODE_640x350_COL16  13 /* EGA-like mode with standard 16-color palette */
#define XOMODE_640x480_COL16  14 /* VGA-like mode with standard 16-color palette */
#define XOMODE_800x600_COL16  15 /* SVGA-like mode with standard 16-color palette */

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

/* Trick for 5 shades grayscale mode over black and white */
#define XOCOLOR_BRIGHT_GRAY XOCOLOR_BRIGHT_CYAN
/*
   Virtual 5 shades of gray:

   xogray5(0) -> XOCOLOR_BLACK
   xogray5(1) -> XOCOLOR_GRAY
   xogray5(2) -> XOCOLOR_BRIGHT_GRAY
   xogray5(3) -> XOCOLOR_WHITE
   xogray5(4) -> XOCOLOR_BRIGHT_WHITE
*/

/* Configuration flags (bits 0..15 for supported graphics modes) */
#define XOCONFIG_BIGENDIAN 0x00010000 /* bit 16 */
#define XOCONFIG_32BITINT  0x00020000 /* bit 17 */
#define XOCONFIG_NTSCTV    0x00040000 /* bit 18 */
#define XOCONFIG_PALTV     0x00080000 /* bit 19 */
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
#define XOCONTROL_FIRE   XOCONTROL_LEFT2
#define XOCONTROL_X      XOCONTROL_UP2
#define XOCONTROL_Y      XOCONTROL_LEFT2
#define XOCONTROL_A      XOCONTROL_RIGHT2
#define XOCONTROL_B      XOCONTROL_DOWN2
#define XOCONTROL_UP     XOCONTROL_UP1
#define XOCONTROL_LEFT   XOCONTROL_LEFT1
#define XOCONTROL_RIGHT  XOCONTROL_RIGHT1
#define XOCONTROL_DOWN   XOCONTROL_DOWN1

/* Optional control bits */
#define XOCONTROL_LSHOULDER 0x0100 /* bit 8 */
#define XOCONTROL_RSHOULDER 0x0200 /* bit 9 */
#define XOCONTROL_KEYREADY  0x1000 /* bit 12 - keyboard buffer is not empty */

struct xoimage
{
 short width, height;     /* width and height of the image in pixels */
 unsigned char pitch;     /* pitch in ints (to calculate vertical offset) */
 signed char bpp;         /* bits per pixel (negative if grayscale) */
 int *data;               /* pointer to the image data */
 int *mask;               /* optional pointer to the mask */
};

/* Some macros */
#define VIDEOSCREEN (struct xoimage*)0
#define TEXTNORMAL xotextattr(XOCOLOR_BRIGHT_WHITE,XOCOLOR_BLACK)
#define TEXTINVERT xotextattr(XOCOLOR_BLACK,XOCOLOR_BRIGHT_WHITE)

#ifdef __cplusplus
extern "C" {
#endif

/* System functions */

unsigned long xoconfig(void);                   /* return configuration bits (see above) */
unsigned long xocontrols(void);                 /* return state of controls (see above) */
unsigned long xoframes(void);                   /* return frames counter */
unsigned long xoseconds(void);                  /* return seconds counter */
short xocurline(void);                          /* return current line of the frame (0 is 1st line of the video) */
void xowaitvblank(void);                        /* wait for vertical blank */

/* Graphics functions */

int xoinit(short m);                                    /* set graphics mode -> 0 if failed */
int xopixel(short x, short y, char c);                  /* draw a pixel (-1 means inversion) -> 0 if invalid args */
int xoget(short x, short y);                            /* get state of the pixel (actual value) -> -1 if not supported */
int xoline(short x1, short y1, short x2, short y2, char c); /* draw a line with a color (-1 means inversion) */
int xorect(short x, short y, short w, short h, char c); /* draw a rectangular with a color (-1 means inversion) */
int xobar(short x, short y, short w, short h, char c);  /* draw a solid rectangular with a color (-1 means inversion) */
int xocircle(short x, short y, short r, char c);        /* draw a circle with a color (-1 means inversion) */
int xoellipse(short x, short y, short rx, short ry, char c); /* draw an oval with a color (-1 means inversion) */
int xoarc(short x, short y, short rx, short ry, short a1, short a2, char c); /* draw an arc of oval (-1 means inversion) */
int xopie(short x, short y, short rx, short ry, short a1, short a2, char c); /* draw a solid piece of oval (-1 means inversion) */
int xofill(short x, short y, char c);                   /* fill a region with a color (inversion is not applicable) */
int xopoly(char c, short n, short *a);                  /* draw a polygon from array {x,y, x,y, etc. } */
int xotextattr(char i, char p);                         /* set text color attributes (ink, paper) */
int xochar(unsigned char x, unsigned char y, char c);   /* print character using text location */
int xostring(unsigned char x, unsigned char y, char* s);/* print string using text location */
int xoprintf(char* s, ...);                             /* print string to current position with a possible scroll */
int xouserchar(char c, unsigned char* p);               /* add user character with code 0...31 using 8 bytes -> 0 if error */
unsigned char xotextwidth(void);                        /* return text screen width */
unsigned char xotextheight(void);                       /* return text screen height */
char xogray5(int i);                                    /* 5 shades of gray function (0,1,2,3,4) returns color */
char xogray5a(int i);                                   /* 5 shades of gray function (0,1,2,3,4) returns ASCII code */
void xoswitchscreens(void);                             /* switch primary and secondary screens (in case of double buffering) */
int xouseprimary(void);                                 /* use primary screen (default) */
int xousesecondary(void);                               /* use secondary screen (in case of double buffering) */
int* xodirectline(short y);                             /* return pointer to video line for direct access */
int* xoprevline(int *p);                                /* return pointer to the previous video line for direct access */
int* xonextline(int *p);                                /* return pointer to the next video line for direct access */
int xocopy(struct xoimage* d, struct xoimage* s);       /* copy one image into another with mask if presented */
int xorcopy(struct xoimage* d, struct xoimage* s);      /* copy one image into another with xor -> 0 if not supported */
int xopcopy(struct xoimage* d, struct xoimage* s, short x, short y, short w, short h); /* partial copy without mask */
int xopcopyk(struct xoimage* d, struct xoimage* s, short x, short y, short w, short h, char c); /* partial copy with a key color */

#ifdef __cplusplus
}
#endif

#endif
