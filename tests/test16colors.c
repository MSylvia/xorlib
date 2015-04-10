/* test16colors.c - A.A.Shabarshin (April 2015) */

#include <stdio.h>
#include <math.h>

#define RGB(r,g,b) (r|(g<<8)|(b<<16))

unsigned long actual[16] = {
 RGB(  4,   4,  12),
 RGB(  1,  19, 159),
 RGB( 70,   2, 172),
 RGB(  1,  86, 255),
 RGB(102,  15,  15),
 RGB(122, 133, 137),
 RGB(249,  49, 131),
 RGB(219, 118, 254),
 RGB(  0,  99,  34),
 RGB(  1, 207, 133),
 RGB(127, 133, 125),
 RGB( 15, 188, 253),
 RGB(138, 185,  54),
 RGB(102, 253,  90),
 RGB(253, 182,  74),
 RGB(220, 217, 202),
};

// 0x00 -> 0
// 0x55 -> 85
// 0xAA -> 170
// 0xFF -> 255

unsigned long desirable[16] = {
 RGB(  0,  0,  0),
 RGB(  0,  0,170),
 RGB(  0,170,  0),
 RGB(  0,170,170),
 RGB(170,  0,  0),
 RGB(170,  0,170),
 RGB(170, 85,  0),
 RGB(170,170,170),
 RGB( 85, 85, 85),
 RGB( 85, 85,255),
 RGB( 85,255, 85),
 RGB( 85,255,255),
 RGB(255, 85, 85),
 RGB(255, 85,255),
 RGB(255,255, 85),
 RGB(255,255,255)
};

int main()
{
 int i,j,me,mj;
 double d,c1,c2;

 printf("\n1-color mix:\n\n\t");
 for(i=0;i<16;i++) printf("X[%i]\t",i);
 printf("\n");
 for(i=0;i<16;i++)
 {
   printf("E[%i]\t",i,actual[i]);
   me = 1000;
   mj = -1;
   for(j=0;j<16;j++)
   {
      d = 0;
      c1 = actual[j]&255;
      c2 = desirable[i]&255;
      d += (c1-c2)*(c1-c2);
      c1 = (actual[j]>>8)&255;
      c2 = (desirable[i]>>8)&255;
      d += (c1-c2)*(c1-c2);
      c1 = (actual[j]>>16)&255;
      c2 = (desirable[i]>>16)&255;
      d += (c1-c2)*(c1-c2);
      d = sqrt(d);
      printf("%3.2lf\t",d);
      if(d < me)
      {
        me = (int)d;
        if(d-me >= 0.5) me++;
        mj = j;
      }
   }
   printf("[%i]\n",mj);
 }

 printf("\n2-color mix:\n\n");
 for(i=0;i<16;i++)
 {
   printf("E[%i]\t",i,actual[i]);
   me = 1000;
   mj = -1;
   for(j=0;j<256;j++)
   {
      d = 0;
      c1 = ((actual[j&15]&255)+(actual[j>>4]&255))/2.0;
      c2 = desirable[i]&255;
      d += (c1-c2)*(c1-c2);
      c1 = (((actual[j&15]>>8)&255)+((actual[j>>4]>>8)&255))/2.0;
      c2 = (desirable[i]>>8)&255;
      d += (c1-c2)*(c1-c2);
      c1 = (((actual[j&15]>>16)&255)+((actual[j>>4]>>16)&255))/2.0;
      c2 = (desirable[i]>>16)&255;
      d += (c1-c2)*(c1-c2);
      d = sqrt(d);
//      printf("%3.2lf\t",d);
      if(d < me)
      {
        me = (int)d;
        if(d-me >= 0.5) me++;
        mj = j;
      }
   }
   printf("#%2.2X ERR = %i\t%c\n",mj,me,((mj>>4)==(mj&15))?' ':'!');
 }

 return 0;
}
