/**************************************************************
 *
 *                     STEP 2
 *
 *     Assignment: arith
 *     Authors: Xenia Rudchenko, Erena Inoue
 *     Date: 03/03/20  
 *
 *     Summary:
 *     This program handles the compression and decompression 
 *     steps of getting individual pixel information and packing 
 *     it into data per 4 pixels. 
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
#include "step2.h"

float bindAY2(float val);

/*******************************************
*  Name        :   compressMap
*  Params      :   int i, int j, A2Methods_UArray2 arr, void *elem, void *cl
*  Returns     :   void
*  Function    :   Apply function that maps over an array of pixels and gathers 
*                  the 4 y and average pb and pr values from each corresponding
*                  block in the blocked array of pixels
******************************************/
void compressMap(int i, int j, A2Methods_UArray2 arr, void *elem, void *cl)
{
  (void) arr;
  applyStruct info = cl;
  blockpx currCell = elem; 
  px px1 = (info->methodsb->at(info->arr2b, 2*i, 2*j));
  px px2 = (info->methodsb->at(info->arr2b, 2*i + 1, 2*j));
  px px3 = (info->methodsb->at(info->arr2b, 2*i, 2*j + 1));
  px px4 = (info->methodsb->at(info->arr2b, 2*i + 1, 2*j + 1));

  float y1 = px1 -> y;
  y1 = bindAY2(y1);
  float y2 = px2 -> y;
  y2 = bindAY2(y2);
  float y3 = px3 -> y;
  y3 = bindAY2(y3);
  float y4 = px4 -> y;
  y4 = bindAY2(y4);

  currCell -> a = (y4 + y3 + y2 + y1) / 4.0;
  currCell -> b = (y4 + y3 - y2 - y1) / 4.0;
  currCell -> c = (y4 - y3 + y2 - y1) / 4.0; 
  currCell -> d = (y4 - y3 - y2 + y1) / 4.0;

  float avgPb = ((px1 -> pb) + (px2 -> pb) + (px3 -> pb) + (px4 -> pb)) / 4.0;
  currCell -> chromaPb = Arith40_index_of_chroma(avgPb);
  float avgPr = ((px1 -> pr) + (px2 -> pr) + (px3 -> pr) + (px4 -> pr)) / 4.0;
  currCell -> chromaPr = Arith40_index_of_chroma(avgPr);
}
  
/*******************************************
*  Name        :   packBits
*  Params      :   A2Methods_UArray2 arr2b, A2Methods_T methods, A2Methods_T methodsb
*  Returns     :   A2Methods_UArray2
*  Function    :   Returns 2D array of block pixel values (4 ys and avg pb, pr)
******************************************/
A2Methods_UArray2 packBits(A2Methods_UArray2 arr2b, A2Methods_T methods, 
                           A2Methods_T methodsb) 
{
 int hgt = (methodsb -> height(arr2b)) / 2;
 int wid = (methodsb -> width(arr2b)) / 2;
 A2Methods_UArray2 blockpxImg = methods -> new(wid, hgt, 
                                           sizeof(struct blockpx));
 applyStruct initialStruct = malloc(sizeof(struct applyStruct)); 
 initialStruct -> arr2b = arr2b;
 initialStruct -> methodsb = methodsb;
 initialStruct -> methods = methods;
 methods -> map_row_major(blockpxImg, compressMap, initialStruct);
 methodsb -> free(&arr2b);
 free(initialStruct);
 return blockpxImg;  
}

/*******************************************
*  Name        :   decompressMap
*  Params      :   int i, int j, A2Methods_UArray2 arr, void *elem, void *cl
*  Returns     :   void
*  Function    :   Apply function that maps over an array of pixels and assigns 
*                  the y and pb and pr values from each corresponding
*                  block in the blocked array of pixels to the current array
******************************************/
void decompressMap(int i, int j, A2Methods_UArray2 arr, void *elem, void *cl)
 {
   (void) arr;
   applyStruct info = cl;
   blockpx curr = elem; 
   px px1 = (info->methodsb->at(info->arr2b, 2*i, 2*j));
   px px2 = (info->methodsb->at(info->arr2b, 2*i + 1, 2*j));
   px px3 = (info->methodsb->at(info->arr2b, 2*i, 2*j + 1));
   px px4 = (info->methodsb->at(info->arr2b, 2*i + 1, 2*j + 1));

   float currA = curr -> a;
   float currB = curr -> b;
   float currC = curr -> c;
   float currD = curr -> d;

   float y1 = currA - currB - currC + currD;
   y1 = bindAY2(y1);
   float y2 = currA - currB + currC - currD;
   y2 = bindAY2(y2);
   float y3 = currA + currB - currC - currD;
   y3 = bindAY2(y3);
   float y4 = currA + currB + currC + currD;
   y4 = bindAY2(y4);

   px1->y = y1;
   px2->y = y2;
   px3->y = y3;
   px4->y = y4;
  
   float pb = Arith40_chroma_of_index(curr->chromaPb);
   float pr = Arith40_chroma_of_index(curr->chromaPr);

   px1->pb = pb;
   px2->pb = pb;
   px3->pb = pb;
   px4->pb = pb;

   px1->pr = pr;
   px2->pr = pr;
   px3->pr = pr;
   px4->pr = pr;
 }

 /*******************************************
 *  Name        :   unpackBits
 *  Params      :   A2Methods_UArray2 arr2, A2Methods_T methods, 
 *                  A2Methods_T methodsb
 *  Returns     :   A2Methods_UArray2
 *  Function    :   Returns a 2D blocked array with block pixels of YPP data
 *                  from the block pixels, splitting the information into 4 pixels
 *                  per each cell 
 ******************************************/ 
A2Methods_UArray2 unpackBits(A2Methods_UArray2 arr2, A2Methods_T methods, 
                             A2Methods_T methodsb) 
{
  int hgt = methods -> height(arr2);
  int wid = methods -> width(arr2);
  A2Methods_UArray2 pxImg = methodsb -> new_with_blocksize(wid * 2, hgt * 2, 
                                        sizeof(struct px), 2);
  applyStruct initialStruct = malloc(sizeof(struct applyStruct)); 
  initialStruct -> arr2b = pxImg;
  initialStruct -> methodsb = methodsb;
  initialStruct -> methods = methods;
  methods -> map_row_major(arr2, decompressMap, initialStruct);
  methods->free(&arr2);
  free(initialStruct);
  return pxImg;
}

/*******************************************
*  Name        :   bindAY2
*  Params      :   float val
*  Returns     :   float
*  Function    :   Returns a float forced to a value between 0 and 1
******************************************/
float bindAY2(float val)
{
  if (val > 1.0) {
    val = 1.0;
  }
  if (val < 0.0) {
    val = 0.0;
  }
  return val;
}