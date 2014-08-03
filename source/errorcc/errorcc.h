#ifndef ERRORCC_H
#define ERRORCC_H
#define STATE_0 0
#define STATE_1 1
#define STATE_2 2
#define STATE_3 3
#define STATES 4
#define PARENTS 2


/*List of State Transition Values*/


unsigned char * conv_encode (unsigned char *bit_stream, long *message_length);
unsigned char * conv_decode (unsigned char *bit_stream, long stream_length);


#endif // ERROR_CC_H
