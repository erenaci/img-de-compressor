/**************************************************************
 *
 *                     40image
 *
 *     Assignment: arith
 *     Authors: Xenia Rudchenko, Erena Inoue
 *     Date: 03/03/20  
 *
 *     Summary:
 *     This program takes an image from a file or standard input
 *     and calls compress40.c to handle the image compression
 *     and decompression, using the bitpacking interface.
 *
 **************************************************************/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "compress40.h"
#include <pnm.h>
#include "step1.h"
#include "step2.h"
#include "step3.h"

static void (*compress_or_decompress)(FILE *input) = compress40;

int main(int argc, char *argv[])
{
        int i;

        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-c") == 0) {
                        compress_or_decompress = compress40;
                } else if (strcmp(argv[i], "-d") == 0) {
                        compress_or_decompress = decompress40;
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n",
                                argv[0], argv[i]);
                        exit(1);
                } else if (argc - i > 2) {
                        fprintf(stderr, "Usage: %s -d [filename]\n"
                                "       %s -c [filename]\n",
                                argv[0], argv[0]);
                        exit(1);
                } else {
                        break;
                }
        }
        assert(argc - i <= 1);    /* at most one file on command line */
        if (i < argc) {
                FILE *fp = fopen(argv[i], "r");
                assert(fp != NULL);
                compress_or_decompress(fp);
                fclose(fp);
        } else {
                compress_or_decompress(stdin);
        }

        return EXIT_SUCCESS; 
}

