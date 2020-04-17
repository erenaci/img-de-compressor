/**************************************************************
 *
 *                     bitpack
 *
 *     Assignment: arith
 *     Authors: Xenia Rudchenko, Erena Inoue
 *     Date: 03/03/20  
 *
 *     Summary:
 *     This program manipulates words of type int64_t and uint64_t
 *     getting different sections of the word, changing sections of
 *     the word, and making sure a given value can fit into the 
 *     provided bit representation. 
 *
 **************************************************************/
 
 #include <stdlib.h>
 #include <stdio.h>
 #include <bitpack.h>
 #include <string.h>
 #include <stdbool.h>
 #include <assert.h>
 #include <pnm.h>
 #include <a2methods.h>
 #include <a2plain.h>
 #include <a2blocked.h>
 #include <uarray2b.h>
 #include <uarray2.h>
 #include "except.h"

Except_T Bitpack_Overflow = { "Overflow packing bits" };

/*******************************************
*  Name        :   Bitpack_fitsu
*  Params      :   uint64_t n, unsigned width
*  Returns     :   bool
*  Function    :   This function ensures that an unsizgned integer value fits 
*                  into the number of bit slots alloted for it
******************************************/
bool Bitpack_fitsu(uint64_t n, unsigned width)
 {
   assert(width > 0 && width < 64);
    unsigned offset = 64 - width;
    uint64_t shifted_n = n << offset;
    shifted_n = shifted_n >> offset;
    
    if (shifted_n == n) {
      return true;
    }
    else {
      return false;
    }
 }

 /*******************************************
 *  Name        :   Bitpack_fitss
 *  Params      :   int64_t n, unsigned width
 *  Returns     :   bool
 *  Function    :   This function ensures that a signed integer value fits 
 *                  into the number of bit slots alloted for it
 ******************************************/
bool Bitpack_fitss(int64_t n, unsigned width)
 {
   assert(width > 0 && width < 64);
   unsigned offset = 64 - width;
   int64_t shifted_n = n << offset;
   shifted_n = shifted_n >> offset;
   if (shifted_n == n) {
     return true;
   }
   else {
     return false;
   } 
 }

 /*******************************************
 *  Name        :   Bitpack_getu
 *  Params      :   uint64_t word, unsigned width, unsigned lsb
 *  Returns     :   uint64_t
 *  Function    :   This function returns the bits in a word from a given least
 *                  significant bit value, with a given width, in an unsigned 
 *                  integer form
 ******************************************/ 
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
 {
   if (width == 0) {
     return 0;
   }
   assert(width <= 64);
   assert(width + lsb <= 64);
   uint64_t mask = ~0;
   mask = mask << (64 - width);
   mask = mask >> (64 - width - lsb);
   uint64_t afterMask = word & mask;
   afterMask = afterMask >> lsb;
   return afterMask;
 }

 /*******************************************
 *  Name        :   Bitpack_gets
 *  Params      :   uint64_t word, unsigned width, unsigned lsb
 *  Returns     :   int64_t
 *  Function    :   This function returns the bits in a word from a given least
 *                  significant bit value, with a given width, in signed 
 *                  integer form
 ******************************************/  
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
 {
   if (width == 0) {
     return 0;
   }
   assert(width <= 64);
   assert(width + lsb <= 64);
   uint64_t mask = ~0;
   mask = mask << (64 - width);
   mask = mask >> (64 - width - lsb);
   uint64_t checkNeg = width + lsb - 1;
   checkNeg = 1 << checkNeg;
   if ((checkNeg & word) == checkNeg) {
     int64_t afterMask1 = (word & mask) >> lsb;
     uint64_t negMask = ~0;
     negMask = negMask << width;
     afterMask1 = afterMask1 | negMask;
     return afterMask1;
   }
   else {
     uint64_t afterMask2 = (word & mask) >> lsb;
     return afterMask2;
   }
 }
 
 /*******************************************
 *  Name        :   Bitpack_newu
 *  Params      :   uint64_t word, unsigned width, unsigned lsb
 *  Returns     :   uint64_t
 *  Function    :   This function returns a word with updated bits at given 
 *                  least significant bit, with a given width, and a given 
 *                  unsigned value
 ******************************************/  
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, uint64_t value)
 {
   assert(width <= 64);
   assert(width + lsb <= 64);
   if (! Bitpack_fitsu(value, width)) {
     RAISE(Bitpack_Overflow);
   }
   uint64_t mask_1 = ~0;
   mask_1 = mask_1 << (lsb + width);
   uint64_t mask_2 = ~0;
   mask_2 = mask_2 >> (64 - lsb);
   uint64_t mask = mask_1 | mask_2;
   uint64_t valToInsert = value << lsb;
   uint64_t afterMask = (word & mask) | valToInsert;
   return afterMask;
 }


 /*******************************************
 *  Name        :   Bitpack_news
 *  Params      :   uint64_t word, unsigned width, unsigned lsb
 *  Returns     :   uint64_t
 *  Function    :   This function returns a word with updated bits at given 
 *                  least significant bit, with a given width, and a given 
 *                  signed value
 ******************************************/  
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb,  int64_t value)
 {   
   assert(width <= 64);
   assert(width + lsb <= 64);
   if (! Bitpack_fitss(value, width)) {
     RAISE(Bitpack_Overflow);
   }
   uint64_t mask_1 = ~0;
   mask_1 = mask_1 << (lsb + width);
   uint64_t mask_2 = ~0;
   mask_2 = mask_2 >> (64 - lsb);
   uint64_t mask = mask_1 | mask_2; 
   if (value < 0) {
     uint64_t mask_3 = ~0;
     mask_3 = mask_3 >> (64 - (width + lsb));
     uint64_t mask_4 = ~0;
     mask_4 = mask_4 << lsb;
     mask_4 = mask_4 & mask_3;
     value = value << lsb;
     value = value & mask_4;
   }
   else if (value >= 0) {
     value = value << lsb;
   }
   uint64_t afterMask = (word & mask) | value;
   return afterMask;
 }
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 