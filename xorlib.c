/*

XORLib - old school game library licensed under the MIT License
===============================================================

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

#include "xorlib.h"

#ifdef PIC32NTSC
#define PIC32ANY
#endif
#ifdef PIC32NTSCQ
#define PIC32ANY
#endif
#ifdef DOS32
#define DOSANY
#endif
#ifdef DOS16
#define DOSANY
#endif
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#ifdef TERM
#include <ncurses.h>
#define XOEXIT
#else
#ifndef NOFONT
#include "nedofont.h"
#endif
#ifdef DOSANY
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#ifdef DOS32
#include <graph.h>
#endif
#define XOEXIT
#endif
#endif

/*******************************************************************
 * PIC32 code was modified by A.A.Shabarshin (March-April 2015)
 * Optimized, recalculated to 14.31818 MHz and added colors...
 *
 * Original code was taken from "NTSC TV interface" examples:
 * http://hackaday.io/project/2032-pic32-oscilloscope
 * Bruce Land Cornell University
 * June 2014
 * "This code uses many cool ideas from
 * Programming 32-bit Microcontrollers in C: Exploring the PIC32
 * by Lucio Di Jasio"
 *
 * Uses two Compare units from one timer to do sync and video timing
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * VIDEO is PortA.1
 * SYNC  is PortB.1
 *
 */

#ifdef PIC32ANY
#include <plib.h>
#include <xc.h> // need for pps
#endif

#ifdef TERM
#define DX 80
#define DY 25
#else
#define DX xorlib_width
#ifdef DOSANY
#define DY xorlib_height
#endif
#endif

int xorlib_curmode = -1;
int xorlib_offset = 0;
int xorlib_width = 0;
#ifdef DOSANY
int xorlib_height = 200;
unsigned long xorlib_start = 0;
#endif
#ifdef DOS32
#define FPS 60
unsigned char* xorlib_ptry[200];
int xorlib_cury = 0;
#endif
int xorlib_maxcol = 79;
int xorlib_maxrow = 24;
volatile int xorlib_pitch = 0;
#ifdef PIC32ANY
volatile unsigned long xorlib_seconds = 0;
volatile unsigned long xorlib_frames = 0;
// Current line number which is modified
// by a state machine in the timer2 ISR
volatile int xorlib_curline = 0;

#ifdef PIC32NTSC

#define FPS 60
#define DY 200
// video timing
#define line_cycles 1905 // 63.5 uSec at 30 MHz Fpb, prescaler=1
#define us_5_cycles  150 // 5 uSec at 30 MHz Fpb, prescaler=1
// CPU configuration
#define SYS_FREQ 60000000
//                      8MHZ                          4MHz               60MHz            30   <-----<---    60MHz
#pragma config FNOSC = FRCPLL, POSCMOD = OFF, FPLLIDIV = DIV_2, FPLLMUL = MUL_15, FPBDIV = DIV_2, FPLLODIV = DIV_1
#pragma config FWDTEN = OFF
#pragma config FSOSCEN = OFF, JTAGEN = OFF

#endif

#ifdef PIC32NTSCQ

#define FPS 60
#define DY 200
// video timing
#define line_cycles 1823 // 63.5 uSec at 28.6 MHz Fpb (ideally it's 227.5 cycles of subcarrier freq)
#define us_5_cycles  135 // 4.7 uSec at 28.6 MHz Fpb, prescaler=1
// CPU configuration
#define SYS_FREQ 57272720
//                     14.31818MHZ                      3.579545MHz      57.27272MHz      28.63636  <-----<--- 57.27272MHz
#pragma config FNOSC = PRIPLL, POSCMOD = HS, FPLLIDIV = DIV_4, FPLLMUL = MUL_16, FPBDIV = DIV_2, FPLLODIV = DIV_1
#pragma config FWDTEN = OFF
#pragma config FSOSCEN = OFF, JTAGEN = OFF

#endif

#define SCREENSZ 18400
unsigned char xorlib_screen_buffer[SCREENSZ];
volatile unsigned char *xorlib_screen_buffer_addr = xorlib_screen_buffer;
volatile unsigned char *xorlib_screen_ptr;

// video active lines -- 200 total
#define image_start 20
#define image_end (image_start+DY)

// == OC3 ISR ============================================
// VECTOR 14 is OC3 vector -- set up of ipl3 in main
// vector names from int_1xx_2xx.h
void __ISR(14, ipl3) OC3Handler(void) // 14
{
    // mPORTBSetBits(BIT_1);
    // Convert DMA to SPI control
    DmaChnSetEventControl(1, DMA_EV_START_IRQ(_SPI1_TX_IRQ)); //
    //DmaChnEnable(1);
    // clear the timer interrupt flag -- name from
    // http://people.ece.cornell.edu/land/courses/ece4760/PIC32/Microchip_stuff/32-bit-Peripheral-Library-Guide.pdf
    // Table 8.2
    mOC3ClearIntFlag();
    // mPORTBClearBits(BIT_1);  // for profiling the ISR execution time
}


// == Timer 2 ISR =========================================
void __ISR(_TIMER_2_VECTOR, ipl2) Timer2Handler(void)
{
    //mPORTBSetBits(BIT_1); // for profiling the ISR execution time
    // update the current scanline number
    xorlib_curline++ ;

//    OpenOC5(OC_ON | OC_TIMER2_SRC | OC_CONTINUE_PULSE, 4+sound[xorlib_curline], 2);

    // start the DMA byte blaster to the screen
    if (xorlib_curline >= image_start && xorlib_curline < image_end){
        // set the Chan1 DMA transfer parameters: source & destination address,
        // source & destination size, number of bytes per event
        // 40,80 or 92 bytes / line with 1 byte per transfer (SPI in 8 bit mode)
        // screen_ptr = screen_buffer + ((xorlib_curline - image_start)*xorlib_pitch) ;
        DmaChnSetTxfer(1, (void*)xorlib_screen_ptr, (void*)&SPI1BUF, xorlib_pitch, 1, 1);
        // IRO 17 is the output compare 3 interrupt (See datasheet table 7.1)
        DmaChnSetEventControl(1, DMA_EV_START_IRQ(17)); //
        // turn it on
        DmaChnEnable(1);
        // increment the image memory pointer for the next ISR pass
        xorlib_screen_ptr += xorlib_pitch; // bytes per line
    }
    // update the frame time_tick immediately after image is copied
    else if(xorlib_curline==image_end)
    {
        // a general puropose time base
        if((++xorlib_frames % FPS)==0) xorlib_seconds++;
    }
    // == SYNC state machine ====
    // begin long (Vertical) synch after line 247
    else if (xorlib_curline==248) {OC2R = line_cycles - us_5_cycles ;}
    // back to regular sync after line 250
    // the first condition eliminates sync for one line (to avoid duplicate)
    else if (xorlib_curline==250) {OC2R = 0 ;}
    else if (xorlib_curline==251) {OC2R = us_5_cycles ;}
    // start new frame after line 262 and reset the image memory pointer
    else if (xorlib_curline==263) {
        xorlib_curline = 1;
        // reset for the next frame
        xorlib_screen_ptr = xorlib_screen_buffer_addr;
    }

    // clear the timer interrupt flag
    mT2ClearIntFlag();
    //mPORTBClearBits(BIT_1);  // for profiling the ISR execution time
}

#endif

#ifdef DEBUG
FILE* xorlib_debug = NULL;
#endif

#ifdef XOEXIT
void xoexit(void)
{
#ifdef DEBUG
  if(xorlib_debug!=NULL) fprintf(xorlib_debug,"xoexit\n");
#endif
#ifdef TERM
  nodelay(stdscr, FALSE);
  getch();
  endwin();
#endif
#ifdef DOS32
  getch();
  _setvideomode(-1);
#endif
#ifdef DEBUG
  if(xorlib_debug!=NULL) fclose(xorlib_debug);
#endif
}
#endif

int xoinit(short m)
{

#ifdef PIC32ANY
  int l,o;

  memset(xorlib_screen_buffer,0,sizeof(SCREENSZ));

/* delay before video */
  l = 360;
#ifdef PIC32NTSC
  l += 40;
#endif

  switch(m)
  {
      default: m = XOMODE_320x200_MONO;

      case XOMODE_320x200_MONO:
      case XOMODE_160x100_GRAY5:
          xorlib_width = 320;
          xorlib_pitch = 40;
          xorlib_offset = 0;
          o = 4;
          break;

      case XOMODE_640x200_MONO:
      case XOMODE_213x200_GRAY4:
          l -= 16;
          xorlib_width = 640;
          xorlib_pitch = 81;
          xorlib_offset = 1;
          o = 2;
          break;

      case XOMODE_160x200_COL15:
      case XOMODE_160x100_COL120:
          xorlib_width = 640;
#ifdef PIC32NTSCQ
          l -= 208;
          xorlib_pitch = 92;
          xorlib_offset = 12;
          xopalette(0);
#endif
#ifdef PIC32NTSC
          l -= 16;
          xorlib_pitch = 81;
          xorlib_offset = 1;
          m = XOMODE_640x200_MONO;
#endif
          o = 2;
          break;
  }

  xorlib_maxcol = (xorlib_width>>3)-1;
  xorlib_curmode = m;

   // Configure the device for maximum performance but do not change the PBDIV
   // Given the options, this function will change the flash wait states, RAM
   // wait state and enable prefetch cache but will not change the PBDIV.
   // The PBDIV value is already set via the pragma FPBDIV option above..
   SYSTEMConfig(SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);

    //make sure analog is cleared
    //ANSELA =0;
    //ANSELB =0;

    // timer interrupt //////////////////////////
    // Set up timer2 on,  interrupts, internal clock, prescalar 1, toggle rate
    // 63.5 microSec
    OpenTimer2(T2_ON | T2_SOURCE_INT | T2_PS_1_1, line_cycles);
    // set up the timer interrupt with a priority of 2
    ConfigIntTimer2(T2_INT_ON | T2_INT_PRIOR_2);
    mT2ClearIntFlag(); // and clear the interrupt flag

    // Compare match setup //////////////////////
    //Set up compare match unit to produce sync pulses
    // 5 uSec low
    // or 63.5-5 = 58.5 microSec (2340 ticks) low
    // pulse duration will be controlled in Timer2 ISR
    // #define OpenOC2( config, value1, value2) ( OC2RS = (value1), OC2R = (value2), OC2CON = (config) )
    OpenOC2(OC_ON | OC_TIMER2_SRC | OC_CONTINUE_PULSE, line_cycles-1, us_5_cycles);
    // OC2 is PPS group 2, map to RPB1 (pin 5)
    PPSOutput(2, RPB1, OC2);

    // OC3 setup /////////////////////////////////
    // Compare unit for video timing,
    // using the interrupt flag to trigger the first DMA,
    // then use the ISR to change the DMA control to SPI
    // #define OpenOC2( config, value1, value2) ( OC2RS = (value1), OC2R = (value2), OC2CON = (config) )
    // Pulse needs to be TWO cycles long
    OpenOC3(OC_ON | OC_TIMER2_SRC | OC_CONTINUE_PULSE, l+2, l);
    // turn on ISR so that DMA can covert to SPI control
    ConfigIntOC3(OC_INT_PRIOR_1 | OC_INT_ON); //3 // 
    mOC3ClearIntFlag(); // and clear the interrupt flag

    // OC4 setup ///////////////////////////////// highlighting of color burst is not required...
    OpenOC4(OC_ON | OC_TIMER2_SRC | OC_CONTINUE_PULSE, l+0, l+72);
    // OC4 is PPS group 3, map to RPB13 (pin 24)
    PPSOutput(3, RPB13, OC4);

    // OC5 setup /////////////////////////////////
    OpenOC5(OC_ON | OC_TIMER2_SRC | OC_CONTINUE_PULSE, line_cycles-200, line_cycles-100);
    // OC5 is PPS group 3, map to RPA4 (pin 12)
    PPSOutput(3, RPA4, OC5);

    // SPI configure /////////////////////////////
    // SCK1 is pin 25 RB14
    // SDO1 is PPS group 2, map to RPA1 (pin 3)
    // SDI1 is PPS group 2, map to RPB8 (pin 17) ???
    // SS1 input is PPS group 1, map to RPB7 (pin 16) for framing
    // specify PPS group, signal, logical pin name
//    PPSInput (1, SS1, RPB7);
    PPSOutput(2, RPA1, SDO1);
    // control sync for DAC
    mPORTBSetPinsDigitalOut(BIT_0|BIT_2|BIT_3);
    mPORTBSetBits(BIT_0);
    mPORTBClearBits(BIT_2);
    mPORTBClearBits(BIT_3);

    // divide Fpb by N, configure the I/O ports. 8 bit transfer
    SpiChnOpen(SPI_CHANNEL1, SPI_OPEN_ON | SPI_OPEN_MODE8 | SPI_OPEN_MSTEN, o);

    //=== DMA Channel 1 ================================
    // Open DMA Chan1 and chain from channel zero
    DmaChnOpen(1, 0, DMA_OPEN_DEFAULT);

    // setup system wide interrupts  ///
    INTEnableSystemMultiVectoredInt();

#endif

#ifdef DOS32
  int i = 0;
  xorlib_start = clock();
  atexit(xoexit);
  switch(m)
  {
      default: m = XOMODE_320x200_MONO;

      case XOMODE_320x200_MONO:
      case XOMODE_160x100_GRAY5:
          xorlib_width = 320;
          _setvideomode(5);
          break;

      case XOMODE_640x200_MONO:
      case XOMODE_213x200_GRAY4:
          xorlib_width = 640;
          _setvideomode(6);
          i = -1;
          break;

      case XOMODE_160x200_COL15:
      case XOMODE_160x100_COL120:
          xorlib_width = 640;
          _setvideomode(6);
          i = -1;
          outp(0x3D8,inp(0x3D8)&0xFB);
          break;
  }
  xorlib_maxcol = (xorlib_width>>3)-1;
  if(i<0)
  {
     for(i=0;i<200;i++)
     {
       if(i&1) xorlib_ptry[i] = ((unsigned char*)0xBA000) + i*40 - 40;
       else    xorlib_ptry[i] = ((unsigned char*)0xB8000) + i*40;
     }
  }
  xorlib_curmode = m;
#endif

#ifdef TERM
  atexit(xoexit);
  initscr();
  raw();
  keypad(stdscr, TRUE);
  noecho();
  nodelay(stdscr, TRUE);
  if(m<0) m = XOMODE_320x200_MONO;
  xorlib_curmode = m;
#endif

#ifdef DEBUG
  if(xorlib_debug==NULL) xorlib_debug = fopen("xorlib.out","wt");
  if(xorlib_debug!=NULL) fprintf(xorlib_debug,"xoinit\n");
#endif
  return m;
}

/* End of PIC32 code */

int xopalette(short p)
{
#ifdef PIC32NTSCQ
    register int i,y;
    register unsigned char *b = xorlib_screen_buffer;
    if(xorlib_pitch!=92) return 0;
    for(y=0;y<DY;y++)
    {
        switch(p)
        {
            case 0: b[0]=0x00; b[1]=0x0C; b[2]=0xCC; b[3]=0xCC; b[4]=0xCC; b[5]=0xCC; break;
            case 1: b[0]=0x00; b[1]=0x19; b[2]=0x99; b[3]=0x99; b[4]=0x99; b[5]=0x98; break;
            case 2: b[0]=0x00; b[1]=0x33; b[2]=0x33; b[3]=0x33; b[4]=0x33; b[5]=0x30; break;
            case 3: b[0]=0x00; b[1]=0x66; b[2]=0x66; b[3]=0x66; b[4]=0x66; b[5]=0x60; break;
        }
        for(i=6;i<12;i++) b[i]=0x00;
        b += xorlib_pitch;
    }
    return 1;
#else
    return 0;
#endif
}

unsigned long xoconfig(void)
{
  int ib = 0;
  union{long l;char c[4];}u;
  int is = (sizeof(int)==4)?XOCONFIG_32BITINT:0;
  u.c[0]=1;u.c[1]=2;u.c[2]=3;u.c[3]=4;
  if(u.l==0x01020304) ib = XOCONFIG_BIGENDIAN;
  return (1<<XOMODE_320x200_MONO)|
         (1<<XOMODE_160x100_GRAY5)|
#ifndef TERM
         (1<<XOMODE_640x200_MONO)|
         (1<<XOMODE_213x200_GRAY4)|
#endif
#ifdef PIC32NTSCQ
         (1<<XOMODE_160x200_COL15)|
         (1<<XOMODE_160x100_COL120)|
          XOCONFIG_NTSCTV|
#endif
#ifdef PIC32NTSC
          XOCONFIG_NTSCTV|
#endif
#ifdef DOSANY
         (1<<XOMODE_160x200_COL15)|
         (1<<XOMODE_160x100_COL120)|
#endif
          is|ib;
}

unsigned long xocontrols(void)
{
  unsigned long controls = 0;
#ifdef TERM
  switch(getch())
  {
     case KEY_LEFT:  controls |= XOCONTROL_LEFT;  break;
     case KEY_RIGHT: controls |= XOCONTROL_RIGHT; break;
     case KEY_UP:    controls |= XOCONTROL_UP;    break;
     case KEY_DOWN:  controls |= XOCONTROL_DOWN;  break;
     case ' ':       controls |= XOCONTROL_FIRE;  break;
  }
#endif
  return controls;
}

unsigned long xoframes(void)
{
#ifdef PIC32ANY
    return xorlib_frames;
#else
#ifdef DOS32
    return clock()*FPS/CLOCKS_PER_SEC;
#else
    return 0;
#endif
#endif
}

unsigned long xoseconds(void)
{
#ifdef PIC32ANY
    return xorlib_seconds;
#else
#ifdef DOS32
    return clock()/CLOCKS_PER_SEC;
#else
    return 0;
#endif
#endif
}

int xocurline(void)
{
#ifdef PIC32ANY
    return xorlib_curline;
#else
    return 0;
#endif
}

int xomode(void)
{
    return xorlib_curmode;
}

void xowaitretrace(void)
{
#ifdef PIC32ANY
    unsigned long frame = xorlib_frames;
    while(xorlib_frames==frame);
#endif
}

#ifdef PIC32ANY
#define Xodirectline(y) &xorlib_screen_buffer[xorlib_pitch*(y)+xorlib_offset]
#else
#define Xodirectline(y) xodirectline(y)
#endif

unsigned char* xodirectline(short y)
{
#ifdef PIC32ANY
   return Xodirectline(y);
#else
#ifdef DOS32
   xorlib_cury = y;
   return xorlib_ptry[y];
#else
   return NULL;
#endif
#endif
}

#ifdef PIC32ANY
#define Xonextline(p) ((p)+xorlib_pitch)
#else
#define Xonextline(p) xonextline(p)
#endif

unsigned char* xonextline(unsigned char *p)
{
#ifdef PIC32ANY
    return Xonextline(p);
#else
#ifdef DOS32
    if(p!=xorlib_ptry[xorlib_cury])
    {
      for(xorlib_cury=0;xorlib_cury<xorlib_height;xorlib_cury++)
      {
         if(xorlib_ptry[xorlib_cury]==p) break;
      }
      if(xorlib_cury==xorlib_height) return NULL;
    }
    if(++xorlib_cury >= xorlib_height) xorlib_cury = 0;
    return xorlib_ptry[xorlib_cury];
#else
    return NULL;
#endif
#endif
}

unsigned char* xoprevline(unsigned char *p)
{
#ifdef PIC32ANY
    return p-xorlib_pitch;
#else
#ifdef DOS32
    if(p!=xorlib_ptry[xorlib_cury])
    {
      for(xorlib_cury=0;xorlib_cury<xorlib_height;xorlib_cury++)
      {
         if(xorlib_ptry[xorlib_cury]==p) break;
      }
      if(xorlib_cury==xorlib_height) return NULL;
    }
    if(--xorlib_cury < 0) xorlib_cury = xorlib_height-1;
    return xorlib_ptry[xorlib_cury];
#else
    return NULL;
#endif
#endif
}

int xowidth(void)
{
    return DX;
}

int xoheight(void)
{
    return DY;
}

int xotextwidth(void)
{
    return xorlib_maxcol + 1;
}

int xotextheight(void)
{
    return xorlib_maxrow + 1;
}

int xochar(unsigned char x, unsigned char y, char c)
{
#ifdef DEBUG
  if(xorlib_debug!=NULL) fprintf(xorlib_debug,"xochar %i,%i,'%c'#%2.2X\n",x,y,(c>=32)?c:' ',c);
#endif
  if(x>=0 && x<=xorlib_maxcol && y>=0 && y<=xorlib_maxrow)
  {
#ifdef TERM
    mvaddch(y,x,c);
#else
#ifndef NOFONT
    register const unsigned char *ptr = font8x8[(c&255)-FONT8X8_FIRST];
    unsigned char *line_buffer = Xodirectline(y<<3);
    line_buffer[x] = ptr[0]; line_buffer = Xonextline(line_buffer);
    line_buffer[x] = ptr[1]; line_buffer = Xonextline(line_buffer);
    line_buffer[x] = ptr[2]; line_buffer = Xonextline(line_buffer);
    line_buffer[x] = ptr[3]; line_buffer = Xonextline(line_buffer);
    line_buffer[x] = ptr[4]; line_buffer = Xonextline(line_buffer);
    line_buffer[x] = ptr[5]; line_buffer = Xonextline(line_buffer);
    line_buffer[x] = ptr[6]; line_buffer = Xonextline(line_buffer);
    line_buffer[x] = ptr[7];
#endif
#endif
  }
  else
  {
#ifdef DEBUG
     if(xorlib_debug!=NULL) fprintf(xorlib_debug,"ERROR: Out of bounds!\n");
#endif
  }
#ifdef TERM
  refresh();
#endif
  return 1;
}

/* NOTE: NO PLATFORM SPECIFIC CODE BELOW THIS LINE !!! */

int xogray5(int i)
{
    register int c = 0xDB;
    switch(i)
    {
        case 0: c=0x20; break;
        case 1: c=0xB0; break;
        case 2: c=0xB1; break;
        case 3: c=0xB2; break;
        case 4: c=0xDB; break;
    }
    return c;
}

int xopixel(short x, short y, char c)
{
   if(x<0||x>=DX) return 0;
   if(y<0||y>=DY) return 0;
#ifdef TERM
   xochar(x,y,(c<10)?('0'+c):('A'+c-10)); /* text mode hack */
#else
   register unsigned char *line_buffer = Xodirectline(y);
   if (c > 0)
     line_buffer[x >> 3] |= 1<<(7-(x&7));
   else if (c==0)
     line_buffer[x >> 3] &= ~(1<<(7-(x&7)));
   else // c < 0
     line_buffer[x >> 3] ^= 1<<(7-(x&7));
#endif
   return 1;
}

int xoget(short x, short y)
{
#ifdef TERM
    return 0;
#else
    /* The following construction detects exactly one bit at the x,y location */
    register unsigned char* line_buffer = Xodirectline(y);
    return (line_buffer[(x >> 3) + (y * xorlib_pitch)] & (1<<(7-(x&7))))?1:0 ;
#endif
}

int xostring(unsigned char x, unsigned char y, char *str)
{
   register char i;
   for (i=0; str[i]!=0; i++)
   {
      xochar(x++,y,str[i]);
   }
   return 1;
}

#define XORLIB_TEMPSTR_SZ 1024

char xorlib_tempstr[XORLIB_TEMPSTR_SZ];

int xoprintf(char *s,...)
{
 char *po;
 int n,o = 0;
 va_list arg;
 static int xorlib_curcol = 0;
 static int xorlib_currow = 0;
 if(xomode() < 0) xoinit(-1);
#ifdef DEBUG
 if(xorlib_debug!=NULL){fputs("xopintf ",xorlib_debug);fputs(s,xorlib_debug);fputc('\n',xorlib_debug);}
#endif
 va_start(arg,s);
 o = vsnprintf(xorlib_tempstr,XORLIB_TEMPSTR_SZ,s,arg);
 va_end(arg);
 xorlib_tempstr[XORLIB_TEMPSTR_SZ-1] = 0;
 n = 0;
 po = xorlib_tempstr;
 while(*po)
 {
   if(*po < 0x20)
   {
      if(*po=='\n') n++;
      if(*po=='\t')
      {
         xorlib_curcol = (xorlib_curcol&0xF8)+8;
         if(xorlib_curcol > xorlib_maxcol) n++;
      }
   }
   else
   {
      xochar(xorlib_curcol++,xorlib_currow,*po);
      if(xorlib_curcol > xorlib_maxcol) n++;
   }
   if(n)
   {
      n = 0;
      xorlib_curcol = 0;
      if(++xorlib_currow > xorlib_maxrow)
      {
         /* TODO: scroll */
         xorlib_currow = xorlib_maxrow;
      }
   }
   po++;
 }
 return o;
}

/*
 plot a line
 at x1,y1 to x2,y2 with color 1=white 0=black 2=invert
 NOTE: this function requires signed chars
 Code is from David Rodgers,
 "Procedural Elements of Computer Graphics",1985
*/
int xoline(short x1, short y1, short x2, short y2, char c)
{
   int e;
   signed int dx,dy,j, temp;
   signed char s1,s2, xchange;
        signed int x,y;

   x = x1;
   y = y1;

   /* take absolute value */
   if (x2 < x1) {
      dx = x1 - x2;
      s1 = -1;
   }
   else if (x2 == x1) {
      dx = 0;
      s1 = 0;
   }
   else {
      dx = x2 - x1;
      s1 = 1;
   }

   if (y2 < y1) {
      dy = y1 - y2;
      s2 = -1;
   }
   else if (y2 == y1) {
      dy = 0;
      s2 = 0;
   }
   else {
      dy = y2 - y1;
      s2 = 1;
   }

   xchange = 0;

   if (dy>dx) {
      temp = dx;
      dx = dy;
      dy = temp;
      xchange = 1;
   }

   e = ((int)dy<<1) - dx;

   for (j=0; j<=dx; j++) {
      xopixel(x,y,c);

      if (e>=0) {
         if (xchange==1) x = x + s1;
         else y = y + s2;
         e = e - ((int)dx<<1);
      }

      if (xchange==1) y = y + s2;
      else x = x + s1;

      e = e + ((int)dy<<1);
   }

   return 1;
}

int xorect(short x, short y, short w, short h, char c)
{
    int x2 = x + w - 1;
    int y2 = y + h - 1;
    xoline(x,y,x2,y,c);
    xoline(x,y2,x2,y2,c);
    xoline(x,y,x,y2,c);
    xoline(x2,y,x2,y2,c);
    if(xomode()>=2) /* ??? */
    {
        xoline(x+1,y,x+1,y2,c);
        xoline(x2-1,y,x2-1,y2,c);
    }
    return 1;
}
