/* xdec_gif.c for XORLib by Shaos based on libnsgif (April 2015)
 *
 * Changes from libnsgif:
 * - libnsgif.h and libnsgif.c were combined in this single file
 * - removed asserts
 * - changed bools to unsigned chars
 * - changed some types in structures
 * - color tables are static now (no memory allocation for it)
 * - reallocate frame data by bigger portions (2x bigger than previously allocated)
 * - made code big-endian friendly
 * - added support for 1-bit, 2-bit, 4-bit and 8-bit buffers (with color maps)
 *
 * ToDo:
 * - for now width must fit into longs even for smaller bpps...
 *
 * See original copyrights below:
 *
 * Copyright 2004 Richard Wilson <richard.wilson@netsurf-browser.org>
 * Copyright 2008 Sean Fox <dyntryx@gmail.com>
 *
 * This file is part of NetSurf's libnsgif, http://www.netsurf-browser.org/
 * Licenced under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 */

#define malloc my_malloc
#define realloc my_realloc
#define free my_free

void* my_malloc(unsigned long s);
void* my_realloc(void* p, unsigned long s);
void my_free(void* p);

/** \file
 * Progressive animated GIF file decoding (interface).
 */
/*
#ifndef _LIBNSGIF_H_
#define _LIBNSGIF_H_

#include <stdbool.h>
#include <inttypes.h>
*/
/*	Error return values
*/
typedef enum {
	GIF_WORKING = 1,
	GIF_OK = 0,
	GIF_INSUFFICIENT_FRAME_DATA = -1,
	GIF_FRAME_DATA_ERROR = -2,
	GIF_INSUFFICIENT_DATA = -3,
	GIF_DATA_ERROR = -4,
	GIF_INSUFFICIENT_MEMORY = -5,
	GIF_FRAME_NO_DISPLAY = -6,
	GIF_END_OF_FRAME = -7,
	GIF_INVALID_BPP = -8 /* SH-05.04.2015 */
} gif_result;

/*	The GIF frame data
*/
typedef struct gif_frame {
  	unsigned char display;			/**< whether the frame should be displayed/animated */
  	unsigned short frame_delay;		/**< delay (in cs) before animating the frame */
	/**	Internal members are listed below
	*/
  	unsigned long frame_pointer;		/**< offset (in bytes) to the GIF frame data */
  	unsigned char virgin;			/**< whether the frame has previously been used */
	unsigned char opaque;			/**< whether the frame is totally opaque */
	unsigned char redraw_required;		/**< whether a forcable screen redraw is required */
	unsigned char disposal_method;		/**< how the previous frame should be disposed; affects plotting */
	unsigned char transparency;	 	/**< whether we acknoledge transparency */
	unsigned char transparency_index;	/**< the index designating a transparent pixel */
	unsigned short redraw_x;		/**< x co-ordinate of redraw rectangle */
	unsigned short redraw_y;		/**< y co-ordinate of redraw rectangle */
	unsigned short redraw_width;		/**< width of redraw rectangle */
	unsigned short redraw_height;		/**< height of redraw rectangle */
} gif_frame;

/*	API for Bitmap callbacks
*/
typedef void* (*gif_bitmap_cb_create)(int width, int height, int* bpp, unsigned long** map); /* SH-04.04.2015: extended with bpp and map pointers */
typedef void (*gif_bitmap_cb_destroy)(void *bitmap);
typedef unsigned char* (*gif_bitmap_cb_get_buffer)(void *bitmap);
typedef void (*gif_bitmap_cb_set_opaque)(void *bitmap, unsigned char opaque);
typedef unsigned char (*gif_bitmap_cb_test_opaque)(void *bitmap);
typedef void (*gif_bitmap_cb_modified)(void *bitmap);

/*	The Bitmap callbacks function table
*/
typedef struct gif_bitmap_callback_vt {
	gif_bitmap_cb_create bitmap_create;		/**< Create a bitmap. */
	gif_bitmap_cb_destroy bitmap_destroy;		/**< Free a bitmap. */
	gif_bitmap_cb_get_buffer bitmap_get_buffer;	/**< Return a pointer to the pixel data in a bitmap. */
	/**	Members below are optional
	*/
	gif_bitmap_cb_set_opaque bitmap_set_opaque;	/**< Sets whether a bitmap should be plotted opaque. */
	gif_bitmap_cb_test_opaque bitmap_test_opaque;	/**< Tests whether a bitmap has an opaque alpha channel. */
	gif_bitmap_cb_modified bitmap_modified;		/**< The bitmap image has changed, so flush any persistant cache. */
} gif_bitmap_callback_vt;

/*	The GIF animation data
*/
typedef struct gif_animation {
	gif_bitmap_callback_vt bitmap_callbacks;	/**< callbacks for bitmap functions */
	unsigned char *gif_data;			/**< pointer to GIF data */
	unsigned short width;				/**< width of GIF (may increase during decoding) */
	unsigned short height;				/**< heigth of GIF (may increase during decoding) */
	unsigned short frame_count;			/**< number of frames decoded */
	unsigned short frame_count_partial;		/**< number of frames partially decoded */
	gif_frame *frames;				/**< decoded frames */
	short decoded_frame;				/**< current frame decoded to bitmap */
	void *frame_image;				/**< currently decoded image; stored as bitmap from bitmap_create callback */
	int bpp;					/**< SH-04.04.2015: extension to have compact decoded image (bits per pixel:1,2,4,8,32) */
	unsigned long* map;				/**< SH-04.04.2015: extension to have compact decoded image (suitable for bpp=1/2/4/8) */
	short loop_count;				/**< number of times to loop animation */
	gif_result current_error;			/**< current error type, or 0 for none*/
	/**	Internal members are listed below
	*/
	unsigned long buffer_position;			/**< current index into GIF data */
	unsigned long buffer_size;			/**< total number of bytes of GIF data available */
	unsigned short frame_holders;			/**< current number of frame holders */
	unsigned char background_index;			/**< index in the colour table for the background colour */
	unsigned int aspect_ratio;			/**< image aspect ratio (ignored for now) */
	unsigned short colour_table_size;		/**< size of colour table (in entries) */
	unsigned char global_colours;			/**< whether the GIF has a global colour table */
	unsigned long global_colour_table[256];		/**< global colour table SH-04.04.2015: array of longs! */
	unsigned long local_colour_table[256];		/**< local colour table SH-04.04.2015: array of longs! */
	/* SH-04.04.2015: colour format is 0xAABBGGRR and we will reuse AA because it's useless for color map... */
} gif_animation;

void gif_create(gif_animation *gif, gif_bitmap_callback_vt *bitmap_callbacks);
gif_result gif_initialise(gif_animation *gif, size_t size, unsigned char *data);
gif_result gif_decode_frame(gif_animation *gif, unsigned int frame);
void gif_finalise(gif_animation *gif);
/*
#endif
*/

/*
 * Copyright 2004 Richard Wilson <richard.wilson@netsurf-browser.org>
 * Copyright 2008 Sean Fox <dyntryx@gmail.com>
 *
 * This file is part of NetSurf's libnsgif, http://www.netsurf-browser.org/
 * Licenced under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 */

/*#include <stdbool.h>*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/*#include <assert.h>*/
/*#include "libnsgif.h"*/
/*#include "utils/log.h"*/

/*	READING GIF FILES
	=================

	The functions provided by this file allow for efficient progressive GIF
	decoding. Whilst the initialisation does not ensure that there is
	sufficient image data to complete the entire frame, it does ensure that
	the information provided is valid. Any subsequent attempts to decode an
	initialised GIF are guaranteed to succeed, and any bytes of the image
	not present are assumed to be totally transparent.

	To begin decoding a GIF, the 'gif' structure must be initialised with
	the 'gif_data' and 'buffer_size' set to their initial values. The
	'buffer_position' should initially be 0, and will be internally updated
	as the decoding commences. The caller should then repeatedly call
	gif_initialise() with the structure until the function returns 1, or
	no more data is avaliable.

	Once the initialisation has begun, the decoder completes the variables
	'frame_count' and 'frame_count_partial'. The former being the total
	number of frames that have been successfully initialised, and the
	latter being the number of frames that a partial amount of data is
	available for. This assists the caller in managing the animation whilst
	decoding is continuing.

	To decode a frame, the caller must use gif_decode_frame() which updates
	the current 'frame_image' to reflect the desired frame. The required
	'disposal_method' is also updated to reflect how the frame should be
	plotted. The caller must not assume that the current 'frame_image' will
	be valid between calls if initialisation is still occuring, and should
	either always request that the frame is decoded (no processing will
	occur if the 'decoded_frame' has not been invalidated by initialisation)
	or perform the check itself.

	It should be noted that gif_finalise() should always be called, even if
	no frames were initialised.  Additionally, it is the responsibility of
	the caller to free 'gif_data'.

	[rjw] - Fri 2nd April 2004
*/

/*	TO-DO LIST
	=================

	+ Plain text and comment extensions could be implemented if there is any
	interest in doing so.
*/

/*	EXTENSIONS FOR EMBEDDED
	=======================

	I extended bitmap_create callback with two additional arguments:
	3rd - pointer to int bpp (bits per pixel) of output video device;
	4th - pointer to pointer to unsigned long for optional color map of output device.
	Purpose of this map is to reduce size of the buffer for embedded applications.
	Default value of bpp is 32 and in this case behavior is the same as before -
	no map and buffer has 32-bit integers with full AABBGGRR color info per pixel.
	If output video device is not supporting full color then user may use smaller
	numbers for bpp - 8 for 1 pixel per byte, 4 for 2 pixels per byte, 2 for 4 pixels
	per byte and 1 for 8 pixels per byte. To map colors the program needs to have
	a color map with 256 entries for bpp=8 (buffer is 4 times smaller), 16 entries for
	bpp=4 (buffer is 8 times smaller), 4 entries for bpp=2 (buffer is 16 times smaller)
	and 2 entries for bpp=1 (buffer is 32 times smaller). Mapping colors are happening
	in moment of reading color tables from GIF file - program simply chooses closest
	color from provided map for every GIF color and stores index in highest bytes of
	color table entry (instead of alpha value).
	
	Shaos, Sat 4th April 2015
*/

/*	Maximum colour table size
*/
#define GIF_MAX_COLOURS 256

/*	Internal flag that the colour table needs to be processed (SH-04.04.2015: make more As...)
*/
#define GIF_PROCESS_COLOURS 0xAAAAAAAA

/*	Internal flag that a frame is invalid/unprocessed
*/
#define GIF_INVALID_FRAME -1

/*	Maximum LZW bits available
*/
#define GIF_MAX_LZW 12

/* Transparent colour
*/
#define GIF_TRANSPARENT_COLOUR 0x00000000

/*	GIF Flags
*/
#define GIF_FRAME_COMBINE 1
#define GIF_FRAME_CLEAR 2
#define GIF_FRAME_RESTORE 3
#define GIF_FRAME_QUIRKS_RESTORE 4
#define GIF_IMAGE_SEPARATOR 0x2c
#define GIF_INTERLACE_MASK 0x40
#define GIF_COLOUR_TABLE_MASK 0x80
#define GIF_COLOUR_TABLE_SIZE_MASK 0x07
#define GIF_EXTENSION_INTRODUCER 0x21
#define GIF_EXTENSION_GRAPHIC_CONTROL 0xf9
#define GIF_DISPOSAL_MASK 0x1c
#define GIF_TRANSPARENCY_MASK 0x01
#define GIF_EXTENSION_COMMENT 0xfe
#define GIF_EXTENSION_PLAIN_TEXT 0x01
#define GIF_EXTENSION_APPLICATION 0xff
#define GIF_BLOCK_TERMINATOR 0x00
#define GIF_TRAILER 0x3b

/*	Internal GIF routines
*/
static gif_result gif_initialise_sprite(gif_animation *gif, unsigned int width, unsigned int height);
static gif_result gif_initialise_frame(gif_animation *gif);
static gif_result gif_initialise_frame_extensions(gif_animation *gif, const int frame);
static gif_result gif_skip_frame_extensions(gif_animation *gif);
static unsigned int gif_interlaced_line(int height, int y);
/* SH-05.04.2015 */
static unsigned long gif_choosecolor(int mapsz, unsigned long* map, unsigned long color);

/*	Internal LZW routines
*/
static void gif_init_LZW(gif_animation *gif);
static unsigned char gif_next_LZW(gif_animation *gif);
static int gif_next_code(gif_animation *gif, int code_size);

/*	General LZW values. They are shared for all GIFs being decoded, and
	thus we can't handle progressive decoding efficiently without having
	the data for each image which would use an extra 10Kb or so per GIF.
*/
static unsigned char buf[4];
static unsigned char *direct;
static int maskTbl[16] = {0x0000, 0x0001, 0x0003, 0x0007, 0x000f, 0x001f, 0x003f, 0x007f,
			  0x00ff, 0x01ff, 0x03ff, 0x07ff, 0x0fff, 0x1fff, 0x3fff, 0x7fff};
static int table[2][(1 << GIF_MAX_LZW)];
static unsigned char stack[(1 << GIF_MAX_LZW) * 2];
static unsigned char *stack_pointer;
static int code_size, set_code_size;
static int max_code, max_code_size;
static int clear_code, end_code;
static int curbit, lastbit, last_byte;
static int firstcode, oldcode;
static unsigned char zero_data_block = 0;
static unsigned char get_done;

/*	Whether to clear the decoded image rather than plot
*/
static unsigned char clear_image = 0;



/**	Initialises necessary gif_animation members.
*/
void gif_create(gif_animation *gif, gif_bitmap_callback_vt *bitmap_callbacks) {
	memset(gif, 0, sizeof(gif_animation));
	gif->bitmap_callbacks = *bitmap_callbacks;
	gif->decoded_frame = GIF_INVALID_FRAME;
}


/**	Initialises any workspace held by the animation and attempts to decode
	any information that hasn't already been decoded.
	If an error occurs, all previously decoded frames are retained.

	@return GIF_FRAME_DATA_ERROR for GIF frame data error
		GIF_INSUFFICIENT_FRAME_DATA for insufficient data to process
		          any more frames
		GIF_INSUFFICIENT_MEMORY for memory error
		GIF_DATA_ERROR for GIF error
		GIF_INSUFFICIENT_DATA for insufficient data to do anything
		GIF_OK for successful decoding
		GIF_WORKING for successful decoding if more frames are expected
*/
gif_result gif_initialise(gif_animation *gif, size_t size, unsigned char *data) {
	unsigned char *gif_data;
	unsigned int index;
	gif_result return_value;

	/* 	The GIF format is thoroughly documented; a full description
	 *	can be found at http://www.w3.org/Graphics/GIF/spec-gif89a.txt
	*/

	/*	Initialize values
	*/
	gif->buffer_size = size;
	gif->gif_data = data;
	
	/*	Check for sufficient data to be a GIF (6-byte header + 7-byte logical screen descriptor)
	*/
	if (gif->buffer_size < 13) return GIF_INSUFFICIENT_DATA;

	/*	Get our current processing position
	*/
	gif_data = gif->gif_data + gif->buffer_position;

	/*	See if we should initialise the GIF
	*/
	if (gif->buffer_position == 0) {

		/*	We want everything to be NULL before we start so we've no chance
			of freeing bad pointers (paranoia)
		*/
		gif->frame_image = NULL;
		gif->frames = NULL;
/*
		gif->local_colour_table = NULL;
		gif->global_colour_table = NULL;
*/
		/*	The caller may have been lazy and not reset any values
		*/
		gif->frame_count = 0;
		gif->frame_count_partial = 0;
		gif->decoded_frame = GIF_INVALID_FRAME;

		/* 6-byte GIF file header is:
		 *
		 *	+0	3CHARS	Signature ('GIF')
		 *	+3	3CHARS	Version ('87a' or '89a')
		 */
		if (strncmp((const char *) gif_data, "GIF", 3) != 0)
			return GIF_DATA_ERROR;
		gif_data += 3;

		/*	Ensure GIF reports version 87a or 89a
		*/
/*		if ((strncmp(gif_data, "87a", 3) != 0) &&
				(strncmp(gif_data, "89a", 3) != 0))
			LOG(("Unknown GIF format - proceeding anyway"));
*/		gif_data += 3;

		/* 7-byte Logical Screen Descriptor is:
		 *
		 *	+0	SHORT	Logical Screen Width
		 *	+2	SHORT	Logical Screen Height
		 *	+4	CHAR	__Packed Fields__
		 * 			1BIT	Global Colour Table Flag
		 * 			3BITS	Colour Resolution
		 * 			1BIT	Sort Flag
		 * 			3BITS	Size of Global Colour Table
		 *	+5	CHAR	Background Colour Index
		 *	+6	CHAR	Pixel Aspect Ratio
		 */
		gif->width = gif_data[0] | (gif_data[1] << 8);
		gif->height = gif_data[2] | (gif_data[3] << 8);
		gif->global_colours = (gif_data[4] & GIF_COLOUR_TABLE_MASK)?1:0;
		gif->colour_table_size = (2 << (gif_data[4] & GIF_COLOUR_TABLE_SIZE_MASK));
		gif->background_index = gif_data[5];
		gif->aspect_ratio = gif_data[6];
		gif->bpp = 32; /* SH-04.04.2015: default value is 32 bits per pixel */
		gif->map = NULL; /* SH-04.04.2015: default value is NULL (no map) */
		gif->loop_count = 1;
		gif_data += 7;

		/*	Some broken GIFs report the size as the screen size they were created in. As
			such, we detect for the common cases and set the sizes as 0 if they are found
			which results in the GIF being the maximum size of the frames.
		*/
		if (((gif->width == 640) && (gif->height == 480)) ||
				((gif->width == 640) && (gif->height == 512)) ||
				((gif->width == 800) && (gif->height == 600)) ||
				((gif->width == 1024) && (gif->height == 768)) ||
				((gif->width == 1280) && (gif->height == 1024)) ||
				((gif->width == 1600) && (gif->height == 1200)) ||
				((gif->width == 0) || (gif->height == 0)) ||
				((gif->width > 2048) || (gif->height > 2048))) {
			gif->width = 1;
			gif->height = 1;
		}

		/*	Allocate some data irrespective of whether we've got any colour tables. We
			always get the maximum size in case a GIF is lying to us. It's far better
			to give the wrong colours than to trample over some memory somewhere.
			
			SH-04.04.2015: no allocations here, because colour tables are now static arrays
		*/
/*
		gif->global_colour_table = calloc(GIF_MAX_COLOURS, sizeof(unsigned int));
		gif->local_colour_table = calloc(GIF_MAX_COLOURS, sizeof(unsigned int));
		if ((gif->global_colour_table == NULL) || (gif->local_colour_table == NULL)) {
			gif_finalise(gif);
			return GIF_INSUFFICIENT_MEMORY;
		}
*/
		/*	Set the first colour to a value that will never occur in reality so we
			know if we've processed it
		*/
		gif->global_colour_table[0] = GIF_PROCESS_COLOURS;
		
		/*	Check if the GIF has no frame data (13-byte header + 1-byte termination block)
		 *	Although generally useless, the GIF specification does not expressly prohibit this
		 */
		if (gif->buffer_size == 14) {
			if (gif_data[0] == GIF_TRAILER)
				return GIF_OK;
			else
				return GIF_INSUFFICIENT_DATA;
		}

		/*	Initialise enough workspace for 10 frames initially (SH-04.04.2015)
		*/
		if ((gif->frames = (gif_frame *)malloc(10 * sizeof(gif_frame))) == NULL) {
			gif_finalise(gif);
			return GIF_INSUFFICIENT_MEMORY;
		}
		gif->frame_holders = 10;

		/*	Initialise the sprite header
		*/
		/*assert(gif->bitmap_callbacks.bitmap_create);*/
		if ((gif->frame_image = gif->bitmap_callbacks.bitmap_create(gif->width, gif->height, &gif->bpp, &gif->map)) == NULL) {
			gif_finalise(gif);
			return GIF_INSUFFICIENT_MEMORY;
		}
		/* SH-05.04.2015 */
		if((gif->bpp!=32 && gif->map==NULL) ||
		   (gif->bpp!=32 && gif->bpp!=8 && gif->bpp!=4 && gif->bpp!=2 && gif->bpp!=1))
		{
			return GIF_INVALID_BPP;
		}

		/*	Remember we've done this now
		*/
		gif->buffer_position = gif_data - gif->gif_data;
	}

	/*	Do the colour map if we haven't already. As the top byte is always 0xff or 0x00
		depending on the transparency we know if it's been filled in.
	*/
	if (gif->global_colour_table[0] == GIF_PROCESS_COLOURS) {
		/*	Check for a global colour map signified by bit 7
		*/
		if (gif->global_colours) {
			if (gif->buffer_size < (gif->colour_table_size * 3 + 12)) {
				return GIF_INSUFFICIENT_DATA;
			}
			for (index = 0; index < gif->colour_table_size; index++) {
				/* Gif colour map contents are r,g,b.
				 *
				 * We want to pack them bytewise into the 
				 * colour table, such that the red component
				 * is in byte 0 and the alpha component is in
				 * byte 3.
				 */
#if 0
				unsigned char *entry = (unsigned char *) &gif->
						global_colour_table[index];
				entry[0] = gif_data[0];	/* r */
				entry[1] = gif_data[1];	/* g */
				entry[2] = gif_data[2];	/* b */
				entry[3] = 0xff;	/* a */
#else
/* SH-04.04.2015: big-endian friendly code: */
				gif->global_colour_table[index] = 0xFF000000 | (gif_data[2]<<16) | (gif_data[1]<<8) | gif_data[0];
#endif
				/* SH-05.04.2015: choose closest color index and store it in highest byte */
				if(gif->bpp < 32)
					gif->global_colour_table[index] =
						gif_choosecolor(1<<gif->bpp,gif->map,gif->global_colour_table[index]);
				gif_data += 3;
			}
			gif->buffer_position = (gif_data - gif->gif_data);
		} else {
			/*	Create a default colour table with the first two colours as black and white
			*/
			unsigned long *entry = gif->global_colour_table;

			entry[0] = 0xFF000000;
			/* Force Alpha channel to opaque */
			entry[1] = 0xFFFFFFFF;

			/* SH-05.04.2015: choose closest color index and store it in highest byte */
			if(gif->bpp < 32)
			{
				entry[0] = gif_choosecolor(1<<gif->bpp,gif->map,entry[0]);
				entry[1] = gif_choosecolor(1<<gif->bpp,gif->map,entry[1]);
			}
		}
	}

	/*	Repeatedly try to initialise frames
	*/
	while ((return_value = gif_initialise_frame(gif)) == GIF_WORKING)
	{
	}

	/*	If there was a memory error tell the caller
	*/
	if ((return_value == GIF_INSUFFICIENT_MEMORY) ||
			(return_value == GIF_DATA_ERROR))
		return return_value;

	/*	If we didn't have some frames then a GIF_INSUFFICIENT_DATA becomes a
		GIF_INSUFFICIENT_FRAME_DATA
	*/
	if ((return_value == GIF_INSUFFICIENT_DATA) && (gif->frame_count_partial > 0))
		return GIF_INSUFFICIENT_FRAME_DATA;

	/*	Return how many we got
	*/
	return return_value;
}


/**	Updates the sprite memory size

	@return GIF_INSUFFICIENT_MEMORY for a memory error
		GIF_OK for success
*/
static gif_result gif_initialise_sprite(gif_animation *gif, unsigned int width, unsigned int height) {
	unsigned int max_width;
	unsigned int max_height;
	struct bitmap *buffer;

	/*	Check if we've changed
	*/
	if ((width <= gif->width) && (height <= gif->height))
		return GIF_OK;

	/*	Get our maximum values
	*/
	max_width = (width > gif->width) ? width : gif->width;
	max_height = (height > gif->height) ? height : gif->height;

	/*	Allocate some more memory
	*/
	/*assert(gif->bitmap_callbacks.bitmap_destroy);*/
	gif->bitmap_callbacks.bitmap_destroy(gif->frame_image); /* SH-04.04.2015: free memory first ??? */
	/*assert(gif->bitmap_callbacks.bitmap_create);*/
	if ((buffer = gif->bitmap_callbacks.bitmap_create(max_width, max_height, &gif->bpp, &gif->map)) == NULL)
		return GIF_INSUFFICIENT_MEMORY;
	/* SH-05.04.2015 */
	if((gif->bpp!=32 && gif->map==NULL) ||
	   (gif->bpp!=32 && gif->bpp!=8 && gif->bpp!=4 && gif->bpp!=2 && gif->bpp!=1))
	{
		return GIF_INVALID_BPP;
	}
	/* SH-04.04.2015: originally bitmap_destroy was here... */
	gif->frame_image = buffer;
	gif->width = max_width;
	gif->height = max_height;

	/*	Invalidate our currently decoded image
	*/
	gif->decoded_frame = GIF_INVALID_FRAME;
	return GIF_OK;
}


/**	Attempts to initialise the next frame

	@return GIF_INSUFFICIENT_DATA for insufficient data to do anything
		GIF_FRAME_DATA_ERROR for GIF frame data error
		GIF_INSUFFICIENT_MEMORY for insufficient memory to process
		GIF_INSUFFICIENT_FRAME_DATA for insufficient data to complete the frame
		GIF_DATA_ERROR for GIF error (invalid frame header)
		GIF_OK for successful decoding
		GIF_WORKING for successful decoding if more frames are expected
*/
static gif_result gif_initialise_frame(gif_animation *gif) {
	int frame;
	gif_frame *temp_buf;

	unsigned char *gif_data, *gif_end;
	int gif_bytes;
	unsigned int flags = 0;
	unsigned int width, height, offset_x, offset_y;
	unsigned int block_size, colour_table_size;
	unsigned char first_image = 1;
	gif_result return_value;

	/*	Get the frame to decode and our data position
	*/
	frame = gif->frame_count;

	/*	Get our buffer position etc.
	*/
	gif_data = (unsigned char *)(gif->gif_data + gif->buffer_position);
	gif_end = (unsigned char *)(gif->gif_data + gif->buffer_size);
	gif_bytes = (gif_end - gif_data);

	/*	Check if we've finished
	*/
	if ((gif_bytes > 0) && (gif_data[0] == GIF_TRAILER)) return GIF_OK;
	
	/*	Check if we have enough data
	 *	The shortest block of data is a 4-byte comment extension + 1-byte block terminator + 1-byte gif trailer
	*/
	if (gif_bytes < 6) return GIF_INSUFFICIENT_DATA;

	/*	We could theoretically get some junk data that gives us millions of frames, so
		we ensure that we don't have a silly number
	*/

	if (frame > 4096) return GIF_FRAME_DATA_ERROR;

	/*	Get some memory to store our pointers in etc.
	*/
	if ((int)gif->frame_holders <= frame) {
		/*	Allocate more memory
		*/
		if ((temp_buf = (gif_frame *)realloc(gif->frames,
					(frame * 2) * sizeof(gif_frame))) == NULL) /* SH-04.04.2015: more memory */
			return GIF_INSUFFICIENT_MEMORY;
		gif->frames = temp_buf;
		gif->frame_holders = frame * 2; /* SH-04.04.2015: more memory */
	}

	/*	Store our frame pointer. We would do it when allocating except we
		start off with one frame allocated so we can always use realloc.
	*/
	gif->frames[frame].frame_pointer = gif->buffer_position;
	gif->frames[frame].display = 0;
	gif->frames[frame].virgin = 1;
	gif->frames[frame].disposal_method = 0;
	gif->frames[frame].transparency = 0;
	gif->frames[frame].frame_delay = 100;
	gif->frames[frame].redraw_required = 0;

	/*	Invalidate any previous decoding we have of this frame
	*/
	if (gif->decoded_frame == frame)
		gif->decoded_frame = GIF_INVALID_FRAME;

	/*	We pretend to initialise the frames, but really we just skip over all
		the data contained within. This is all basically a cut down version of
		gif_decode_frame that doesn't have any of the LZW bits in it.
	*/

	/*	Initialise any extensions
	*/
	gif->buffer_position = gif_data - gif->gif_data;
	if ((return_value = gif_initialise_frame_extensions(gif, frame)) != GIF_OK)
		return return_value;
	gif_data = (gif->gif_data + gif->buffer_position);
	gif_bytes = (gif_end - gif_data);

	/*	Check if we've finished
	*/
	if ((gif_bytes = (gif_end - gif_data)) < 1)
		return GIF_INSUFFICIENT_FRAME_DATA;
	else if (gif_data[0] == GIF_TRAILER) {
		gif->buffer_position = (gif_data - gif->gif_data);
		gif->frame_count = frame + 1;
		return GIF_OK;
	}

	/*	If we're not done, there should be an image descriptor
	*/

	if (gif_data[0] != GIF_IMAGE_SEPARATOR) return GIF_FRAME_DATA_ERROR;

	/*	Do some simple boundary checking
	*/
	offset_x = gif_data[1] | (gif_data[2] << 8);
	offset_y = gif_data[3] | (gif_data[4] << 8);
	width = gif_data[5] | (gif_data[6] << 8);
	height = gif_data[7] | (gif_data[8] << 8);

	/*	Set up the redraw characteristics. We have to check for extending the area
		due to multi-image frames.
	*/
	if (!first_image) {
		if (gif->frames[frame].redraw_x > offset_x) {
			gif->frames[frame].redraw_width += (gif->frames[frame].redraw_x - offset_x);
			gif->frames[frame].redraw_x = offset_x;
		}
		if (gif->frames[frame].redraw_y > offset_y) {
			gif->frames[frame].redraw_height += (gif->frames[frame].redraw_y - offset_y);
			gif->frames[frame].redraw_y = offset_y;
		}
		if ((offset_x + width) > (gif->frames[frame].redraw_x + gif->frames[frame].redraw_width))
			gif->frames[frame].redraw_width = (offset_x + width) - gif->frames[frame].redraw_x;
		if ((offset_y + height) > (gif->frames[frame].redraw_y + gif->frames[frame].redraw_height))
			gif->frames[frame].redraw_height = (offset_y + height) - gif->frames[frame].redraw_y;
	} else {
		first_image = 0;
		gif->frames[frame].redraw_x = offset_x;
		gif->frames[frame].redraw_y = offset_y;
		gif->frames[frame].redraw_width = width;
		gif->frames[frame].redraw_height = height;
	}

	/*	if we are clearing the background then we need to redraw enough to cover the previous
		frame too
	*/
	gif->frames[frame].redraw_required = ((gif->frames[frame].disposal_method == GIF_FRAME_CLEAR) ||
						(gif->frames[frame].disposal_method == GIF_FRAME_RESTORE));

	/*	Boundary checking - shouldn't ever happen except with junk data
	*/
	if (gif_initialise_sprite(gif, (offset_x + width), (offset_y + height)))
		return GIF_INSUFFICIENT_MEMORY;

	/*	Decode the flags
	*/
	flags = gif_data[9];
	colour_table_size = 2 << (flags & GIF_COLOUR_TABLE_SIZE_MASK);

	/*	Move our data onwards and remember we've got a bit of this frame
	*/
	gif_data += 10;
	gif_bytes = (gif_end - gif_data);
	gif->frame_count_partial = frame + 1;

	/*	Skip the local colour table
	*/
	if (flags & GIF_COLOUR_TABLE_MASK) {
		gif_data += 3 * colour_table_size;
		if ((gif_bytes = (gif_end - gif_data)) < 0)
			return GIF_INSUFFICIENT_FRAME_DATA;
	}

	/*	Ensure we have a correct code size
	*/
	if (gif_data[0] > GIF_MAX_LZW)
		return GIF_DATA_ERROR;

	/*	Move our pointer to the actual image data
	*/
	gif_data++;
	if (--gif_bytes < 0)
		return GIF_INSUFFICIENT_FRAME_DATA;

	/*	Repeatedly skip blocks until we get a zero block or run out of data
	 *	These blocks of image data are processed later by gif_decode_frame()
	*/
	block_size = 0;
	while (block_size != 1) {
		block_size = gif_data[0] + 1;
		/*	Check if the frame data runs off the end of the file
		*/
		if ((int)(gif_bytes - block_size) < 0) {
			/*	Try to recover by signaling the end of the gif.
			 *	Once we get garbage data, there is no logical
			 *	way to determine where the next frame is.
			 *	It's probably better to partially load the gif
			 *	than not at all.
			*/
			if (gif_bytes >= 2) {
				gif_data[0] = 0;
				gif_data[1] = GIF_TRAILER;
				gif_bytes = 1;
				++gif_data;
				break;
			} else
				return GIF_INSUFFICIENT_FRAME_DATA;
		} else {
			gif_bytes -= block_size;
			gif_data += block_size;
		}
	}

	/*	Add the frame and set the display flag
	*/
	gif->buffer_position = gif_data - gif->gif_data;
	gif->frame_count = frame + 1;
	gif->frames[frame].display = 1;

	/*	Check if we've finished
	*/
	if (gif_bytes < 1)
		return GIF_INSUFFICIENT_FRAME_DATA;
	else
		if (gif_data[0] == GIF_TRAILER) return GIF_OK;
	return GIF_WORKING;
}

/**	Attempts to initialise the frame's extensions

	@return GIF_INSUFFICIENT_FRAME_DATA for insufficient data to complete the frame
		GIF_OK for successful initialisation
*/
static gif_result gif_initialise_frame_extensions(gif_animation *gif, const int frame) {
	unsigned char *gif_data, *gif_end;
	int gif_bytes;
	unsigned int block_size;

	/*	Get our buffer position etc.
	*/
	gif_data = (unsigned char *)(gif->gif_data + gif->buffer_position);
	gif_end = (unsigned char *)(gif->gif_data + gif->buffer_size);
	
	/*	Initialise the extensions
	*/
	while (gif_data[0] == GIF_EXTENSION_INTRODUCER) {
		++gif_data;
		gif_bytes = (gif_end - gif_data);

		/*	Switch on extension label
		*/
		switch(gif_data[0]) {
			/* 6-byte Graphic Control Extension is:
			 *
			 *	+0	CHAR	Graphic Control Label
			 *	+1	CHAR	Block Size
			 *	+2	CHAR	__Packed Fields__
			 *			3BITS	Reserved
			 *			3BITS	Disposal Method
			 *			1BIT	User Input Flag
			 *			1BIT	Transparent Color Flag
			 *	+3	SHORT	Delay Time
			 *	+5	CHAR	Transparent Color Index
			*/
			case GIF_EXTENSION_GRAPHIC_CONTROL:
				if (gif_bytes < 6) return GIF_INSUFFICIENT_FRAME_DATA;
				gif->frames[frame].frame_delay = gif_data[3] | (gif_data[4] << 8);
				if (gif_data[2] & GIF_TRANSPARENCY_MASK) {
					gif->frames[frame].transparency = 1;
					gif->frames[frame].transparency_index = gif_data[5];
				}
				gif->frames[frame].disposal_method = ((gif_data[2] & GIF_DISPOSAL_MASK) >> 2);
				/*	I have encountered documentation and GIFs in the wild that use
				 *	0x04 to restore the previous frame, rather than the officially
				 *	documented 0x03.  I believe some (older?) software may even actually
				 *	export this way.  We handle this as a type of "quirks" mode.
				*/
				if (gif->frames[frame].disposal_method == GIF_FRAME_QUIRKS_RESTORE)
					gif->frames[frame].disposal_method = GIF_FRAME_RESTORE;
				gif_data += (2 + gif_data[1]);
				break;

			/* 14-byte+ Application Extension is:
			 *
			 *	+0	CHAR	Application Extension Label
			 *	+1	CHAR	Block Size
			 *	+2	8CHARS	Application Identifier
			 *	+10	3CHARS	Appl. Authentication Code
			 *	+13	1-256	Application Data (Data sub-blocks)
			*/
			case GIF_EXTENSION_APPLICATION:
				if (gif_bytes < 17) return GIF_INSUFFICIENT_FRAME_DATA;
				if ((gif_data[1] == 0x0b) &&
					(strncmp((const char *) gif_data + 2,
						"NETSCAPE2.0", 11) == 0) &&
					(gif_data[13] == 0x03) &&
					(gif_data[14] == 0x01)) {
						gif->loop_count = gif_data[15] | (gif_data[16] << 8);
				}
				gif_data += (2 + gif_data[1]);
				break;

			/*	Move the pointer to the first data sub-block
			 *	Skip 1 byte for the extension label
			*/
			case GIF_EXTENSION_COMMENT:
				++gif_data;
				break;

			/*	Move the pointer to the first data sub-block
			 *	Skip 2 bytes for the extension label and size fields
			 *	Skip the extension size itself
			*/
			default:
				gif_data += (2 + gif_data[1]);
		}

		/*	Repeatedly skip blocks until we get a zero block or run out of data
		 *	This data is ignored by this gif decoder
		*/
		gif_bytes = (gif_end - gif_data);
		block_size = 0;
		while (gif_data[0] != GIF_BLOCK_TERMINATOR) {
			block_size = gif_data[0] + 1;
			if ((gif_bytes -= block_size) < 0)
				return GIF_INSUFFICIENT_FRAME_DATA;
			gif_data += block_size;
		}
		++gif_data;
	}

	/*	Set buffer position and return
	*/
	gif->buffer_position = (gif_data - gif->gif_data);
	return GIF_OK;
}


/**	Decodes a GIF frame.

	@return GIF_FRAME_DATA_ERROR for GIF frame data error
		GIF_INSUFFICIENT_FRAME_DATA for insufficient data to complete the frame
		GIF_DATA_ERROR for GIF error (invalid frame header)
		GIF_INSUFFICIENT_DATA for insufficient data to do anything
		GIF_INSUFFICIENT_MEMORY for insufficient memory to process
		GIF_OK for successful decoding
		If a frame does not contain any image data, GIF_OK is returned and
			gif->current_error is set to GIF_FRAME_NO_DISPLAY
*/
gif_result gif_decode_frame(gif_animation *gif, unsigned int frame) {
	unsigned int index = 0;
	unsigned char *gif_data, *gif_end;
	int gif_bytes;
	unsigned int width, height, offset_x, offset_y;
	unsigned int flags, colour_table_size, interlace;
	unsigned long *colour_table;
	unsigned long *frame_data = 0;	// Set to 0 for no warnings
	unsigned long *frame_scanline;
	unsigned int save_buffer_position;
	unsigned int return_value = 0;
	unsigned int x, y, decode_y, burst_bytes;
	int last_undisposed_frame = (frame - 1);
	unsigned char colour;
	unsigned long tmpul,tmpul2; /* SH-05.04.2015 */
	int tmpi,offset_scanline=0; /* SH-05.04.2015 */

	/*	Ensure this frame is supposed to be decoded
	*/
	if (gif->frames[frame].display == 0) {
		gif->current_error = GIF_FRAME_NO_DISPLAY;
		return GIF_OK;
	}

	/*	Ensure we have a frame to decode
	*/
	if (frame > gif->frame_count_partial)
		return GIF_INSUFFICIENT_DATA;
	if ((!clear_image) && ((int)frame == gif->decoded_frame))
		return GIF_OK;

	/*	Get the start of our frame data and the end of the GIF data
	*/
	gif_data = gif->gif_data + gif->frames[frame].frame_pointer;
	gif_end = gif->gif_data + gif->buffer_size;
	gif_bytes = (gif_end - gif_data);

	/*	Check if we have enough data
	 *	The shortest block of data is a 10-byte image descriptor + 1-byte gif trailer
	*/
	if (gif_bytes < 12) return GIF_INSUFFICIENT_FRAME_DATA;

	/*	Save the buffer position
	*/
	save_buffer_position = gif->buffer_position;
	gif->buffer_position = gif_data - gif->gif_data;

	/*	Skip any extensions because we all ready processed them
	*/
	if ((return_value = gif_skip_frame_extensions(gif)) != GIF_OK)
		goto gif_decode_frame_exit;
	gif_data = (gif->gif_data + gif->buffer_position);
	gif_bytes = (gif_end - gif_data);

	/*	Ensure we have enough data for the 10-byte image descriptor + 1-byte gif trailer
	*/
	if (gif_bytes < 12) {
		return_value = GIF_INSUFFICIENT_FRAME_DATA;
		goto gif_decode_frame_exit;
	}

	/* 10-byte Image Descriptor is:
	 *
	 *	+0	CHAR	Image Separator (0x2c)
	 *	+1	SHORT	Image Left Position
	 *	+3	SHORT	Image Top Position
	 *	+5	SHORT	Width
	 *	+7	SHORT	Height
	 *	+9	CHAR	__Packed Fields__
	 *			1BIT	Local Colour Table Flag
	 *			1BIT	Interlace Flag
	 *			1BIT	Sort Flag
	 *			2BITS	Reserved
	 *			3BITS	Size of Local Colour Table
	*/
	if (gif_data[0] != GIF_IMAGE_SEPARATOR) {
		return_value = GIF_DATA_ERROR;
		goto gif_decode_frame_exit;
	}
	offset_x = gif_data[1] | (gif_data[2] << 8);
	offset_y = gif_data[3] | (gif_data[4] << 8);
	width = gif_data[5] | (gif_data[6] << 8);
	height = gif_data[7] | (gif_data[8] << 8);

	/*	Boundary checking - shouldn't ever happen except unless the data has been
		modified since initialisation.
	*/
	if ((offset_x + width > gif->width) || (offset_y + height > gif->height)) {
		return_value = GIF_DATA_ERROR;
		goto gif_decode_frame_exit;
	}

	/*	Decode the flags
	*/
	flags = gif_data[9];
	colour_table_size = 2 << (flags & GIF_COLOUR_TABLE_SIZE_MASK);
	interlace = (flags & GIF_INTERLACE_MASK)?1:0;

	/*	Move our pointer to the colour table or image data (if no colour table is given)
	*/
	gif_data += 10;
	gif_bytes = (gif_end - gif_data);

	/*	Set up the colour table
	*/
	if (flags & GIF_COLOUR_TABLE_MASK) {
		if (gif_bytes < (int)(3 * colour_table_size)) {
			return_value = GIF_INSUFFICIENT_FRAME_DATA;
			goto gif_decode_frame_exit;
		}
		colour_table = gif->local_colour_table;
		if (!clear_image) {
			for (index = 0; index < colour_table_size; index++) {
				/* Gif colour map contents are r,g,b.
				 *
				 * We want to pack them bytewise into the 
				 * colour table, such that the red component
				 * is in byte 0 and the alpha component is in
				 * byte 3.
				 */
#if 0
				unsigned char *entry = 
					(unsigned char *) &colour_table[index];

				entry[0] = gif_data[0];	/* r */
				entry[1] = gif_data[1];	/* g */
				entry[2] = gif_data[2];	/* b */
				entry[3] = 0xff;	/* a */
#else
/* SH-04.04.2015: big-endian friendly code: */
				colour_table[index] = 0xFF000000 | (gif_data[2]<<16) | (gif_data[1]<<8) | gif_data[0];
#endif
				/* SH-05.04.2015: choose closest color index and store it in highest byte */
				if(gif->bpp < 32)
					colour_table[index] =
						gif_choosecolor(1<<gif->bpp,gif->map,colour_table[index]);
				gif_data += 3;
			}
		} else {
			gif_data += 3 * colour_table_size;
		}
		gif_bytes = (gif_end - gif_data);
	} else {
		colour_table = gif->global_colour_table;
	}

	/*	Check if we've finished
	*/
	if (gif_bytes < 1) {
		return_value = GIF_INSUFFICIENT_FRAME_DATA;
		goto gif_decode_frame_exit;
	} else if (gif_data[0] == GIF_TRAILER) {
		return_value = GIF_OK;
		goto gif_decode_frame_exit;
	}

	/*	Get the frame data
	*/
	/*assert(gif->bitmap_callbacks.bitmap_get_buffer);*/
	frame_data = (void *)gif->bitmap_callbacks.bitmap_get_buffer(gif->frame_image);
	if (!frame_data)
		return GIF_INSUFFICIENT_MEMORY;

	/*	If we are clearing the image we just clear, if not decode
	*/
	if (!clear_image) {
		/*	Ensure we have enough data for a 1-byte LZW code size + 1-byte gif trailer
		*/
		if (gif_bytes < 2) {
			return_value = GIF_INSUFFICIENT_FRAME_DATA;
			goto gif_decode_frame_exit;
		/*	If we only have a 1-byte LZW code size + 1-byte gif trailer, we're finished
		*/
		} else if ((gif_bytes == 2) && (gif_data[1] == GIF_TRAILER)) {
			return_value = GIF_OK;
			goto gif_decode_frame_exit;
		}

		/*	If the previous frame's disposal method requires we restore the background
		 *	colour or this is the first frame, clear the frame data
		*/
		if ((frame == 0) || (gif->decoded_frame == GIF_INVALID_FRAME)) {
			if(gif->bpp < 32) {
				memset((char*)frame_data, 0x00, (gif->width * gif->height * gif->bpp) >> 3);
			} else {
				memset((char*)frame_data, 0x00, gif->width * gif->height * sizeof(long));
			}
			gif->decoded_frame = frame;
			/* The line below would fill the image with its background color, but because GIFs support
			 * transparency we likely wouldn't want to do that. */
			/* memset((char*)frame_data, colour_table[gif->background_index], gif->width * gif->height * sizeof(int)); */
		} else if ((frame != 0) && (gif->frames[frame - 1].disposal_method == GIF_FRAME_CLEAR)) {
			clear_image = 1;
			if ((return_value = gif_decode_frame(gif, (frame - 1))) != GIF_OK)
				goto gif_decode_frame_exit;
			clear_image = 0;
		/*	If the previous frame's disposal method requires we restore the previous
		 *	image, find the last image set to "do not dispose" and get that frame data
		*/
		} else if ((frame != 0) && (gif->frames[frame - 1].disposal_method == GIF_FRAME_RESTORE)) {
			while ((last_undisposed_frame != -1) && (gif->frames[--last_undisposed_frame].disposal_method == GIF_FRAME_RESTORE))
				;

			/*	If we don't find one, clear the frame data
			 */
			if (last_undisposed_frame == -1) {
				/* see notes above on transparency vs. background color */
				if(gif->bpp < 32) {
					memset((char*)frame_data, 0x00, (gif->width * gif->height * gif->bpp) >> 3);
				} else {
					memset((char*)frame_data, 0x00, gif->width * gif->height * sizeof(long));
				}
			} else {
				if ((return_value = gif_decode_frame(gif, last_undisposed_frame)) != GIF_OK)
					goto gif_decode_frame_exit;
				/*	Get this frame's data
				*/
				/*assert(gif->bitmap_callbacks.bitmap_get_buffer);*/
				frame_data = (void *)gif->bitmap_callbacks.bitmap_get_buffer(gif->frame_image);
				if (!frame_data)
					return GIF_INSUFFICIENT_MEMORY;
			}
		}
		gif->decoded_frame = frame;

		/*	Initialise the LZW decoding
		*/
		set_code_size = gif_data[0];
		gif->buffer_position = (gif_data - gif->gif_data) + 1;

		/*	Set our code variables
		*/
		code_size = set_code_size + 1;
		clear_code = (1 << set_code_size);
		end_code = clear_code + 1;
		max_code_size = clear_code << 1;
		max_code = clear_code + 2;
		curbit = lastbit = 0;
		last_byte = 2;
		get_done = 0;
		direct = buf;
		gif_init_LZW(gif);

		/*	Decompress the data
		*/
		for (y = 0; y < height; y++) {
			if (interlace)
				decode_y = gif_interlaced_line(height, y) + offset_y;
			else
				decode_y = y + offset_y;
			if(gif->bpp < 32)
			{
			frame_scanline = frame_data + (((offset_x + decode_y * gif->width) * gif->bpp)>>5);
			switch(gif->bpp)
			{
				case 1: offset_scanline = (offset_x & 31);    break;
				case 2: offset_scanline = (offset_x & 15)<<1; break;
				case 4: offset_scanline = (offset_x &  7)<<2; break;
				case 8: offset_scanline = (offset_x &  3)<<3; break;
			}
			} else {
			frame_scanline = frame_data + offset_x + (decode_y * gif->width);
			}
			/*	Rather than decoding pixel by pixel, we try to burst out streams
				of data to remove the need for end-of data checks every pixel.
			*/
			x = width;
			while (x > 0) {
				burst_bytes = (stack_pointer - stack);
				if (burst_bytes > 0) {
					if (burst_bytes > x)
						burst_bytes = x;
					x -= burst_bytes;
					while (burst_bytes-- > 0) {
						colour = *--stack_pointer;
						if (((gif->frames[frame].transparency) &&
							(colour != gif->frames[frame].transparency_index)) ||
							(!gif->frames[frame].transparency))
						{
							switch(gif->bpp)
							{
							case 1:
							if(colour_table[colour]&0xFF000000)
								*frame_scanline |= ((colour_table[colour]<<7)&0x80000000)>>offset_scanline;
							else
								*frame_scanline &= ~(((colour_table[colour]<<7)&0x80000000)>>offset_scanline);
							break;
							
							case 2:
							tmpul = 0xC0000000;
							*frame_scanline &= ~(tmpul>>offset_scanline);
							*frame_scanline |= ((colour_table[colour]<<6)&0xC0000000)>>offset_scanline;
							break;
							
							case 4:
							tmpul = 0xF0000000;
							*frame_scanline &= ~(tmpul>>offset_scanline);
							*frame_scanline |= ((colour_table[colour]<<4)&0xF0000000)>>offset_scanline;
							break;
							
							case 8:
							tmpul = 0xFF000000;
							*frame_scanline &= ~(tmpul>>offset_scanline);
							*frame_scanline |= (colour_table[colour]&0xFF000000)>>offset_scanline;
							break;
							
							default: *frame_scanline = colour_table[colour]; break;
							}
						}
						/* move one pixel right */
						if(gif->bpp < 32)
						{
							offset_scanline += gif->bpp;
							if(offset_scanline >= 32)
							{
								frame_scanline++;
								offset_scanline=0;
							}
						}
						else frame_scanline++;
					}
				} else {
					if (!gif_next_LZW(gif)) {
						/*	Unexpected end of frame, try to recover
						*/
						if (gif->current_error == GIF_END_OF_FRAME)
							return_value = GIF_OK;
						else
							return_value = gif->current_error;
						goto gif_decode_frame_exit;
					}
				}
			}
		}
	} else {
		/*	Clear our frame
		*/
		if (gif->frames[frame].disposal_method == GIF_FRAME_CLEAR) {
			for (y = 0; y < height; y++) {
				if(gif->bpp < 32)
				{
				frame_scanline = frame_data + (((offset_x + (y + offset_y) * gif->width) * gif->bpp)>>5);
				switch(gif->bpp)
				{
				case 1: offset_scanline = (offset_x & 31);    tmpul2=(colour_table[gif->background_index]<<7)&0x80000000; break;
				case 2: offset_scanline = (offset_x & 15)<<1; tmpul2=(colour_table[gif->background_index]<<6)&0xC0000000; break;
				case 4: offset_scanline = (offset_x &  7)<<2; tmpul2=(colour_table[gif->background_index]<<4)&0xF0000000; break;
				case 8: offset_scanline = (offset_x &  3)<<3; tmpul2=colour_table[gif->background_index]&0xFF000000; break;
				}
				if (gif->frames[frame].transparency) tmpul2 = 0;
				for(tmpi=0;tmpi<width;tmpi++)
				{
					switch(gif->bpp)
					{
					case 1:
						if(tmpul2)
							*frame_scanline |= tmpul2>>offset_scanline;
						else
							*frame_scanline &= ~(tmpul2>>offset_scanline);
						break;
						
					case 2:
						tmpul = 0xC0000000;
						*frame_scanline &= ~(tmpul>>offset_scanline);
						*frame_scanline |= tmpul2>>offset_scanline;
						break;
						
					case 4:
						tmpul = 0xF0000000;
						*frame_scanline &= ~(tmpul>>offset_scanline);
						*frame_scanline |= tmpul2>>offset_scanline;
						break;
						
					case 8:
						tmpul = 0xFF000000;
						*frame_scanline &= ~(tmpul>>offset_scanline);
						*frame_scanline |= tmpul2>>offset_scanline;
						break;
					}
					offset_scanline += gif->bpp;
					if(offset_scanline >= 32)
					{
						frame_scanline++;
						offset_scanline=0;
					}
				}
				} else {
				frame_scanline = frame_data + offset_x + ((offset_y + y) * gif->width);
#if 0
/* SH-05.04.2015: is it a bug here where 32-bit colors are copied as bytes?... */
				if (gif->frames[frame].transparency)
					memset(frame_scanline, GIF_TRANSPARENT_COLOUR, width * 4);
				else
					memset(frame_scanline, colour_table[gif->background_index], width * 4);
#else
/* SH-05.04.2015 */
				if (gif->frames[frame].transparency) tmpul = GIF_TRANSPARENT_COLOUR;
				else tmpul = colour_table[gif->background_index];
				for(tmpi=0; tmpi < width; tmpi++) frame_scanline[tmpi] = tmpul;
#endif
				}
			}
		}
	}
gif_decode_frame_exit:

	/*	Check if we should test for optimisation
	*/
	if (gif->frames[frame].virgin) {
		if (gif->bitmap_callbacks.bitmap_test_opaque)
			gif->frames[frame].opaque = gif->bitmap_callbacks.bitmap_test_opaque(gif->frame_image);
		else
			gif->frames[frame].opaque = 0;
		gif->frames[frame].virgin = 0;
	}
	if (gif->bitmap_callbacks.bitmap_set_opaque)
		gif->bitmap_callbacks.bitmap_set_opaque(gif->frame_image, gif->frames[frame].opaque);
	if (gif->bitmap_callbacks.bitmap_modified)
		gif->bitmap_callbacks.bitmap_modified(gif->frame_image);

	/*	Restore the buffer position
	*/
	gif->buffer_position = save_buffer_position;

	/*	Success!
	*/
	return return_value;

}

/**	Skips the frame's extensions (which have been previously initialised)

	@return GIF_INSUFFICIENT_FRAME_DATA for insufficient data to complete the frame
		GIF_OK for successful decoding
*/
static gif_result gif_skip_frame_extensions(gif_animation *gif) {
	unsigned char *gif_data, *gif_end;
	int gif_bytes;
	unsigned int block_size;

	/*	Get our buffer position etc.
	*/
	gif_data = (unsigned char *)(gif->gif_data + gif->buffer_position);
	gif_end = (unsigned char *)(gif->gif_data + gif->buffer_size);
	gif_bytes = (gif_end - gif_data);

	/*	Skip the extensions
	*/
	while (gif_data[0] == GIF_EXTENSION_INTRODUCER) {
		++gif_data;

		/*	Switch on extension label
		*/
		switch(gif_data[0]) {
			/*	Move the pointer to the first data sub-block
			 *	1 byte for the extension label
			*/
			case GIF_EXTENSION_COMMENT:
				++gif_data;
				break;

			/*	Move the pointer to the first data sub-block
			 *	2 bytes for the extension label and size fields
			 *	Skip the extension size itself
			*/
			default:
				gif_data += (2 + gif_data[1]);
		}

		/*	Repeatedly skip blocks until we get a zero block or run out of data
		 *	This data is ignored by this gif decoder
		*/
		gif_bytes = (gif_end - gif_data);
		block_size = 0;
		while (gif_data[0] != GIF_BLOCK_TERMINATOR) {
			block_size = gif_data[0] + 1;
			if ((gif_bytes -= block_size) < 0)
				return GIF_INSUFFICIENT_FRAME_DATA;
			gif_data += block_size;
		}
		++gif_data;
	}

	/*	Set buffer position and return
	*/
	gif->buffer_position = (gif_data - gif->gif_data);
	return GIF_OK;
}

static unsigned int gif_interlaced_line(int height, int y) {
	if ((y << 3) < height) return (y << 3);
	y -= ((height + 7) >> 3);
	if ((y << 3) < (height - 4)) return (y << 3) + 4;
	y -= ((height + 3) >> 3);
	if ((y << 2) < (height - 2)) return (y << 2) + 2;
	y -= ((height + 1) >> 2);
	return (y << 1) + 1;
}

/*	Releases any workspace held by the animation
*/
void gif_finalise(gif_animation *gif) {
	/*	Release all our memory blocks
	*/
	if (gif->frame_image) {
		/*assert(gif->bitmap_callbacks.bitmap_destroy);*/
		gif->bitmap_callbacks.bitmap_destroy(gif->frame_image);
	}
	gif->frame_image = NULL;
	free(gif->frames);
	gif->frames = NULL;
/*
	free(gif->local_colour_table);
	gif->local_colour_table = NULL;
	free(gif->global_colour_table);
	gif->global_colour_table = NULL;
*/
}

/**
 * Initialise LZW decoding
 */
void gif_init_LZW(gif_animation *gif) {
	int i;

	gif->current_error = 0;
	if (clear_code >= (1 << GIF_MAX_LZW)) {
		stack_pointer = stack;
		gif->current_error = GIF_FRAME_DATA_ERROR;
		return;
	}

	/* initialise our table */
	memset(table, 0x00, (1 << GIF_MAX_LZW) * 8);
	for (i = 0; i < clear_code; ++i)
		table[1][i] = i;

	/* update our LZW parameters */
	code_size = set_code_size + 1;
	max_code_size = clear_code << 1;
	max_code = clear_code + 2;
	stack_pointer = stack;
	do {
		firstcode = oldcode = gif_next_code(gif, code_size);
	} while (firstcode == clear_code);
	*stack_pointer++ =firstcode;
}


static unsigned char gif_next_LZW(gif_animation *gif) {
	int code, incode;
	int block_size;
	int new_code;

	code = gif_next_code(gif, code_size);
	if (code < 0) {
	  	gif->current_error = code;
		return 0;
	} else if (code == clear_code) {
		gif_init_LZW(gif);
		return 1;
	} else if (code == end_code) {
		/* skip to the end of our data so multi-image GIFs work */
		if (zero_data_block) {
			gif->current_error = GIF_FRAME_DATA_ERROR;
			return 0;
		}
		block_size = 0;
		while (block_size != 1) {
			block_size = gif->gif_data[gif->buffer_position] + 1;
			gif->buffer_position += block_size;
		}
		gif->current_error = GIF_FRAME_DATA_ERROR;
		return 0;
	}

	incode = code;
	if (code >= max_code) {
		*stack_pointer++ = firstcode;
		code = oldcode;
	}

	/* The following loop is the most important in the GIF decoding cycle as every
	 * single pixel passes through it.
	 *
	 * Note: our stack is always big enough to hold a complete decompressed chunk. */
	while (code >= clear_code) {
		*stack_pointer++ = table[1][code];
		new_code = table[0][code];
		if (new_code < clear_code) {
			code = new_code;
			break;
		}
		*stack_pointer++ = table[1][new_code];
		code = table[0][new_code];
		if (code == new_code) {
		  	gif->current_error = GIF_FRAME_DATA_ERROR;
			return 0;
		}
	}

	*stack_pointer++ = firstcode = table[1][code];

	if ((code = max_code) < (1 << GIF_MAX_LZW)) {
		table[0][code] = oldcode;
		table[1][code] = firstcode;
		++max_code;
		if ((max_code >= max_code_size) && (max_code_size < (1 << GIF_MAX_LZW))) {
			max_code_size = max_code_size << 1;
			++code_size;
		}
	}
	oldcode = incode;
	return 1;
}

static int gif_next_code(gif_animation *gif, int code_size) {
	int i, j, end, count, ret;
	unsigned char *b;

	end = curbit + code_size;
	if (end >= lastbit) {
		if (get_done)
			return GIF_END_OF_FRAME;
		buf[0] = direct[last_byte - 2];
		buf[1] = direct[last_byte - 1];

		/* get the next block */
		direct = gif->gif_data + gif->buffer_position;
		zero_data_block = ((count = direct[0]) == 0);
		if ((gif->buffer_position + count) >= gif->buffer_size)
			return GIF_INSUFFICIENT_FRAME_DATA;
		if (count == 0)
			get_done = 1;
		else {
			direct -= 1;
			buf[2] = direct[2];
			buf[3] = direct[3];
		}
		gif->buffer_position += count + 1;

		/* update our variables */
		last_byte = 2 + count;
		curbit = (curbit - lastbit) + 16;
		lastbit = (2 + count) << 3;
		end = curbit + code_size;
	}

	i = curbit >> 3;
	if (i < 2)
		b = buf;
	else
		b = direct;

	ret = b[i];
	j = (end >> 3) - 1;
	if (i <= j) {
		ret |= (b[i + 1] << 8);
		if (i < j)
			ret |= (b[i + 2] << 16);
	}
	ret = (ret >> (curbit % 8)) & maskTbl[code_size];
	curbit += code_size;
	return ret;
}

static unsigned long gif_choosecolor(int mapsz, unsigned long* map, unsigned long color)
{
 int i,j = -1;
 int f,e = 30000;
 long a,r,g,b;
 r = color & 0x000000FF;
 g = color & 0x0000FF00;
 b = color & 0x00FF0000;
 for(i=0;i<mapsz;i++)
 {
   f = 0; /* current error */
   a = map[i] & 0x000000FF;
   if(a < r) f+=r-a;
   else f+=a-r;
   a = map[i] & 0x0000FF00;
   if(a < g) f+=(g-a)>>8;
   else f+=(a-g)>>8;
   a = map[i] & 0x00FF0000;
   if(a < b) f+=(b-a)>>16;
   else f+=(a-b)>>16;
   if(f < e) /* found next possible close color */
   {
      e = f;
      j = i;
   }
 }
 printf(">>> gif_choosecolor mapsz=%i color=#%8.8X closest=%i error=%i\n",mapsz,color,j,e);
 if(j >= 0) color = (color & 0x00FFFFFF) | (j<<24);
 return color;
}

/*<><><><><><><><><><><><><><> XORLib interface <><><><><><><><><><><><><><>*/

#undef malloc
#undef realloc
#undef free

void* my_malloc(unsigned long s)
{
  void* p = malloc(s);
  printf(">>> malloc %lu\t-> 0x%8.8X\n",s,p);
  return p;
}

void* my_realloc(void* p, unsigned long s)
{
  void* pp = realloc(p,s);
  printf(">>> realloc 0x%8.8X %lu\t-> 0x%8.8X\n",p,s,pp);
  return pp;
}

void my_free(void* p)
{
  printf(">>> free 0x%8.8X\n",p);
  free(p);
}


















