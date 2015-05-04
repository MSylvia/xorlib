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
       p[i++] = 0xF0;
       for(;i<3;i++)  p[i] = 0x00;
       for(;i<6;i++)  if(y&1) p[i] = 0x40; else p[i] = 0x04;
       for(;i<9;i++)  p[i] = 0x44; /* 0100 */
       for(;i<12;i++) if(y&1) p[i] = 0xE4; else p[i] = 0x4E;
       for(;i<15;i++) p[i] = 0xEE; /* 1110 */
       for(;i<18;i++) if(y&1) p[i] = 0xEC; else p[i] = 0xCE;
       for(;i<21;i++) p[i] = 0xCC; /* 1100 */
       for(;i<24;i++) if(y&1) p[i] = 0xC8; else p[i] = 0x8C;
       for(;i<27;i++) p[i] = 0x88; /* 1000 */
       for(;i<30;i++) if(y&1) p[i] = 0xD8; else p[i] = 0x8D;
       for(;i<33;i++) p[i] = 0xDD; /* 1101 */ 
       for(;i<36;i++) if(y&1) p[i] = 0xD9; else p[i] = 0x9D;
       for(;i<39;i++) p[i] = 0x99; /* 1001 */
       for(;i<42;i++) if(y&1) p[i] = 0x91; else p[i] = 0x19;
       for(;i<45;i++) p[i] = 0x11; /* 0001 */
       for(;i<48;i++) if(y&1) p[i] = 0xB1; else p[i] = 0x1B;
       for(;i<51;i++) p[i] = 0xBB; /* 1011 */
       for(;i<54;i++) if(y&1) p[i] = 0xB3; else p[i] = 0x3B;
       for(;i<57;i++) p[i] = 0x33; /* 0011 */
       for(;i<60;i++) if(y&1) p[i] = 0x32; else p[i] = 0x23;
       for(;i<63;i++) p[i] = 0x22; /* 0010 */
       for(;i<66;i++) if(y&1) p[i] = 0x72; else p[i] = 0x27;
       for(;i<69;i++) p[i] = 0x77; /* 0111 */
       for(;i<72;i++) if(y&1) p[i] = 0x76; else p[i] = 0x67;
       for(;i<75;i++) p[i] = 0x66; /* 0110 */
       for(;i<78;i++) if(y&1) p[i] = 0xF6; else p[i] = 0x6F;
       for(;i<80;i++) p[i] = 0xFF; /* 1111 */
 }

 return 0;
}
