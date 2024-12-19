/*
* File:     nibbler.c
*
* Purpose:  Normally files are read and written one byte at a time, but
*           these routines let us read and write files one nibble (4 bits)
*           at a time.
*/

#include "nibbler.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
* Purpose:  Open a file for use by nib_get_nibble() or nib_put_nibble().
*
* Using fopen(), open file "filename" with mode "mode".
* If fopen() returns NULL, then this function returns NULL.
* Allocate a NIB struct using calloc(), and initialize it:
*
*    nib->underlying_f holds the return value from fopen().
*    nib->opened_for_read is set to 1 if mode starts with 'r'.
*
* Return a pointer to the NIB.
*/
NIB *nib_open(const char *filename, const char *mode) {

    FILE *f = fopen(filename, mode);

    if (f == NULL){
            return NULL;
    }

    NIB *NIB = calloc(1, sizeof(NIB));
    if (NIB == NULL){
            fclose(f);
            return NULL;
    }

    NIB->underlying_f = f;

    if (mode[0] == 'r'){
            NIB->opened_for_read = 1;
    }

    return NIB;
}

/*
* Purpose:  Read a nibble from an open input file.
*
* if nib->num_nibbles > 0:
*    nib->num_nibbles = 0
*    Return nib->stored_nibble.
* else
*    int byte = fgetc(nib->underlying_f)
*    If byte is EOF then return EOF.
*    nib->stored_nibble = low nibble of the byte.
*    nib->num_nibbles = 1
*    Return high nibble of the byte.
*/
int nib_get_nibble(NIB *nib) {

    if (nib->num_nibbles > 0){
            nib->num_nibbles = 0;
            return nib->stored_nibble;
    }
    else{
            int byte = fgetc(nib ->underlying_f);
            if (byte == EOF){
                    return EOF;
            }

            nib->stored_nibble = byte & 0xF;
            nib -> num_nibbles = 1;
            return (byte >> 4) & 0xF;
    }

}

/*
* Purpose:  Write a nibble to an open NIB.
*
* if nib->num_nibbles > 0:
*    Create a byte using nib->stored_nibble as the high nibble and the
*       parameter "nibble" as the low nibble.
*    Write the byte using fputc(nib->underlying_f).
*    nib->num_nibbles = 0
* else
*    nib->stored_nibble = nibble
*    nib->num_nibbles = 1
*/
void nib_put_nibble(int nibble, NIB *nib) {

        if (nib->num_nibbles > 0){
                int byte =( nib -> stored_nibble << 4) | (nibble & 0xF);

                int result = fputc(byte, nib -> underlying_f);

                nib -> num_nibbles = 0;

        }
        else{
                nib -> stored_nibble = nibble;
                nib -> num_nibbles = 1;

        }


}

/*
* Purpose:  Close the NIB file.
*
* If the file is opened for write and nib->num_nibbles > 0 then
*    create a byte with nib->stored_nibble as the high nibble.
*    write the byte using fputc(nib->underlying_f).
* Close file with fclose(nib->underlying_f).
*/
void nib_close(NIB *nib) {

        if (!nib-> opened_for_read && nib -> num_nibbles > 0){
                int byte = nib -> stored_nibble << 4;
                fputc(byte, nib -> underlying_f);

        }

                fclose(nib -> underlying_f);
                free(nib);
}
