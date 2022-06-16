/**
* Header file for the image ADT.
*
* @author Sheldon Pang
* @version 1.1
 * 1.1 update notes: Added box blur filter, "void  image_apply_blur_filter(Image* img)"
 *                   Added structure for thread infos
*/

#ifndef BMP_PROCESSOR_MULTI_THREAD_IMAGE_H
#define BMP_PROCESSOR_MULTI_THREAD_IMAGE_H

#include <stdio.h>
#include <stdlib.h>

typedef struct Image Image;

struct Image {
    struct Pixel** pArr;
    int width;
    int height;
};

struct Pixel{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};

struct thread_args {
    int thread_height;
    int thread_width;
    struct Pixel** pArr;
};

/** Creates a new image and returns it.
*
 * @param  pArr: Pixel array of this image.
 * @param  width: Width of this image.
 * @param  height: Height of this image.
 * @return A pointer to a new image.
*/
Image* image_create(struct Pixel** pArr, int width, int height);


/** Destroys an image. Does not deallocate internal pixel array.
*
 * @param  img: the image to destroy.
*/
void image_destroy(Image** img);

/** Returns a double pointer to the pixel array.
*
 * @param  img: the image.
*/
struct Pixel** image_get_pixels(Image* img);

/** Returns the width of the image.
*
 * @param  img: the image.
*/
int image_get_width(Image* img);

/** Returns the height of the image.
*
 * @param  img: the image.
*/
int image_get_height(Image* img);

/** Apply box blur filter to image.
*   Output pixel is computed as the average of itself and each of its neighbors
*
 * @param  img: the image.
 * @param  thread_count: the total number of thread.
 * @param  thread_id: the id of the thread
*/
void*  image_apply_blur_filter(void* arguments);

/** Apply Swiss Cheese filter to image.
*   Tint the image (at the pixel level) towards being slightly yellow.
*
 * @param  img: the image.
 * @param  thread_count: the total number of thread.
 * @param  thread_id: the id of the thread
*/
void* image_apply_swiss_cheese_filter(void* thread_args);
void image_apply_holes(Image* img, int average_radius_holes);
void compute_holes (Image* img, int x, int y, int r);

#endif //BMP_PROCESSOR_MULTI_THREAD_IMAGE_H
