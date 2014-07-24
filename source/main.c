#include <stdio.h>
#include "parseimg/parseimg.h"

/*
 * "main.c", by Sean Soderman
 * Test your stuff here!
 * Right now this simply does the equivalent of "cp f1 f2", but for bmps only.
 */
int main(int argc, char * argv[])
{
   bmp_file bmp;
   load_img(&bmp, argv[1]);
   write_img(bmp, argv[2]);

   return 0;
}
