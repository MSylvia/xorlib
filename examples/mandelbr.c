/* mandelbr.c - A.A.Shabarshin (May 2015) */

#include "xorlib.h"

/* 27 colors for Mandelbrot Set visualization (14 solid and 13 dithered) */

#define MANLIMIT (27 << 4)
#define MANCOLOR1(x) colors1[(x) >> 4]
#define MANCOLOR2(x) colors2[(x) >> 4]

int colors[] = {15,15,6,6,7,7,2,2,3,3,1,1,11,11,9,9,8,8,13,13,12,12,14,14,4,4,0,0};

int main()
{

 int i,j,y,c1,c2,c1n,c2n;
 unsigned char *p,saved[80];
 long double x0,y0,dx,dy,sx,sy,x1,y1,x2,y2,xc,yc,d1,d2,m;
 int *colors1 = &colors[1];
 int *colors2 = &colors[0];

 xoinit(XOMODE_160x200_COL15); /* gray colors 5 and 10 are identical */

#if 1
 x0 = -3.0; y0 = 1.5; dx = 4.0; /* start view */
#else
// x0 = -0.219759; y0 = -0.813599; dx = 0.000977;
// x0 =  0.300260; y0 =  0.025391; dx = 0.007812;
 x0 = -0.175000; y0 = -1.023438; dx = 0.031250;
#endif
 xc = -0.16219; /* x coord of point of interest */
 yc = -1.03500; /* y coord of point of interest */
 m = 0.95; /* scale coefficient between frames */ 
 while(1)
 {
  dy = dx*0.75; /* assume 4:3 ratio */
  sx = dx*0.00632911; /* horizontal step for 158 pixels */
  sy = dy*0.01020408; /* vertical step for 98 pixels */
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
       Z[i]*Z[i] => (x2+y2*i)*(x2+y2*i) => x2*x2 + 2*x2*y2*i - y2*y2 =>
       => (x2*x2 - y2*y2, 2*x2*y2)
       Z[i]*Z[i] + C => (x2*x2 - y2*y2 + x1, 2*x2*y2 + y1)
*/
       if(i==0) c1=c1n=15;
       else
       {
         x2 = y2 = 0;
         j = 0;
         while(j++ < MANLIMIT)
         {
           d1 = x2*x2;
           d2 = y2*y2;
           if(d1 + d2 > 4) break;
           y2 = 2*x2*y2 + y1;
           x2 = d1 - d2 + x1;
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
           d1 = x2*x2;
           d2 = y2*y2;
           if(d1 + d2 > 4) break;
           y2 = 2*x2*y2 + y1;
           x2 = d1 - d2 + x1;
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
  x0 = xc - (xc-x0)*m;
  y0 = yc - (yc-y0)*m;
  dx *= m;
 }

 return 0;
}
