/**
 * Implementation of the main.c file, BMPHandler.c and Image.c
 *
 * @author Bichen Pang
 * @version 1.0 May 28 2022
*/

////////////////////////////////////////////////////////////////////////////////
//INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "BMPHandler.h"
#include "Image.h"


////////////////////////////////////////////////////////////////////////////////
// Forward Declaration
void usage(void);
void process_args(int ac, char *av[], char **output_filename, int *grayscale,
                  char **input_file, float *scale,
                  int *red_shift, int *green_shift, int *blue_shift);

////////////////////////////////////////////////////////////////////////////////
// MAIN
// Note: command to compile in gcc 'gcc PangImageProcessor.c Image.c BMPHandler.c -o ImageProcessor'
int main(int argc,char* argv[]) {

    int grayscale = 0;
    float scale = 1;
    char *input_filename = NULL;
    char *output_filename = "default_output.bmp"; // default name

    // default color shift value is 0
    int red_shift = 0, green_shift = 0, blue_shift = 0;

    // call function to parse command line option
    process_args(argc,argv,
                 &output_filename,
                 &grayscale,
                 &input_filename,
                 &scale,
                 &red_shift, &green_shift, & blue_shift);

    // printout user options
    if (input_filename) {
        printf("Input filename is: -f %s\n", input_filename);
    }
    if (grayscale == 1) {
        printf("Convert RGB to grayscale equivalent -w\n");
    }
    if (red_shift) {
        printf("Shifting color red by: -r %d\n", red_shift);
    }
    if (green_shift) {
        printf("Shifting color green by: -g %d\n", green_shift);
    }
    if (blue_shift) {
        printf("Shifting color blue by: -b %d\n", blue_shift);
    }
    if (scale != 1 && scale > 0) {
        printf("Resize the image by factor of -s %f\n", scale);
    }
    printf("\n");

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

    // store pixels info into array pixels
    readPixelsBMP(file_input, pixels, DIB.image_width, DIB.image_height);

    // finished reading image and close file
    fclose(file_input);

/////////////////////////////////////////////////////////////////////////////////////
//-------------------------------Image Manipulation--------------------------------//
/////////////////////////////////////////////////////////////////////////////////////
    Image* img = image_create(pixels, DIB.image_width, DIB.image_height);

    // Grayscale filter will only trigger is user enter -w option
    if (grayscale == 1) {
        image_apply_bw(img);
    }

    // Color shift filter will only trigger if user enter -r or -g or -b with any value
    if (red_shift != 0 || green_shift != 0 || blue_shift != 0) {
        image_apply_colorshift(img, red_shift, green_shift, blue_shift);
    }

    // resize will only trigger when factor is greater than 0 and not default 1
    if (scale != 1 && scale > 0) {
        image_apply_resize(img, scale);
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

    printf("----------------------------------\n");
    printf("   Image processed successfully\n");
    printf("----------------------------------\n\n");

    return 0;
}

// parse command line arguments using getopt.
void process_args(int ac, char *av[], char **output_filename,
                  int *grayscale, char **input_file,
                  float *scale,
                  int *red_shift, int *green_shift, int *blue_shift)
{

    int command, f = 0;
    int length, i;

    while(1){
        // Note: "r:"  means -r option has an arg  "w"  -w does not
        // 'f:'   must have option for input filename
        // 'w'    option for grayscale
        // 'r:', 'g:', 'b:' option for rgb color shift followed by an integer
        // 's:'   option for scale followed by a float
        // 'o:'   option for output file name
        // 'h'    option for help manu
        command = getopt(ac, av, "f: w r: g: b: s: o: h");

        if( command == -1) {
            break; // nothing left to parse in the command line
        }

        switch(command) {  // note: switch stmt can be used in place of some chaining if-else if
            case 'h': usage();
                exit(0); // exit program
                break;
            case 'f': *input_file = optarg; // ask for input file
                f = 1;
                break;
            case 'w': *grayscale = 1;
                break;
            case 'r':
                length = strlen (optarg);
                for (i = 0; i < length; i++)
                    if (!isdigit(optarg[i]))
                    {
                        printf ("\n-----------------------------------------------\n");
                        printf ("         Entered input is not a number\n");
                        printf ("      Please enter -r follow by an integer\n");
                        printf ("-----------------------------------------------\n\n");
                        exit(1);
                    }
                *red_shift = atoi(optarg); // note: atoi converts a string to an int
                break;
            case 'g':
                length = strlen (optarg);
                for (i = 0; i < length; i++)
                    if (!isdigit(optarg[i]))
                    {
                        printf ("\n-----------------------------------------------\n");
                        printf ("         Entered input is not a number\n");
                        printf ("      Please enter -g follow by an integer\n");
                        printf ("-----------------------------------------------\n\n");
                        exit(1);
                    }
                *green_shift = atoi(optarg);
                break;
            case 'b':
                length = strlen (optarg);
                for (i = 0; i < length; i++)
                    if (!isdigit(optarg[i]))
                    {
                        printf ("\n-----------------------------------------------\n");
                        printf ("         Entered input is not a number\n");
                        printf ("      Please enter -b follow by an integer\n");
                        printf ("-----------------------------------------------\n\n");
                        exit(1);
                    }
                *blue_shift = atoi(optarg);
                break;
            case 's':
                *scale = atof(optarg);
                if(*scale <= 0){
                    fprintf(stderr, "\nError: scale value -s %f is less than 0\n"
                                    "Must enter a value greater than 0\n", *scale);
                    exit(1); //error, exit program
                }
                break;
            case 'o': *output_filename = optarg;
                break;

            case ':': fprintf(stderr, "\n Error -%c missing arg\n", optopt);
                usage();
                exit(1);
                break;
            case '?': fprintf(stderr, "\n Error unknown arg -%c\n", optopt);
                usage();
                exit(1);
                break;
            default: printf("optopt: %c\n", optopt);
        }
    }

    if(!f) {
        fprintf(stderr,"!!!Error: must enter input file name.!!!\n");
        usage();
        exit(1);
    }
}

// prints out error message when user tries to run with bad command line args or
// when user runs with the -h command line arg
void usage(void){
    fprintf(stderr,
            " usage:\n"
            "    ./ImageProcessor -f filename [-h] [-r -g -b val] [-w] [-s val] [-o filename]\n"
            "       -f  filename:    !!!must have a input file name  to run!!！\n"
            "       -r  value:       use value to increase or decrease the color red\n"
            "       -g  value:       use value to increase or decrease the color green\n"
            "       -b  value:       use value to increase or decrease the color blue\n"
            "       -w:              convert RGB to grayscale equivalent\n"
            "       -s  float:       use value to resize the image\n"
            "       -o  filename:    optional to customize output filename\n"
            "       -h:              print out this help message\n"
            "\n");
}
