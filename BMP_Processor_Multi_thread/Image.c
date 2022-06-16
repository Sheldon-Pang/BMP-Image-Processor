/**
* Implementation of the image ADT.
*
* @author Sheldon Pang
* @version 1.1
 * 1.1 update notes: Added box blur filter, "void*  image_apply_blur_filter(void* thread_args)"
 *                   Added Swiss Cheese filter, "void* image_apply_swiss_cheese_filter(void* thread_args)"
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Image.h"

/** Creates a new image and returns it.
*
 * @param  pArr: Pixel array of this image.
 * @param  width: Width of this image.
 * @param  height: Height of this image.
 * @return A pointer to a new image.
*/
Image* image_create(struct Pixel** pArr, int width, int height) {
    Image *image = (Image*)malloc(sizeof (Image));

    image->width = width;
    image->height = height;
    image->pArr = pArr;

    return image;
}

/** Destroys an image. Does not deallocate internal pixel array.
*
 * @param  img: the image to destroy.
*/
void image_destroy(Image** img) {
    free(*img);
    *img = NULL;
}

/** Returns a double pointer to the pixel array.
*
 * @param  img: the image.
*/
struct Pixel** image_get_pixels(Image* img) {
    return img->pArr;
}

/** Returns the width of the image.
*
 * @param  img: the image.
*/
int image_get_width(Image* img) {
    return img->width;
}

/** Returns the height of the image.
*
 * @param  img: the image.
*/
int image_get_height(Image* img) {
    return img->height;
}

/** Apply box blur filter to image.
*   Output pixel is computed as the average of itself and each of its neighbors
*
 * @param  img: the image.
 * @param  thread_count: the total number of thread.
 * @param  thread_id: the id of the thread
*/
void*  image_apply_blur_filter(void* thread_args) {

    int thread_height = ((struct thread_args*)thread_args)->thread_height;
    int thread_width = ((struct thread_args*)thread_args)->thread_width;

    for (int i = 0; i < thread_height; i++) {
        for (int j = 0; j < thread_width; j++) {
            int increment_i = i + 1, decrement_i = i - 1, increment_j = j + 1, decrement_j = j - 1;
            if (increment_i == thread_height)       /* prevent index out of bounds */
                increment_i = i;
            if (decrement_i < 0)                    /* prevent index out of bounds */
                decrement_i = 0;
            if (increment_j == thread_width)        /* prevent index out of bounds */
                increment_j = j;
            if (decrement_j < 0)                    /* prevent index out of bounds */
                decrement_j = 0;

            /* The logic of blur filter is sums up all neighbor pixels and divide by 9 include self */
            ((struct thread_args*)thread_args)->pArr[i][j].red = (
                    ((struct thread_args*)thread_args)->pArr[decrement_i][decrement_j].red +
                    ((struct thread_args*)thread_args)->pArr[decrement_i][j].red+
                    ((struct thread_args*)thread_args)->pArr[decrement_i][increment_j].red +
                    ((struct thread_args*)thread_args)->pArr[i][decrement_j].red +
                    ((struct thread_args*)thread_args)->pArr[i][increment_j].red +
                    ((struct thread_args*)thread_args)->pArr[increment_i][decrement_j].red +
                    ((struct thread_args*)thread_args)->pArr[increment_i][j].red +
                    ((struct thread_args*)thread_args)->pArr[increment_i][increment_j].red
                            ) / 9;

            ((struct thread_args*)thread_args)->pArr[i][j].green = (
                    ((struct thread_args*)thread_args)->pArr[decrement_i][decrement_j].green +
                    ((struct thread_args*)thread_args)->pArr[decrement_i][j].green +
                    ((struct thread_args*)thread_args)->pArr[decrement_i][increment_j].green +
                    ((struct thread_args*)thread_args)->pArr[i][decrement_j].red +
                    ((struct thread_args*)thread_args)->pArr[i][increment_j].green +
                    ((struct thread_args*)thread_args)->pArr[increment_i][decrement_j].green +
                    ((struct thread_args*)thread_args)->pArr[increment_i][j].green +
                    ((struct thread_args*)thread_args)->pArr[increment_i][increment_j].green
                            ) / 9;

            ((struct thread_args*)thread_args)->pArr[i][j].blue = (
                    ((struct thread_args*)thread_args)->pArr[decrement_i][decrement_j].blue +
                    ((struct thread_args*)thread_args)->pArr[decrement_i][j].blue +
                    ((struct thread_args*)thread_args)->pArr[decrement_i][increment_j].blue +
                    ((struct thread_args*)thread_args)->pArr[i][decrement_j].blue +
                    ((struct thread_args*)thread_args)->pArr[i][increment_j].blue +
                    ((struct thread_args*)thread_args)->pArr[increment_i][decrement_j].blue +
                    ((struct thread_args*)thread_args)->pArr[increment_i][j].blue +
                    ((struct thread_args*)thread_args)->pArr[increment_i][increment_j].blue
                            ) / 9;

        }
    }
}

/** Apply Swiss Cheese filter to image.
*   Tint the image (at the pixel level) towards being slightly yellow.
*
 * @param  img: the image.
 * @param  thread_height: height of image.
 * @param  thread_width: width of image.
*/
void* image_apply_swiss_cheese_filter(void* thread_args) {
    int height = ((struct thread_args*)thread_args)->thread_height;
    int width = ((struct thread_args*)thread_args)->thread_width;
    /* color shift image to slightly yellow */
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int afterShift = ((struct thread_args*)thread_args)->pArr[i][j].blue - 100;
            if (afterShift < 0) {
                ((struct thread_args*)thread_args)->pArr[i][j].blue = 0;
            } else {
                ((struct thread_args*)thread_args)->pArr[i][j].blue = afterShift;
            }
        }
    }
}

/** equation of a circle: (x - a)^2 + (y - b)^2 = r^2
 * a & b are the center point coordinates
 * All x & y points that satisfy this equation are part of the circle
 * */
/* logic for adding holes as circles of black pixels */
void image_apply_holes(Image* img, int average_radius_holes) {
    /* initialize random number generator */
    time_t t;
    srand((unsigned) time(&t));

    /* three different size of radius */
    int average_radius = average_radius_holes;
    int small_radius = average_radius_holes * 0.5;
    int large_radius = average_radius_holes * 1.5;

    /* 60% of holes will be average size, the rest are small and large */
    int number_of_average_sized_holes = average_radius_holes * 0.5;
    int number_of_small_sized_holes = average_radius_holes * 0.25;
    int number_of_large_sized_holes = average_radius_holes * 0.25;

    /* evenly distribute center point among number of threads */
//    int number_of_holes_per_thread = 0;
//    if (THREAD_COUNT > number_of_average_sized_holes) {
//        number_of_average_sized_holes = 1; /* if too much thread each gets one holes to compute */
//    } else {
//        number_of_holes_per_thread = average_radius_holes / THREAD_COUNT;
//    }

    for (int i = 0; i < number_of_average_sized_holes; i++)
        compute_holes(img, rand() % img->width, rand() % img->height, average_radius);
    for (int j = 0; j < number_of_small_sized_holes; j++)
        compute_holes(img, rand() % img->width, rand() % img->height, small_radius);
    for (int k = 0; k < number_of_large_sized_holes; k++)
        compute_holes(img, rand() % img->width, rand() % img->height, large_radius);

}

void compute_holes (Image* img, int x, int y, int r) {
    /* logic for adding holes as circles of black pixels */
    int center_x = x, center_y = y;   /* center point of circle */
    int radius = r;                     /* circle radius */
    int radius_squared = radius * radius;
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            int calculate_circle = ((j - center_x) * (j - center_x)) + ((i - center_y) * (i - center_y));
            if (calculate_circle <=  radius_squared) { /* <= means include the points that lie inside the circle */
                img->pArr[i][j].red = 0;
                img->pArr[i][j].green = 0;
                img->pArr[i][j].blue = 0;
            }
        }
    }
}

