/*
 * CONVOLUTIONAL ENCODER
 * Copyright (c) 1999, Spectrum Applications, Derwood, MD, USA
 * All rights reserved
 * Version 2.0 Last Modified 1999.02.17
 */
//#include <alloc.h>
//#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
 
#include "vdsim.h"
 
void cnv_encd(int g[2][K], long input_len, unsigned char *in_array, unsigned char *out_array) {

    int m;                     /* K - 1 */
    long t, tt;                /* bit time, symbol time */
    int j, k;                  /* loop variables */
    int *unencoded_data;       /* pointer to data array */
    int shift_reg[K];          /* the encoder shift register */
    int sr_head;               /* index to the first elt in the sr */
    int p, q;                  /* the upper and lower xor gate outputs */

    m = K - 1;
 
    /* allocate space for the zero-padded input data array */
    unencoded_data = malloc( (input_len + m) * sizeof(int) );
    if (unencoded_data == NULL) {
        printf("\ncnv_encd.c: Can't allocate enough memory for unencoded data!  Aborting...");
        exit(1);
    }

    /* read in the data and store it in the array */
    for (t = 0; t < input_len; t++)
        *(unencoded_data + t) = *(in_array + t);

    /* zero-pad the end of the data */
    for (t = 0; t < m; t++) {
        *(unencoded_data + input_len + t) = 0;
    }
 
    /* Initialize the shift register */
    for (j = 0; j < K; j++) {
        shift_reg[j] = 0;
    }
 
    /* To try to speed things up a little, the shift register will be operated
       as a circular buffer, so it needs at least a head pointer. It doesn't
       need a tail pointer, though, since we won't be taking anything out of
       it--we'll just be overwriting the oldest entry with the new data. */
    sr_head = 0;

    /* initialize the channel symbol output index */
    tt = 0;

    /* Now start the encoding process */
    /* compute the upper and lower mod-two adder outputs, one bit at a time */
    for (t = 0; t < input_len + m; t++) {
        shift_reg[sr_head] = *( unencoded_data + t );
        p = 0;
        q = 0;
        for (j = 0; j < K; j++) {
            k = (j + sr_head) % K;
            p ^= shift_reg[k] & g[0][j];
            q ^= shift_reg[k] & g[1][j];
        }

        /* write the upper and lower xor gate outputs as channel symbols */
        *(out_array + tt) = p;
        tt = tt + 1;
        *(out_array + tt) = q;
        tt = tt + 1;
       

        sr_head -= 1;    /* equivalent to shifting everything right one place */
        if (sr_head < 0) /* but make sure we adjust pointer modulo K */
            sr_head = m;

    }
 
    /* free the dynamically allocated array */
    free(unencoded_data);
 
}
