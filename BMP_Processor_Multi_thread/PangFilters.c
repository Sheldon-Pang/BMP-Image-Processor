/**
* (multi thread BMP processing with box blur and cheese filter)
*
* @author Sheldon Pang
* @version 1.0
 *
 * Note: command to compile in gcc
 * 'gcc -c Image.c BMPHandler.c -lm -pthread'
 * 'gcc PangFilters.c Image.o BMPHandler.o -o PangFilters'
 * './PangFilters -f filename.bmp -o blurry_cheese_out.bmp -b -c'
 *
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include "BMPHandler.h"
#include "Image.h"

#define THREAD_COUNT 4 /* image width must divisible by thread count */
/* Also, the thread count can not exceed the width of the image */

/** The average radius and number of holes
 * should be 8% of the smallest side of the input image  */
int compute_average_radius_holes(int width, int height);

void usage(void);
void process_args(int ac, char *av[], char **output_filename,
                  int *blur_filter, int *swiss_cheese_filter, char **input_file);

int main(int argc, char* argv[]) {

    int blur_filter_trigger = 0;
    int cheese_filter_trigger = 0;
    char *input_filename = NULL;
    char *output_filename = "default_output.bmp"; // default output filename if not specified by user

    // call function to parse command line option
    process_args(argc,argv,
                 &output_filename,
                 &blur_filter_trigger,
                 &cheese_filter_trigger,
                 &input_filename);

    // printout user options
    if (input_filename) {
        printf("\nInput filename is: %s\n", input_filename);
    }
    if (blur_filter_trigger == 1) {
        printf("----------Apply box blue filter----------\n");
    }
    if (cheese_filter_trigger == 1) {
        printf("----------Apply swiss cheese filter----------\n");
    }

/////////////////////////////////////////////////////////////////////////////////////
//---------------------------------Reading Image-----------------------------------//
/////////////////////////////////////////////////////////////////////////////////////

    struct BMP_Header BMP;
    struct DIB_Header DIB;

    FILE* file_input;

    // check if file exists
    if ((file_input = fopen(input_filename, "rb"))) {
        fclose(file_input);
    } else {
        printf("----------------------------------------------------------------\n");
        printf("   File %s does not exist or not within the current folder.\n", input_filename);
        printf("----------------------------------------------------------------\n\n");
        exit(1);
    }

    file_input = fopen(input_filename, "rb");


    readBMPHeader(file_input, &BMP);
    readDIBHeader(file_input, &DIB);

    // allocate memory for multi array
    struct Pixel** pixels = (struct Pixel**)malloc(sizeof(struct Pixel*) * DIB.image_height);
    for (int p = 0; p < DIB.image_height; p++) {
        pixels[p] = (struct Pixel*)malloc(sizeof(struct Pixel) * DIB.image_width);
    }

    printf("The image height is: %d width is: %d\n", DIB.image_height, DIB.image_width);

    if (THREAD_COUNT > DIB.image_width) {
        printf("Thread count too large, please pick a smaller value.");
        exit(1);
    } else if (DIB.image_width % THREAD_COUNT != 0) {
        printf("image width not divisible by thread count %d, please fix.", THREAD_COUNT);
        exit(1);
    }

    // store pixels info into array pixels
    readPixelsBMP(file_input, pixels, DIB.image_width, DIB.image_height);

    // finished reading image and close file
    fclose(file_input);

    /* create image */
    Image* img = image_create(pixels, DIB.image_width, DIB.image_height);

    /* allocate memory for each threads */
    struct Pixel** pixels_thread[THREAD_COUNT];
    for (int each_thread_id = 0; each_thread_id < THREAD_COUNT; each_thread_id++) {

        /* divide image vertically base on number of the thread count */
        int divided_width = (img->width / THREAD_COUNT);

        /* each thread will have their own piece of data to process based on their unique id */
        int thread_width_start = divided_width * each_thread_id;

        /* allocate memory for each threads of a minimal size based on the divided_width */
        pixels_thread[each_thread_id] = (struct Pixel**)malloc(sizeof(struct Pixel*) * DIB.image_height);
        for (int p = 0; p < DIB.image_height; p++) {
            pixels_thread[each_thread_id][p] = (struct Pixel*)malloc(sizeof(struct Pixel) * divided_width);
        }

        /* copy image data to individual pixels_thread[each_thread_id] */
        for (int height = 0; height < img->height; height++) {
            /* every outer loop will reset this value */
            int pixels_start = divided_width * each_thread_id;
            for (int width = 0; width < divided_width; width++) {
                pixels_thread[each_thread_id][height][width].red = pixels[height][pixels_start].red;
                pixels_thread[each_thread_id][height][width].green = pixels[height][pixels_start].green;
                pixels_thread[each_thread_id][height][width].blue = pixels[height][pixels_start].blue;
                pixels_start++;
            }
        }
    }

/////////////////////////////////////////////////////////////////////////////////////
//-------------------------------Image Manipulation--------------------------------//
/////////////////////////////////////////////////////////////////////////////////////
    /* Pthread */
    pthread_t th_blur[THREAD_COUNT], th_swiss[THREAD_COUNT];

    /* swiss cheese filter */
    int average_radius_holes = compute_average_radius_holes(DIB.image_width, DIB.image_height);

    if (cheese_filter_trigger == 1) {
        printf("Average radius: %d, number of holes: %d\n", average_radius_holes, average_radius_holes);

        struct thread_args* swiss_cheese_thread_args[THREAD_COUNT];

        for (int i = 0; i < THREAD_COUNT; i++) {
            swiss_cheese_thread_args[i] = (struct thread_args*)malloc(sizeof(struct thread_args));
            swiss_cheese_thread_args[i]->pArr = pixels_thread[i];
            swiss_cheese_thread_args[i]->thread_height = img->height;
            swiss_cheese_thread_args[i]->thread_width = img->width / THREAD_COUNT;
            /* create thread */
            if (pthread_create(&th_swiss[i], NULL, &image_apply_swiss_cheese_filter, (void*)swiss_cheese_thread_args[i]) != 0) { /* The if statement will make sure the thread is created successfully */
                perror("Failed to create thread");
                return 1;
            }
        }
        for (int i = 0; i < THREAD_COUNT; i++) {
            /* wait thread to finish */
            if (pthread_join(th_swiss[i], NULL) != 0) {
                perror("Failed to join thread");
                return 1;
            }
        }
    }


    /* box blur filter */
    if (blur_filter_trigger == 1) {
        struct thread_args* box_blur_thread_args[THREAD_COUNT];

        for (int i = 0; i < THREAD_COUNT; i++) {
            box_blur_thread_args[i] = (struct thread_args*)malloc(sizeof(struct thread_args));
            box_blur_thread_args[i]->pArr = pixels_thread[i];
            box_blur_thread_args[i]->thread_height = img->height;
            box_blur_thread_args[i]->thread_width = (img->width / THREAD_COUNT); /* extra width is for box blur algorithm */
            /* create thread */
            if (pthread_create(&th_blur[i], NULL, &image_apply_blur_filter, (void*)box_blur_thread_args[i]) != 0) { /* The if statement will make sure the thread is created successfully */
                perror("Failed to create thread");
                return 1;
            }
        }
        for (int i = 0; i < THREAD_COUNT; i++) {
            /* wait thread to finish */
            if (pthread_join(th_blur[i], NULL) != 0) {
                perror("Failed to join thread");
                return 1;
            }
        }
    }

    /* Combine each thread back to img */
    for (int each_thread_id = 0; each_thread_id < THREAD_COUNT; each_thread_id++) {

        /* divide image vertically base on number of the thread count */
        int divided_width = img->width / THREAD_COUNT;

        /* copy image data to individual pixels_thread[each_thread_id] */
        for (int height = 0; height < img->height; height++) {
            /* every outer loop will reset this value */
            int pixels_start = divided_width * each_thread_id;
            for (int width = 0; width < divided_width; width++) {
                pixels[height][pixels_start].red = pixels_thread[each_thread_id][height][width].red;
                pixels[height][pixels_start].green = pixels_thread[each_thread_id][height][width].green;
                pixels[height][pixels_start].blue = pixels_thread[each_thread_id][height][width].blue;
                pixels_start++;
            }
        }
    }

    if (cheese_filter_trigger == 1) {
        image_apply_holes(img, average_radius_holes);
    }

    FILE* file_output = fopen(output_filename, "wb");

    // update header and dib info
    makeBMPHeader(&BMP, img->width, img->height);
    makeDIBHeader(&DIB, img->width, img->height);

    // write update header and dib info
    writeBMPHeader(file_output, &BMP);
    writeDIBHeader(file_output, &DIB);

    // write pixels info into new files
    writePixelsBMP(file_output, image_get_pixels(img), image_get_width(img), image_get_height(img));

    // finished writing and close file
    fclose(file_output);

    // free memory
    image_destroy(&img);
    free(pixels);
    pixels = NULL;

    printf("----------------------------------\n");
    printf("   Image processed successfully\n");
    printf("----------------------------------\n\n");

    return 0;
}

/** The average radius and number of holes
 * should be 8% of the smallest side of the input image  */
int compute_average_radius_holes(int width, int height) {
    /* get the smallest side */
    int smallest_side = width;
    if (width < height)
        smallest_side = height;

    /* compute average radius */
    int average_radius = smallest_side * 0.08;

    return average_radius;
};

// prints out error message when user tries to run with bad command line args or
// when user runs with the -h command line arg
void usage(void){
    fprintf(stderr,
            " usage:\n"
            "    ./PangFilters -i filename [-b] [-c] [-o filename]\n"
            "       -f  filename:    must have a input file name  to run\n"
            "       -b               apply box blur filter\n"
            "       -c               apply swiss cheese filter\n"
            "       -o  filename:    optional to customize output filename\n"
            "       -h:              print out this help message\n"
            "\n");
}

// parse command line arguments using getopt.
void process_args(int ac, char *av[], char **output_filename,
                  int *blur_filter, int *swiss_cheese_filter, char **input_file)
{

    int command, f = 0;

    while(1){
        command = getopt(ac, av, "f: b c o: h");

        if( command == -1) {
            break; // nothing left to parse in the command line
        }
        switch(command) {  // note: switch stmt can be used in place of some chaining if-else if
            case 'h': usage();
                exit(0); // exit program
            case 'f': *input_file = optarg; // ask for input file
                f = 1;
                break;
            case 'b': *blur_filter = 1;
                break;
            case 'c': *swiss_cheese_filter = 1;
                break;
            case 'o': *output_filename = optarg;
                break;
            case ':': fprintf(stderr, "\n Error -%c missing arg\n", optopt);
                usage();
                exit(1);
            case '?': fprintf(stderr, "\n Error unknown arg -%c\n", optopt);
                usage();
                exit(1);
            default: printf("optopt: %c\n", optopt);
        }
    }
    if(!f) {
        fprintf(stderr,"!!!Error: must enter input file name.!!!\n");
        usage();
        exit(1);
    }
}