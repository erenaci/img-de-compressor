/**************************************************************
 *
 *                     STEP 3
 *
 *     Assignment: arith
 *     Authors: Xenia Rudchenko, Erena Inoue
 *     Date: 03/03/20  
 *
 *     Summary:
 *     This program handles the compression and decompression 
 *     steps of converting the picture information into 
 *     bitpacked words and the other way around. 
 *
 **************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <compress40.h>
#include <string.h>
#include <assert.h>
#include <a2methods.h>
#include <a2plain.h>
#include <a2blocked.h>
#include <uarray2b.h>
#include <uarray2.h>
#include <arith40.h>
#include <bitpack.h>
#include "step2.h"
#include "step3.h"

const float SCALE_A = 511.0;
const float SCALE_BCD = 50.0;
const int WIDTH_CHROMA_P = 4;
const int WIDTH_BCD = 5;
const int WIDTH_A = 9;

float bindBCD(float val);
float bindAY3(float val);

/*******************************************
*  Name        :   packWords
*  Params      :   A2Methods_UArray2 arr2, A2Methods_T methods
*  Returns     :   A2Methods_UArray2
*  Function    :   Takes in 2D array of blockpx and 
*                  returns a 2D array of 32bit word 
******************************************/
A2Methods_UArray2 packWords(A2Methods_UArray2 arr2, A2Methods_T methods) 
{
  int hgt = methods -> height(arr2);
  int wid = methods -> width(arr2);
  A2Methods_UArray2 wordImg = methods -> new(wid, hgt, sizeof(uint32_t));
  applyStruct2 initialStruct = malloc(sizeof(struct applyStruct2));
  initialStruct -> arr = arr2;
  initialStruct -> methods = methods;
  methods -> map_row_major(wordImg, blockpxToWord, initialStruct);
  methods -> free(&arr2);
  free(initialStruct);
  return wordImg;   
}

/*******************************************
*  Name        :   blockpxToWord
*  Params      :   int i, int j, A2Methods_UArray2 arr, void *elem, void *cl
*  Returns     :   None
*  Function    :   Apply function that maps over block pixels and packs data
*                  bits to the corresponding 32 bit word
******************************************/
void blockpxToWord(int i, int j, A2Methods_UArray2 arr, void *elem, void *cl)
{
  (void) arr;
  applyStruct2 info = cl;
  uint32_t *word = elem;
  blockpx cell = (info->methods->at(info->arr, i, j));
  int pr = cell->chromaPr; 
  int pb = cell->chromaPb;
  *word = Bitpack_newu(*word, WIDTH_CHROMA_P, 0, pr);  
  *word = Bitpack_newu(*word, WIDTH_CHROMA_P, 4, pb);
  
  float a = cell->a;
  a = bindAY3(a);
  int  a_new = a * SCALE_A;
  *word = Bitpack_newu(*word, WIDTH_A, 23, a_new);

  float b = cell->b;
  b = bindBCD(b);
  int b_new = b * SCALE_BCD;
  *word = Bitpack_news(*word, WIDTH_BCD, 18, b_new); 

  float c = cell->c;
  c = bindBCD(c);
  int c_new = c * SCALE_BCD;
  *word = Bitpack_news(*word, WIDTH_BCD, 13, c_new);
  
  float d = cell->d;
  d = bindBCD(d);
  int d_new = d * SCALE_BCD;
  *word = Bitpack_news(*word, WIDTH_BCD, 8, d_new);  
}

/*******************************************
*  Name        :   unpackWords
*  Params      :   A2Methods_UArray2 arr2, A2Methods_T methods
*  Returns     :   A2Methods_UArray2
*  Function    :   Returns a 2D array with all the block pixel info from the 
*                  array of 32 bit words provided
******************************************/
A2Methods_UArray2 unpackWords(A2Methods_UArray2 arr2, A2Methods_T methods)
 {
   int hgt = methods -> height(arr2);
   int wid = methods -> width(arr2);
   A2Methods_UArray2 blockpxImg = methods -> new(wid, hgt, 
                                  sizeof(struct blockpx));
   applyStruct2 initialStruct = malloc(sizeof(struct applyStruct2));
   initialStruct -> arr = arr2;
   initialStruct -> methods = methods;
   methods -> map_row_major(blockpxImg, wordToBlockPx, initialStruct);
   methods -> free(&arr2);
   free(initialStruct);
   return blockpxImg;
 }

 /*******************************************
 *  Name        :   wordToBlockPx
 *  Params      :   int i, int j, A2Methods_UArray2 arr, void *elem, void *cl
 *  Returns     :   none
 *  Function    :   Apply function that maps across 2D array and gets block 
 *                  pixel data from the corresponding array of 32 bit words
 ******************************************/
 void wordToBlockPx(int i, int j, A2Methods_UArray2 arr, void *elem, void *cl)
 {
   (void) arr;
   applyStruct2 info = cl;
   blockpx cell = elem;
   uint32_t *word = (info->methods->at(info->arr, i, j));
   cell->chromaPr = Bitpack_getu(*word, WIDTH_CHROMA_P, 0);
   cell->chromaPb = Bitpack_getu(*word, WIDTH_CHROMA_P, 4);
   cell->a = ((Bitpack_getu(*word, WIDTH_A, 23)) / SCALE_A);
   cell->a = bindAY3(cell->a);
   
   cell->b = ((Bitpack_gets(*word, WIDTH_BCD, 18)) / SCALE_BCD);
   cell->b = bindBCD(cell->b);
   
   cell->c = ((Bitpack_gets(*word, WIDTH_BCD, 13)) / SCALE_BCD);
   cell->c = bindBCD(cell->c);
   
   cell->d = ((Bitpack_gets(*word, WIDTH_BCD, 8)) / SCALE_BCD);
   cell->d = bindBCD(cell->d);
 }

 /*******************************************
 *  Name        :   bindBCD
 *  Params      :   float val
 *  Returns     :   float
 *  Function    :   Returns a float forced to a value between -0.3 and 0.3
 ******************************************/
float bindBCD(float val)
{
  if (val > 0.3) {
    val = 0.3;
  }
  if (val < -0.3) {
    val = -0.3;
  }
  return val;
}

/*******************************************
*  Name        :   bindAY1
*  Params      :   float val
*  Returns     :   float
*  Function    :   Returns a float forced to a value between 0 and 1
******************************************/
float bindAY3(float val)
{
  if (val > 1.0) {
    val = 1.0;
  }
  if (val < 0.0) {
    val = 0.0;
  }
  return val;
}

