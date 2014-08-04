#define K 6              /* constraint length */
#define TWOTOTHEM 4      /* 2^(K - 1) -- change as required */
#define PI 3.141592654   /* circumference of circle divided by diameter */

#define MSG_LEN 100000l  /* how many bits in each test message */
#define DOENC            /* test with convolutional encoding/Viterbi decoding */
#undef  DONOENC          /* test with no coding */
#define LOESN0 0.0       /* minimum Es/No at which to test */
#define HIESN0 3.5       /* maximum Es/No at which to test */
#define ESN0STEP 0.5     /* Es/No increment for test driver */
