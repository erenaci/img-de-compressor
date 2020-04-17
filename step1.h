/*
 * step1.h
 * Xenia Rudchenko(xrudch01) and Erena Inoue(einoue01)
 * Comp40, HW4 arith
 * Due: , 2020
 * Header file for step1.c
 */
#ifndef STEP1_H
#define STEP1_H
#include <compress40.h>
#include <pnm.h>
#include <a2methods.h>
#include <uarray2b.h>
#include <uarray2.h>
#include <arith40.h>

struct px {
 float y;
 float pb;
 float pr;
};
typedef struct px *px;

A2Methods_UArray2 rgbToYpp(Pnm_ppm img, A2Methods_UArray2 arr2b, 
                          A2Methods_T methodsb);
A2Methods_UArray2 yppToRgb (A2Methods_UArray2 arr2b, A2Methods_T methods,
                           A2Methods_T methodsb);
 
 #endif