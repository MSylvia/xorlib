/* mandelbr.c - A.A.Shabarshin (May 2015) */

/* THIS IS PUBLIC DOMAIN AND PROVIDED AS IS */

#include <stdio.h> /* for sprintf */

#include "xorlib.h"

/*
 FIXED32 is fixed point 4.28 - fast, but not precise enough to go deep
 FIXED64 is fixed point 8.56 - slower, but very precise 
 if both FIXED32 and FIXED64 are commented out then "long double" is used
*/

//#define FIXED32
#define FIXED64

/* 27 colors for Mandelbrot Set visualization (14 solid and 13 dithered) */

#ifdef FIXED32
/* just to make it a little faster for 32-bit version (216 iterations) */
#define MANLIMIT (27<<3)
#define MANCOLOR1(x) colors1[(x)>>3]
#define MANCOLOR2(x) colors2[(x)>>3]
#else
/* regularly it is 432 iterations max */
#define MANLIMIT (27<<4)
#define MANCOLOR1(x) colors1[(x)>>4]
#define MANCOLOR2(x) colors2[(x)>>4]
#endif

int colors[] = {15,15,6,6,7,7,2,2,3,3,1,1,11,11,9,9,8,8,13,13,12,12,14,14,4,4,0,0};

/* Fixed Point 4.28 */
#ifdef FIXED32
#define FIXED
#define REAL long
#define TOREAL(x) (long)((x)*268435456.0)
#define MUREAL(x,y) ((x)>>14)*((y)>>14)
#endif

/* Fixed Point 8.56 */
#ifdef FIXED64
#define FIXED
#define REAL long long
#if 0 /* this is not working in WATCOM-C v19 for negative numbers, so use longer version */
#define TOREAL(x) (long long)(((long double)(x))*72057594037927936.0)
#else
#define TOREAL(x) (((x)<0)?(-(long long)((0.0-(x))*72057594037927936.0)):((long long)((x)*72057594037927936.0)))
#endif
#define MUREAL(x,y) ((x)>>28)*((y)>>28)
#endif

/* 64-bit Floating Point */
#ifndef FIXED
#define REAL long double
#define TOREAL(x) ((long double)(x))
#define MUREAL(x,y) (x)*(y)
#endif

int main()
{
 char s[16];
 int y,c1,c2,c1n,c2n;
 unsigned char *p,saved[80];
 register int i,j;
 REAL x2,y2,d0,d1,d2,four; /* better be fast */
 REAL x0,y0,dx,dy,sx,sy,x1,y1,xc,yc,m,ratio,hstep,vstep;
 int *colors1 = &colors[1];
 int *colors2 = &colors[0];

 xoinit(XOMODE_160x200_COL15);

#if 1
 x0 = TOREAL(-3); y0 = TOREAL(1.5); dx = TOREAL(4); /* start view */
#else
// x0 = TOREAL(-0.219759); y0 = TOREAL(-0.813599); dx = TOREAL(0.000977);
// x0 = TOREAL(0.30026); y0 = TOREAL(0.025391); dx = TOREAL(0.007812);
 x0 = TOREAL(-0.175); y0 = TOREAL(-1.023438); dx = TOREAL(0.03125);
#endif
 xc = TOREAL(-0.16219); /* x coord of point of interest */
 yc = TOREAL(-1.03500); /* y coord of point of interest */
 m = TOREAL(0.97); /* scale coefficient between frames */
 four = TOREAL(4); /* value to check that point is outside */
#if 1
 ratio = TOREAL(3.0/4); /* assume 4:3 ratio (default) */
#else
 ratio = TOREAL(9.0/16); /* assume 16:9 ratio (for widescreen TV) */
#endif
 hstep = TOREAL(1.0/158); /* horizontal step for 158 pixels */
 vstep = TOREAL(1.0/98); /* vertical step for 98 pixels */
 while(1)
 {
  dy = MUREAL(dx,ratio);
  sx = MUREAL(dx,hstep);
  sy = MUREAL(dy,vstep);
  y1 = y0;
  for(y=0;y<200;y++)
  {
   p = xodirectline(y);
   if(y<2||y>=198)
   {   /* white frame */
       for(i=0;i<80;i++) p[i] = 0xFF;
       continue;
   }
   x1 = x0;
   if(y&1) for(i=0;i<80;i++) p[i] = saved[i];
   else 
   {
     for(i=0;i<80;i++)
     {
/* 
       Z[i+1] = Z[i]*Z[i] + C
       C is (x1,y1)
       Z is (x2,y2)
       Z[i]*Z[i] => (x2+y2*i)*(x2+y2*i) => x2*x2 + 2*x2*y2*i - y2*y2 => (x2*x2 - y2*y2, 2*x2*y2)
       Z[i]*Z[i] + C => (x2*x2 - y2*y2 + x1, 2*x2*y2 + y1)
*/
       if(i==0) c1=c1n=15;
       else
       {
         x2 = y2 = 0;
         j = 0;
         while(j++ < MANLIMIT)
         {
           d1 = MUREAL(x2,x2);
           d2 = MUREAL(y2,y2);
           if(d1 + d2 > four) break;
           d0 = MUREAL(x2,y2);
           x2 = d1 - d2 + x1;
           y2 = d0 + d0 + y1;
         }
         if(j>=MANLIMIT) c1=c1n=0;
         else
         {
           c1 = MANCOLOR1(j);
           c1n = MANCOLOR2(j);
         }
         x1 += sx;
       }
       if(i==79) c2=c2n=15;
       else
       {
         x2 = y2 = 0;
         j = 0;
         while(j++ < MANLIMIT)
         {
           d1 = MUREAL(x2,x2);
           d2 = MUREAL(y2,y2);
           if(d1 + d2 > four) break;
           d0 = MUREAL(x2,y2);
           x2 = d1 - d2 + x1;
           y2 = d0 + d0 + y1;
         }
         if(j>=MANLIMIT) c2=c2n=0;
         else
         {
           c2 = MANCOLOR1(j);
           c2n = MANCOLOR2(j);
         }
         x1 += sx;
       }
       p[i] = (c1<<4)|c2;
       saved[i] = (c1n<<4)|c2n;
     }
     y1 -= sy;
   }
  }
#if 1
  *((int*)s) = 2037542744; /* little-endian magic number ;) */
  y = xoseconds(); /* show minutes:seconds.hundreds (approx) */
  sprintf(s+4,"%c %i:%02d.%02d",97,y/60,y%60,(xoframes()%60)*5/3);
  xostring(1,23,s);
#endif
  x0 = xc - MUREAL(xc-x0,m);
  y0 = yc - MUREAL(yc-y0,m);
  dx = MUREAL(dx,m);
 }

 return 0;
}
