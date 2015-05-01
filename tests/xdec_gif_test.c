/* decode_gif.c - Modified by A.A.Shabarshin in April 2015
 *
 * Copyright 2008 Sean Fox <dyntryx@gmail.com>
 * Copyright 2008 James Bursa <james@netsurf-browser.org>
 *
 * This file is part of NetSurf's libnsgif, http://www.netsurf-browser.org/
 * Licenced under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 */

//#include <assert.h>
#include <errno.h>
//#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
//#include "libnsgif.h"

#include "xdec_gif.c"

#define GIFNAME "shaos.gif"

#define XBMTRICK
//#define XPMTRICK

//#define TEST32BITS
//#define TEST8BITS
#define TEST4BITS
//#define TEST2BITS
//#define TEST1BITS

unsigned char *load_file(const char *path, size_t *data_size);
void warning(const char *context, int code);
void *bitmap_create(int width, int height, int* bpp, unsigned long **map);
unsigned char *bitmap_get_buffer(void *bitmap);
void bitmap_destroy(void *bitmap);
/*
void bitmap_set_opaque(void *bitmap, unsigned char opaque);
unsigned char bitmap_test_opaque(void *bitmap);
void bitmap_modified(void *bitmap);
*/

int main(int argc, char *argv[])
{
	gif_bitmap_callback_vt bitmap_callbacks = {
		bitmap_create,
		bitmap_destroy,
		bitmap_get_buffer,
		NULL,NULL,NULL
/*
		bitmap_set_opaque,
		bitmap_test_opaque,
		bitmap_modified
*/
	};
	gif_animation gif;
	size_t size;
	gif_result code;
	unsigned long l;
	unsigned int i,j,co;
	int width;
/*
	if (argc != 2) {
		fprintf(stderr, "Usage: %s image.gif\n", argv[0]);
		return 1;
	}
*/
	/* create our gif animation */
	gif_create(&gif, &bitmap_callbacks);

	/* load file into memory */
	unsigned char *data = NULL;
	if(argc>1) data = load_file(argv[1], &size);
	else data = load_file(GIFNAME, &size);

	/* begin decoding */
	do {    printf(">>> gif_initialise\n");
		code = gif_initialise(&gif, size, data);
		if (code != GIF_OK && code != GIF_WORKING) {
			warning("gif_initialise", code);
			exit(1);
		}
	} while (code != GIF_OK);

	printf(">>> %s\n", argv[1]);
	printf(">>> width                %u \n", gif.width);
	printf(">>> height               %u \n", gif.height);
	printf(">>> frame_count          %u \n", gif.frame_count);
	printf(">>> frame_count_partial  %u \n", gif.frame_count_partial);
	printf(">>> loop_count           %u \n", gif.loop_count);
	printf(">>> colour_table_size    %u \n", gif.colour_table_size);
	printf(">>> global_colours       %u \n", gif.global_colours);

	/* decode the frames */
	for (i = 0; i != gif.frame_count; i++) {
		unsigned int row, col;

                printf(">>> gif_decode_frame %i\n",i);
		code = gif_decode_frame(&gif, i);
		if (code != GIF_OK)
			warning("gif_decode_frame", code);

		unsigned long* image = (unsigned long*)gif.frame_image;

		printf(">>> frame %u (display=%u delay=%u):\n", i,
			gif.frames[i].display, gif.frames[i].frame_delay);
		width = gif.width;
#ifdef TEST1BITS
		width /= 32;
#endif
#ifdef TEST2BITS
		width /= 16;
#endif
#ifdef TEST4BITS
		width /= 8;
#endif
#ifdef TEST8BITS
		width /= 4;
#endif
		co = 0;
		for (row = 0; row != gif.height; row++) {
#ifdef TEST32BITS
			printf(">> row %u\n",row);
#endif
#ifdef XPMTRICK
			printf("\",\n\"");
#endif
			for (col = 0; col != width; col++) {
				l = image[row*width+col];
#ifdef TEST32BITS
				printf("R=#%2.2X G=#%2.2X B=#%2.2X A=#%2.2X\n",
					(unsigned char) l&255,
					(unsigned char) (l>>8)&255,
					(unsigned char) (l>>16)&255,
					(unsigned char) (l>>24)&255);
#else
#ifdef XBMTRICK
				unsigned long c = 0;
				unsigned long n = 1;
				unsigned long m = 0x80000000;
				for(j=0;j<32;j++)
                                {
                                    if(!(l&n)) c|=m;
                                    n <<= 1;
                                    m >>= 1;
                                }
//                                printf("0x%8.8X -> 0x%8.8X\n",l,c);
                                l = c;
				printf("0x%2.2x, 0x%2.2x, 0x%2.2x, 0x%2.2x, ",
					(unsigned char) l&255,
					(unsigned char) (l>>8)&255,
					(unsigned char) (l>>16)&255,
					(unsigned char) (l>>24)&255);
#else
#ifdef XPMTRICK
				printf("%1.1X%1.1X%1.1X%1.1X%1.1X%1.1X%1.1X%1.1X",
					(unsigned char) (l>>28)&15,
					(unsigned char) (l>>24)&15,
					(unsigned char) (l>>20)&15,
					(unsigned char) (l>>16)&15,
					(unsigned char) (l>>12)&15,
					(unsigned char) (l>>8)&15,
					(unsigned char) (l>>4)&15,
					(unsigned char) l&15);
#else
				printf("0x%2.2x, 0x%2.2x, 0x%2.2x, 0x%2.2x, ",
					(unsigned char) (l>>24)&255,
					(unsigned char) (l>>16)&255,
					(unsigned char) (l>>8)&255,
					(unsigned char) l&255);
#endif
#endif
#ifndef XPMTRICK
				if(((co++)%3)==2) printf("\n");
#endif
#endif
			}
		}
		printf("\n");
	}

	/* clean up */
	gif_finalise(&gif);
	free(data);

	return 0;
}


unsigned char *load_file(const char *path, size_t *data_size)
{
	FILE *fd;
	struct stat sb;
	unsigned char *buffer;
	size_t size;
	size_t n;

	fd = fopen(path, "rb");
	if (!fd) {
		perror(path);
		exit(EXIT_FAILURE);
	}

	if (stat(path, &sb)) {
		perror(path);
		exit(EXIT_FAILURE);
	}
	size = sb.st_size;

	buffer = malloc(size);
	if (!buffer) {
		fprintf(stderr, "Unable to allocate %lld bytes\n",
				(long long) size);
		exit(EXIT_FAILURE);
	}

	n = fread(buffer, 1, size, fd);
	if (n != size) {
		perror(path);
		exit(EXIT_FAILURE);
	}

	fclose(fd);

	*data_size = size;
	return buffer;
}


void warning(const char *context, gif_result code)
{
	fprintf(stderr, "%s failed: ", context);
	switch (code)
	{
	case GIF_INSUFFICIENT_FRAME_DATA:
		fprintf(stderr, "GIF_INSUFFICIENT_FRAME_DATA");
		break;
	case GIF_FRAME_DATA_ERROR:
		fprintf(stderr, "GIF_FRAME_DATA_ERROR");
		break;
	case GIF_INSUFFICIENT_DATA:
		fprintf(stderr, "GIF_INSUFFICIENT_DATA");
		break;
	case GIF_DATA_ERROR:
		fprintf(stderr, "GIF_DATA_ERROR");
		break;
	case GIF_INSUFFICIENT_MEMORY:
		fprintf(stderr, "GIF_INSUFFICIENT_MEMORY");
		break;
	case GIF_INVALID_BPP:
		fprintf(stderr, "GIF_INVALID_BPP");
		break;
	default:
		fprintf(stderr, "unknown code %i", code);
		break;
	}
	fprintf(stderr, "\n");
}

#define RGB(r,g,b) (r|(g<<8)|(b<<16))
#define RGBHTML(i) ((i&0xFF0000)>>16)|(i&0xFF00)|((i&0xFF)<<16);

unsigned long map1[2] = { RGB(0,0,0), RGB(255,255,255) };

unsigned long map2[4] = {
 RGB(0x00,0x00,0x00),
 RGB(0xFF,0x55,0xFF),
 RGB(0x55,0xFF,0xFF),
 RGB(0xFF,0xFF,0xFF)
};

#if 0

/* Colors from picture of SONY LCD TV: */

unsigned long map4[16] = {
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

#else

/* Colors from shifter CGA composite emulation in DOSBox: */

unsigned long map4[16] = {
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
};

#endif

unsigned long map8[256];

void *bitmap_create(int width, int height, int* bpp, unsigned long **map)
{
printf(">>> bitmap_create %i %i 0x%8.8X 0x%8.8X\n",width,height,(int)bpp,(int)map);
#ifdef TEST1BITS
	if(bpp!=NULL) *bpp = 1;
	if(map!=NULL) *map = map1;
	return malloc(width * height * 4);
#endif
#ifdef TEST2BITS
	if(bpp!=NULL) *bpp = 2;
	if(map!=NULL) *map = map2;
	return malloc(width * height * 4);
#endif
#ifdef TEST4BITS
	if(bpp!=NULL) *bpp = 4;
	if(map!=NULL) *map = map4;
	return malloc(width * height * 4);
#endif
#ifdef TEST32BITS
	return malloc(width * height * sizeof(long));
#endif
}


unsigned char *bitmap_get_buffer(void *bitmap)
{
printf(">>> bitmap_get_buffer 0x%8.8X\n",bitmap);
//	assert(bitmap);
	return bitmap;
}


void bitmap_destroy(void *bitmap)
{
printf(">>> bitmap_destroy 0x%8.8X\n",bitmap);
//	assert(bitmap);
	free(bitmap);
}

/*
oid bitmap_set_opaque(void *bitmap, unsigned char opaque)
{
printf(">>> bitmap_set_opaque 0x%8.8X %i\n",bitmap,opaque);
//	(void) opaque;
//	assert(bitmap);
}

unsigned char bitmap_test_opaque(void *bitmap)
{
printf(">>> bitmap_test_opaque 0x%8.8X\n",bitmap);
//	assert(bitmap);
	return 0;
}

void bitmap_modified(void *bitmap)
{
printf(">>> bitmap_modified 0x%8.8X\n",bitmap);
//	assert(bitmap);
	return;
}
*/
