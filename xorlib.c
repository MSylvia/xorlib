/*

XORLib - old school game library licenced under the MIT License
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
#define TEXT32
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
#endif
#ifndef NOFONT
#include "nedofont.h"
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

#define DX xorlib_width

#ifdef TEXT32
volatile int xorlib_pitch = 8;
int xorlib_offset = 0;
int xorlib_width = 256;
int xorlib_maxcol = 31;
int xorlib_maxrow = 24;
#else
volatile int xorlib_pitch = 20;
int xorlib_offset = 0;
int xorlib_width = 640;
int xorlib_maxcol = 79;
int xorlib_maxrow = 24;
#endif
int xorlib_curmode = -1;
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
#define line_cycles 2032 // 63.5 uSec at 32 MHz Fpb, prescaler=1
#define us_5_cycles  160 // 5 uSec at 32 MHz Fpb, prescaler=1
// CPU configuration
#define SYS_FREQ 64000000
//                      8MHZ                          4MHz               64MHz            32   <-----<---    64MHz
#pragma config FNOSC = FRCPLL, POSCMOD = OFF, FPLLIDIV = DIV_2, FPLLMUL = MUL_16, FPBDIV = DIV_2, FPLLODIV = DIV_1
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
#endif

#pragma config FWDTEN = OFF
#pragma config FSOSCEN = OFF, JTAGEN = OFF

#define SCREENSZ 4600 // 18400 bytes
int xorlib_screen_buffer[SCREENSZ];
volatile int *xorlib_screen_buffer_addr = xorlib_screen_buffer;
volatile int *xorlib_screen_ptr;

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
        // 32 bytes / line with 4 bytes per transfer (SPI in 32 bit mode)
        //screen_ptr = screen_buffer + ((xorlib_curline - image_start)<<5) ;
        DmaChnSetTxfer(1, (void*)xorlib_screen_ptr, (void*)&SPI1BUF, xorlib_pitch<<2, 4, 4); 
        // IRO 17 is the output compare 3 interrupt (See datasheet table 7.1)
        DmaChnSetEventControl(1, DMA_EV_START_IRQ(17)); //
        // turn it on for 32 bytes
        DmaChnEnable(1);
        // increment the image memory pointer for the next ISR pass
        xorlib_screen_ptr += xorlib_pitch; // 32-bit words per line
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

#ifdef TERM
void xoexit(void)
{
#ifdef DEBUG
  if(xorlib_debug!=NULL) fprintf(xorlib_debug,"xoexit\n");
#endif
  nodelay(stdscr, FALSE);
  getch();
  endwin();
#ifdef DEBUG
  if(xorlib_debug!=NULL) fclose(xorlib_debug);
#endif
}
#endif

unsigned long xoconfig(void)
{
  return 0;
}

int xoinit(short m)
{
  int l,o=0,mo=m;  
    
#ifdef TERM
  atexit(xoexit);
  initscr();
  raw();
  keypad(stdscr, TRUE);
  noecho();
  nodelay(stdscr, TRUE);
  o = 1;
#endif

#ifdef PIC32ANY

  memset(xorlib_screen_buffer,0,sizeof(SCREENSZ)*sizeof(int));
#ifdef PIC32NTSCQ
  l = 168;
#endif
#ifdef PIC32NTSC
  l = 352;
#endif
  switch(m)
  {
      case XOMODE_256x200_MONO:
      case XOMODE_128x100_GRAY5:
          xorlib_width = 256;
          xorlib_pitch = 8;
          xorlib_offset = 0;
          o = 6;
#ifdef PIC32NTSCQ
          o = 4;
          l += 340;
#endif
          break;
      case XOMODE_320x200_MONO:
      case XOMODE_160x100_GRAY5:
          xorlib_width = 320;
          xorlib_pitch = 10;
          xorlib_offset = 0;
          o = 4;
#ifdef PIC32NTSCQ
          l += 192;
#endif
#ifdef PIC32NTSC
          l += 136;
#endif          
          break;
      case XOMODE_640x200_MONO:
      case XOMODE_320x100_GRAY5:
          xorlib_width = 640;
          xorlib_pitch = 20;
          xorlib_offset = 0;
          o = 2;
#ifdef PIC32NTSCQ
          l += 192;
#endif
#ifdef PIC32NTSC
          l += 136;
#endif          
          break;
      case XOMODE_160x200_COL16:
      case XOMODE_160x100_COL256:
          xorlib_width = 640;
#ifdef PIC32NTSCQ
          xorlib_pitch = 23;
          xorlib_offset = 3;
          xopalette(0);
#endif
#ifdef PIC32NTSC
          m = XOMODE_640x200_MONO;
          xorlib_pitch = 20;
          xorlib_offset = 0;
#endif
          o = 2;
          break;
      default:
#ifdef PIC32NTSC
          m = XOMODE_256x200_MONO;
          o = 4;
          xorlib_pitch = 8;
#endif          
#ifdef PIC32NTSCQ
          m = XOMODE_640x200_MONO;
          o = 2;
          l += 192;
          xorlib_pitch = 20;
#endif          
          xorlib_offset = 0;
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

    // divide Fpb by N, configure the I/O ports. 32 bit transfer
    SpiChnOpen(SPI_CHANNEL1, SPI_OPEN_ON | SPI_OPEN_MODE32 | SPI_OPEN_MSTEN , o);

    //=== DMA Channel 1 ================================
    // Open DMA Chan1 and chain from channel zero
    DmaChnOpen(1, 0, DMA_OPEN_DEFAULT);

    // setup system wide interrupts  ///
    INTEnableSystemMultiVectoredInt();

    if(mo==xorlib_curmode)
         o = 1;
    else o = 0;
    
#endif
#ifdef DEBUG
  xorlib_debug = fopen("xorlib.out","wt");
  if(xorlib_debug!=NULL) fprintf(xorlib_debug,"xofist\n");
#endif
  return o;
}

int xopalette(short p)
{
    register int y,*b = xorlib_screen_buffer;
    if(xorlib_pitch!=23) return 0;
    for(y=0;y<DY;y++)
    {
        switch(p)
        {
            case 0: b[0]=0x0CCCCCCC; b[1]=0xCC000000; break;
            case 1: b[0]=0x19999999; b[1]=0x98000000; break;
            case 2: b[0]=0x33333333; b[1]=0x30000000; break;
            case 3: b[0]=0x66666666; b[1]=0x60000000; break;
        }
        b += xorlib_pitch;
    }
    return 1;
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
    return xorlib_frames;
}

unsigned long xoseconds(void)
{
    return xorlib_seconds;
}

int xocurline(void)
{
    return xorlib_curline;
}

int xomode(void)
{
    return xorlib_curmode;
}

void xowaitvblank(void)
{
    unsigned long frame = xorlib_frames;
    while(xorlib_frames==frame);
}

int* xolinedirect(int y)
{
#ifdef PIC32ANY
   return &xorlib_screen_buffer[xorlib_pitch*y + xorlib_offset];
#endif
#ifdef DOS32
   
#endif
}

int* xonextline(int *p)
{
    return p + xorlib_pitch;
}

int* xoprevline(int *p)
{
    return p - xorlib_pitch;
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
    register int j = x>>2;
    register int shf = (3-(x&3))<<3;
    register int msk = ~(255<<shf);
    register unsigned char *ptr = font8x8[(c&255)-FONT8X8_FIRST];
    int *line_buffer = xolinedirect(y<<3);
    line_buffer[j]=(line_buffer[j] & msk)|(*(ptr++)<<shf);line_buffer=xonextline(line_buffer);
    line_buffer[j]=(line_buffer[j] & msk)|(*(ptr++)<<shf);line_buffer=xonextline(line_buffer);
    line_buffer[j]=(line_buffer[j] & msk)|(*(ptr++)<<shf);line_buffer=xonextline(line_buffer);
    line_buffer[j]=(line_buffer[j] & msk)|(*(ptr++)<<shf);line_buffer=xonextline(line_buffer);
    line_buffer[j]=(line_buffer[j] & msk)|(*(ptr++)<<shf);line_buffer=xonextline(line_buffer);
    line_buffer[j]=(line_buffer[j] & msk)|(*(ptr++)<<shf);line_buffer=xonextline(line_buffer);
    line_buffer[j]=(line_buffer[j] & msk)|(*(ptr++)<<shf);line_buffer=xonextline(line_buffer);
    line_buffer[j]=(line_buffer[j] & msk)|(*ptr<<shf);
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
    register int c = XOCOLOR_BRIGHT_WHITE;
    switch(i)
    {
        case 0: c=XOCOLOR_BLACK; break;
        case 1: c=XOCOLOR_GRAY; break;
        case 2: c=XOCOLOR_BRIGHT_GRAY; break;
        case 3: c=XOCOLOR_WHITE; break;
        case 4: c=XOCOLOR_BRIGHT_WHITE; break;
    }
    return c;
}

int xogray5a(int i)
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
   if(x<0 || x>=xowidth()) return 0;
   if(y<0 || y>=xoheight()) return 0;
   register int *line_buffer = xolinedirect(y);
   if (c > 0)
     line_buffer[x >> 5] |= 1<<(31-(x & 0x1f));
   else if (c==0)
     line_buffer[x >> 5] &= ~(1<<(31-(x & 0x1f)));
   else // c < 0
     line_buffer[x >> 5] ^= 1<<(31-(x & 0x1f));
   return 1;
}

int xoget(short x, short y)
{
    //The following construction detects exactly one bit at the x,y location
    register int* line_buffer = xolinedirect(y);
    return (line_buffer[(x >> 5) + (y * xorlib_pitch)] & (1<<(31-(x & 0x1f))))?1:0 ;
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
         if(xorlib_curcol > xotextwidth()-1) n++;
      }
   }
   else
   {
      xochar(xorlib_curcol++,xorlib_currow,*po);
      if(xorlib_curcol > xotextwidth()-1) n++;
   }
   if(n)
   {
      n = 0;
      xorlib_curcol = 0;
      if(++xorlib_currow > xotextheight()-1)
      {
         // TODO: scroll
         xorlib_currow = xorlib_maxrow;
      }
   }
   po++;
 }
 return o;
}

//==================================
//plot a line
//at x1,y1 to x2,y2 with color 1=white 0=black 2=invert
//NOTE: this function requires signed chars
//Code is from David Rodgers,
//"Procedural Elements of Computer Graphics",1985
int xoline(short x1, short y1, short x2, short y2, char c)
{
   int e;
   signed int dx,dy,j, temp;
   signed char s1,s2, xchange;
        signed int x,y;

   x = x1;
   y = y1;

   //take absolute value
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
    if(xomode()>=4)
    {
        xoline(x+1,y,x+1,y2,c);
        xoline(x2-1,y,x2-1,y2,c);
    }
}