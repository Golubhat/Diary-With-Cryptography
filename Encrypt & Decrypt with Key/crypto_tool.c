#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <limits.h>
#include <windows.h>

#define KEY_SIZE 256
#define KEY_FILENAME "key.dat"
#define MAGIC_HEADER "ENC1"
#define MAGIC_LEN 4
#define CHUNK_SIZE (64 * 1024) /* 64 KB */

char *get_exec_dir(char *buffer, size_t size)
{
#ifdef _WIN32
    GetModuleFileName(NULL, buffer, size);
    char *last = strrchr(buffer, '\\');
#else
    ssize_t len = readlink("/proc/self/exe", buffer, size - 1);
    if (len != -1)
        buffer[len] = '\0';
    char *last = strrchr(buffer, '/');
#endif
    if (last)
        *last = '\0';
    return buffer;
}
char EXE_DIR[PATH_MAX];

static int create_key(void)
{
    char keypath[PATH_MAX];
    snprintf(keypath, sizeof(keypath), "%s/%s", EXE_DIR, KEY_FILENAME);
    FILE *kf = fopen(keypath, "wb");
    if (!kf)
    {
        perror("Failed to create key file");
        return 0;
    }

    /* Seed RNG and write KEY_SIZE random bytes (0-255) */
    srand((unsigned)time(NULL));
    for (size_t i = 0; i < KEY_SIZE; ++i)
    {
        unsigned r = rand() & 0xFF; /* 0..255 */
        uint8_t b = (uint8_t)r;
        if (fwrite(&b, 1, 1, kf) != 1)
        {
            perror("Failed writing key");
            fclose(kf);
            return 0;
        }
    }

    fclose(kf);
    printf("Key generated: %s (%d bytes)\n", KEY_FILENAME, KEY_SIZE);
    return 1;
}

static int load_key(uint8_t *key)
{
    char keypath[PATH_MAX];
    snprintf(keypath, sizeof(keypath), "%s/%s", EXE_DIR, KEY_FILENAME);
    FILE *kf = fopen(keypath, "rb");
    if (!kf)
    {
        fprintf(stderr, "Key file '%s' not found. Generate it first.\n", KEY_FILENAME);
        return 0;
    }
    size_t n = fread(key, 1, KEY_SIZE, kf);
    fclose(kf);
    if (n != KEY_SIZE)
    {
        fprintf(stderr, "Invalid key file size (expected %d bytes).\n", KEY_SIZE);
        return 0;
    }
    return 1;
}

static int file_exists(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f)
        return 0;
    fclose(f);
    return 1;
}

static int encrypt_file(void)
{
    char inpath[1024];
    printf("Enter path to file to encrypt: ");
    if (!fgets(inpath, sizeof(inpath), stdin))
        return 0;
    /* trim newline */
    inpath[strcspn(inpath, "\r\n")] = '\0';
    if (inpath[0] == '\0')
        return 0;

    if (!file_exists(inpath))
    {
        fprintf(stderr, "File not found: %s\n", inpath);
        return 0;
    }

    uint8_t key[KEY_SIZE];
    if (!load_key(key))
        return 0;

    FILE *fin = fopen(inpath, "rb");
    if (!fin)
    {
        perror("Failed to open input file");
        return 0;
    }

    /* check magic header */
    char header[MAGIC_LEN];
    size_t got = fread(header, 1, MAGIC_LEN, fin);
    if (got == MAGIC_LEN && memcmp(header, MAGIC_HEADER, MAGIC_LEN) == 0)
    {
        fprintf(stderr, "File already encrypted (magic header found). Aborting.\n");
        fclose(fin);
        return 0;
    }
    /* rewind */
    fseek(fin, 0, SEEK_SET);

    /* prepare output path */
    char outpath[1150];
    snprintf(outpath, sizeof(outpath), "%s/%s.enc", EXE_DIR, strrchr(inpath, '/') ? strrchr(inpath, '/') + 1 : inpath);
    FILE *fout = fopen(outpath, "wb");
    if (!fout)
    {
        perror("Failed to create output file");
        fclose(fin);
        return 0;
    }

    /* write magic header */
    if (fwrite(MAGIC_HEADER, 1, MAGIC_LEN, fout) != MAGIC_LEN)
    {
        perror("Failed writing header");
        fclose(fin);
        fclose(fout);
        return 0;
    }

    uint8_t *buf = (uint8_t *)malloc(CHUNK_SIZE);
    uint8_t *out = (uint8_t *)malloc(CHUNK_SIZE);
    if (!buf || !out)
    {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(fin);
        fclose(fout);
        free(buf);
        free(out);
        return 0;
    }

    size_t pos = 0;
    size_t total = 0;
    while (1)
    {
        size_t r = fread(buf, 1, CHUNK_SIZE, fin);
        if (r == 0)
            break;
        for (size_t i = 0; i < r; ++i)
        {
            out[i] = (uint8_t)((buf[i] + key[(pos + i) % KEY_SIZE]) & 0xFF);
        }
        if (fwrite(out, 1, r, fout) != r)
        {
            perror("Write error");
            free(buf);
            free(out);
            fclose(fin);
            fclose(fout);
            return 0;
        }
        pos += r;
        total += r;
    }

    free(buf);
    free(out);
    fclose(fin);
    fclose(fout);

    printf("Encrypted -> (%zu bytes processed)\n", total);

    return 1;
}

static int decrypt_file(void)
{
    char inpath[1024];
    printf("Enter path to .enc file to decrypt: ");
    if (!fgets(inpath, sizeof(inpath), stdin))
        return 0;
    inpath[strcspn(inpath, "\r\n")] = '\0';
    if (inpath[0] == '\0')
        return 0;

    if (!file_exists(inpath))
    {
        fprintf(stderr, "File not found: %s\n", inpath);
        return 0;
    }

    uint8_t key[KEY_SIZE];
    if (!load_key(key))
        return 0;

    FILE *fin = fopen(inpath, "rb");
    if (!fin)
    {
        perror("Failed to open input file");
        return 0;
    }

    char header[MAGIC_LEN];
    size_t got = fread(header, 1, MAGIC_LEN, fin);
    if (got != MAGIC_LEN || memcmp(header, MAGIC_HEADER, MAGIC_LEN) != 0)
    {
        fprintf(stderr, "Invalid or missing encryption header. Aborting.\n");
        fclose(fin);
        return 0;
    }

    /* Build output path */
    char name[512];

    /* Extract base filename from input path */
    const char *base =
        strrchr(inpath, '/') ? strrchr(inpath, '/') + 1 : strrchr(inpath, '\\') ? strrchr(inpath, '\\') + 1
                                                                                : inpath;

    /* Copy base filename */
    snprintf(name, sizeof(name), "%s", base);

    /* Remove ".enc" extension */
    size_t nlen = strlen(name);
    if (nlen > 4 && strcmp(name + nlen - 4, ".enc") == 0)
        name[nlen - 4] = '\0';

    /* Final output: EXE_DIR + base filename (without .enc) */
    char outpath[1024];
    snprintf(outpath, sizeof(outpath), "%s/%s", EXE_DIR, name);

    FILE *fout = fopen(outpath, "wb");
    if (!fout)
    {
        perror("Failed to open output file");
        fclose(fin);
        return 0;
    }

    uint8_t *buf = (uint8_t *)malloc(CHUNK_SIZE);
    uint8_t *out = (uint8_t *)malloc(CHUNK_SIZE);
    if (!buf || !out)
    {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(fin);
        fclose(fout);
        free(buf);
        free(out);
        return 0;
    }

    size_t pos = 0;
    size_t total = 0;
    while (1)
    {
        size_t r = fread(buf, 1, CHUNK_SIZE, fin);
        if (r == 0)
            break;
        for (size_t i = 0; i < r; ++i)
        {
            out[i] = (uint8_t)((buf[i] - key[(pos + i) % KEY_SIZE] + 256) & 0xFF);
        }
        if (fwrite(out, 1, r, fout) != r)
        {
            perror("Write error");
            free(buf);
            free(out);
            fclose(fin);
            fclose(fout);
            return 0;
        }
        pos += r;
        total += r;
    }

    free(buf);
    free(out);
    fclose(fin);
    fclose(fout);

    printf("Decrypted -> (%zu bytes processed)\n", total);

    return 1;
}

int main(void)
{
    get_exec_dir(EXE_DIR, sizeof(EXE_DIR));
    while (1)
    {
        system("cls");
        printf("1) Generate key\n");
        printf("2) Encrypt file\n");
        printf("3) Decrypt file\n");
        printf("4) Exit\n");
        printf("Choose option: ");

        char opt[8];
        if (!fgets(opt, sizeof(opt), stdin))
            break;

        if (opt[0] == '1')
            create_key();
        else if (opt[0] == '2')
            encrypt_file();
        else if (opt[0] == '3')
            decrypt_file();
        else if (opt[0] == '4')
            break;
        else
            printf("Invalid choice.\n");

        system("pause");
    }

    printf("Goodbye.\n");
    return 0;
}
