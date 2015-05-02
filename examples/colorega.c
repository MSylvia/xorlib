/* colorega.c - A.A.Shabarshin (May 2015) */

#include "xorlib.h"

int main()
{
 int i,y;
 unsigned char *p;

 xoinit(XOMODE_160x200_COL15); /* gray colors 5 and 10 are identical */

 /* Imitator of default EGA palette */

 for(y=0;y<200;y++)
 {
   p = xodirectline(y);
   if(y<2||y>=198)
   {   /* white frame */
       for(i=0;i<80;i++) p[i] = 0xFF;
       continue;
   }
   p[0] = 0xF0;
   for(i=1;i<5;i++)   p[i] = 0x00; /* 0 */
   for(i=5;i<10;i++)  p[i] = 0x11; /* 1 */
   for(i=10;i<15;i++) p[i] = 0x88; /* 2 */
   for(i=15;i<20;i++) if(y&1) p[i] = 0x91; else p[i] = 0x19; /* 3 */
   for(i=20;i<25;i++) p[i] = 0x44; /* 4 */
   for(i=25;i<30;i++) p[i] = 0x22; /* 5 */
   for(i=30;i<35;i++) if(y&1) p[i] = 0xC4; else p[i] = 0x4C; /* 6 */
   for(i=35;i<40;i++) if(y&1) p[i] = 0xF5; else p[i] = 0x5F; /* 7 */
   for(i=40;i<45;i++) if(y&1) p[i] = 0x50; else p[i] = 0x05; /* 8 */
   for(i=45;i<50;i++) p[i] = 0x33; /* 9 */
   for(i=50;i<55;i++) p[i] = 0xDD; /* 10 */
   for(i=55;i<60;i++) p[i] = 0xBB; /* 11 */
   for(i=60;i<65;i++) p[i] = 0x66; /* 12 */
   for(i=65;i<70;i++) p[i] = 0x77; /* 13 */
   for(i=70;i<75;i++) p[i] = 0xEE; /* 14 */
   for(i=75;i<80;i++) p[i] = 0xFF; /* 15 */
 }

 return 0;
}
