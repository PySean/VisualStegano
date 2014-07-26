#include <stdio.h>
#include <stdlib.h>
#include "../parseimg/parseimg.h"
#include "filter.h"

/*
 * "filter.c", by Sean Soderman
 * Implentation of the alpha mean trimmed filter, using the same parameters
 * used by the great Lisa M. Marvel and her colleagues.
 *
 * For now, it does not take into consideration 24 bit rgb bitmaps.
 * Unless this affects more than my portion in terms of workload though,
 * I would like to give it a shot.
 */


unsigned trim_mean(size_t type, unsigned char * array, unsigned trim, unsigned area);



//Simply return the sum of the RGB pixel values within a pixel24 struct.
unsigned pixel_sum(pixel24 p);

/*
 * Imposes an alpha trimmed mean filtering algorithm on the bitmap info.
 * bmp: The struct containing relevant bitmap information.
 * w_size: Specifies both the height and width dimensions of the window.
 * Note: This struct should have its data initialized with the load_img() function
 * in filter.h before calling this function on it.
 *
 */

void alpha_filter(bmp_file bmp, size_t w_size)
{
   int i = 0, j = 0, k = 0, l = 0;
   //Check if the area isn't odd.
   if (!(w_size & 1))
   {
      fprintf(stderr, "Please enter an odd dimension for the window");
      exit(1);
   }
   //Expand window into w_size x 1 array.
   //I am *not* sure if I have to iterate over every single pixel and assign
   //every pixel the mean of its surrounding values. If I have to, then the
   //missing window values will have to be symmetrically reflected onto these elements.
   if (bmp.dheader.bits_per_pixel <= 8)
   {
      //Initialize array that will hold every element from the window.
      char exp[w_size * w_size];

      //Don't go beyond the ends of the array with the window. Calculated with
      //the center pixel in mind. NOTE: If this scheme is incorrect,
      //I will devise a mirroring algorithm for all partially filled window
      //cases.
      int hlimit = bmp.dheader.height - w_size;
      int wlimit = bmp.dheader.width - (w_size / 2);
      
      //Iterate through all bitmap elements. Assign means to center pixels.
      
      for (i; i < hlimit; i++)
      {
         //The offset will be incremented w_size times by the innermost loop.
         //This is key to expanding the matrix's values into the 1d array.
         for (j = 0; j < wlimit; j++)
         {
            short offset = 0;
            //Assign appropriate window values to 1d array.
            for (k = 0; k < w_size; k++)
            {
               for (l = 0; l < w_size; l++)
               {
                  exp[l + offset] = bmp.img_data8bit[k + i][j + l];
               }
               offset += w_size;
            }
            //Assign mean to center pixel once array is filled.
            bmp.img_data8bit[i + 1][j + 1] = trim_mean(bmp.dheader.bits_per_pixel, 
                                                      exp, 1, w_size * w_size);
         }
      }
   }
   return;
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

unsigned trim_mean(size_t type, unsigned char * array, unsigned trim, unsigned area)
{
   unsigned sum = 0, mean = 0;
   int i = 0, j = 0;

   //A selection sort is imposed on the array.
   for (i; i < area; i++)
   {
      for (j = i; j < area; j++)
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
   for (i; i < area - trim; i++)
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
