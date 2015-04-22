/* coloroto.c - A.A.Shabarshin (April 2015) */

#include "xorlib.h"
#include <stdio.h>

/* Lets keep it in RAM */
unsigned char sinus[90] = { 0 , 4 , 8 , 13 , 17 , 22 , 26 , 31 , 35 , 40 , 44 , 48 , 53 ,
  57 , 61 , 66 , 70 , 74 , 79 , 83 , 87 , 91 , 95 , 100 , 104 , 108 , 112 ,
  116 , 120 , 124 , 128 , 131 , 135 , 139 , 143 , 146 , 150 , 154 , 157 ,
  161 , 164 , 167 , 171 , 174 , 177 , 181 , 184 , 187 , 190 , 193 , 196 ,
  198 , 201 , 204 , 207 , 209 , 212 , 214 , 217 , 219 , 221 , 223 , 226 ,
  228 , 230 , 232 , 233 , 235 , 237 , 238 , 240 , 242 , 243 , 244 , 246 ,
  247 , 248 , 249 , 250 , 251 , 252 , 252 , 253 , 254 , 254 , 255 , 255 ,
  255 , 255 , 255 };

/* Macros for faster calculations */
#define SIN(x) ((x<90)?sinus[x]:((x<180)?sinus[179-x]:((x<270)?(-sinus[x-180]):(-sinus[359-x]))))
#define COS(x) ((x<90)?sinus[89-x]:((x<180)?(-sinus[x-90]):((x<270)?(-sinus[269-x]):sinus[x-270])))

int div10[160]; /* to make fast division by 10 */

int main()
{

 register int x,b,c,r,a=0;
 int y,*p,s=60;
 unsigned long f;
 
 xoinit(XOMODE_160x200_COL15); /* gray colors 5 and 10 are identical */

/*
 0 - Default composite mode colors
 1 - Similar to CGA composite mode colors
 2 - Similar to Tandy composite mode colors
 3 - Similar to PCjr composite mode colors
*/
 xopalette(1);

 /* fill array for faster division by 10 */
 for(c=0;c<160;c++) div10[c] = c/10;
 
/*
 X' = X*COS(a) - Y*SIN(a) + X0
 Y' = X*SIN(a) + Y*COS(a) + Y0
*/
 while(1)
 {
  /* it takes about full frame (1/60 sec) with -O1 option: */
  for(y=0;y<200;y++) /* go through every horizontal line */
  {
   p = xodirectline(y); /* get pointer to the videomemory of line Y */
   if(!p) break;
   b = 80 - (((y-100)*SIN(a))>>9);
   for(x=0;x<160;x++) /* go through every pixel in this line */
   {
     /* we need only X' to get color, so we will ignore Y' for now */
     c = (((x-80)*COS(a))>>8) + b;
     if(c<0 || c>=160) c = 5; /* lets be gray outside of the color chart */
     else c = div10[c]; /* calculate color value - it has to be from 0 to 15 */
     r = (!(x&7))?c:((r<<4)|c); /* append integer with next pixel */
     if((x&7)==7) p[x>>3] = r; /* copy assembled integer into video memory */
   }
  }
  if(++a==360) a = 0; /* rotate and check that it is in the range */
#if 1
  /* wait some number of frames in the beginning and then no wait */
  if(s>1)
  {
    f = xoframes() + (255-sinus[89-s--]);
    while(xoframes() < f);
  }
#endif
 }

 return 0;
}
