/* colors.c - A.A.Shabarshin (April 2015) */

/* THIS PUBLIC DOMAIN SOURCE CODE IS PROVIDED AS IS */

#include "xorlib.h"

int main()
{

 int i,y;
 unsigned char *p;

 xoinit(XOMODE_160x200_COL15); /* gray colors 5 and 10 are identical */

/*
 0 - Default composite mode colors
 1 - Similar to CGA composite mode colors
 2 - Similar to Tandy composite mode colors
 3 - Similar to PCjr composite mode colors
*/
 xopalette(0);

 for(y=0;y<200;y++)
 {
   p = xodirectline(y);
   if(y<2||y>=198)
   {   /* white frame */
       for(i=0;i<80;i++) p[i] = 0xFF;
       continue;
   }
   p[0] = 0xF0;
   for(i=1;i<5;i++) p[i] = 0x00;
   for(i=5;i<10;i++) p[i] = 0x11;
   for(i=10;i<15;i++) p[i] = 0x22;
   for(i=15;i<20;i++) p[i] = 0x33;
   for(i=20;i<25;i++) p[i] = 0x44;
   for(i=25;i<30;i++) p[i] = 0x55;
   for(i=30;i<35;i++) p[i] = 0x66;
   for(i=35;i<40;i++) p[i] = 0x77;
   for(i=40;i<45;i++) p[i] = 0x88;
   for(i=45;i<50;i++) p[i] = 0x99;
   for(i=50;i<55;i++) p[i] = 0xAA;
   for(i=55;i<60;i++) p[i] = 0xBB;
   for(i=60;i<65;i++) p[i] = 0xCC;
   for(i=65;i<70;i++) p[i] = 0xDD;
   for(i=70;i<75;i++) p[i] = 0xEE;
   for(i=75;i<80;i++) p[i] = 0xFF;
 }

 return 0;
}
