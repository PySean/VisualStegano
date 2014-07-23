#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "parseimg.h"
/*
 *
 *"parseimg.c", by Sean Soderman
 *
 * Functions that can determine the type of BMP we're working with, as well
 * as load up bitmap data into a struct containing the header info, palette info,
 * as well as the raw bitmap data (so basically the whole bitmap).
 *
 * A specialized function for writing the bitmap to a file is also
 * specified. This is because there are many variations on bitmaps
 * and I made the all-encompassing "holder" struct to handle any 
 * type of bitmap. So, simply writing with sizeof(bmp_file) won't work.
 */

//NOTE: Main is here only for testing. Will be integrated into rest of codebase
//later.

//Token error printing function definition.
void err_sys(char * msg);

int main(int argc, char * argv[])
{
   //Name of the input file.
   char * fname = argv[1];   
   bmp_file bro;
   load_img(&bro, fname);
   return 0;
}

/*
 * Loads all image data into the bmp struct.
 *
 * bmp: Address at which the file data will be loaded.
 * file_name: Name of the file to be loaded.
 *
 */
void load_img(bmp_file * bmp, char * file_name)
{
   //Preliminary operations. Just open the file up and check if it exists.
   FILE * file = NULL;
   if ( (file = fopen(file_name, "r") ) == NULL)
      err_sys("File open error");
   
   //Initialize fheader size for more readable fread, version to 0.
   size_t fheader_size = sizeof(bmp->fheader);
   unsigned version = 0;
   fread(&bmp->fheader, fheader_size, 1, file);
   //Figure out what version BMP this is (in bytes). 
   //Then seek back to the position after the file header.

   int orig_pos = ftell(file);
   fread(&version, sizeof(version), 1, file);
   fseek(file, orig_pos, SEEK_SET);
   //Read "version" number of bytes into bmp data header.
   fread(&bmp->dheader, version, 1, file); 

   //The size of the palette entries is equal to 2^bits_per_pixel.
   //If p_entries is greater than 256, there is no bitmap.
   unsigned p_entries = 1 << bmp->dheader.bits_per_pixel;

   if (p_entries <= 256)
   {

      //The palette is composed of (s | norm)_pixel * no. of palette entry
      //bytes.
      size_t s_pixels = (sizeof(sp_element) * p_entries);
      size_t norm_pixels = (sizeof(p_element) * p_entries);
      printf("norm_pixels: %u", norm_pixels);
      if (version == v2)
      {
         bmp->s_palette = malloc(s_pixels);
         fread(&bmp->s_palette, s_pixels, 1, file);
      }
    /*  else //TODO: This is where the problem happens.
      {
         bmp->palette = malloc(norm_pixels);
         fread(&bmp->palette, norm_pixels, 1, file);
      }

      //Now read in the bitmap data. 
      int i = 0, j = 0;
      long width = bmp->dheader.width;
      long height = bmp->dheader.height;
      printf("width: %i, height: %i", width, height);
      bmp->img_data8bit = malloc(height * sizeof(*bmp->img_data8bit));
      for (i; i < height; i++)
      {
         bmp->img_data8bit[i] = malloc(width); //Multiplying by 1 byte isn't necessary 
         fread(bmp->img_data8bit, width, 1, file);
      }*/
   }

   return;
}

/* Write all data within bmp struct to specified filename.
 * This will be pertinent to stegoimage generation as well as cover image
 * recovery.
 *
 * bmp: The bmp file to be output
 * file_name: The name of this new file.
 */
void write_img(bmp_file bmp, char * file_name)
{
   return;
}


//Token error printing function.
//Simply invokes perror on a error message string. Makes for better error handling.
void err_sys(char * msg)
{
   perror(msg);
   exit(1);
}
