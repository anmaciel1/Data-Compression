#include "nibbler.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_TESTS 12

void create_file_from_string(const char *filename, const char *data) {
    FILE *f = fopen(filename, "w");
    assert(f);
    fprintf(f, "%s", data);
    fclose(f);
}

void verify_file_from_string(const char *filename, const char *p) {
    FILE *f = fopen(filename, "r");
    assert(f);

    for (;;) {
        int ch = fgetc(f);

        if (ch == EOF) {
            assert(*p == '\0');
            break;
        }

        assert(ch == *p);
        ++p;
    }

    fclose(f);
}

void test_nib_open_for_read(void) {
    const char *filename = "_test_nib_open.txt";
    create_file_from_string(filename, "'Twas brillig");
    NIB *nib = nib_open(filename, "r");

    assert(nib->underlying_f != NULL);
    assert(nib->opened_for_read);
    assert(nib->num_nibbles == 0);

    unlink(filename);

    printf("PASS\n");
}

void test_nib_open_no_file(void) {
    const char *filename = "_nonexistent.txt";
    unlink(filename); // be certain that there is no file with that name
    NIB *nib = nib_open(filename, "r");

    assert(!nib);

    printf("PASS\n");
}

void test_nib_open_for_write(void) {
    const char *filename = "_test_nib_open.txt";
    NIB *nib = nib_open(filename, "w");

    assert(nib->underlying_f != NULL);
    assert(!nib->opened_for_read);
    assert(nib->num_nibbles == 0);

    unlink(filename);
    printf("PASS\n");
}

void test_nib_close_after_write_0(void) {
    const char *filename = "_test_nib_close.txt";
    FILE *f = fopen(filename, "w");

    NIB nib;
    nib.underlying_f = f;
    nib.opened_for_read = 0;
    nib.num_nibbles = 0;
    nib.stored_nibble = 0x7;

    nib_close(&nib); // file should be empty

    verify_file_from_string(filename, "");

    unlink(filename);

    printf("PASS\n");
}

void test_nib_close_after_write_1(void) {
    const char *filename = "_test_nib_close.txt";
    FILE *f = fopen(filename, "w");

    NIB nib;
    nib.underlying_f = f;
    nib.opened_for_read = 0;
    nib.num_nibbles = 1;
    nib.stored_nibble = 0x7;

    nib_close(&nib); // 0x70 == 'p' should get written

    verify_file_from_string(filename, "p");

    unlink(filename);

    printf("PASS\n");
}

void test_nib_get_nibble_1(void) {
    NIB nib;
    nib.underlying_f = stdin;
    nib.opened_for_read = 1;
    nib.num_nibbles = 1;
    nib.stored_nibble = 0xa;

    assert(nib_get_nibble(&nib) == 0xa);
    assert(nib.num_nibbles == 0);

    printf("PASS\n");
}
void test_nib_get_nibble_2(void) {
    const char *filename = "_test_nib_get_nibble.txt";
    create_file_from_string(filename, "\x12");
    NIB *nib = nib_open(filename, "r");

    assert(nib_get_nibble(nib) == 1);
    assert(nib_get_nibble(nib) == 2);
    assert(nib_get_nibble(nib) == EOF);

    unlink(filename);

    printf("PASS\n");
}

void test_nib_get_nibble_4(void) {
    const char *filename = "_test_nib_get_nibble.txt";
    create_file_from_string(filename, "\x12\x34");
    NIB *nib = nib_open(filename, "r");

    assert(nib_get_nibble(nib) == 1);
    assert(nib_get_nibble(nib) == 2);
    assert(nib_get_nibble(nib) == 3);
    assert(nib_get_nibble(nib) == 4);
    assert(nib_get_nibble(nib) == EOF);

    unlink(filename);

    printf("PASS\n");
}

void test_nib_put_nibble_1(void) {
    const char *filename = "_test_nib_put_nibble.txt";
    FILE *f = fopen(filename, "w");

    NIB nib;
    nib.underlying_f = f;
    nib.opened_for_read = 0;
    nib.num_nibbles = 1;
    nib.stored_nibble = ('U' >> 4) & 0xf;

    nib_put_nibble('U' & 0xf, &nib);
    assert(nib.num_nibbles == 0);

    fclose(f);

    verify_file_from_string(filename, "U");

    unlink(filename);

    printf("PASS\n");
}

void test_nib_put_nibble_2(void) {
    const char *filename = "_test_nib_put_nibble.txt";
    NIB *nib = nib_open(filename, "w");
    nib_put_nibble(0x1, nib);
    nib_put_nibble(0x2, nib);
    nib_close(nib);
   verify_file_from_string(filename, "\x12");

    unlink(filename);

    printf("PASS\n");
}

void test_nib_put_nibble_4(void) {
    const char *filename = "_test_nib_put_nibble.txt";
    NIB *nib = nib_open(filename, "w");
    nib_put_nibble(0x1, nib);
    nib_put_nibble(0x2, nib);
    nib_put_nibble(0x3, nib);
    nib_put_nibble(0x4, nib);
    nib_close(nib);

    verify_file_from_string(filename, "\x12\x34");

    unlink(filename);

    printf("PASS\n");
}

void test_nib_put_nibble_3(void) {
    const char *filename = "_test_nib_put_nibble.txt";
    NIB *nib = nib_open(filename, "w");
    nib_put_nibble(0x1, nib);
    nib_put_nibble(0x2, nib);
    nib_put_nibble(0x3, nib);
    nib_close(nib);

    verify_file_from_string(filename, "\x12\x30");

    unlink(filename);

    printf("PASS\n");
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: ./unittests NUMBER_OF_TESTS (1 to %d)\n", NUM_TESTS);
        exit(0);
    }

    int m = atoi(argv[1]);

    if (m > NUM_TESTS) {
        printf("Usage: ./unittests NUMBER_OF_TESTS (1 to %d)\n", NUM_TESTS);
        exit(0);
    }

    printf("Running %d of %d tests\n", m, NUM_TESTS);

    for (int i = 1; i <= m; ++i) {
        printf("\ntest %d:\n", i);

        switch (i) {
        case 1: test_nib_open_for_read(); break;
        case 2: test_nib_open_no_file(); break;
        case 3: test_nib_open_for_write(); break;
        case 4: test_nib_close_after_write_0(); break;
        case 5: test_nib_close_after_write_1(); break;
       case 6: test_nib_get_nibble_1(); break;
        case 7: test_nib_get_nibble_2(); break;
        case 8: test_nib_get_nibble_4(); break;
        case 9: test_nib_put_nibble_1(); break;
        case 10: test_nib_put_nibble_2(); break;
        case 11: test_nib_put_nibble_4(); break;
        case 12:
            test_nib_put_nibble_3();
            break;
            // Rememeber to update NUM_TESTS
        }
    }

    printf("\n");
    printf("Have we proven that your Nibbler ADT has no bugs?\n");
    printf("No.  We cannot prove that a program works.\n");
    printf("We can only show when it doesn't work.\n");

    return 0;
}
