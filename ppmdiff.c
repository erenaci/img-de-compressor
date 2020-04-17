/*
 * ppmdiff.c
 * Xenia Rudchenko(xrudch01) and Erena Inoue(einoue01)
 * Comp40, HW4 arith
 * Due: , 2020
 * Purpose: 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assert.h"
#include "compress40.h"
#include "pnm.h"
#include "a2methods.h"
#include "a2plain.h"
#include <math.h>

struct pixel {
  float red;
  float green;
  float blue;
};

typedef struct pixel *pixel;

void checkDiff(Pnm_ppm img1, Pnm_ppm img2);
float calcSum(Pnm_ppm img1, Pnm_ppm img2, int small_wid, int small_hgt);
void freeThings(pixel* pixelsArr1, pixel* pixelsArr2, int len);

int main(int argc, char *argv[]) 
{
  FILE *src1;
  FILE *src2;
  Pnm_ppm img1;
  Pnm_ppm img2;
  A2Methods_T methods = uarray2_methods_plain; 
  if (argc > 3) {
    fprintf(stderr, "Too many arguments.\n");
    exit(EXIT_FAILURE);
  }
  else if (argc == 3) {
    if (strcmp(argv[1], "-") == 0) {
      fprintf(stderr, "HERE 1\n");
      src1 = stdin;
    } 
    else {
      src1 = fopen(argv[1], "rb");
    }
    assert(src1 != NULL);
    img1 = Pnm_ppmread(src1, methods);
    
    if (strcmp(argv[2], "-") == 0) {
      fprintf(stderr, "HERE 2\n");
      src2 = stdin;
    } 
    else {
      src2 = fopen(argv[2], "rb");
    }
    assert(src2 != NULL);
    img2 = Pnm_ppmread(src2, methods);
    checkDiff(img1, img2);
  }

  exit(EXIT_SUCCESS);
}


void checkDiff(Pnm_ppm img1, Pnm_ppm img2)
{
  int height1 = img1 -> height;
  int height2 = img2 -> height;
  int width1 = img1 -> width;
  int width2 = img2 -> width;
  int small_hgt, small_wid;
  
  if (height1 < height2) {
    small_hgt = height1;
  }
  else {
    small_hgt = height2;
  }
  
  if (width1 < width2) {
    small_wid = width1;
  }
  else {
    small_wid = width2;
  }
  
  if (abs(height1 - height2) > 1 || abs(width1 - width2) > 1) {
      fprintf(stderr, "ERROR: the images are too different.\n");
      printf("1.0\n");
      exit(EXIT_FAILURE);
  }

  float sum = calcSum(img1, img2, small_wid, small_hgt);
  float rms = sqrt((float)((sum/(float)(3 * small_wid * small_hgt))));
  
  printf("E = %0.4f\n", rms);
}

float calcSum(Pnm_ppm img1, Pnm_ppm img2, int small_wid, int small_hgt)
{
    pixel* pixelsArr1 = malloc(small_hgt * small_wid * sizeof(pixel));
    int length = small_hgt * small_wid;
    float sum = 0.0000;
    int index = 0;
      for (int i = 0; i < small_hgt; i++) {
        for (int j = 0; j < small_wid; j++) {
            Pnm_rgb location = (img1 -> methods) -> at(img1 -> pixels, j, i);
            pixel deets = malloc(sizeof(struct pixel));
            deets -> red = (location -> red) / ((float) img1 -> denominator);
            deets -> green = (location -> green) / ((float) img1 -> denominator);
            deets -> blue = (location -> blue) / ((float) img1 -> denominator);
            pixelsArr1[index] = deets;
            index++;
        }
      }
      index = 0;
      pixel* pixelsArr2 = malloc(small_hgt * small_wid * sizeof(pixel));
      for (int i = 0; i < small_hgt; i++) {
        for (int j = 0; j < small_wid; j++) {
            Pnm_rgb location = (img2 -> methods) -> at(img2 -> pixels, j, i);
            pixel deets = malloc(sizeof(struct pixel));
            deets -> red = (location -> red) / ((float) img2 -> denominator);
            deets -> green = (location -> green) / ((float) img2 -> denominator);
            deets -> blue = (location -> blue) / ((float) img2 -> denominator);
            pixelsArr2[index] = deets;
            index++;
        }
      }
      
      for (int i = 0; i < length; i++) {
        float diffR = (pixelsArr1[i] -> red) - (pixelsArr2[i] -> red);
        sum += pow(diffR, 2);
        float diffG = (pixelsArr1[i] -> green) - (pixelsArr2[i] -> green);
        sum += pow(diffG, 2);
        float diffB = (pixelsArr1[i] -> blue) - (pixelsArr2[i] -> blue);
        sum += pow(diffB, 2);
      } 
      freeThings(pixelsArr1, pixelsArr2, length);
      return sum;
}

void freeThings(pixel* pixelsArr1, pixel* pixelsArr2, int len) 
{
  for(int i = 0; i < len; i ++) {
    free(pixelsArr1[i]);
    free(pixelsArr2[i]);
  }
  free(pixelsArr1);
  free(pixelsArr2);  
}