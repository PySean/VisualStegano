#include <stdio.h>
#include <stdlib.h>
#include "../parseimg/parseimg.h"
#include "filter.h"

//Marvel et al's implementation uses a trim of 1. This is also what we will use.
#define TRIM 1

/*
 * "filter.c", by Sean Soderman
 * Implentation of the alpha mean trimmed filter, using the same parameters
 * used by the great Lisa M. Marvel and her colleagues.
 *
 * For now, it does not take into consideration 24 bit rgb bitmaps.
 * Unless this affects more than my portion in terms of workload though,
 * I would like to give it a shot.
 */

//Returns the alpha-trimmed mean of the array.
unsigned trim_mean(size_t type, unsigned char * array, unsigned trim, 
                  size_t size);



//Simply return the sum of the RGB pixel values within a pixel24 struct.
unsigned pixel_sum(pixel24 p);

/*
 * Imposes an alpha trimmed mean filtering algorithm on the bitmap info.
 * bmp: The struct containing relevant bitmap information.
 * w_size: Specifies both the height and width dimensions of the window.
 * Note: This struct should have its data initialized with the load_img() function
 * in filter.h before calling this function on it. 
 * Other note: w_size should be an odd number.
 */

void alpha_filter(bmp_file bmp, size_t w_size)
{
   //i and j iterate through the entirety of the image, k and l are the window
   //iterators.
   int i = 0, j = 0, k = 0, l = 0;
   
   //Calculate & store the area of the window.
   size_t area = w_size * w_size;

   //This value is necessary for iterating over all surrounding pixels.
   //Also good to avoid excessive division computations.
   size_t outer_ndx = w_size / 2;
   int height = bmp.dheader.height;
   int width = bmp.dheader.width;
   
   //Check if the area isn't odd
   if (!(w_size & 1))
   {
      fprintf(stderr, "Please enter an odd dimension for the window");
      exit(1);
   }
   //Declare array to expand window into, and its indexer.
   unsigned char exp_w[area];
   int exp_ndx = 0;
   //Iterate through bitmap data, maintain window iterators.
   for (i; i < height; i++)
   {
      for (j = 0; j < width; j++)
      {
         for (k = 0; k < w_size; k++)
         {
            for (l = 0; l < w_size; l++)
            {
               int ndx_y = i - outer_ndx + k;
               int ndx_x = j - outer_ndx + l;
               //Prevent indexing from out of bound areas.
               if (ndx_y >= 0 && ndx_x >= 0 && ndx_y < height && ndx_x < width)
               {
                  exp_w[exp_ndx++] = bmp.img_data8bit[ndx_y][ndx_x]; 
               }
            }
         }
         //Now assign the mean of the pixel values to the center pixel.
         unsigned bpp = bmp.dheader.bits_per_pixel;
         bmp.img_data8bit[i][j] = trim_mean(bpp, exp_w, TRIM, exp_ndx);
         exp_ndx = 0;
      }
   }
}

// Returns the sum of each pixel within p.
unsigned pixel_sum(pixel24 p)
{
   return p.blue + p.green + p.red;
}

/*
 * Returns the mean of the array after sorting it from least to greatest.
 *
 * type: 8 denotes 8 bits/pixel or lower, 24 is 24 bits/pixel.
 * This should be passed bmp.dheader.bits_per_pixel.
 *
 * array: The passed in array that will get its mean calculated.
 * This array should be of type char[] if type is 8, and
 * pixel24 if it is 24.
 *
 * trim: The number of elements to omit at the end and beginning
 * of the array. In this implementation, this will just be 1.
 */

unsigned trim_mean(size_t type, unsigned char * array, unsigned trim, size_t size)
{
   unsigned sum = 0, mean = 0;
   int i = 0, j = 0;
   //A selection sort is imposed on the array.
   for (i; i < size; i++)
   {
      for (j = i; j < size; j++)
      {
         if (array[j] < array[i])
         {
            unsigned char temp = array[i];
            array[i] = array[j];
            array[j] = temp;
         }
      }
   }

   //Exclude "trim" amount of elements from beginning and end of array.
   i = trim;
   for (i; i < size - trim; i++)
   {
      //This comparison anticipates the acceptance of 24-bit bitmaps.
      if (type == 8)
      {
         sum += array[i];
      }
   }
   //i is 1 + the number of elements post-trim.
   mean = sum / (i - 1);
   return mean;
}
