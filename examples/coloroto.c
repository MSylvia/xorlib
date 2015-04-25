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
 register unsigned char* p;
 register int x,x2,c,r;
 register short co,si;
 int y,s=60,a=0;
 unsigned long f=0;
 char str[16];

 xoinit(XOMODE_160x200_COL15); /* gray colors 5 and 10 are identical, so it's 15 unique colors */

/*
 0 - Default composite mode colors
 1 - Similar to CGA composite mode colors
 2 - Similar to Tandy composite mode colors
 3 - Similar to PCjr composite mode colors
*/
 xopalette(0);

 /* fill array for faster division by 10 */
 for(c=0;c<160;c++) div10[c] = c/10;

/*
 X' = X*COS(a) - Y*SIN(a) + X0
 Y' = X*SIN(a) + Y*COS(a) + Y0
*/
 while(1)
 {
  co = COS(a); /* precalculate cosine here */
  si = SIN(a); /* precalculate sine here */
  for(y=0;y<200;y++) /* go through every horizontal line */
  {
   p = xodirectline(y); /* get pointer to the videomemory of line Y */
   if(!p) break;
   x2 = 80 - (((y-100)*si)>>9); /* this is Y portion of the formula */
   for(x=0;x<160;x+=2) /* go through every 2 pixels in this line */
   {
     /* we need only X' to get color value for bar, so we will ignore Y' for now */
     c = (((x-80)*co)>>8) + x2; /* x2 was calculated before this loop */
     if(c<0 || c>=160) r = 0x50; /* lets be gray outside of the color chart */
     else r = div10[c]<<4; /* calculate color value of 1st pixel (0...15) */
     /* calculate color of 2nd pixel and store assembled byte with 2 pixels */
     c = (((x-79)*co)>>8) + x2; /* x2 was calculated before this loop */
     if(c<0 || c>=160) p[x>>1] = r|5; /* lets be gray outside of the color chart */
     else p[x>>1] = r|div10[c]; /* calculate color value of 2nd pixel (0...15) */
   }
  }
  if(++a==360) a = 0; /* rotate and check that it's in the range */
#if 1
  /* wait some number of frames in the beginning */
  if(s>1)
  {
    f = xoframes() + (255-sinus[89-s--]);
    while(xoframes() < f);
  }
#else
  /* measure performance - it has to be less than 0.200 to fit 1 frame */
  sprintf(str,"%i.%i ",xoframes()-f-1,xocurline());
  xostring(0,24,str);
#endif
  xowaitretrace();
 }

 return 0;
}
