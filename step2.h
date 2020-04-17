/*
 * step2.h
 * Xenia Rudchenko(xrudch01) and Erena Inoue(einoue01)
 * Comp40, HW4 arith
 * Due: , 2020
 * Header file for step2.c
 */

#ifndef STEP2_H
#define STEP2_H
#include <compress40.h>
#include <a2methods.h>
#include <uarray2b.h>
#include <uarray2.h>
#include <arith40.h>

struct blockpx {
 unsigned chromaPb;
 unsigned chromaPr;
 float a;
 float b;
 float c;
 float d;
};
typedef struct blockpx *blockpx;

struct applyStruct {
 A2Methods_UArray2 arr2b;
 A2Methods_T methodsb;
 A2Methods_T methods;
};
typedef struct applyStruct *applyStruct;

void compressMap(int i, int j, A2Methods_UArray2 arr, void *elem, void *cl);
void decompressMap(int i, int j, A2Methods_UArray2 arr, void *elem, void *cl);
A2Methods_UArray2 packBits(A2Methods_UArray2 arr2b, A2Methods_T methods, A2Methods_T methodsb);
A2Methods_UArray2 unpackBits(A2Methods_UArray2 arr2, A2Methods_T methods, A2Methods_T methodsb);

#endif 