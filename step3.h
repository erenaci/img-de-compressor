/*
 * step3.h
 * Xenia Rudchenko(xrudch01) and Erena Inoue(einoue01)
 * Comp40, HW4 arith
 * Due: , 2020
 * Header file for step3.c
 */

#ifndef STEP3_H
#define STEP3_H
#include <compress40.h>
#include <a2methods.h>
#include <uarray2b.h>
#include <uarray2.h>
#include <arith40.h>

struct applyStruct2 {
 A2Methods_UArray2 arr;
 A2Methods_T methods;
};
typedef struct applyStruct2 *applyStruct2;

A2Methods_UArray2 packWords(A2Methods_UArray2 arr2, A2Methods_T methods);
void blockpxToWord(int i, int j, A2Methods_UArray2 arr, void *elem, void *cl);
A2Methods_UArray2 unpackWords(A2Methods_UArray2 arr2, A2Methods_T methods);
void wordToBlockPx(int i, int j, A2Methods_UArray2 arr, void *elem, void *cl);

#endif