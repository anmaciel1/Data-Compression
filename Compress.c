/*
* Program:      compress
*
* Purpose:      Compress a file.
*
* Usage:        compress INFILE -o OUTFILE
*/

#include "nibbler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_usage(void) {
    printf("Usage:  compress INFILE -o OUTFILE\n");
    exit(0);
}

/*
* Compress FILE *fin into the NIB *nib.  Assumes that fin already has been
* opened by fin = fopen(...) and that nib already has been opened by nib =
* nib_open(...).
*
* compress(FILE *fin, NIB *nib):
*    loop
*       Use fgetc(fin) to get an int ch.
*
*       If ch is EOF then break
*
*       If ch is in the table of Fig. 2:
*          Convert ch into its corresponding nibble from Fig. 2.
*          Write the nibble to nib using nib_put_nibble(nibble, nib).
*       else
*           Write a 0 nibble using nib_put_nibble(nibble, nib).
*           Write the high nibble of ch using nib_put_nibble(nibble, nib).
*           Write the low nibble of ch using nib_put_nibble(nibble, nib).
*/
void compress(FILE *fin, NIB *nib) {

        const char lookup[] = {
        '\0',
        ' ',
        'e',
        't',
        'n',
        'r',
        'o',
        'a',
        'i',
        's',
        'd',
        'l',
        'h',
        'c',
        'f',
        'p',
        };


    int ch;
    while ((ch = fgetc(fin)) != EOF) {
        int found = -1;
        for (int i = 1; i < 16; i++) {
            if ((char)ch == lookup[i]) {
                found = i;
                break;
            }
        }

        if (found > 0) {
            nib_put_nibble(found, nib);
        } else {
            nib_put_nibble(0, nib);
            unsigned char byte = (unsigned char)ch;
            nib_put_nibble((byte >> 4) & 0xF, nib);
            nib_put_nibble(byte & 0xF, nib);
        }
    }
}

/*
* Usage:  ./compress INFILE -o OUTFILE
*
* Confirm that there are four command-line arguments.
* Confirm that argv[2] is "-o".
* Treat argv[1] as the name of the input file.
* Treat argv[3] as the name of the output file.
* Open the input file using fopen(), and report any error.
* Open the output file using nib_open(), and report any error.
* Call compress().
* Close the files.
*/


int main(int argc, char **argv) {
    if (argc != 4) {
        print_usage();
            return 1;
    }

    if (strcmp(argv[2], "-o") != 0) {
        print_usage();
        return 1;
    }

    FILE *fin = fopen(argv[1], "r");
    if (fin == NULL) {
        print_usage();
        return 1;
    }

    NIB *nib = nib_open(argv[3], "w");
    if (nib == NULL) {
        print_usage();
        fclose(fin);
        return 1;
    }

    compress(fin, nib);

    fclose(fin);
    nib_close(nib);

    return 0;
}
