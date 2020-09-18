//
// Created by nillerusr on 30.01.20.
//

#ifndef COLOR_ANALYZER_IMAGES_H
#define COLOR_ANALYZER_IMAGES_H

#include <png.h>
#include <jpeglib.h>

#define IMAGE_FORMAT_JPEG 0
#define IMAGE_FORMAT_PNG 1

typedef struct point_s
{
    double color[3];
    int count;
} point_t;

typedef struct image_s
{
    point_t *points;
    int count;
} image_t;

typedef struct cluster_s
{
    point_t *plist;
    int pcount;
    point_t center;
} cluster_t;

image_t *decodeImage(const char *fname, int imageformat);

#endif //COLOR_ANALYZER_IMAGES_H
