/*
 * Generates the "Encryption Code.dat" file used by the Diary program.
 *
 * Portable across:
 *   - Windows
 *   - Linux
 *   - macOS
 *
 * File format:
 *   8 bytes  : ASCII magic "DIARYEC1"
 *   512 bytes: 256 unsigned 16-bit values, little-endian
 *
 * The generated values are a permutation of 0..255
 *
 * IMPORTANT:
 * Run this program ONCE to create Encryption Code.dat.
 * Do not generate a new code if you already have diary data encrypted
 * with an existing Encryption Code.dat.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define ENCRYPTION_ARRAY_SIZE 256
#define ENCRYPTION_FILE "Encryption Code.dat"
#define ENCRYPTION_FILE_MAGIC "DIARYEC1"
#define ENCRYPTION_FILE_MAGIC_LEN 8

static int write_u16_le(FILE *fp, uint16_t value)
{
    unsigned char bytes[2];

    bytes[0] = (unsigned char)(value & 0xFFu);
    bytes[1] = (unsigned char)((value >> 8) & 0xFFu);

    return fwrite(bytes, 1, 2, fp) == 2;
}

int main(void)
{
    uint16_t encryptionCode[ENCRYPTION_ARRAY_SIZE];
    int available[ENCRYPTION_ARRAY_SIZE];
    FILE *fp;

    /*
     * Initialize the list with 0..255.
     * We then remove one randomly selected value at each position,
     * producing a permutation.
     */
    for (int i = 0; i < ENCRYPTION_ARRAY_SIZE; i++)
        available[i] = i;

    /*
     * rand() is sufficient for compatibility with the original Diary
     * algorithm. The important requirement here is that the generated
     * file has a fixed, portable representation.
     */
    srand((unsigned int)time(NULL));

    for (int i = 0; i < ENCRYPTION_ARRAY_SIZE; i++)
    {
        int remaining = ENCRYPTION_ARRAY_SIZE - i;
        int randomIndex = rand() % remaining;

        encryptionCode[i] = (uint16_t)available[randomIndex];

        for (int j = randomIndex + 1; j < remaining; j++)
            available[j - 1] = available[j];
    }

    /*
     * Never silently overwrite an existing encryption code.
     * Existing diary files depend on the existing code.
     */
    fp = fopen(ENCRYPTION_FILE, "rb");
    if (fp != NULL)
    {
        fclose(fp);

        printf("ERROR: \"%s\" already exists.\n", ENCRYPTION_FILE);
        printf("It was NOT overwritten.\n\n");
        printf("Your existing diary files may depend on this encryption code.\n");
        printf("Keep the existing file instead of generating a new one.\n");

        return 1;
    }

    fp = fopen(ENCRYPTION_FILE, "wb");
    if (fp == NULL)
    {
        perror("Could not create Encryption Code.dat");
        return 1;
    }

    /* Write file identifier/version. */
    if (fwrite(ENCRYPTION_FILE_MAGIC,
               1,
               ENCRYPTION_FILE_MAGIC_LEN,
               fp) != ENCRYPTION_FILE_MAGIC_LEN)
    {
        fclose(fp);
        remove(ENCRYPTION_FILE);
        printf("ERROR: Could not write file header.\n");
        return 1;
    }

    /* Write all 256 values as exactly two little-endian bytes each. */
    for (int i = 0; i < ENCRYPTION_ARRAY_SIZE; i++)
    {
        if (!write_u16_le(fp, encryptionCode[i]))
        {
            fclose(fp);
            remove(ENCRYPTION_FILE);
            printf("ERROR: Could not write encryption code.\n");
            return 1;
        }
    }

    fclose(fp);

    printf("Encryption Code.dat created successfully.\n");
    printf("\n");
    printf("File: %s\n", ENCRYPTION_FILE);
    printf("Format: DIARYEC1 + 256 x uint16_t (little-endian)\n");
    printf("\n");
    printf("IMPORTANT:\n");
    printf("Keep this file safe.\n");
    printf("Use this SAME file with the Diary program on Windows,\n");
    printf("Linux and macOS. Do not generate another one for the\n");
    printf("same diary data.\n");

    return 0;
}
