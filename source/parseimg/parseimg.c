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
   //Name of the input file and output file. Will use getopt eventually...
   char * fname = argv[1];   
   char * outname = argv[2];
   bmp_file bro;
   load_img(&bro, fname);
   write_img(bro, outname);
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
   printf("Version is: %u\n", version); 
   //Read "version" number of bytes into bmp data header.
   fread(&bmp->dheader, version, 1, file); 

   //The size of the palette entries is equal to 2^bits_per_pixel.
   //If p_entries is greater than 256, there is no palette.

   unsigned p_entries = 1 << bmp->dheader.bits_per_pixel;

   if (p_entries <= 256)
   {

      //The palette is composed of (s | norm)_pixel * no. of palette entry
      //bytes.
      size_t s_pixels = (sizeof(sp_element) * p_entries);
      size_t norm_pixels = (sizeof(p_element) * p_entries);

      if (version == v2)
      {
         bmp->s_palette = malloc(s_pixels);
         fread(&bmp->s_palette, s_pixels, 1, file);
      }
      else 
      {
         bmp->palette = malloc(norm_pixels);
         fread(bmp->palette, norm_pixels, 1, file);
      }
      
      //Read in the "profile" if the bitmap is v5.
      if (version == v5)
      {
         size_t prof_size = bmp->dheader.profile_size;
         bmp->profile_bytes = malloc(prof_size);
         fread(bmp->profile_bytes, prof_size, 1, file);
      }
      //Now read in the bitmap data.
      int i = 0, j = 0;
      int width = bmp->dheader.width;
      int height = bmp->dheader.height;
      bmp->img_data8bit = malloc(height * sizeof(*bmp->img_data8bit));

      //The problem is I'm not hitting the end of the file for some reason.
      for (i; i < height; i++)
      {
         bmp->img_data8bit[i] = malloc(width); //Multiplying by 1 byte isn't necessary 
         fread(bmp->img_data8bit[i], width, 1, file);
      }
   }
   //A bit of duplicate code here for reading in non paletted bmps
   //May clean up later.
   else
   {
      int i = 0;
      int width = bmp->dheader.width;
      int height = bmp->dheader.height;

      //Read in the "profile" if the bitmap is v5.
      if (version == v5)
      {
         size_t prof_size = bmp->dheader.profile_size;
         bmp->profile_bytes = malloc(prof_size);
         fread(bmp->profile_bytes, prof_size, 1, file);
      }

      bmp->img_data24bit = malloc(height * sizeof(*bmp->img_data24bit));
      for (i; i < height; i++)
      {
         bmp->img_data24bit[i] = malloc(width * sizeof(pixel24)); 
         fread(bmp->img_data24bit[i], width * sizeof(pixel24), 1, file);
      }
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
   FILE * file = fopen(file_name, "w"); //It's clobberin' time
   unsigned size = bmp.dheader.size;
   //Write out the generic file header (as always).
   fwrite(&bmp.fheader, sizeof(bmp.fheader), 1, file);
   
   //Write out data header, palette if the size is right. 
   fwrite(&bmp.dheader, size, 1, file);
   if (bmp.dheader.bits_per_pixel <= 8)
   {
      //No. of palette entries = 2^bits_per_pixel
      //Size of palette = no. of entries * entry element size
      size_t p_entries = 1 << bmp.dheader.bits_per_pixel;
      size_t s_pixels = p_entries * sizeof(sp_element);
      size_t norm_pixels = p_entries * sizeof(p_element);
      
      //Pixels are 3 bytes if bmp is v2, 4 bytes otherwise.
      if (bmp.dheader.size == v2)
         fwrite(bmp.s_palette, s_pixels, 1, file);
      else
         fwrite(bmp.palette, norm_pixels, 1, file);

      //Write out profile info if this is a version 5 bitmap.
      if (size == v5 && bmp.dheader.profile_size != 0)
      {
         fwrite(bmp.profile_bytes, bmp.dheader.profile_size, 1, file);
      }

      //Finally, write out the bitmap data (8bit/pixel and lower).
      int i = 0;
      for (i; i < bmp.dheader.height; i++)
      {
         fwrite(bmp.img_data8bit[i], bmp.dheader.width, 1, file);
      }
   }
   
   //Write out profile info if this is a version 5 bitmap.
   //This is duplicate code because this must be done before the bmp
   //data is written.
   if (size == v5 && bmp.dheader.profile_size != 0)
   {
      fwrite(bmp.profile_bytes, bmp.dheader.profile_size, 1, file);
   }

   if (bmp.dheader.bits_per_pixel > 8) //Just write out bmp data since there's no palette
   {
      int i = 0;
      for (i; i < bmp.dheader.height; i++)
      {
         fwrite(bmp.img_data24bit[i], bmp.dheader.width * sizeof(pixel24), 1, file);
      }
   }
   fclose(file);
   return;
}


//Token error printing function.
//Simply invokes perror on error string msg. Makes for briefer error handling.
void err_sys(char * msg)
{
   perror(msg);
   exit(1);
}
