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

#include "xorlib.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

extern unsigned char xorlib_maxcol;
extern unsigned char xorlib_maxrow;
extern unsigned char xorlib_curcol;
extern unsigned char xorlib_currow;
extern unsigned char xorlib_curmode;

#define XORLIB_TEMPSTR_SZ 1024

char xorlib_tempstr[XORLIB_TEMPSTR_SZ];

#ifdef DEBUG
FILE* xorlib_debug = NULL;
#endif

int xoprintf(char *s,...)
{
 char *po;
 int n,o = 0;
 va_list arg;
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
         // TODO: scroll
         xorlib_currow = xorlib_maxrow;
      }
   }
   po++;
 }
 return o;
}
