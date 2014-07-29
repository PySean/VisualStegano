#include <limits.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "normal/normal.h"
#include "normal/normsinv.h"
#include "spread_spectrum.h"

#define COVSIZ 40

int compare_ints(int orgin, int relativeto);
/*
int main()
{
   unsigned char cover[96]  = "abcd efgh ijkl mnop qrst uvwx yzzy xwvu tsrq ponabcd efgh ijkl mnop qrst uvwx yzzy xwvu tsrq pon";
   unsigned char ocover[96] = "abcd efgh ijkl mnop qrst uvwx yzzy xwvu tsrq ponabcd efgh ijkl mnop qrst uvwx yzzy xwvu tsrq pon";
   unsigned char * decrypted;
   int mlen;
   int seed_init = 3;
   int seed, index;
   seed = seed_init;
   unsigned char message[12] = "hello world";
   mlen = CHAR_BIT * 11;
   embed_message((unsigned char *)cover, (unsigned char *)&message, mlen, &seed);
   for (index = 0; index < 8; index++)
        printf("%d : %x\n", index , cover[index]);
   printf("%s\n",cover);
   printf("P_LOW %Lf\n", normsinv(P_HIGH));
   printf("P_HIGH %Lf\n", normsinv(P_LOW));
   seed = seed_init;
   decrypted = decode_message(ocover, cover, mlen, &seed);
   printf("message: %s\n decrypted message: %s\n",message, decrypted);
   free(decrypted);
   return 0;
}
*/
int embed_message(unsigned char ** cover, unsigned char *message, int width, int height, int numb, int *seed)
{
   int bits_embed;
   long double noise_stream;
   int cove_char, mess_char,mask;


   /* Using 0 for the regular noise stream value and 1 for the
    * alternative stream value.
    *
    * the stream values are then used in the inverse normal cdf
    * function
    * */
   for ( bits_embed = 0; bits_embed < numb; bits_embed++)
   {

      mess_char = message[bits_embed/CHAR_BIT];
      cove_char = cover[bits_embed/width][bits_embed % width];
      mask = (1 << (bits_embed % CHAR_BIT));

      mess_char &= mask;
      noise_stream = 0.0;

      noise_stream = r8_uniform_01(seed);
      if ( mess_char != REG_STREAM)
      {
         noise_stream = noise_stream < MIDP ? noise_stream + MIDP: noise_stream - MIDP;
      }
      /*Don't want extremely large or small values from the inverse cdf function
       *but also want to preserve randomness, so scale values to new domain.
       */
      noise_stream = P_LOW + noise_stream * (P_HIGH - P_LOW);
      noise_stream = normsinv(noise_stream) * SCALE;
      printf("noise_stream final %Lf \n", noise_stream);
      cove_char = cove_char + (int)noise_stream < 0 ? 0 :cove_char + (int) noise_stream;
      cover[bits_embed/width][bits_embed % width] = cove_char > UCHAR_MAX ? UCHAR_MAX: cove_char;
   }

   return bits_embed;
}

unsigned char * decode_message(unsigned char **cover, unsigned char **stegan, int width, int height, int numb, int *seed)
{
   int bits_decode;
   long double noise_stream, alt_noise_stream;
   int cove_char, mess_char, steg_char, img_diff;
   unsigned char *message;

   /* Using 0 for the regular noise stream value and 1 for the
    * alternative stream value.
    *
    * This function compares the noise stream
    * */
   message = (unsigned char *) calloc(numb/CHAR_BIT+ 1, 1);
   message[numb/CHAR_BIT] = '\0';
   for ( bits_decode = 0; bits_decode < numb; bits_decode++)
   {

      mess_char = 0;
      cove_char = cover[bits_decode/width][bits_decode % width];
      steg_char = stegan[bits_decode/width][bits_decode % width];
      img_diff = steg_char - cove_char;

      noise_stream = 0.0;
      alt_noise_stream = 0.0;

      noise_stream = r8_uniform_01(seed);
      alt_noise_stream = noise_stream < MIDP ? noise_stream + MIDP: noise_stream - MIDP;
      /*Don't want extremely large or small values from the inverse cdf function
       *but also want to preserve randomness, so scale values to new domain.
       */
      noise_stream = P_LOW + noise_stream * (P_HIGH - P_LOW);
      alt_noise_stream = P_LOW + alt_noise_stream * (P_HIGH - P_LOW);
      noise_stream = normsinv(noise_stream) * SCALE;
      alt_noise_stream = normsinv(alt_noise_stream) * SCALE;

      //The stream is selected by the difference stream with the smallest absolute value
      /* normsinv(P_HIGH) = -1.972961 and normsinv(P_LOW) = -1.972961
      *
      *  At boundary values check for which of the values is positive and which is negative then
      *  assign the stream that makes more sense. Because of how the function is decided there will
      *  one positive or zero value and one negative value.
      */
      if(cove_char == 0)
      {
         mess_char = compare_ints(noise_stream, alt_noise_stream) < 0 ? REG_STREAM : ALT_STREAM;
      }
      else if(cove_char == UCHAR_MAX)
      {
         mess_char = compare_ints(noise_stream,alt_noise_stream) > 0 ? REG_STREAM : ALT_STREAM;
      }
      else{
         mess_char = abs(img_diff - noise_stream) >= abs(img_diff - alt_noise_stream) ? ALT_STREAM : REG_STREAM;
      }
      printf("%d\n",mess_char);
      message[bits_decode/CHAR_BIT] += (unsigned char)( mess_char << bits_decode % CHAR_BIT);
   }
   return message;
}

int compare_ints(int orgin, int relativeto)
{
   return (orgin > relativeto) - (orgin > relativeto);
}

