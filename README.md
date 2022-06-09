# BMP-Image-Processor
/**
 * A program that read an BMP image file, applying filter, and save to disk.
 * Filter implemented in this version:
 * 1. Grayscale Filter
 * 2. Color Shift Filter
 * 3. Scaling Filter
 *
 * @author Sheldon Pang
 * @version 1.1
 *
 * Update info:
 * In version 1.1: fixed image offset issue.
 * Now will skip bytes based on the image offset value, and update the output file's offset value accordingly.
*/

Note: command to compile in gcc 'gcc PangImageProcessor.c Image.c BMPHandler.c -o ImageProcessor'

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

