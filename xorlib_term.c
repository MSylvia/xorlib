/* xorlib_term.c - terminal emulation of XORLib through ncurses

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
#include <ncurses.h>
#include <stdio.h>

#ifdef DEBUG
extern FILE* xorlib_debug;
#endif

unsigned long xorlib_frames = 0;
unsigned long xorlib_seconds = 0;
short xorlib_curline = 0;
unsigned char xorlib_maxcol = 79;
unsigned char xorlib_maxrow = 24;
unsigned char xorlib_curcol = 0;
unsigned char xorlib_currow = 0;
unsigned char xorlib_curmode = -1;
unsigned char xorlib_first = 1;

void xoexit(void)
{
#ifdef DEBUG
  if(xorlib_debug!=NULL) fprintf(xorlib_debug,"xoexit\n");
#endif
  nodelay(stdscr, FALSE);
  getch();
#ifdef DEBUG
  if(xorlib_debug!=NULL) fclose(xorlib_debug);
#endif
  endwin();
}

void xofirst(void)
{
  xorlib_first = 0;
  atexit(xoexit);
  initscr();
  raw();
  keypad(stdscr, TRUE);
  noecho();
  nodelay(stdscr, TRUE);
#ifdef DEBUG
  xorlib_debug = fopen("xorlib.out","wt");
  if(xorlib_debug!=NULL) fprintf(xorlib_debug,"xofist\n");
#endif
}

unsigned long xoconfig(void)
{
  return 0;
}

unsigned long xocontrols(void)
{
  unsigned long controls = 0;
  switch(getch())
  {
     case KEY_LEFT:  controls |= XOCONTROL_LEFT;  break;
     case KEY_RIGHT: controls |= XOCONTROL_RIGHT; break;
     case KEY_UP:    controls |= XOCONTROL_UP;    break;
     case KEY_DOWN:  controls |= XOCONTROL_DOWN;  break;
     case ' ':       controls |= XOCONTROL_FIRE;  break;
  }
  return controls;
}

void xowaitvblank(void)
{
}

int xochar(unsigned char x, unsigned char y, char c)
{
  int i,j;
  if(xorlib_first) xofirst();
#ifdef DEBUG
  if(xorlib_debug!=NULL) fprintf(xorlib_debug,"xochar %i,%i,'%c'#%2.2X\n",x,y,(c>=32)?c:' ',c);
#endif
  if(x>=0 && x<=xorlib_maxcol && y>=0 && y<=xorlib_maxrow)
  {
     mvaddch(y,x,c);
  }
  else
  {
#ifdef DEBUG
     if(xorlib_debug!=NULL) fprintf(xorlib_debug,"ERROR: Out of bounds!\n");
#endif
  }
  refresh();
  return 1;
}


