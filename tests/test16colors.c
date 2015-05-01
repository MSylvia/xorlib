/* test16colors.c - A.A.Shabarshin (Apr-May 2015) */

#include <stdio.h>
#include <math.h>

#define RGB(r,g,b) (r|(g<<8)|(b<<16))

unsigned long actual[16] = {
#if 0
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
 RGB(220, 217, 202)
#else
 RGB(0  , 0  , 0  ), /* #000000 H=0   S=0   V=0   0000 */
 RGB(0  , 81 , 169), /* #0051A9 H=211 S=100 V=66  0001 */
 RGB(123, 6  , 199), /* #7B06C7 H=276 S=97  V=78  0010 */
 RGB(110, 88 , 255), /* #6E58FF H=248 S=65  V=100 0011 */
 RGB(140, 45 , 0  ), /* #8C2D00 H=19  S=100 V=55  0100 */
 RGB(127, 127, 127), /* #7F7F7F H=0   S=0   V=50  0101 */
 RGB(255, 52 , 157), /* #FF349D H=329 S=80  V=100 0110 */
 RGB(250, 134, 255), /* #FA86FF H=298 S=47  V=100 0111 */
 RGB(4  , 120, 0  ), /* #047800 H=118 S=100 V=47  1000 */
 RGB(0  , 202, 97 ), /* #00CA61 H=149 S=100 V=79  1001 */
 RGB(127, 127, 127), /* #7F7F7F H=0   S=0   V=50  1010 */
 RGB(114, 209, 255), /* #72D1FF H=200 S=55  V=100 1011 */
 RGB(144, 166, 0  ), /* #90A600 H=68  S=100 V=65  1100 */
 RGB(131, 248, 55 ), /* #83F837 H=96  S=78  V=97  1101 */
 RGB(255, 173, 85 ), /* #FFAD55 H=31  S=67  V=100 1110 */
 RGB(255, 255, 255)  /* #FFFFFF H=0   S=0   V=100 1111 */
#endif
};

/*
 0x00 -> 0
 0x55 -> 85
 0xAA -> 170
 0xFF -> 255
*/

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
 int i,j,mf,me,mj,r,g,b;
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
   me = 1000;
   mj = -1;
   for(j=0;j<256;j++)
   {
      d = 0;
      c1 = ((actual[j&15]&255)+(actual[j>>4]&255))/2.0;
      r = c1; if(c1-r>=0.5) r++; if(r>255) r=255;
      c2 = desirable[i]&255;
      d += (c1-c2)*(c1-c2);
      c1 = (((actual[j&15]>>8)&255)+((actual[j>>4]>>8)&255))/2.0;
      g = c1; if(c1-g>=0.5) g++; if(g>255) g=255;
      c2 = (desirable[i]>>8)&255;
      d += (c1-c2)*(c1-c2);
      c1 = (((actual[j&15]>>16)&255)+((actual[j>>4]>>16)&255))/2.0;
      b = c1; if(c1-b>=0.5) b++; if(b>255) b=255;
      c2 = (desirable[i]>>16)&255;
      d += (c1-c2)*(c1-c2);
      d = sqrt(d);
      switch(i)
      {
        case 0: if(j==0x00) mf=(int)d; break;
        case 1: if(j==0x11) mf=(int)d; break;
        case 2: if(j==0x88) mf=(int)d; break;
        case 3: if(j==0x99) mf=(int)d; break;
        case 4: if(j==0x44) mf=(int)d; break;
        case 5: if(j==0x22) mf=(int)d; break;
        case 6: if(j==0xCC) mf=(int)d; break;
        case 7: if(j==0x55) mf=(int)d; break;
        case 8: if(j==0xAA) mf=(int)d; break;
        case 9: if(j==0x33) mf=(int)d; break;
        case 10:if(j==0xDD) mf=(int)d; break;
        case 11:if(j==0xBB) mf=(int)d; break;
        case 12:if(j==0x66) mf=(int)d; break;
        case 13:if(j==0x77) mf=(int)d; break;
        case 14:if(j==0xEE) mf=(int)d; break;
        case 15:if(j==0xFF) mf=(int)d; break;
      }
      if(d < me)
      {
        me = (int)d;
        if(d-me >= 0.5) me++;
        mj = j;
      }
      if(i==0) printf("%3d %3d %3d [%i] 0x%2.2X\n",r,g,b,j,j);
      if(i==0 && j==255) printf("\n");
   }
   printf("E[%i]\t#%2.2X ERR=%i\tERR'=%i\t%c\n",i,mj,me,mf,((mj>>4)==(mj&15))?' ':'!');
 }

 return 0;
}

