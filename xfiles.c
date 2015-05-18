/* xfiles.c - encapsulation of compiled files on PIC and using actual files on DOS */

#include "xorlib.h"
#include <stdio.h>

#ifdef XFILES
#define XOSTR(x) #x
#define XOCO4(w,x,y,z) w##x##y##z
#ifdef PIC32NTSC
#define XFILES_INC
#endif
#ifdef PIC32NTSCQ
#define XFILES_INC
#endif
#ifdef XFILES_INC

/* include your BIN2C-generated files or XBM-files below */

#include "xorya2_p32.c" /* this file is required for JPG and multicolor GIF decoding */

#define ADDFILE(name,ext,w,h) { XOSTR(name) "." XOSTR(ext), XOCO4(name,_,ext,_data), XOCO4(name,_,ext,_size), w, h },
#else
#define ADDFILE(name,ext,w,h) { XOSTR(name) "." XOSTR(ext), NULL, 0, w, h },
#endif

/* this is a list of included files with additional information */

struct xfiles_struct
{
  char filename[16]; /* better be 8.3 for DOS port compatibility */
  const unsigned char* data; /* pointer to included array for PIC or loaded file for DOS */
  unsigned int size; /* size of the file in bytes */
  unsigned short width,height; /* optional width and height */
} xfiles[] = {
/*
  Add information about your files below (one file per line) in form

  ADDFILE(filename,extension,width,height) // for images (gif,jpg,myg,eta)
  ADDFILE(filename,extension,frequency,0)  // for raw sound files (unsigned 8-bit)
  ADDFILE(filename,extension,0,0)          // for other files

*/
  ADDFILE(xorya2,p32,-1,-1) /* this line should be last one and negative numbers may be used only here */
};

/* DON'T TOUCH CODE BELOW !!! */

int xocopyfile(struct xoimage* d, const char* n, int f)
{

}

int xoplayfile(unsigned char where, unsigned char volume, unsigned short playfreq, const char* n)
{

}


#endif
