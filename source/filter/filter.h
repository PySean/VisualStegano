/*
 * "filter.h", by Sean Soderman
 *
 * The definitions for all alpha-trimmed mean filtering functions
 * are declared here. Admittedly, there aren't many.
 *
 */



/*
 * Expands a height x width array into an area x 1 array.
 * It then sorts this array and calculates its mean beginning at the element
 * indexed by trim, and ending at area - trim.
 * TODO: Figure whether it would be better to have this function here or *there*.
 */
//unsigned calc_mean(char ** window, unsigned height, unsigned width, unsigned short trim);


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
