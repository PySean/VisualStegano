#include <limits.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "normal/normal.h"
#include "normal/normsinv.h"

/*    This file provides functions aiding in embedding as well as creating
 * the spread spectrum encoding of the message. It does not handle the final
 * embedding process. It also handles decoding of them message once the stream is
 * obtained.
 *
 * The embedding and decoding process are both little endian meaning the smallest bit
 * is embedded before the largest bit.
 */


#define MIDP 0.5 //Middle Point cutoff
#define SCALE 4 //Scaling for gaussian noise

#define ALT_STREAM 1
#define REG_STREAM 0

/* This function takes a seed, numb message bits, numb cover bytes and embeds the
 * message into them
 *
 * This function does not do any safety checking so make sure numb is correct.
 *
 * The seed is changed during use.
 * The cover is as well
 *
 * Returns number of bits embedded in cover
 */
int embed_message(unsigned char * cover, unsigned char *message, int numb, int *seed);

/* Takes the stegan message and the cover, generates the guassian noise streams
 * and compares the produced values with the difference between the stegan image
 * and the cover image. Guesses the result with the least difference which then
 * goes to the error correcting code
 * Numb is the number of message bits to decode
 * Seed is the same seed given to the embed function, seed is altered.
 *
 * Returns an array of message bit in unsigned char form.
 * Be sure to free the array after use to prevent memory leaks
 */
unsigned char * decode_message(unsigned char * cover, unsigned char * stegan, int numb, int *seed);
//*
int main()
{
   char cover[8] = "abcdefg";
   int seed = 3;
   char message = 'j';
   embed_message((unsigned char *)cover, (unsigned char *)&message, 8, &seed);
   for (seed = 0; seed < 8; seed++)
        printf("%d : %x\n", seed , cover[seed]);
   printf("%s",cover);
   return 0;
}
//*/
int embed_message(unsigned char * cover, unsigned char *message, int numb, int *seed)
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
      cove_char = cover[bits_embed];
      mask = (1 << (bits_embed % CHAR_BIT));

      mess_char &= mask;
      noise_stream = 0.0;
      /*Don't want extremely large or small values from the inverse cdf function
       *but also want to preserve randomness, so scale values to new domain.
       */
      noise_stream = P_LOW + r8_uniform_01 (seed) * (P_HIGH - P_LOW);
      printf("noise_stream uniform %Lf \n", noise_stream);
      if ( mess_char != REG_STREAM)
      {
         noise_stream = noise_stream < MIDP ? noise_stream + MIDP: noise_stream - MIDP;
      }

      noise_stream = normsinv(noise_stream) * SCALE;
      printf("noise_stream final %Lf \n", noise_stream);
      cove_char = cove_char + (int)noise_stream < 0.0 ? 0 :cove_char + (int) noise_stream;
      cover[bits_embed] = cove_char > UCHAR_MAX ? UCHAR_MAX: cove_char;
   }

   return bits_embed;
}

unsigned char * decode_message(unsigned char *cover, unsigned char * stegan, int numb, int *seed)
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
   message = (unsigned char *) malloc(numb/CHAR_BIT + 1);
   for ( bits_decode = 0; bits_decode < numb; bits_decode++)
   {

      mess_char = 0;
      cove_char = cover[bits_decode];
      steg_char = stegan[bits_decode];
      img_diff = steg_char - cove_char;

      noise_stream = 0.0;
      alt_noise_stream = 0.0;
      /*Don't want extremely large or small values from the inverse cdf function
       *but also want to preserve randomness, so scale values to new domain.
       */
      noise_stream = P_LOW + r8_uniform_01 (seed) * (P_HIGH - P_LOW);
      alt_noise_stream = noise_stream < MIDP ? noise_stream + MIDP: noise_stream - MIDP;
      noise_stream = normsinv(noise_stream) * SCALE;
      alt_noise_stream = normsinv(alt_noise_stream) * SCALE;

      //The stream is selected by the difference stream with the smallest absolute value
      mess_char = abs(img_diff - noise_stream) >= abs(img_diff - alt_noise_stream) ? ALT_STREAM : REG_STREAM;
      message[bits_decode] |= ( mess_char << bits_decode % CHAR_BIT);
   }

   return message;
}

