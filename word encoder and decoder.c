#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char base49_chars[] =
    "ABCDEFGHIJKLMOPQRUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";

typedef struct {
    char *word;
    int value;
    int frequency;
} WordEntry;

/* ================= Utility ================= */

int is_word_char(int ch) {
    return isalnum(ch);
}

int find_word(WordEntry *words, int count, const char *word) {
    for (int i = 0; i < count; i++)
        if (strcmp(words[i].word, word) == 0)
            return i;
    return -1;
}

void add_word(WordEntry **words, int *count, int *capacity, const char *word) {
    if (*count == *capacity) {
        *capacity = (*capacity == 0) ? 128 : (*capacity * 2);
        *words = realloc(*words, (*capacity) * sizeof(WordEntry));
    }

    (*words)[*count].word = strdup(word);
    (*words)[*count].value = (*count == 0) ? 0 : (*words)[*count - 1].value + 1;
    (*words)[*count].frequency = 1;

    (*count)++;
}

char *get_word_by_value(WordEntry *words, int count, int value) {
    for (int i = 0; i < count; i++)
        if (words[i].value == value)
            return words[i].word;
    return NULL;
}

/* ================= Base49 ================= */

void encode_word(int val, char *out) {
    char buffer[32];
    int i = 0;

    do {
        buffer[i++] = base49_chars[val % 49];
        val /= 49;
    } while (val > 0);

    for (int j = 0; j < i; j++)
        out[j] = buffer[i - j - 1];

    out[i] = '\0';
}

int decode_token(const char *token) {
    int val = 0;

    for (int i = 0; token[i]; i++) {
        char *ptr = strchr(base49_chars, token[i]);
        if (!ptr) return -1;
        val = val * 49 + (ptr - base49_chars);
    }
    return val;
}

/* ================= Dictionary ================= */

int read_dictionary(const char *filename, WordEntry **words, int *capacity) {
    FILE *f = fopen(filename, "rb");
    if (!f) return 0;

    int count = 0;
    *capacity = 128;
    *words = malloc(*capacity * sizeof(WordEntry));

    while (1) {
        int len;
        if (fread(&len, sizeof(int), 1, f) != 1) break;

        if (count == *capacity) {
            *capacity *= 2;
            *words = realloc(*words, *capacity * sizeof(WordEntry));
        }

        (*words)[count].word = malloc(len + 1);
        fread((*words)[count].word, 1, len, f);
        (*words)[count].word[len] = '\0';

        fread(&(*words)[count].value, sizeof(int), 1, f);
        fread(&(*words)[count].frequency, sizeof(int), 1, f);

        count++;
    }

    fclose(f);
    return count;
}

void write_dictionary(const char *filename, WordEntry *words, int count) {
    FILE *f = fopen(filename, "wb");

    for (int i = 0; i < count; i++) {
        int len = strlen(words[i].word);
        fwrite(&len, sizeof(int), 1, f);
        fwrite(words[i].word, 1, len, f);
        fwrite(&words[i].value, sizeof(int), 1, f);
        fwrite(&words[i].frequency, sizeof(int), 1, f);
    }

    fclose(f);
}

void free_dictionary(WordEntry *words, int count) {
    for (int i = 0; i < count; i++)
        free(words[i].word);
    free(words);
}

/* ================= Whitespace ================= */

void flush_ws(FILE *out, int *sp, int *nl, int *tb) {
    if (*sp > 0) {
        fprintf(out, (*sp == 1) ? "S" : "S%d", *sp);
        *sp = 0;
    }
    if (*nl > 0) {
        fprintf(out, (*nl == 1) ? "N" : "N%d", *nl);
        *nl = 0;
    }
    if (*tb > 0) {
        fprintf(out, (*tb == 1) ? "T" : "T%d", *tb);
        *tb = 0;
    }
}

/* ================= Encode ================= */

int encode(const char *inName, const char *outName) {
    FILE *in = fopen(inName, "r");
    FILE *out = fopen(outName, "w");

    WordEntry *words = NULL;
    int capacity = 0;
    int wordCount = read_dictionary("dictionary.bin", &words, &capacity);

    char *buffer = NULL;
    int buf_size = 0, buf_cap = 0;

    int ch;
    int sp = 0, nl = 0, tb = 0;

    while ((ch = fgetc(in)) != EOF) {
        if (is_word_char(ch)) {

            flush_ws(out, &sp, &nl, &tb);

            buf_size = 0;

            do {
                if (buf_size + 1 >= buf_cap) {
                    buf_cap = buf_cap ? buf_cap * 2 : 32;
                    buffer = realloc(buffer, buf_cap);
                }
                buffer[buf_size++] = ch;
                ch = fgetc(in);
            } while (is_word_char(ch));

            buffer[buf_size] = '\0';

            int idx = find_word(words, wordCount, buffer);

            if (idx == -1) {
                add_word(&words, &wordCount, &capacity, buffer);
                idx = wordCount - 1;
            } else {
                words[idx].frequency++;
            }

            char enc[32];
            encode_word(words[idx].value, enc);
            fputs(enc, out);

            if (ch != EOF)
                ungetc(ch, in);
        }
        else if (ch == ' ') sp++;
        else if (ch == '\n') nl++;
        else if (ch == '\t') tb++;
        else {
            flush_ws(out, &sp, &nl, &tb);
            fputc(ch, out);
        }
    }

    flush_ws(out, &sp, &nl, &tb);

    write_dictionary("dictionary.bin", words, wordCount);

    free(buffer);
    free_dictionary(words, wordCount);

    fclose(in);
    fclose(out);

    printf("Encoded.\n");
    return 0;
}

/* ================= Decode ================= */

int decode(const char *inName, const char *outName) {
    FILE *in = fopen(inName, "r");
    FILE *out = fopen(outName, "w");

    WordEntry *words = NULL;
    int capacity = 0;
    int wordCount = read_dictionary("dictionary.bin", &words, &capacity);

    int ch;

    while ((ch = fgetc(in)) != EOF) {

        if (ch == 'S' || ch == 'N' || ch == 'T') {
            int num = 0, next = fgetc(in);

            if (!isdigit(next)) {
                num = 1;
                if (next != EOF) ungetc(next, in);
            } else {
                num = next - '0';
                while (isdigit(next = fgetc(in)))
                    num = num * 10 + (next - '0');
                if (next != EOF) ungetc(next, in);
            }

            char outc = (ch == 'S') ? ' ' : (ch == 'N') ? '\n' : '\t';
            for (int i = 0; i < num; i++)
                fputc(outc, out);
        }

        else if (strchr(base49_chars, ch)) {
            char token[32];
            int i = 0;

            token[i++] = ch;

            while (1) {
                int next = fgetc(in);
                if (next == EOF || !strchr(base49_chars, next)) {
                    if (next != EOF) ungetc(next, in);
                    break;
                }
                token[i++] = next;
            }

            token[i] = '\0';

            int val = decode_token(token);
            char *word = get_word_by_value(words, wordCount, val);

            if (word)
                fputs(word, out);
            else
                fprintf(out, "[%s]", token);
        }

        else {
            fputc(ch, out);
        }
    }

    free_dictionary(words, wordCount);

    fclose(in);
    fclose(out);

    printf("Decoded.\n");
    return 0;
}

/* ================= Main ================= */

int main() {
    int choice;
    char in[256], out[256];

    do {
        system("cls");
        printf("1. Encode\n2. Decode\n3. Exit\nChoice: ");
        scanf("%d", &choice);
        getchar();

        if (choice == 1) {
            printf("Input file: ");
            fgets(in, 256, stdin);
            in[strcspn(in, "\n")] = 0;

            printf("Output file: ");
            fgets(out, 256, stdin);
            out[strcspn(out, "\n")] = 0;

            encode(in, out);
        }

        else if (choice == 2) {
            printf("Input file: ");
            fgets(in, 256, stdin);
            in[strcspn(in, "\n")] = 0;

            printf("Output file: ");
            fgets(out, 256, stdin);
            out[strcspn(out, "\n")] = 0;

            decode(in, out);
        }

        system("pause");

    } while (choice != 3);

    return 0;
}
