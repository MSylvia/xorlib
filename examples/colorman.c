/* colorman.c - A.A.Shabarshin (May 2015) */

#include "xorlib.h"

/* 27 colors for Mandelbrot Set visualization (14 solid and 13 dithered) */

int main()
{

 int i,y;
 unsigned char *p;

 xoinit(XOMODE_160x200_COL15); /* gray colors 5 and 10 are identical */

 for(y=0;y<200;y++)
 {
   p = xodirectline(y);
   if(y<2||y>=198)
   {   /* white frame */
       for(i=0;i<80;i++) p[i] = 0xFF;
       continue;
   }

       i = 0;
       p[i++] = 0xF0; /* left column of white frame */
       for(;i<3;i++)  p[i] = 0x00; /* 0000 Black */
       for(;i<6;i++)  if(y&1) p[i] = 0x40; else p[i] = 0x04;
       for(;i<9;i++)  p[i] = 0x44; /* 0100 Hue=19 */
       for(;i<12;i++) if(y&1) p[i] = 0xE4; else p[i] = 0x4E;
       for(;i<15;i++) p[i] = 0xEE; /* 1110 Hue=31 */
       for(;i<18;i++) if(y&1) p[i] = 0xEC; else p[i] = 0xCE;
       for(;i<21;i++) p[i] = 0xCC; /* 1100 Hue=68 */
       for(;i<24;i++) if(y&1) p[i] = 0xDC; else p[i] = 0xCD;
       for(;i<27;i++) p[i] = 0xDD; /* 1101 Hue=96 */
       for(;i<30;i++) if(y&1) p[i] = 0xD8; else p[i] = 0x8D;
       for(;i<33;i++) p[i] = 0x88; /* 1000 Hue=118 */
       for(;i<36;i++) if(y&1) p[i] = 0x98; else p[i] = 0x89;
       for(;i<39;i++) p[i] = 0x99; /* 1001 Hue=149 */
       for(;i<42;i++) if(y&1) p[i] = 0xB9; else p[i] = 0x9B;
       for(;i<45;i++) p[i] = 0xBB; /* 1011 Hue=200 */
       for(;i<48;i++) if(y&1) p[i] = 0xB1; else p[i] = 0x1B;
       for(;i<51;i++) p[i] = 0x11; /* 0001 Hue=211 */
       for(;i<54;i++) if(y&1) p[i] = 0x31; else p[i] = 0x13;
       for(;i<57;i++) p[i] = 0x33; /* 0011 Hue=248 */
       for(;i<60;i++) if(y&1) p[i] = 0x32; else p[i] = 0x23;
       for(;i<63;i++) p[i] = 0x22; /* 0010 Hue=276 */
       for(;i<66;i++) if(y&1) p[i] = 0x72; else p[i] = 0x27;
       for(;i<69;i++) p[i] = 0x77; /* 0111 Hue=298 */
       for(;i<72;i++) if(y&1) p[i] = 0x76; else p[i] = 0x67;
       for(;i<75;i++) p[i] = 0x66; /* 0110 Hue=329 */
       for(;i<78;i++) if(y&1) p[i] = 0xF6; else p[i] = 0x6F;
       for(;i<80;i++) p[i] = 0xFF; /* 1111 White */
 }

 return 0;
}
