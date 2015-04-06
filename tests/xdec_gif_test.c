/* Modified by A.A.Shabarshin in April 2015
 *
 * decode_gif.c
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
	unsigned int i,co;
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
			for (col = 0; col != width; col++) {
				l = image[row*width+col];
#ifdef TEST32BITS
				printf("R=#%2.2X G=#%2.2X B=#%2.2X A=#%2.2X\n",
					(unsigned char) l&255,
					(unsigned char) (l>>8)&255,
					(unsigned char) (l>>16)&255,
					(unsigned char) (l>>24)&255);
#else
				printf("0x%2.2x, 0x%2.2x, 0x%2.2x, 0x%2.2x, ",
					(unsigned char) (l>>24)&255,
					(unsigned char) (l>>16)&255,
					(unsigned char) (l>>8)&255,
					(unsigned char) l&255);
				if(((co++)%3)==2) printf("\n");
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

unsigned long map1[2] = { RGB(0,0,0), RGB(255,255,255) };

unsigned long map2[4] = { RGB(0x00,0x00,0x00), RGB(0xFF,0x55,0xFF), RGB(0x55,0xFF,0xFF), RGB(0xFF,0xFF,0xFF) };

unsigned long map4[16] = {
 RGB(  0,   0,   0), /*  #000000 - 0000 - black (no color) */
 RGB( 32,  92, 160), /*  #205CA0 - 0001 - gray-blue */
 RGB(112,  48, 192), /*  #7030C0 - 0010 - dark purple */
 RGB(104, 112, 228), /*  #6870E4 - 0011 - sky blue */
 RGB(136,  48,  32), /*  #883020 - 0100 - dark brown */
 RGB( 84,  84,  84), /*  #545054 - 0101 - gray (no color) */
 RGB(204,  84, 148), /*  #CC5494 - 0110 - pink */
 RGB(164,  64, 236), /*  #A440EC - 0111 - light purple */
 RGB( 12,  84,  32), /*  #0C5420 - 1000 - dark green */
 RGB( 44, 168, 132), /*  #2CA884 - 1001 - light gray-green */
 RGB( 84,  84,  84), /*  #545054 - 1010 - gray (no color) */
 RGB( 20, 124, 232), /*  #147CE8 - 1011 - light blue */
 RGB(152, 152,  56), /*  #989838 - 1100 - yellow (identical to color burst) */
 RGB( 36, 188,  56), /*  #24BC38 - 1101 - light green */
 RGB(200,  96,  48), /*  #C86030 - 1110 - light brown */
 RGB(160, 152, 176)  /*  #A098B0 - 1111 - white (no color) */
};

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
