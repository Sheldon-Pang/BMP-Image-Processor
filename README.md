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
