#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define KEY_SIZE 256
#define KEY_FILENAME "key.dat"
#define HEADER "ENC1"
#define HEADER_SIZE 4

// Progress bar utility
void show_progress(long current, long total, const char *label) {
    int bar_width = 50;
    float percent = (float) current / total;
    int filled = (int)(percent * bar_width);

    printf("\r%s [", label);
    for (int i = 0; i < bar_width; ++i)
        printf(i < filled ? "#" : "-");
    printf("] %3.0f%%", percent * 100);
    fflush(stdout);
}

// Check if key.dat exists
int key_exists() {
    FILE *f = fopen(KEY_FILENAME, "rb");
    if (f) {
        fclose(f);
        return 1;
    }
    return 0;
}

// Generate key.dat with random unique values
void generate_key() {
    if (key_exists()) {
        printf("Key file '%s' already exists. Aborting key generation.\n", KEY_FILENAME);
        return;
    }

    unsigned char key[KEY_SIZE];
    for (int i = 0; i < KEY_SIZE; i++) key[i] = i;

    srand((unsigned) time(NULL));
    for (int i = KEY_SIZE - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        unsigned char temp = key[i];
        key[i] = key[j];
        key[j] = temp;
    }

    FILE *file = fopen(KEY_FILENAME, "wb");
    if (!file) {
        perror("Key creation failed");
        return;
    }
    fwrite(key, 1, KEY_SIZE, file);
    fclose(file);
    printf("Key file '%s' successfully generated.\n", KEY_FILENAME);
}

// Load key into array
int load_key(unsigned char *key) {
    FILE *file = fopen(KEY_FILENAME, "rb");
    if (!file) return 0;
    fread(key, 1, KEY_SIZE, file);
    fclose(file);
    return 1;
}

// Check for encryption header
int has_encryption_header(FILE *file) {
    char buf[HEADER_SIZE + 1] = {0};
    fread(buf, 1, HEADER_SIZE, file);
    rewind(file);
    return strcmp(buf, HEADER) == 0;
}

// Encrypt file
void encrypt_file(unsigned char *key) {
    char input_name[200];
    printf("Enter file to encrypt: ");
    scanf("%s", input_name);

    FILE *in = fopen(input_name, "rb");
    if (!in) {
        perror("Error opening input");
        return;
    }

    if (has_encryption_header(in)) {
        printf("File is already encrypted.\n");
        fclose(in);
        return;
    }

    char output_name[250];
    snprintf(output_name, sizeof(output_name), "%s.enc", input_name);

    FILE *out = fopen(output_name, "wb");
    if (!out) {
        perror("Error opening output");
        fclose(in);
        return;
    }

    fseek(in, 0, SEEK_END);
    long total_size = ftell(in);
    rewind(in);

    fwrite(HEADER, 1, HEADER_SIZE, out);

    int ch;
    long pos = 0;
    while ((ch = fgetc(in)) != EOF) {
        unsigned char enc = (ch + key[pos % KEY_SIZE]) % 256;
        fputc(enc, out);
        pos++;
        if (pos % 1000 == 0 || pos == total_size)
            show_progress(pos, total_size, "Encrypting ");
    }

    printf("\nEncrypted file created: %s\n", output_name);
    fclose(in);
    fclose(out);
    remove(input_name);
}

// Decrypt file
void decrypt_file(unsigned char *key) {
    char input_name[200];
    printf("Enter file to decrypt (.enc): ");
    scanf("%s", input_name);

    FILE *in = fopen(input_name, "rb");
    if (!in) {
        perror("Error opening encrypted file");
        return;
    }

    if (!has_encryption_header(in)) {
        printf("File is not encrypted with this program.\n");
        fclose(in);
        return;
    }

    char output_name[250];
    strncpy(output_name, input_name, sizeof(output_name));
    char *dot = strstr(output_name, ".enc");
    if (dot) *dot = '\0';
    else strcat(output_name, ".dec");

    FILE *out = fopen(output_name, "wb");
    if (!out) {
        perror("Error opening output");
        fclose(in);
        return;
    }

    fseek(in, 0, SEEK_END);
    long total_size = ftell(in) - HEADER_SIZE;
    fseek(in, HEADER_SIZE, SEEK_SET);

    int ch;
    long pos = 0;
    while ((ch = fgetc(in)) != EOF) {
        unsigned char dec = (ch - key[pos % KEY_SIZE] + 256) % 256;
        fputc(dec, out);
        pos++;
        if (pos % 1000 == 0 || pos == total_size)
            show_progress(pos, total_size, "Decrypting ");
    }

    printf("\nDecrypted file created: %s\n", output_name);
    fclose(in);
    fclose(out);
    remove(input_name);
}

// Main menu
int main() {
    int choice;
    printf("=============================\n");
    printf("      FILE CRYPTO TOOL\n");
    printf("=============================\n");
    printf("1. Generate Key\n");
    printf("2. Encrypt File\n");
    printf("3. Decrypt File\n");
    printf("Choose an option (1/2/3): ");
    scanf("%d", &choice);

    if (choice == 1) {
        generate_key();
    } else {
        if (!key_exists()) {
            printf("Key file '%s' not found. Please generate it first.\n", KEY_FILENAME);
            return 1;
        }

        unsigned char key[KEY_SIZE];
        if (!load_key(key)) {
            printf("Failed to load key.\n");
            return 1;
        }

        if (choice == 2) encrypt_file(key);
        else if (choice == 3) decrypt_file(key);
        else printf("Invalid choice.\n");
    }

    return 0;
}
