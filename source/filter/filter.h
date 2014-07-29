/*
 * "filter.h", by Sean Soderman
 *
 * The definition for the alpha-trimmed mean filtering function
 * are declared here. I decided since the trim_mean subroutine
 * is only relevant to this portion of the program to keep it
 * exclusively declared within filter.c.
 */


/*
 * "Filters" the bitmap data portion of the bmp structure.
 *
 * This is done by moving a 3 x 3 "window" over the bitmap data elements.
 * This window is used to calculate the mean of all but the highest and lowest
 * values. 
 *
 * Then, the value in the middle of this window is assigned
 * the calculated mean.
 */
void alpha_filter(bmp_file bmp, size_t w_size);
