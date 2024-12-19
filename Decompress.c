/*
* Program:      decompress
*
* Purpose:      Decompress a file that was compressed with "compress".
*
* Usage:        decompress INFILE -o OUTFILE
*/

#include "nibbler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_usage(void) {
    printf("Usage:  decompress INFILE -o OUTFILE\n");
    exit(0);
}

/*
* Decompress NIB *nib into the FILE *fout.  Assumes that nib already has been
* opened by nib = nib_open(...) and that fout already has been opened by
* fout = fopen(...).
*
* decompress(NIB *nib, FILE *fout):
*    loop
*       read a nibble using nib_get_nibble(nib)
*       if the nibble is EOF then break
*
*       if the nibble > 0 then
*          Convert the nibble into its corresponding byte using Fig. 4
*          Write the byte using fputc(fout)
*       else
*          read nibble1 using nib_get_nibble(nib)
*          if the nibble1 == EOF then break
*
*          read nibble2 using nib_get_nibble(nib)
*          if the nibble2 == EOF then break
*
*          Create a byte with
*             nibble1 as the high nibble and nibble2 as the low nibble
*
*          Write the byte using fputc(fout)
*/
void decompress(NIB *nib, FILE *fout) {
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


        while(1){
                int nibble = nib_get_nibble(nib);
                if(nibble == EOF){
                        break;
                }
                if (nibble > 0){
                        fputc(lookup[nibble], fout);
                }
                else{
                        int nibble1 = nib_get_nibble(nib);
                        if (nibble1 == EOF){
                                break;
                        }
                int nibble2 = nib_get_nibble(nib);
                if (nibble2 == EOF){
                break;
        }
                unsigned char byte = (nibble1 << 4) | nibble2;
                fputc(byte,fout);
                }
        }

}
/*
* Usage:  ./decompress INFILE -o OUTFILE
*
* Confirm that there are four command-line arguments.
* Confirm that argv[2] is "-o".
* Treat argv[1] as the name of the input file.
* Treat argv[3] as the name of the output file.
* Open the input file using nib_open(), and report any error.
* Open the output file using fopen(), and report any error.
* Call decompress().
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

    NIB *nib = nib_open(argv[1], "r");
    if (nib == NULL) {
        print_usage();
        return 1;
    }

    FILE *fout = fopen(argv[3], "w");
    if (fout == NULL) {
        print_usage();
        nib_close(nib);
        return 1;
    }

    decompress(nib, fout);

    nib_close(nib);
    fclose(fout);

    return 0;
}
