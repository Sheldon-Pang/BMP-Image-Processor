/**
* Specification for an image ADT.
*
* @author Sheldon Pang
* @version 1.0
*/

////////////////////////////////////////////////////////////////////////////////
// Include Files
#include <stdio.h>
#include <stdlib.h>
#include "Image.h"

////////////////////////////////////////////////////////////////////////////////
//Function Declarations

/* Creates a new image and returns it.
*
 * @param  pArr: Pixel array of this image.
 * @param  width: Width of this image.
 * @param  height: Height of this image.
 * @return A pointer to a new image.
*/
Image* image_create(struct Pixel** pArr, int width, int height) {
    Image *image = (Image*) malloc(sizeof (Image));

    image->width = width;
    image->height = height;
    image->pArr = pArr;

    return image;
}


/* Destroys an image. Does not deallocate internal pixel array.
*
 * @param  img: the image to destroy.
*/
void image_destroy(Image** img) {
    free(*img);
    *img = NULL;
}

/* Returns a double pointer to the pixel array.
*
 * @param  img: the image.
*/
struct Pixel** image_get_pixels(Image* img) {
    return img->pArr;
}

/* Returns the width of the image.
*
 * @param  img: the image.
*/
int image_get_width(Image* img) {
    return img->width;
}

/* Returns the height of the image.
*
 * @param  img: the image.
*/
int image_get_height(Image* img) {
    return img->height;
}

/* Converts the image to grayscale.
*
 * @param  img: the image.
*/
void image_apply_bw(Image* img) {
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            // calculate grayscale
            int grayscale =
                    0.114 * (img->pArr[i][j].blue) +
                    0.587 * (img->pArr[i][j].green) +
                    0.299 * (img->pArr[i][j].red);
            // convert to grayscale
            img->pArr[i][j].blue = grayscale;
            img->pArr[i][j].green = grayscale;
            img->pArr[i][j].red = grayscale;
        }
    }
}

/**
 * Shift color of the internal Pixel array. The dimension of the array is width * height.
 * The shift value of RGB is rShift, gShift，bShift. Useful for color shift.
 *
 * @param  img: the image.
 * @param  rShift: the shift value of color r shift
 * @param  gShift: the shift value of color g shift
 * @param  bShift: the shift value of color b shift
 */
void image_apply_colorshift(Image* img, int rShift, int gShift, int bShift) {
    // apply Grayscale filter
    image_apply_bw(img);

    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            // blue color shift
            int afterShift = img->pArr[i][j].blue + bShift;

            if (afterShift < 0) {
                img->pArr[i][j].blue = 0;
            } else if (afterShift > 255) {
                img->pArr[i][j].blue = 255;
            } else {
                img->pArr[i][j].blue = afterShift;
            }

            // green color shift
            afterShift = img->pArr[i][j].green + gShift;

            if (afterShift < 0) {
                img->pArr[i][j].green = 0;
            } else if (afterShift > 255) {
                img->pArr[i][j].green = 255;
            } else {
                img->pArr[i][j].green = afterShift;
            }

            // red color shift
            afterShift = img->pArr[i][j].red + rShift;

            if (afterShift < 0) {
                img->pArr[i][j].red = 0;
            } else if (afterShift > 255) {
                img->pArr[i][j].red = 255;
            } else {
                img->pArr[i][j].red = afterShift;
            }
        }
    }



}

/* Converts the image to grayscale. If the scaling factor is less than 1 the new image will be
 * smaller, if it is larger than 1, the new image will be larger.
 *
 * @param  img: the image.
 * @param  factor: the scaling factor
*/
void image_apply_resize(Image* img, float factor) {
    int newWidth = img->width * factor;
    int newHeight = img->height * factor;

    // verify the factor is greater than 0
    if (factor <= 0) {
        printf("Please try enter a factor greater than 0.\n");
        return;
    }

    // when factor is smaller than 1, overwriting original array
    if (factor <= 1 && factor > 0) {
        for (int i = 0; i < newHeight; i++) {
            for (int j = 0; j < newWidth; j++) {
                int heightFactor = i / factor;
                int widthFactor = j / factor;
                img->pArr[i][j].blue =  img->pArr[heightFactor][widthFactor].blue;
                img->pArr[i][j].green = img->pArr[heightFactor][widthFactor].green;
                img->pArr[i][j].red = img->pArr[heightFactor][widthFactor].red;
            }
        }
    }

    // if factor is greater than 1
    if (factor > 1) {
        // allocate more
        struct Pixel** newPixels = (struct Pixel**)malloc(sizeof(struct Pixel*) * newHeight);
        for (int p = 0; p < newHeight; p++) {
            newPixels[p] = (struct Pixel*)malloc(sizeof(struct Pixel) * newWidth);
        }
        // store pixel info into new array
        for (int i = 0; i < newHeight; i++) {
            for (int j = 0; j < newWidth; j++) {
                int heightFactor = i / factor;
                int widthFactor = j / factor;
                newPixels[i][j].blue =  img->pArr[heightFactor][widthFactor].blue;
                newPixels[i][j].green = img->pArr[heightFactor][widthFactor].green;
                newPixels[i][j].red = img->pArr[heightFactor][widthFactor].red;
            }
        }
        // update array pointer
        img->pArr = newPixels;
    }

    // update width and height
    img->width = newWidth;
    img->height = newHeight;
}