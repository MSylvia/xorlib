/* ETA_PLAY.C - A.A.Shabarshin (April 2015) - for Turbo-C 2.01
==============================================================

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

#include <stdio.h>
#include <conio.h>

/*

 Encoded Text Animation (ETA):
 =============================
 Header: '@',Cols,Rows,Number
 User: Number*8 - user characters (up to 32)
 Data: Sequence of bytes that ends with 0xF0:
 0xxxxxxx - ASCII character itself (including user space <0x20)
 100xxxxx - Repeat space 2...33 times (x+2)
 1010xxxx - Skip 1...16 characters (0 means 16)
 1011xxxx - GFX character itself (0xB0...0xBF)
 110xxxxx - GFX character itself (0xC0...0xDF)
 1110xxxx - Repeat next character 1...16 times (0 means 16) + 1 byte after that
 1111xxxx - Extended GFX character itself, but
 11110000 - End of the image/movie (0xF0)
 11110001 - End of the frame that should stay for N/30 sec and start from row M
            (0xF1 + 2 bytes N and M after that)

 Shaos, April 2015

*/

int main(int argc, char** argv)
{
 int i,c,n,m,w,h,x,y;
 FILE *f;
 if(argc<2)
 {
   printf("Usage: ETA_PLAY FILE.ETA\n");
   return -1;
 }
 f = fopen(argv[1],"rb");
 if(f==NULL)
 {
   printf("Can't open file '%s'!\n",argv[1]);
   return -2;
 }
 c = fgetc(f);
 if(c!='@')
 {
   printf("Invalid ETA file!\n");
   return -3;
 }
 w = fgetc(f);
 h = fgetc(f);
 c = fgetc(f);
 if(c!=0)
 {
   printf("This player doesn't support user defined characters!\n");
   return -3;
 }
 x = (80-w)/2;
 y = (25-h)/2;
 clrscr();
 window(x,y,x+w-1,y+h);
#if 0
 /* this is for later borlands */
 _setcursortype(_NOCURSOR);
#endif
 m = 0;
 while(c!=0xF0)
 {
   c = fgetc(f);
   switch(c>>4)
   {
     case 0:
     case 1:
     case 2:
     case 3:
     case 4:
     case 5:
     case 6:
     case 7:
       putch(c);
       break;
     case 8:
     case 9:
       n = (c & 0x1F) + 2;
       for(i=0;i<n;i++) putch(' ');
       break;
     case 10:
       n = c & 0x0F;
       if(n==0) n = 16;
       i = wherex() + n;
       if(i > w)
          gotoxy(i-w,wherey()+1);
       else
          gotoxy(i,wherey());
       break;
     case 11:
     case 12:
     case 13:
       putch(c);
       break;
     case 14:
       n = c & 0x0F;
       if(n==0) n = 16;
       c = fgetc(f);
       for(i=0;i<n;i++) putch(c);
       break;
     case 15:
       if(c==0xF0) break;
       if(c==0xF1)
       {
          m++;
          delay(33*fgetc(f));
          gotoxy(1,fgetc(f)+1);
          if(kbhit() && getch()==27) c=0xF0;
       }
       else putch(c);
       break;
   }
 }
 fclose(f);
 return 0;
}