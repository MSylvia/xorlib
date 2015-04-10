/* hello.c - A.A.Shabarshin (April 2015)

Default mode for PIC32NTSC (internal 8MHz) is XOMODE_256x200_MONO with text 32x25
Default mode for PIC32NTSCQ (external 14.318MHz) is XOMODE_640x200_MONO with text 80x25

*/

#include "xorlib.h"

int main()
{
 xoprintf("Hello, World!");
 return 0;
}
