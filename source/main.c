#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parseimg/parseimg.h"
#include "spreadspec/spreadspectrum.h"

/*
 * "main.c", by Group 6
 *
 * Start to piece the final program together please.
 */
void usage(char *programname);


int main(int argc, char * argv[])
{
   FILE *message_file;
   long message_length; /*In BYTES*/
   unsigned char *message_buffer;
   unsigned char *message_ecc;
   unsigned char **recovered_cover;
   char *new_file_name;
   bmp_file bmp;
   bmp_file tempbmp; /*Only used for alpha_trim, will modify function later*/
   int seed;
   int namelen;
   /*If our command line options get more complicated we should
    *do proper argument parsing until such a point though this
    *is needs suiting.
    * */
   if(argc != 5 || '-' != argv[1][0] || ('D' != argv[1][1] && 'E' != argv[1][1]))
      {
      usage(argv[0]);
      exit(EXIT_FAILURE);
      }
   seed = atoi(argv[4]);
   if(argv[1][1] == 'E')
      {
      load_img(&bmp, argv[2]);
      message_file = fopen(argv[3], "r");
      //Go to end of file
      fseek(message_file,0, SEEK_END);
      //count bytes in message file using offset from orgin
      message_length = ftell(message_file);
      fseek(message_file, 0, SEEK_SET);
      message_buffer = (char * ) malloc(message_length);
      fread(message_buffer, message_length, 1 , message_file); //Read in entire message into buffer
      /* Error Correcting Code */
      //message_ecc = (unsigned char *) call_to_ecc_encode(params);
      
      
      
      /*This may return a different message size so we need to make sure it will return a new size as well*/
      /*Message ecc is returned from Error Correcting code*/
      embed_message(bmp.img_data8bit, message_ecc, bmp.dheader.width, bmp.dheader.height, message_len, &seed);
      /* Write Changes to imagename.stego.bmp */
      /**/
      namelen = strlen(argv[2]);
      new_file_name = malloc(namelen + 6);
      new_file_name[0] = '\0';
      strcat(new_file_name, argv[2]);
      if(namelen > 4)
         {
         new_file_name[namelen - 5] = '\0';
         }
      strcat(new_file_name, "stego.bmp");
      write_img(&bmp, new_file_name);
      free(new_file_name);
      free(message_buffer);
      fclose(message_file);
      }
   else
      {
      load_img(&bmp, argv[2]);
      load_img(&tempbmp, argv[2]);
      /*Recovery of original */
      alpha_filter(tempbmp, 3);
      recovered_cover = tempbmp.dheader.img_data8bit;
      /*SpreadSpec Decoding (shouldn't this be decode_message?)*/
      embed_message(bmp.img_data8bit, recovered_cover, bmp.dheader.width, bmp.dheader.height, bmp.dheader.width * bmp.dheader.height, &seed);
      /*Clipping Function For messages that are shorter than image length we may want to truncate them*/
      
      
      /*Error Correcting Code*/
      // message_buffer = (unsigned char *) call_to_ecc_decode(params);
      
      message_file = fopen(argv[3], "+w");
      /*Write message to file*/
      fwrite(message_buffer, 1, message_length, message_file);
      fclose(message_file);
      }
   return 0;
}

void usage(char *programname)
{
   printf("%s encodes and decodes spread spectrum images\n
           to call this program use the following format\n
           %s [-E coverimage message]|[-D stegoimage outputfile] seed\n", programname , programname);
}

