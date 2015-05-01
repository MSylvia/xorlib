/* UNIX2DOS.C - A.A.Shabarshin (May 2015) */

#include <stdio.h>

#define TMPFILE "__TMP__"

int main(int argc, char** argv)
{
 int c;
 unsigned long l,sz;
 FILE *f,*fo;
 if(argc<2)
 {
   printf("Usage:\n\tDOS2UNIX filename\n\n");
   return -1;
 }
 fo = fopen(TMPFILE,"wb");
 if(fo==NULL)
 {
   printf("Can't open temporary file!\n");
   return -2;
 }
 f = fopen(argv[1],"rb");
 if(f==NULL)
 {
   fclose(fo);
   printf("Can't open '%s'\n",argv[1]);
   return -3;
 }
 fseek(f,0,SEEK_END);
 sz = ftell(f);
 fseek(f,0,SEEK_SET);
 for(l=0;l<sz;l++)
 {
   c = fgetc(f);
   if(c==0x0D)
   {
      fclose(f);
      fclose(fo);
      unlink(TMPFILE);
      printf("File '%s' is already in DOS format!\n",argv[1]);
      return -4;
   }
   if(c==0x0A) fputc(0x0D,fo);
   fputc(c,fo);
 }
 fclose(f);
 fclose(fo);
 unlink(argv[1]);
 rename(TMPFILE,argv[1]);
 printf("Ok\n");
 return 0;
}
