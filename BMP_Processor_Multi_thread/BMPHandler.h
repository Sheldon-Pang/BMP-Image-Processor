/**
* Header file of several functions to manipulate BMP files.
*
* @author Sheldon Pang
* @version 1.1
* update notes in 1.1: Added individual comments to explain each struct variables
*/

#ifndef BMP_PROCESSOR_MULTI_THREAD_BMPHANDLER_H
#define BMP_PROCESSOR_MULTI_THREAD_BMPHANDLER_H

#include <stdio.h>
#include "Image.h"

struct BMP_Header {
    char signature[2];          /* File Signature: "BM" */
    int size;                   /* Size of the BMP file in bytes */
    short reserved1;            /* Reserved1 */
    short reserved2;            /* Reserved2 */
    int offset_pixel_array;     /* File offset value indicate the start of the image data */
};

struct DIB_Header{
    int dib_header;             /* DIB Header size in bytes */
    int image_width;            /* Image's width */
    int image_height;           /* Image's height */
    short planes;               /* Number of the color planes in the image */
    short bits_per_pixel;       /* Number of bits per pixel */
    int compression;            /* Compression type */
    int image_size;             /* Size of the image data in bytes */
    int x_pixel_per_meter;      /* Horizontal resolution */
    int y_pixel_per_meter;      /* Vertical resolution */
    int color_table;            /* 4 * NumColors bytes */
    int important_color_count;  /* Number of important color 0 = all */
};

/**
 * Read BMP header of a BMP file.
 *
 * @param  file: A pointer to the file being read
 * @param  header: Pointer to the destination BMP header
 */
void readBMPHeader(FILE* file, struct BMP_Header* header);

/**
 * Write BMP header of a file. Useful for creating a BMP file.
 *
 * @param  file: A pointer to the file being written
 * @param  header: The header to write to the file
 */
void writeBMPHeader(FILE* file, struct BMP_Header* header);

/**
 * Read DIB header from a BMP file.
 *
 * @param  file: A pointer to the file being read
 * @param  header: Pointer to the destination DIB header
 */
void readDIBHeader(FILE* file, struct DIB_Header* header);

/**
 * Write DIB header of a file. Useful for creating a BMP file.
 *
 * @param  file: A pointer to the file being written
 * @param  header: The header to write to the file
 */
void writeDIBHeader(FILE* file, struct DIB_Header* header);

/**
 * Make BMP header based on width and height. Useful for creating a BMP file.
 *
 * @param  header: Pointer to the destination DIB header
 * @param  width: Width of the image that this header is for
 * @param  height: Height of the image that this header is for
 */
void makeBMPHeader(struct BMP_Header* header, int width, int height);

/**
* Make new DIB header based on width and height.Useful for creating a BMP file.
*
* @param  header: Pointer to the destination DIB header
* @param  width: Width of the image that this header is for
* @param  height: Height of the image that this header is for
*/
void makeDIBHeader(struct DIB_Header* header, int width, int height);

/**
 * Read Pixels from BMP file based on width and height.
 *
 * @param  file: A pointer to the file being read
 * @param  pArr: Pixel array to store the pixels being read
 * @param  width: Width of the pixel array of this image
 * @param  height: Height of the pixel array of this image
 */
void readPixelsBMP(FILE* file, struct Pixel** pArr, int width, int height);

/**
 * Write Pixels from BMP file based on width and height.
 *
 * @param  file: A pointer to the file being read or written
 * @param  pArr: Pixel array of the image to write to the file
 * @param  width: Width of the pixel array of this image
 * @param  height: Height of the pixel array of this image
 */
void writePixelsBMP(FILE* file, struct Pixel** pArr, int width, int height);

#endif //BMP_PROCESSOR_MULTI_THREAD_BMPHANDLER_H
