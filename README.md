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

  usage:
                ./ImageProcessor -f filename [-h] [-r -g -b val] [-w] [-s val] [-o filename]
                   -f  filename:    must have a input file name  to run!
                   -r  value:       use value to increase or decrease the color red
                   -g  value:       use value to increase or decrease the color green
                   -b  value:       use value to increase or decrease the color blue
                   -w:              convert RGB to grayscale equivalent
                   -s  float:       use value to resize the image
                   -o  filename:    optional to customize output filename
                   -h:              print out this help message

