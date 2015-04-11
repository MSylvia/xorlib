/* colors.h - A.A.Shabarshin (April 2015) */

#include "xorlib.h"

int main()
{

 int y,*p;

 xoinit(XOMODE_160x200_COL16);

 // 0 - Default composite mode colors
 // 1 - Similar to CGA composite mode colors
 // 2 - Similar to Tandy composite mode colors
 // 3 - Similar to PCjr composite mode colors
 xopalette(0);

 for(y=0;y<200;y++)
 {
   p = xolinedirect(y);
   p[0] = 0x00000000;
   p[1] = 0x00111111;
   p[2] = 0x11112222;
   p[3] = 0x22222233;
   p[4] = 0x33333333;
   p[5] = 0x44444444;
   p[6] = 0x44555555;
   p[7] = 0x55556666;
   p[8] = 0x66666677;
   p[9] = 0x77777777;
   p[10] = 0x88888888;
   p[11] = 0x88999999;
   p[12] = 0x9999AAAA;
   p[13] = 0xAAAAAABB;
   p[14] = 0xBBBBBBBB;
   p[15] = 0xCCCCCCCC;
   p[16] = 0xCCDDDDDD;
   p[17] = 0xDDDDEEEE;
   p[18] = 0xEEEEEEFF;
   p[19] = 0xFFFFFFFF;
 }

 xorect(0,0,xowidth(),xoheight(),15);

 return 0;
}
