/* starwars.c - converter to Encoded Text Animation (ETA) for XORLib demo
=========================================================================

Copyright (c) 2015 A.A.Shabarshin <me@shaos.net>

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom
the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*

 It's a converter from http://www.asciimation.co.nz (Simon Jansen)
 to my ETA (Encoded Text Animation) format for XORLib devices.

 Encoded Text Animation (ETA):
 =============================
 Header: '@',Cols,Rows,Number
 User: Number*8 - user characters (up to 32)
 Data: Sequence of bytes that ends with 0xF0:
 0xxxxxxx - ASCII character itself (including user space <0x20)
 100xxxxx - Repeat space 2...33 times (x+2)
 1010xxxx - Skip 1...16 characters (0 means 16)
 1011xxxx - GFX character itself (0xB0...0xBF)
 110xxxxx - GFX character itself (0xC0...0xDF)
 1110xxxx - Repeat next character 1...16 times (0 means 16) + 1 byte after that
 1111xxxx - Extended GFX character itself, but
 11110000 - End of the image/movie (0xF0)
 11110001 - End of the frame that should stay for N/30 sec and start from row M
            (0xF1 + 2 bytes N and M after that)

 Shaos, April 2015

*/

#define KEYFRAMES (10*30)

int total = 0;
int cols = 67;
int rows = 13;
int row,col,fir=2;
char screen[80][25];
char screen_[80][25];

void cls(void)
{
 int i,j;
 for(j=0;j<25;j++){
 for(i=0;i<80;i++){
   screen[i][j] = ' ';
 }}
}

int readscreen(int i, int checkprev)
{
 int irow = i/cols;
 int icol = i%cols;
 if(checkprev && screen[icol][irow]==screen_[icol][irow]) return 0x100+screen[icol][irow];
 return screen[icol][irow];
}

unsigned char code[2000];
int icode,ftime = 0;
FILE *fout = NULL;

void output(int c, int n)
{
//  printf("\noutput(0x%2.2X,%i)\n",c,n);
  if(n==1 && c>=0)
  {
    if(c>=0x100)
    {
        code[icode++] = 0xA1;
    }
    else if(!(c & 0x80) ||
        (c & 0xF0)==0xB0 ||
        (c & 0xE0)==0xC0 ||
       ((c & 0xF0)==0xF0 && c!=0xF0 && c!=0xF1))
    {
        code[icode++] = c;
    }
    else
    {
        code[icode++] = 0xE1;
        code[icode++] = c;
    }
  }
  else
  {
    if(c>=0x100)
    {
        while(n>16)
        {
           code[icode++] = 0xA0;
           n -= 16;
        }
        if(n==16) code[icode++] = 0xA0;
        else if(n>0) code[icode++] = 0xA0 + n;
    }
    else if(c==0x20)
    {
      while(n>33)
      {
        code[icode++] = 0x9F;
        n -= 33;
      }
      if(n>=2) code[icode++] = 0x80 + n - 2;
      else if(n>0) code[icode++] = 0x20;
    }
    else
    {
      while(n>16)
      {
        code[icode++] = 0xE0;
        code[icode++] = c;
        n -= 16;
      }
      if(n==16)
      {
        code[icode++] = 0xE0;
        code[icode++] = c;
      }
      else if(n>0)
      {
        code[icode++] = 0xE0 + n;
        code[icode++] = c;
      }
    }
  }
}

void frame(int stay)
{
 unsigned char code_[2000];
 int i,j,y,z,c=-1;
 int delta[25];
 int copies,lastch,ocode=2000;
 int sz = cols*rows;
 ftime += stay;
 if(!fir) /* compare with saved screen if not 1st frame */
 {
   c = 0;
   y = -1;
   for(j=0;j<25;j++)
   {
     delta[j] = 0;
     for(i=0;i<80;i++)
     {
       if(screen_[i][j]!=screen[i][j])
       {
        c++;
        delta[j]++;
       }
     }
     if(delta[j] && y<0) y = j;
   }
   printf(" delta=%i [%i]",c,y);
   for(j=0;j<25;j++) printf(",%i",delta[j]);
   for(z=24;z>=0;z--) if(delta[z]) break;
   printf(",[%i]=%i",z,z-y+1);
   /* encode partial frame with transparency */
   icode = 0;
   code[icode++] = y; /* start with line y */
   lastch = -1;
   for(i=y*cols;i<(z+1)*cols;i++)
   {
      c = readscreen(i,1);
      if(c<0x100 && c==lastch)
      {
         copies++;
      }
      else if(c>=0x100 && lastch>=0x100)
      {
         copies++;
      }
      else
      {
         if(lastch>=0)
         {
            output(lastch,copies);
         }
         copies = 1;
      }
      lastch = c;
   }
   output(lastch,copies);
   while((code[icode-1]&0xF0)==0xA0)
   {
      icode--;
      c = code[icode]&0x0F;
      printf(" LastTran=%i",c?c:16);
   }
   printf(" tran=%i",icode);
   ocode = icode;
   memcpy(code_,code,ocode);
 }
 if(fir<=1)
 {
  /* encode full frame without transparency */
  icode = 0;
  if(fir==0) code[icode++] = 0; /* start with line 0 */
  lastch = -1;
  for(i=0;i<sz;i++)
  {
   c = readscreen(i,0);
   if(c==lastch)
   {
      copies++;
   }
   else
   {
      if(lastch>=0)
      {
         output(lastch,copies);
      }
      copies = 1;
   }
   lastch = c;
  }
  output(lastch,copies);
 }
 printf(" code=%i %c stay=%i time=%i",icode,icode<ocode?'!':' ',stay,ftime);
 if(fir<=1)
 {
  if(ftime >= KEYFRAMES)
  {
   /* time to record full frame as a key frame */
   ftime = 0;
  }
  else if(icode > ocode)
  {
   /* code with transparency is more compact */
   memcpy(code,code_,ocode);
   icode = ocode;
  }
  else
  {
   /* always clear timer when record full frame */
   ftime = 0;
  }
  while(stay>=128)
  {
   code[icode++] = 0xF1;
   code[icode++] = 127;
   code[icode++] = 0;
   stay -= 127;
  }
  code[icode++] = 0xF1;
  code[icode++] = stay;
  /* stat with line will be recorded in the beginning of the next iteration */
  printf(" final=%i",icode);
  if(fout!=NULL) fwrite(code,1,icode,fout);
  total += icode;
 }
 /* save screen to compare with next frame */
 for(j=0;j<25;j++){
 for(i=0;i<80;i++){
   screen_[i][j] = screen[i][j];
 }}
 if(fir) fir--;
}

int main(int argc, char **argv)
{
 FILE* f;
 int i = -1;
 int n,m = 0;
 int j,k = 0;
 int sta = 0;
 unsigned int s;
 char p[8],c,o;
 if(argc<2) return -1;
 f = fopen(argv[1],"rb");
 if(f==NULL) return -2;
 fseek(f,0,SEEK_END);
 s = ftell(f);
 fseek(f,0,SEEK_SET);
 printf("HTML size is %i bytes\n",s);
 fout = fopen("starwars.eta","wb");
 if(fout!=NULL)
 {
    fputc('@',fout);
    fputc(cols,fout);
    fputc(rows,fout);
    fputc(0,fout);
 }
 n = 0;
 row = 13;
 /* ASCII movie is located between string " film = '" and 0xFF character */
 while(s--)
 {
   c = fgetc(f);
   if(!m && c=='\'')
   {
     j = i;
     if(p[j--]!=' ') continue;
     if(j<0) j=7;
     if(p[j--]!='=') continue;
     if(j<0) j=7;
     if(p[j--]!=' ') continue;
     if(j<0) j=7;
     if(p[j--]!='m') continue;
     if(j<0) j=7;
     if(p[j--]!='l') continue;
     if(j<0) j=7;
     if(p[j--]!='i') continue;
     if(j<0) j=7;
     if(p[j--]!='f') continue;
     if(j<0) j=7;
     if(p[j--]!=' ') continue;
     printf("BINGO!\n");
     o = '$';
     m = 1;
   }
   else if(m)
   {
     if(c=='\xFF') break;
     k++;
     if(o=='$' && c>='0' && c<='9' && row==rows)
     {
        printf("$%c",c);
        n = n*10 + c - '0';
        c = '$';
     }
     else if(o=='\\' && c=='n')
     {
        if(n!=0)
        {
           frame(sta);
           sta = n+n; /* times 2 to get 30 FPS from 15 FPS */
           printf(" wait=%i",n);
           cls();
           row = 0;
        }
        else
        {
           row++;
           while(col<cols)
           {
             printf(" ");
             screen[col++][row] = ' ';
           }
        }
        printf("\n");
        col = 0;
        c = '$';
        n = 0;
     }
     else if(o=='\\' && c=='\'')
     {
        printf("'");
        screen[col++][row] = '\'';
     }
     else if(o=='\\' && c=='\\')
     {
        printf("\\");
        screen[col++][row] = '\\';
        o = 0;
     }
     else if(c!='\\')
     {
        printf("%c",c);
        screen[col++][row] = c;
     }
     if(o) o = c;
     else o = ' ';
   }
   else
   {
     if(i==7) i=-1;
     p[++i] = c;
   }
 }
 fclose(f);
 if(fout!=NULL)
 {
    fputc(0,fout); /* "start with" from previous frame */
    fputc(0xF0,fout); /* end of movie */
    fclose(fout);
 }
 printf("Found %i characters\n",k);
 printf("Total compressed size is %i bytes\n",total);
 return 0;
}
