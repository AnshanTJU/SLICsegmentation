/*
 * Copyright (c) 2012, Shan An <anshan.tju@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and
 * documentation are those of the authors and should not be
 * interpreted as representing official policies, either expressed
 * or implied, of the copyright holder.
 */

/**
 * @mainpage image read/write simplified interface
 *
 * README.txt:
 * @verbinclude README.txt
 */

/**
 * @file io_jpeg.c
 * @brief JPEG read/write simplified interface
 *
 * This is a front-end to libjpeg, with routines to:
 * @li read a JPEG file as a deinterlaced 8bit integer
 * @li write a 8bit integer array to a JPEG file
 *
 * Multi-channel images supported : grey, rgb.
 *
 * Modified according to Junaed Sattar's example for libjpeg.
 * http://www.cim.mcgill.ca/~junaed/libjpeg.php
 * Thanks to Nicolas Limare's io_png interface for libpng
 * and the Image Processing Online http://www.ipol.im/
 * 
 * @author Shan An <anshan.tju@gmail.com>
 */

#include <stdlib.h>
#include <stdio.h>
#include <jpeglib.h>
#include "io_jpeg.h"

unsigned char *read_jpeg_u8(const char *fname, int *width, int *height, 
				  int *bytes_per_pixel, int *color_space)
{
	/* these are standard libjpeg structures for reading(decompression) */
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE *infile = NULL;
	unsigned char *data = NULL;
	unsigned long location = 0;
	unsigned int i = 0;
	/* libjpeg data structure for storing one row, that is, scanline of an image */
	JSAMPROW row_pointer[1];

	/* parameters check */
    if (NULL == fname)
        return NULL;
 	
	/* open the JPEG input file */
    if (NULL == (infile = fopen(fname, "rb")))
        return NULL;

	/* here we set up the standard libjpeg error handler */
	cinfo.err = jpeg_std_error( &jerr );
	/* setup decompression process and source, then read JPEG header */
	jpeg_create_decompress( &cinfo );
	/* this makes the library read from infile */
	jpeg_stdio_src( &cinfo, infile );
	/* reading the image header which contains image information */
	jpeg_read_header( &cinfo, TRUE );
	
	/* Start decompression jpeg here */
	jpeg_start_decompress( &cinfo );
	/* save image dimension information */
	*width = cinfo.image_width; 
	*height = cinfo.image_height; 
	*bytes_per_pixel = cinfo.num_components;
	*color_space =  JCS_RGB;

	/* allocate memory to hold the uncompressed image */
	data = (unsigned char*)malloc( cinfo.image_width*cinfo.image_height*cinfo.num_components );
	/* now actually read the jpeg into the raw buffer */
	row_pointer[0] = (unsigned char *)malloc( cinfo.image_width*cinfo.image_height*cinfo.num_components );
	/* read one scan line at a time */
	while( cinfo.output_scanline < cinfo.image_height )
	{
		jpeg_read_scanlines( &cinfo, row_pointer, 1 );
		for( i = 0; i < cinfo.image_width*cinfo.num_components; i++) 
			data[location++] = row_pointer[0][i];
	}
	/* wrap up decompression, destroy objects, free pointers and close open files */
	jpeg_finish_decompress( &cinfo );
	jpeg_destroy_decompress( &cinfo );
	free( row_pointer[0] );
	fclose( infile );
	/* succeeded! */
	return data;
}


int write_jpeg_u8(const char *fname, unsigned char *data, int width, int height, 
				  int bytes_per_pixel, int color_space)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE *outfile = NULL;
	/* this is a pointer to one row of image data */
	JSAMPROW row_pointer[1];

	/* parameters check */
    if (0 >= width || 0 >= height || 0 >= bytes_per_pixel)
        return -1;
    if (NULL == fname || NULL == data)
        return -1;
 
	/* open the JPEG output file */
    if (NULL == (outfile = fopen(fname, "wb")))
        return -1;

	cinfo.err = jpeg_std_error( &jerr );
	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, outfile);

	/* Setting the parameters of the output file here */
	cinfo.image_width = width;	
	cinfo.image_height = height;
	cinfo.input_components = bytes_per_pixel;
	cinfo.in_color_space = color_space;

    /* default compression parameters, we shouldn't be worried about these */
	jpeg_set_defaults( &cinfo );
	/* Now do the compression .. */
	jpeg_start_compress( &cinfo, TRUE );
	/* like reading a file, this time write one row at a time */
	while( cinfo.next_scanline < cinfo.image_height )
	{
		row_pointer[0] = &data[ cinfo.next_scanline * cinfo.image_width *  cinfo.input_components];
		jpeg_write_scanlines( &cinfo, row_pointer, 1 );
	}
	/* similar to read file, clean up after we're done compressing */
	jpeg_finish_compress( &cinfo );
	jpeg_destroy_compress( &cinfo );
	fclose( outfile );
	/* success code is 1! */
    return 1;
}

/*
int main()
{
	char *infilename = "test.jpg", *outfilename = "test_out.jpg";

	int width, height, bytes_per_pixel, color_space;
	// Try opening a jpeg
	unsigned char *data = read_jpeg_u8(infilename, &width, &height, 
				 &bytes_per_pixel, &color_space);

	printf( "JPEG File Information: \n" );
	printf( "Image width and height: %d pixels and %d pixels.\n", width, height );
	printf( "Color components per pixel: %d.\n", bytes_per_pixel );
	printf( "Color space: %d.\n", color_space );

	// then copy it to another file
	if( write_jpeg_u8(outfilename, data, width, height, 
		bytes_per_pixel, color_space) < 0 ) 
		return -1;

	free(data);
	return 0;
}*/
