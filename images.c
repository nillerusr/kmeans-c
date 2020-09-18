//
// Created by nillerusr on 30.01.20.
//

#include "images.h"
#include <dlfcn.h>
#include <jpeglib.h>
#include <setjmp.h>
#include <stdio.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdlib.h>
#include "config.h"
#include "png.h"

static long int buflen = 0;
static image_t *img;

int width, height;
png_byte color_type;
png_byte bit_depth;
png_bytep *row_pointers = NULL;

bool add_points_count( int r, int g, int b )
{
    for(int i = 0; i < img->count; i++)
    {
        if( img->points[i].color[0] == r && img->points[i].color[1] == g && img->points[i].color[2] == b )
        {
            img->points[i].count++;
            return true;
        }
    }
    return false;
}


void put_png_to_array()
{
    img->points = (point_t*)malloc( width * height * sizeof(point_t));
    img->count = 0;
    for (int y = 0; y < height; y++)
    {
        png_byte *row = row_pointers[y];
        for (int x = 0; x < width; x++)
        {
            png_byte *ptr = &(row[x * 4]);
            if( ptr[3] > 10 )
            {
                if( !add_points_count(ptr[0], ptr[1], ptr[2]) )
                {
                    img->points[img->count].color[0] = ptr[0];
                    img->points[img->count].color[1] = ptr[1];
                    img->points[img->count].color[2] = ptr[2];
                    img->points[img->count].count = 1;
                    img->count++;
                }
            }
        }
        free(row_pointers[y]);
    }
    free(row_pointers);
}


void read_png_file(const char *filename)
{
    FILE *fp = fopen(filename, "rb");
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(!png)
        return;


    png_infop info = png_create_info_struct(png);
    if(!info)
        return;

    png_init_io(png, fp);

    png_read_info(png, info);

    width      = png_get_image_width(png, info);
    height     = png_get_image_height(png, info);
    color_type = png_get_color_type(png, info);
    bit_depth  = png_get_bit_depth(png, info);

    // Read any color_type into 8bit depth, RGBA format.
    // See http://www.libpng.org/pub/png/libpng-manual.txt

    if(bit_depth == 16)
        png_set_strip_16(png);

    if(color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);

    // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
    if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png);

    if(png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);

    // These color_type don't have an alpha channel then fill it with 0xff.
    if(color_type == PNG_COLOR_TYPE_RGB ||
       color_type == PNG_COLOR_TYPE_GRAY ||
       color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

    if(color_type == PNG_COLOR_TYPE_GRAY ||
       color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);

    png_read_update_info(png, info);

    row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
    for(int y = 0; y < height; y++) {
        row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
    }


    png_read_image(png, row_pointers);

    fclose(fp);

    png_destroy_read_struct(&png, &info, NULL);

    put_png_to_array();
}




void put_scanline_someplace (JSAMPLE* ba, int row_stride)
{
    int i;

    for (i=0; i < row_stride/3; i+=3)
    {
        if( !add_points_count(ba[i], ba[i+1], ba[i+2]) )
        {
            img->points[img->count].color[0] = ba[i];
            img->points[img->count].color[1] = ba[i+1];
            img->points[img->count].color[2] = ba[i+2];
            img->points[img->count].count = 1;
            img->count++;
        }
    }
}

void read_JPEG_file (const char * filename)
{
    struct jpeg_error_mgr jerr;

    struct jpeg_decompress_struct cinfo;
    FILE * infile;
    JSAMPARRAY buffer;
    int row_stride;

    if ((infile = fopen(filename, "rb")) == NULL)
    {
        return;
    }

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo); //, JPEG_LIB_VERSION, (size_t) sizeof(struct jpeg_decompress_struct));
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    row_stride = cinfo.output_width * cinfo.output_components;
    buffer = (*cinfo.mem->alloc_sarray)
            ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

    img->points = (point_t*)malloc( cinfo.output_height * cinfo.output_width * sizeof(point_t));

    while (cinfo.output_scanline < cinfo.output_height)
    {
        jpeg_read_scanlines(&cinfo, buffer, 1);
        put_scanline_someplace(buffer[0], row_stride);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
}

image_t *decodeImage(const char *fname, int imageformat)
{
    buflen = 1024;

    img = (image_t*) malloc(sizeof(image_t));
    img->count = 0;

    if (imageformat == IMAGE_FORMAT_JPEG)
        read_JPEG_file(fname);
    else if (imageformat == IMAGE_FORMAT_PNG)
        read_png_file(fname);

    return img;
}
