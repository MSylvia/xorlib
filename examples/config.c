/* config.c - A.A.Shabarshin (April 2015) */

#include "xorlib.h"

int main()
{
 unsigned long conf = xoconfig();

 xoprintf("XORLib v%i.%i configuration:\n",
   XORLIB_VERSION>>8, XORLIB_VERSION&0xFF);

 if(conf & (1<<XOMODE_320x200_MONO)) 
   xoprintf("XOMODE_320x200_MONO\n");
 if(conf & (1<<XOMODE_160x100_GRAY5))
   xoprintf("XOMODE_160x100_GRAY5\n");
 if(conf & (1<<XOMODE_640x200_MONO))
   xoprintf("XOMODE_640x200_MONO\n");
 if(conf & (1<<XOMODE_320x100_GRAY5))
   xoprintf("XOMODE_320x100_GRAY5\n");
 if(conf & (1<<XOMODE_160x200_COL15))
   xoprintf("XOMODE_160x200_COL15\n");
 if(conf & (1<<XOMODE_160x100_COL120))
   xoprintf("XOMODE_160x100_COL120\n");
 if(conf & (1<<XOMODE_320x200_COL4))
   xoprintf("XOMODE_320x200_COL4\n");
 if(conf & (1<<XOMODE_160x200_COL16))
   xoprintf("XOMODE_160x200_COL16\n");
 if(conf & (1<<XOMODE_320x200_COL16))
   xoprintf("XOMODE_320x200_COL16\n");
 if(conf & (1<<XOMODE_160x200_COL256))
   xoprintf("XOMODE_160x200_COL256\n");
 if(conf & (1<<XOMODE_640x200_COL16))
   xoprintf("XOMODE_640x200_COL16\n");
 if(conf & (1<<XOMODE_320x200_COL256))
   xoprintf("XOMODE_320x200_COL256\n");
 if(conf & (1<<XOMODE_640x350_COL16))
   xoprintf("XOMODE_640x350_COL16\n");
 if(conf & (1<<XOMODE_640x480_COL16))
   xoprintf("XOMODE_640x480_COL16\n");
 if(conf & (1<<XOMODE_800x600_COL16))
   xoprintf("XOMODE_800x600_COL16\n");
 if(conf & (1<<XOMODE_EXTENDED_VGA))
   xoprintf("XOMODE_EXTENDED_VGA\n");

 if(conf & XOCONFIG_BIGENDIAN)
   xoprintf("BIG ENDIAN\n");
 else
   xoprintf("LITTLE ENDIAN\n");
 if(conf & XOCONFIG_32BITINT)
   xoprintf("32-BIT INT\n");
 else
   xoprintf("16-BIT INT\n");
 if(conf & XOCONFIG_NTSCTV)
   xoprintf("NTSC TV\n");
 if(conf & XOCONFIG_PALTV)
   xoprintf("PAL TV\n");
 if(!(conf & XOCONFIG_NTSCTV)&&
    !(conf & XOCONFIG_PALTV))
   xoprintf("MONITOR\n");
 if(conf & XOCONFIG_NETWORK)
   xoprintf("NETWORK\n");
 if(conf & XOCONFIG_KEYBOARD)
   xoprintf("KEYBOARD\n");
 if(conf & XOCONFIG_MOUSE)
   xoprintf("MOUSE\n");
 if(conf & XOCONFIG_SDCARD)
   xoprintf("SD-CARD\n");
 if(conf & XOCONFIG_CDROM)
   xoprintf("CD-ROM\n");
 
 return 0;
}
