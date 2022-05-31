/**
* Implementation of several functions to manipulate BMP files.
 *
* @author Bichen Pang
* @version 1.0 May 26 2022
*/

#include <stdio.h>
#include "BMPHandler.h"

/**
 * Read BMP header of a BMP file.
 *
 * @param  file: A pointer to the file being read
 * @param  header: Pointer to the destination BMP header
 */
void readBMPHeader(FILE* file, struct BMP_Header* header) {
    fread(&(header->signature), sizeof(char) * 2, 1, file);
    fread(&(header->size), sizeof(int), 1, file);
    fread(&(header->reserved1), sizeof(short), 1, file);
    fread(&(header->reserved2), sizeof(short), 1, file);
    fread(&(header->offset_pixel_array), sizeof(int), 1, file);
}

/**
 * Write BMP header of a file. Useful for creating a BMP file.
 *
 * @param  file: A pointer to the file being written
 * @param  header: The header to write to the file
 */
void writeBMPHeader(FILE* file, struct BMP_Header* header) {
    fwrite(&(header->signature), sizeof(char) * 2, 1, file);
    fwrite(&(header->size), sizeof(int), 1, file);
    fwrite(&(header->reserved1), sizeof(short), 1, file);
    fwrite(&(header->reserved2), sizeof(short), 1, file);
    fwrite(&(header->offset_pixel_array), sizeof(int), 1, file);
}

/**
 * Read DIB header from a BMP file.
 *
 * @param  file: A pointer to the file being read
 * @param  header: Pointer to the destination DIB header
 */
void readDIBHeader(FILE* file, struct DIB_Header* header) {
    fread(&(header->dib_header), sizeof(int), 1, file);
    fread(&(header->image_width), sizeof(int), 1, file);
    fread(&(header->image_height), sizeof(int), 1, file);
    fread(&(header->planes), sizeof(short), 1, file);
    fread(&(header->bits_per_pixel), sizeof(short), 1, file);
    fread(&(header->compression), sizeof(int), 1, file);
    fread(&(header->image_size), sizeof(int), 1, file);
    fread(&(header->x_pixel_per_meter), sizeof(int), 1, file);
    fread(&(header->y_pixel_per_meter), sizeof(int), 1, file);
    fread(&(header->color_table), sizeof(int), 1, file);
    fread(&(header->important_color_count), sizeof(int), 1, file);
}

/**
 * Write DIB header of a file. Useful for creating a BMP file.
 *
 * @param  file: A pointer to the file being written
 * @param  header: The header to write to the file
 */
void writeDIBHeader(FILE* file, struct DIB_Header* header) {
    fwrite(&(header->dib_header), sizeof(int), 1, file);
    fwrite(&(header->image_width), sizeof(int), 1, file);
    fwrite(&(header->image_height), sizeof(int), 1, file);
    fwrite(&(header->planes), sizeof(short), 1, file);
    fwrite(&(header->bits_per_pixel), sizeof(short), 1, file);
    fwrite(&(header->compression), sizeof(int), 1, file);
    fwrite(&(header->image_size), sizeof(int), 1, file);
    fwrite(&(header->x_pixel_per_meter), sizeof(int), 1, file);
    fwrite(&(header->y_pixel_per_meter), sizeof(int), 1, file);
    fwrite(&(header->color_table), sizeof(int), 1, file);
    fwrite(&(header->important_color_count), sizeof(int), 1, file);

}

/**
 * Make BMP header based on width and height. Useful for creating a BMP file.
 *
 * @param  header: Pointer to the destination DIB header
 * @param  width: Width of the image that this header is for
 * @param  height: Height of the image that this header is for
 */
void makeBMPHeader(struct BMP_Header* header, int width, int height) {
    //we only need change the size of the header
    header->size = width * height * 3 + 54;
}

/**
* Make new DIB header based on width and height.Useful for creating a BMP file.
*
* @param  header: Pointer to the destination DIB header
* @param  width: Width of the image that this header is for
* @param  height: Height of the image that this header is for
*/
void makeDIBHeader(struct DIB_Header* header, int width, int height) {
    // we need to update the width and height info
    header->image_width = width;
    header->image_height = height;
}

/**
 * Read Pixels from BMP file based on width and height.
 *
 * @param  file: A pointer to the file being read
 * @param  pArr: Pixel array to store the pixels being read
 * @param  width: Width of the pixel array of this image
 * @param  height: Height of the pixel array of this image
 */
void readPixelsBMP(FILE* file, struct Pixel** pArr, int width, int height) {
    // calculate padding size
    int length = width * 3;
    if (length % 4 != 0) {
        length = length + 4 - (length % 4);
    }
    int paddingSize = length - (width * 3);

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fread(&pArr[i][j].blue, sizeof(char),1, file);
            fread(&pArr[i][j].green, sizeof(char),1, file);
            fread(&pArr[i][j].red, sizeof(char),1, file);
        }
        //skip padding
        fseek(file, sizeof(unsigned char) * paddingSize, SEEK_CUR);
    }
}

/**
 * Write Pixels from BMP file based on width and height.
 *
 * @param  file: A pointer to the file being read or written
 * @param  pArr: Pixel array of the image to write to the file
 * @param  width: Width of the pixel array of this image
 * @param  height: Height of the pixel array of this image
 */
void writePixelsBMP(FILE* file, struct Pixel** pArr, int width, int height) {
    // calculate padding size
    int length = width * 3;
    if (length % 4 != 0) {
        length = length + 4 - (length % 4);
    }
    int paddingSize = length - (width * 3);

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fwrite(&pArr[i][j].blue, sizeof(char),1, file);
            fwrite(&pArr[i][j].green, sizeof(char),1, file);
            fwrite(&pArr[i][j].red, sizeof(char),1, file);
        }
        //write padding
        for (int k = 0; k < paddingSize; k++) {
            fputc(0, file);
        }
    }
}