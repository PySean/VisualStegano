#include <stdio.h>
#include <stdlib.h>
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
   bmp_file bmp;
   /*If our command line options get more complicated we should
    *do proper argument parsing until such a point though this
    *is needs suiting.
    * */
   if(argc != 4 || '-' != argv[1][0] || ('D' != argv[1][1] && 'E' != argv[1][1]))
      {
      usage(argv[0]);
      exit(EXIT_FAILURE);
      }
   if(argv[1][1] == 'E')
      {
      load_img(&bmp, argv[2]);
      message_file = fopen(argv[3], "r");
      /* Error Correcting Code */
      /* SpreadSpec Embedding */
      /* Write Changes to imagename.stego.bmp */ 
      }
   else
      {
      load_img(&bmp, argv[2]);
      /*Recovery of original */
      /*SpreadSpec Decoding */
      /* Error Correcting Code*/
      message_file = fopen(argv[3], "+w");
      /*Write message to file*/
      }
   return 0;
}

void usage(char *programname)
{
   printf("%s encodes and decodes spread spectrum images\n
           to call this program use the following format\n
           %s [-E coverimage message]|[-D stegoimage outputfile]\n", programname);
}

