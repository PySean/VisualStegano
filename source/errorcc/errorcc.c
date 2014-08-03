#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "errorcc.h"


/*Look Up Table for bit errors between the current state, received symbol, and next state path -1 represents impossible paths
 *
 *   Example:
 *   State: 00, Received: 00, Next State: 10 = 2 bit errors
 *   This table assumes that the current state is correct but our algorithm won't.
 */
const int ERROR_TABLE[STATES][STATES][STATES] = {{ { 0,-1, 2,-1 }, { 1,-1, 1,-1}, { 1,-1, 1,-1},{ 2,-1, 0,-1} }
                                                 ,{ { 0,-1, 2,-1 }, { 1,-1, 1,-1}, { 1,-1, 1,-1},{ 2,-1, 0,-1} }
                                                 ,{ {-1, 1,-1, 1 }, {-1, 0,-1, 2}, {-1, 2,-1, 0},{-1, 1,-1, 1} }
                                                 ,{ {-1, 1,-1, 1 }, {-1, 0,-1, 2}, {-1, 2,-1, 0},{-1, 1,-1, 1} }};
/*List of Alpha and Beta Parents of the State
 * Each State only has two parents so this is a lookup table for them.
*/

const int PARENT_TABLE[STATES][PARENTS] = {{STATE_0,STATE_1},{STATE_2,STATE_3},{STATE_0,STATE_1},{STATE_2,STATE_3}};



/*
   Returns the next (expected) state of the trellis fsm.
*/
unsigned char next_state_return(unsigned char last_state, unsigned char current_bit);
unsigned char next_output_return(unsigned char last_state, unsigned char current_bit);

/*
   Every byte
   I read from the data will double in size as each bit is paired with a "dummy" bit.
   Message is encoded little endian style.

   Note the buffer returned must be freed by the calling code
*/

/*Masks are for grabbing the values of the bit streams*/
const unsigned char MMASK = 0x01;
const unsigned char RMASK = 0x03;

unsigned char * conv_encode (unsigned char *message, long *message_length){

   unsigned char last_bits, current_bit, outsymbol;//Last Holds two bits , current should only hold one.
   unsigned char *encoded_message;
   int bits_encoded;
   unsigned char mess_mask;//Message is One bit

   //Extra two bytes of zeros to clear state
   encoded_message = (unsigned char * ) calloc((*message_length)*2+2, 1);//Clear Bits to make our lives easier
   last_bits = 0;
   /*For each loop calculate mask for current position, And it with message bit
    *Calculate the encode bit, shift, and add it to the encoded_message.
    */
   for(bits_encoded = 0; bits_encoded < (*message_length) * CHAR_BIT; bits_encoded++)
      {
      mess_mask = (MMASK << (bits_encoded% CHAR_BIT ));
      current_bit = message[bits_encoded/CHAR_BIT] & mess_mask;
      outsymbol = next_output_return(last_bits, current_bit);
      last_bits = next_state_return(last_bits, current_bit);
      encoded_message[(bits_encoded*2)/CHAR_BIT] += outsymbol << ((bits_encoded *2)%CHAR_BIT);
      }
   *message_length = *message_length *2 + 2;
   encoded_message[*message_length - 1]=next_state_return(last_bits,0);//Flushing Values
   return encoded_message;
}
/*
 *  Our convolutional decoder.
 *
 *  The decoded has three steps.
 *  First it computes the path metric for smallest possible paths
 *  that result in each state at each point in the path.
 *
 *  Second it will go backwards from the final state to the beginning
 *  always taking the shortest path metric as any previous path metric
 *  will be less than or equal to it. The path taken is recored in the
 *  last row.
 *
 *  Third it takes the values that were recored and inputs them starting from
 *  the beginning all. code is done 
*/
unsigned char * conv_decode (unsigned char *bit_stream, long stream_length)
{
   int *path_metric[STATES+1]; // Stored Path metrics and current path. Length stream_length.
   int row,column, alpha_parent, beta_parent, min_metric, alpha_metric, beta_metric;
   unsigned char current_bits;
   unsigned char *decoded_stream;

   /*Initialize rows to 0*/
   for(row = 0; row < STATES +1; row ++)
   {
         path_metric[row] = (int *) calloc(stream_length * 4, sizeof(int));
   }
   decoded_stream = calloc(stream_length/2 + stream_length%2, 1);

   /*Invalid Opening States Are Marked*/
   path_metric[1][0] = -1;
   path_metric[3][0] = -1;

   for(column = 0; column < stream_length * (CHAR_BIT/2); column++)
      {
      //Grab the current bit by shifting the mask anding it with the current byte and shifting back the bit.
      current_bits = ((RMASK << (column*2 % CHAR_BIT)) & bit_stream[2*column/CHAR_BIT]) >> (column *2 % CHAR_BIT);
      for(row = 0; row < STATES; row++)
         {
         if(path_metric[row][column] == -1) {}//Skip the invalid cases
         else if(column == 0)
            {
            path_metric[row][column] = ERROR_TABLE[STATE_0][current_bits][row];
            }
         else
            {
            /*Only Picking the two valid parents*/
            alpha_parent = PARENT_TABLE[row][0];
            beta_parent = PARENT_TABLE[row][1];

            alpha_metric =path_metric[alpha_parent][column-1];
            beta_metric = path_metric[beta_parent][column-1];
            if(alpha_metric < 0 && beta_metric >= 0)
               {
               alpha_parent = beta_parent;
               alpha_metric = beta_metric;
               }
            else if(alpha_metric >= 0 && beta_metric < 0)
               {
               beta_metric = alpha_metric;
               beta_parent = alpha_parent;
               }

            alpha_metric+=ERROR_TABLE[alpha_parent][current_bits][row];
            beta_metric+= ERROR_TABLE[beta_parent][current_bits][row];

            min_metric = alpha_metric > beta_metric ? beta_metric: alpha_metric;

            path_metric[row][column] = min_metric;
            }

         }
      }
   row = 0;
   min_metric = path_metric[0][stream_length * 4 - 1];
   path_metric[4][stream_length * 4 - 1] = 0;
   for(column = stream_length * 4 - 2; column >= 0; column--)
      {
      alpha_parent = PARENT_TABLE[row][0];
      beta_parent  = PARENT_TABLE[row][1];

      alpha_metric=path_metric[alpha_parent][column-1];
      beta_metric = path_metric[beta_parent][column-1];
      if(alpha_metric < 0 && beta_metric >= 0)
         {
         alpha_parent = beta_parent;
         alpha_metric = beta_metric;
         }
      else if(alpha_metric >= 0 && beta_metric < 0)
         {
         beta_metric = alpha_metric;
         beta_parent = alpha_parent;
         }
      row = alpha_metric > beta_metric ? beta_parent : alpha_parent;
      path_metric[4][column] = row > STATE_1 ? 1 : 0;
      }

   for (column = 0; column < stream_length * 4 ; column++)
      {
      decoded_stream[column/CHAR_BIT] = (MMASK << (column % CHAR_BIT)) * path_metric[4][column];
      }
   return decoded_stream;
}

/*
   Return Next Expected State

*/
unsigned char next_state_return(unsigned char last_state, unsigned char current_bit){

   unsigned char next_state;

   switch(last_state)
      {
      case STATE_0:
         next_state = current_bit == 0 ? STATE_0: STATE_2;
         break;
      case STATE_1:
         next_state = current_bit == 0 ? STATE_0 : STATE_2;
         break;
      case STATE_2:
         next_state = current_bit == 0 ? STATE_1 : STATE_3;
         break;
      case STATE_3:
         next_state = current_bit == 0 ? STATE_1 : STATE_3;
         break;
      default:
         perror("Invalid State");
      }
   return next_state;
}

unsigned char next_output_return(unsigned char last_state, unsigned char current_bit){

   unsigned char next_state;

   switch(last_state)
      {
      case STATE_0:
         next_state = current_bit == 0 ? STATE_0: STATE_3;
         break;
      case STATE_1:
         next_state = current_bit == 0 ? STATE_3 : STATE_0;
         break;
      case STATE_2:
         next_state = current_bit == 0 ? STATE_2 : STATE_1;
         break;
      case STATE_3:
         next_state = current_bit == 0 ? STATE_1 : STATE_2;
         break;
      default:
         perror("Invalid State");
      }
   return next_state;
}


