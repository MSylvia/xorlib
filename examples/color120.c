/* color120.c - A.A.Shabarshin (May 2015) */

/* THIS PUBLIC DOMAIN SOURCE CODE IS PROVIDED AS IS */

#include "xorlib.h"

#define DITHER

int main()
{
 int i,j,k,y;
 unsigned char *p;

 xoinit(XOMODE_160x200_COL15); /* gray colors 5 and 10 are identical */

/*
 0 - Default composite mode colors
 1 - Similar to CGA composite mode colors
 2 - Similar to Tandy composite mode colors
 3 - Similar to PCjr composite mode colors
*/
 xopalette(0);

 /* Display all color combinations 16 x 16 */

 for(y=0;y<192;y++)
 {
   p = xodirectline(y);
   if((y%12)>=2)
   {
     j = y/12;
     for(i=0;i<80;i++)
     {
        k = i/5;
        if(i%5)
        {
#ifdef DITHER
          if(y&1) p[i] = (j<<4)|k; else p[i] = (k<<4)|j;
#else
          if(y&1) p[i] = (k<<4)|k; else p[i] = (j<<4)|j;
#endif
        }
        else
        {
          if(y&1) p[i] = k; else p[i] = j;
        }
     }
   }
 }

 return 0;
}
