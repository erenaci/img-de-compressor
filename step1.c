/**************************************************************
 *
 *                     STEP 1
 *
 *     Assignment: arith
 *     Authors: Xenia Rudchenko, Erena Inoue
 *     Date: 03/03/20  
 *
 *     Summary:
 *     This program handles the compression and decompression 
 *     steps of converting RGP to YPP values and the 
 *     other way around.
 *
 **************************************************************/

 #include <stdlib.h>
 #include <stdio.h>
 #include <compress40.h>
 #include <string.h>
 #include <assert.h>
 #include <pnm.h>
 #include <a2methods.h>
 #include <a2plain.h>
 #include <a2blocked.h>
 #include <uarray2b.h>
 #include <uarray2.h>
 #include <arith40.h>
 #include "step1.h"

const int DENOMINATOR = 255;

float bindAY1(float val);
float bindP(float val);
 
 /*******************************************
 *  Name        :   rgbToYpp
 *  Params      :   Pnm_ppm img, A2Methods_UArray2 arr2b, A2Methods_T methodsb
 *  Returns     :   A2Methods_UArray2
 *  Function    :   Returns a blocked 2D array with each cell corresponding
 *                  to a pixel of the image and their YPP values.
 ******************************************/
 A2Methods_UArray2 rgbToYpp(Pnm_ppm img, A2Methods_UArray2 arr2b, 
                                         A2Methods_T methodsb)
 {   
   for (unsigned i = 0; i < img -> height; i++) {
     for (unsigned j = 0; j < img -> width; j++) {
         Pnm_rgb location = (img -> methods) -> at(img -> pixels, j, i);
         px pixel = malloc(sizeof(struct px));
         assert(pixel != NULL);

         float red = (location -> red) / ((float) img -> denominator);
         float green = (location -> green) / ((float) img -> denominator);
         float blue = (location -> blue) / ((float)img -> denominator);

         pixel -> y = 0.299 * red + 0.587 * green + 0.114 * blue;
         pixel->y = bindAY1(pixel -> y);
         pixel -> pb = -0.168736 * red - 0.331264 * green + 0.5 * blue; 
         pixel -> pb = bindP(pixel -> pb);
         pixel -> pr = 0.5 * red - 0.418688 * green - 0.081312 * blue;
         pixel -> pr = bindP(pixel -> pr);
         px currPx = ((px) methodsb -> at(arr2b, j, i));
         *currPx = *pixel;
         free(pixel);
     }
   }   
   return arr2b;
 }

 /*******************************************
 *  Name        :   yppToRgb
 *  Params      :   A2Methods_UArray2 arr2b, A2Methods_T methods, 
 *                  A2Methods_T methodsb
 *  Returns     :   A2Methods_UArray2
 *  Function    :   Returns a 2D array with each cell corresponding to 
 *                  to the rgb values for each pixel
 ******************************************/
 A2Methods_UArray2 yppToRgb (A2Methods_UArray2 arr2b, A2Methods_T methods, 
                             A2Methods_T methodsb) 
 {
   int wid = methods -> width(arr2b);
   int hgt = methods -> height(arr2b);
   A2Methods_UArray2 finalImg = methods -> new(wid, hgt, 
                                sizeof(struct Pnm_rgb));
   for (int i = 0; i < hgt; i++) {
     for (int j = 0; j < wid; j++) {
        Pnm_rgb RGBpx = methods -> at(finalImg, j, i);
        px currpx = ((px) methodsb -> at(arr2b, j, i));
        float red =  1.0 * (currpx -> y) + 0.0 * (currpx -> pb) + 
                        1.402 * (currpx -> pr);
        red = bindAY1(red);
        float green = 1.0 * (currpx -> y) - 0.344136 * (currpx -> pb) - 
                         0.714136 * (currpx -> pr);
        green = bindAY1(green);
        float blue =  1.0 * (currpx -> y) + 1.772 * (currpx -> pb) + 
                         0.0 * (currpx -> pr);
        blue = bindAY1(blue);
        RGBpx -> red = (unsigned)(red * DENOMINATOR);
        RGBpx -> green = (unsigned)(green * DENOMINATOR);
        RGBpx -> blue = (unsigned)(blue * DENOMINATOR);
     }
   }
   methodsb->free(&arr2b);
   return finalImg;
 }

 /*******************************************
 *  Name        :   bindAY1
 *  Params      :   float val
 *  Returns     :   float
 *  Function    :   Returns a float forced to a value between 0 and 1
 ******************************************/
 float bindAY1(float val)
 {
   if (val > 1.0) {
     val = 1.0;
   }
   if (val < 0.0) {
     val = 0.0;
   }
   return val;
 }
 /*******************************************
 *  Name        :   bindP
 *  Params      :   float val
 *  Returns     :   float
 *  Function    :   Returns a float forced to a value between -0.5 and 0.5
 ******************************************/
 float bindP(float val)
 {
   if (val > 0.5) {
     val = 0.5;
   }
   if (val < -0.5) {
     val = -0.5;
   }
   return val;
 }

