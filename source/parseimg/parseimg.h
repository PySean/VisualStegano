/*
 *"parsefile.h", by Sean Soderman
 * Header file defining techniques to read in and evaluate BMP data.
 */

/* These #defines enumerate the possible types of BMP files we may come across
 * as byte sizes.
 * 
 * Windows v1 type bitmaps are not supported, since they are quite rare.
 * Windows v3 NT and OS/2 bitmaps are also not supported.
 */

#define v2 12
#define v3 40
#define v4 108
#define v5 124
#pragma pack (1)


/* I got the structs bitmap_file_header, bitmap_data_header, and palette_element
 * from www.fileformat.info/format/bmp/egff.htm.
 */

/* Structure defining the BMP file header for BMP files v2 onward.
 * This is not the same as the bitmap header (which prefixes the proper
 * bitmap data.)
 */
typedef struct bitmap_file_header
{
   unsigned short file_type; //This is always "BM"
   unsigned file_size; //Size of file in bytes
   unsigned short reserved1; //Always 0
   unsigned short reserved2; //Always 0
   unsigned bmp_offset; //Denotes the beginning of image data
}bmp_file_hdr;

/* Structure defining the full blown, v4 bitmap header.
 * Since every other BMP header is a subset of this one, it will be partially
 * filled most of the time.
 */
typedef struct bitmap_data_header
{
   unsigned size; //Size of header in bytes 
   int width; //Image width in pixels
   int height; //Image height in pixels
   unsigned short planes; //Number of color planes
   unsigned short bits_per_pixel; //Exactly what it says
   unsigned compression; //Compression methods used
   unsigned size_of_bmp; //Size of bitmap in bytes.
   int horz_resolution; //Horizontal resolution in pixels 
   int vert_resolution; //Vertical resolution in pixels.
   unsigned colors_used; //Number of colors in the image
   unsigned colors_important; //Minimum number of important colors
   
   //These are fields for Windows BMPs v. 4.x.
   unsigned red_mask;   //Mask identifying bits of red component
   unsigned green_mask; //Mask identifying bits of green component
   unsigned blue_mask;  //Mask identifying bits of blue component
   unsigned alpha_mask; //Mask identifying bits of alpha component
   unsigned cs_type;    //Color space type
   int red_x;          //X coordinate of red endpoint.
   int red_y;          //Y coordinate of red endpoint.
   int red_z;          //Z coordinate of red endpoint.
   int green_x;        //X coordinate of green endpoint.
   int green_y;        //Y coordinate of green endpoint.
   int green_z;        //Z coordinate of green endpoint.  
   int blue_x;         //X coordinate of blue endpoint. 
   int blue_y;         //Y coordinate of blue endpoint. 
   int blue_z;         //Z coordinate of blue endpoint. 
   unsigned gamma_red;  //Gamma red coordinate scale value
   unsigned gamma_green;//Gamma green coordinate scale value
   unsigned gamma_blue; //Gamma blue coordinate scale value

   //There's a BMP v5, apparently! Hopefully they don't come out with a new one!
   unsigned intent;
   unsigned profile_data;
   unsigned profile_size;
   unsigned reserved;
}bmp_data_hdr;

/* Structure defining a palette element. Pre v3 elements are composed only
 * of 3 bytes, those being R, G, and B. v3 and later use a "reserved" 
 * field which is always zero.
 *
 * NOTE: Palettes only exist in bitmaps using <= 8 bits/pixel. These
 * are not required by those that use 16 and up.
 */
typedef struct big_palette_element
{
   unsigned char blue;  //Blue component
   unsigned char green; //Green component
   unsigned char red;   //Red component
   unsigned char reserved; //Padding. Always 0.
}p_element;

/* Use this for reading in the palette table from BMP files with <=
 * 8 bits per pixel. Can also double as the struct for reading in
 * raw pixel data for 24 bit/pixel bitmaps.
 *
 */
typedef struct small_palette_element
{
   unsigned char blue;  //Blue component
   unsigned char green; //Green component
   unsigned char red;   //Red component
}sp_element, pixel24;

/* Structure that holds header structures, as well as the entire height x width
 * array of pixel bytes. Some members are mutually exclusive and therefore
 * some will be zero whereas the others will store something. Will essentially
 * store the entirety of the bitmap file. Storing all of this information is necessary
 * for the re-construction of a bitmap file...
 */
typedef struct holder
{
   bmp_file_hdr fheader;   //Generic bmp file header common to all bitmaps.
   bmp_data_hdr dheader;   //Header for bitmap data information.
   sp_element * s_palette; //Color palette for bmp v2.
   p_element * palette;    //Color palette for bmp v3/v4
   char ** img_data8bit;   //If this bitmap is 8 bits/pixel, the img data is here.
   pixel24 ** img_data24bit; //Otherwise if its 24 bits, it is here.
}bmp_file;


//Loads all image data into a struct.
void load_img(bmp_file * bmp, char * file_name);

//Outputs image data to specified filename.
void write_img(bmp_file bmp, char * file_name);
