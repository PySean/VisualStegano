#ifndef SPREAD_SPECTRUM_H
#define SPREAD_SPECTRUM_H

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

#endif /*end SPREAD_SPECTRUM_H*/
