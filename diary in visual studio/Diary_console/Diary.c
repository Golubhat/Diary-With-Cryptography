#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <errno.h>

#ifdef _WIN32
#include <conio.h>
#include <direct.h>
#include <windows.h>
#else
#include <termios.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#define ENCRYPTION_ARRAY_SIZE 256
#define MAX_USERNAME_LEN 20
#define MAX_PASSWORD_LEN 20
#define ENTER_KEY 13
#define BACKSPACE_KEY 8
#define MAX_TITLE_LEN 50
#define MAX_TEXT_LEN 10000
#define ID_LENGTH 5

/* Cross-platform console helpers */
static int read_key(void)
{
#ifdef _WIN32
    return _getch();
#else
    struct termios oldt, newt;
    int c;

    if (tcgetattr(STDIN_FILENO, &oldt) != 0)
        return getchar();

    newt = oldt;
    newt.c_lflag &= (tcflag_t)~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSANOW, &newt) != 0)
        return getchar();

    c = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    /* Normalize Unix terminal codes to the values used by the program. */
    if (c == '\n' || c == '\r')
        return ENTER_KEY;
    if (c == 127)
        return BACKSPACE_KEY;

    return c;
#endif
}

static void clear_screen(void)
{
#ifdef _WIN32
    system("cls");
#else
    printf("\033[2J\033[H");
    fflush(stdout);
#endif
}

static void pause_console(void)
{
#ifdef _WIN32
    printf("Press any key to continue...");
    fflush(stdout);
    (void)_getch();
    putchar('\n');
#else
    printf("Press Enter to continue...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
#endif
}

static void make_backup_directory(void)
{
#ifdef _WIN32
    if (_mkdir("Backup") != 0 && errno != EEXIST)
        return;
#else
    if (mkdir("Backup", 0755) != 0 && errno != EEXIST)
        return;
#endif
}

static void set_console_colour(char background, char foreground)
{
#ifdef _WIN32
    char colorCommand[9] = "color ";
    colorCommand[6] = background;
    colorCommand[7] = foreground;
    colorCommand[8] = '\0';
    system(colorCommand);
#else
    /*
       ANSI 16-colour approximation.
       Background/foreground values use the same 0-F convention
       as the original Windows program.
    */
    int bg = (background >= '0' && background <= '9') ? background - '0' : background - 'A' + 10;
    int fg = (foreground >= '0' && foreground <= '9') ? foreground - '0' : foreground - 'A' + 10;

    if (bg < 0 || bg > 15 || fg < 0 || fg > 15)
        return;

    {
        int fg_code = (fg & 8) ? 90 + (fg & 7) : 30 + fg;
        int bg_code = (bg & 8) ? 100 + (bg & 7) : 40 + bg;
        printf("\033[%d;%dm", bg_code, fg_code);
        fflush(stdout);
    }
#endif
}

/* Decisions used by Encryption/Backup functions */
enum Decision
{
    ENCRYPT = 1,
    DECRYPT = -1,
    MAKE_THE_BACKUP_FILE,
    REMOVE_THE_BACKUP_FILE
};

/* Persistent program settings */
struct DiarySettings
{
    int no_of_user;
    char color[2];
} settings;

/* In-memory user data */
struct DiaryUser
{
    char username[MAX_USERNAME_LEN + 1];
    char password[MAX_PASSWORD_LEN + 1];
    int amount_of_content;
} User;

short int encryptSettings[ENCRYPTION_ARRAY_SIZE];
short int encryptionCode[ENCRYPTION_ARRAY_SIZE];
FILE* fptr;
int i, len;
char ch, choice;
char* title, * text, * new;
int position;

/* -------------------------
   Utility helper functions
   ------------------------- */

void clear_encryptionCode(short int arr[])
{
    for (short int k = 0; k < ENCRYPTION_ARRAY_SIZE; k++)
        arr[k] = 0;
}

void delay(unsigned int mseconds)
{
#ifdef _WIN32
    Sleep(mseconds);
#else
    struct timespec ts;
    ts.tv_sec = mseconds / 1000;
    ts.tv_nsec = (long)(mseconds % 1000) * 1000000L;
    nanosleep(&ts, NULL);
#endif
}

/* Read password from console with '*' echo and backspace support */
void setPassword(char thePassword[])
{
    i = 0;
    while ((ch = read_key()) != ENTER_KEY)
    {
        if (ch == BACKSPACE_KEY)
        {
            if (i > 0)
            {
                putchar('\b');
                putchar(' ');
                putchar('\b');
                i--;
            }
        }
        else if (i < MAX_PASSWORD_LEN)
        {
            thePassword[i++] = ch;
            putchar('*');
        }
    }
    thePassword[i] = '\0';
    printf("\n");
}

/* Copy 'count' bytes from srcStart in src to dstStart in dst (byte-by-byte) */
void copy_bytes(FILE* src, FILE* dst, int srcStart, int dstStart, int count)
{
    for (int k = 0; k < count; k++)
    {
        fseek(src, srcStart + k, SEEK_SET);
        fread(&ch, sizeof(char), 1, src);
        fseek(dst, dstStart + k, SEEK_SET);
        fwrite(&ch, sizeof(char), 1, dst);
    }
}

/* Copy entire source file into destination (from 0..length-1) */
void copy_file(FILE* src, FILE* dst)
{
    fseek(src, 0, SEEK_END);
    int length = ftell(src);
    if (length > 0)
        copy_bytes(src, dst, 0, 0, length);
}

/* -------------------------
   Encryption & Backup
   ------------------------- */

void Encryption(char fileName[], enum Decision direction, short int encrypt[])
{
    fptr = fopen(fileName, "rb+");
    if (!fptr)
        return;
    fseek(fptr, 0, SEEK_END);
    int length = ftell(fptr);
    for (i = 0; i < length; i++)
    {
        fseek(fptr, i, SEEK_SET);
        fread(&ch, sizeof(char), 1, fptr);
        ch += direction * encrypt[i % ENCRYPTION_ARRAY_SIZE];
        fseek(fptr, i, SEEK_SET);
        fwrite(&ch, sizeof(char), 1, fptr);
    }
    fclose(fptr);
}

void Backup(char fileName[], enum Decision choice, short int encrypt[])
{
    char fileInBackup[64] = "Backup/";
    strcat(fileInBackup, fileName);

    if (choice == MAKE_THE_BACKUP_FILE)
    {
        FILE* write = fopen(fileInBackup, "wb");
        FILE* read = fopen(fileName, "rb");
        if (!read || !write)
        {
            if (read)
                fclose(read);
            if (write)
                fclose(write);
            return;
        }
        fseek(read, 0, SEEK_END);
        int length = ftell(read);
        if (length > 0)
            copy_bytes(read, write, 0, 0, length);
        fclose(write);
        fclose(read);

        /* Encrypt the backup file */
        Encryption(fileInBackup, ENCRYPT, encrypt);
    }
    else if (choice == REMOVE_THE_BACKUP_FILE)
    {
        remove(fileInBackup);
    }
}

/* -------------------------
   Initialization & Settings
   ------------------------- */

void initialize()
{
    /* Read global encryption settings */
    fptr = fopen("Encryption Code.dat", "rb");
    fread(&encryptSettings, sizeof(encryptSettings), 1, fptr);
    fclose(fptr);

    /* Ensure backup directory exists */
    make_backup_directory();

    /* Load settings.dat if exists; otherwise create defaults */
    fptr = fopen("settings.dat", "rb");
    if (!fptr)
    {
        settings.no_of_user = 0;
        settings.color[0] = '0';
        settings.color[1] = '7';
        fptr = fopen("settings.dat", "wb");
        if (fptr)
        {
            fwrite(&settings.no_of_user, sizeof(settings.no_of_user), 1, fptr);
            fwrite(&settings.color, sizeof(settings.color), 1, fptr);
            fclose(fptr);
        }
    }
    else
    {
        fclose(fptr);
        /* settings.dat was encrypted on disk: decrypt then read */
        Encryption("settings.dat", DECRYPT, encryptSettings);
        fptr = fopen("settings.dat", "rb");
        if (fptr)
        {
            fread(&settings.no_of_user, sizeof(settings.no_of_user), 1, fptr);
            fread(&settings.color, sizeof(settings.color), 1, fptr);
            fclose(fptr);
        }
    }

    Backup("settings.dat", MAKE_THE_BACKUP_FILE, encryptSettings);

    set_console_colour(settings.color[0], settings.color[1]);

    clear_encryptionCode(encryptionCode);
}

/* Change console foreground/background color and persist it */
void changecolour()
{
    set_console_colour(settings.color[0], settings.color[1]);

    fptr = fopen("settings.dat", "rb+");
    if (fptr)
    {
        fseek(fptr, sizeof(settings.no_of_user), SEEK_SET);
        fwrite(&settings.color, sizeof(settings.color), 1, fptr);
        fclose(fptr);
    }

    Backup("settings.dat", MAKE_THE_BACKUP_FILE, encryptSettings);
}

/* -------------------------
   User Management
   ------------------------- */

int ID_Generation()
{
    int flag, newID;

    srand(time(NULL));
    do
    {
        flag = 1;
        newID = rand() % 90000 + 10000; /* 5-digit ID */

        fptr = fopen("settings.dat", "rb");
        if (!fptr)
            break;
        fread(&settings.no_of_user, sizeof(int), 1, fptr);
        fseek(fptr, sizeof(settings.color), SEEK_CUR);

        for (i = 0; i < settings.no_of_user; i++)
        {
            int checkID;
            fread(&checkID, sizeof(checkID), 1, fptr);
            if (newID == checkID)
            {
                flag = 0;
                break;
            }
            fseek(fptr, sizeof(encryptionCode), SEEK_CUR);
        }
        fclose(fptr);
    } while (!flag);

    /* Build a random permutation for encryptionCode */
    short int oneTo256[ENCRYPTION_ARRAY_SIZE];
    for (short int k = 0; k < ENCRYPTION_ARRAY_SIZE; k++)
        oneTo256[k] = k;

    for (short int k = 0; k < ENCRYPTION_ARRAY_SIZE; k++)
    {
        int random = rand() % (256 - k);
        encryptionCode[k] = oneTo256[random];
        for (short int j = random + 1; j < 256 - k; j++)
            oneTo256[j - 1] = oneTo256[j];
    }

    /* Append new user ID and encryption code into settings.dat */
    settings.no_of_user++;
    fptr = fopen("settings.dat", "rb+");
    if (!fptr)
        return newID;
    fwrite(&settings.no_of_user, sizeof(settings.no_of_user), 1, fptr);
    fseek(fptr, sizeof(settings.color), SEEK_CUR);

    for (i = 1; i < settings.no_of_user; i++)
        fseek(fptr, sizeof(newID) + sizeof(encryptionCode), SEEK_CUR);

    fwrite(&newID, sizeof(newID), 1, fptr);
    fwrite(&encryptionCode, sizeof(encryptionCode), 1, fptr);
    fclose(fptr);

    Backup("settings.dat", MAKE_THE_BACKUP_FILE, encryptSettings);

    printf("Your ID is %d\n", newID);
    pause_console();

    return newID;
}

/* Delete user record and corresponding diary file */
void DeleteUser(char DiaryUserFileName[])
{
    /* Remove the actual diary file first */
    remove(DiaryUserFileName);

    /* Extract desiredID from the filename (first 5 chars) */
    int desiredID = 0;
    for (i = 0; i < 5; i++)
    {
        desiredID *= 10;
        desiredID += DiaryUserFileName[i] - '0';
    }

    /* Decrement user count and update settings.dat structure by removing user record */
    settings.no_of_user--;
    fptr = fopen("settings.dat", "rb+");
    if (!fptr)
        return;

    fwrite(&settings.no_of_user, sizeof(settings.no_of_user), 1, fptr);
    fseek(fptr, sizeof(settings.color), SEEK_CUR);

    int start = 0, end = 0, checkID;
    /* Find the start position of the record to delete */
    do
    {
        start = ftell(fptr);
        fread(&checkID, sizeof(checkID), 1, fptr);
        fseek(fptr, sizeof(encryptionCode), SEEK_CUR);
    } while (desiredID != checkID);

    /* Move to end to get file size */
    fseek(fptr, 0, SEEK_END);
    end = ftell(fptr);

    /* Create temp and copy bytes before start and after the record to remove */
    FILE* ptr = fopen("temp.dat", "wb");
    if (!ptr)
    {
        fclose(fptr);
        return;
    }

    /* Copy head [0..start-1] */
    if (start > 0)
        copy_bytes(fptr, ptr, 0, 0, start);

    /* Copy tail starting from (start + sizeof(desiredID) + sizeof(encryptionCode)) to new position start */
    int tailSrc = start + sizeof(desiredID) + sizeof(encryptionCode);
    int tailCount = end - tailSrc;
    if (tailCount > 0)
        copy_bytes(fptr, ptr, tailSrc, start, tailCount);

    fclose(ptr);
    fclose(fptr);

    remove("settings.dat");
    rename("temp.dat", "settings.dat");

    Backup("settings.dat", MAKE_THE_BACKUP_FILE, encryptSettings);
}

/* -------------------------
   Content Management
   ------------------------- */

   /* Display list of note titles (only titles are read) */
void Display(char DiaryUserFileName[])
{
    clear_screen();
    FILE* userfile = fopen(DiaryUserFileName, "rb");
    if (!userfile)
        return;

    fseek(userfile, sizeof(User.username) + sizeof(User.password), SEEK_SET);
    fread(&User.amount_of_content, sizeof(User.amount_of_content), 1, userfile);

    for (i = 0; i < User.amount_of_content; i++)
    {
        fread(&len, sizeof(int), 1, userfile);
        printf("\n%d) ", i + 1);
        for (int j = 0; j < len; j++)
        {
            fread(&ch, sizeof(char), 1, userfile);
            printf("%c", ch);
        }
        /* read next length (text length) and skip it */
        fread(&len, sizeof(int), 1, userfile);
        fseek(userfile, len, SEEK_CUR);
    }

    fclose(userfile);
    printf("\n\n");
}

/* Insert a title+text at position (shifts subsequent data forward) */
void InsertContent(char DiaryUserFileName[], int titleLen, int textLen)
{
    fptr = fopen(DiaryUserFileName, "rb+");
    if (!fptr)
        return;

    /* Update stored amount_of_content at fixed offset */
    fseek(fptr, sizeof(User.username) + sizeof(User.password), SEEK_SET);
    fwrite(&User.amount_of_content, sizeof(int), 1, fptr);

    /* Seek to beginning of content area */
    fseek(fptr, sizeof(User.username) + sizeof(User.password) + sizeof(User.amount_of_content), SEEK_SET);

    /* Skip to insertion point */
    for (i = 1; i < position; i++)
    {
        fread(&len, sizeof(int), 1, fptr); /* title length */
        fseek(fptr, len, SEEK_CUR);
        fread(&len, sizeof(int), 1, fptr); /* text length */
        fseek(fptr, len, SEEK_CUR);
    }

    int start = ftell(fptr);

    /* Determine file end */
    fseek(fptr, 0, SEEK_END);
    int end = ftell(fptr);

    /* Compute size of record to insert */
    int contentLen = sizeof(int) + titleLen + sizeof(int) + textLen;

    /* Shift tail forward by contentLen bytes:
       copy tail [start..end-1] to [start+contentLen .. start+contentLen+(end-start)-1] using temp file approach */
    FILE* tmp = fopen("temp.dat", "wb");
    if (!tmp)
    {
        fclose(fptr);
        return;
    }

    /* Copy head [0..start-1] to tmp */
    if (start > 0)
        copy_bytes(fptr, tmp, 0, 0, start);

    /* Copy tail [start..end-1] to tmp at offset start+contentLen */
    int tailCount = end - start;
    if (tailCount > 0)
        copy_bytes(fptr, tmp, start, start + contentLen, tailCount);

    fclose(tmp);
    fclose(fptr);

    /* Replace original file with tmp */
    remove(DiaryUserFileName);
    rename("temp.dat", DiaryUserFileName);

    /* Reopen file and write new record at 'start' */
    fptr = fopen(DiaryUserFileName, "rb+");
    if (!fptr)
        return;
    fseek(fptr, start, SEEK_SET);

    /* Write title length and title bytes */
    fwrite(&titleLen, sizeof(int), 1, fptr);
    for (i = 0; i < titleLen; i++)
        fwrite(title + i, sizeof(char), 1, fptr);

    /* Write text length and text bytes */
    fwrite(&textLen, sizeof(int), 1, fptr);
    for (i = 0; i < textLen; i++)
        fwrite(text + i, sizeof(char), 1, fptr);

    fclose(fptr);
}

/* Append new text to an existing note (increases text length and shifts file) */
void AppendInContent(char DiaryUserFileName[])
{
    fptr = fopen(DiaryUserFileName, "rb+");
    if (!fptr)
        return;

    /* Seek to content area and locate the chosen record */
    fseek(fptr, sizeof(User.username) + sizeof(User.password) + sizeof(User.amount_of_content), SEEK_SET);
    for (i = 1; i < position; i++)
    {
        fread(&len, sizeof(int), 1, fptr); /* title length */
        fseek(fptr, len, SEEK_CUR);
        fread(&len, sizeof(int), 1, fptr); /* text length */
        fseek(fptr, len, SEEK_CUR);
    }

    /* Now at the chosen record: read title length and skip title */
    fread(&len, sizeof(int), 1, fptr);
    fseek(fptr, len, SEEK_CUR);

    /* Read text length & compute new length */
    int textLenPos = ftell(fptr); /* position of text length */
    fread(&len, sizeof(int), 1, fptr);
    int oldTextLen = len;
    int newLenTotal = strlen(new) + oldTextLen;

    /* Update the stored text length at textLenPos */
    fseek(fptr, textLenPos, SEEK_SET);
    fwrite(&newLenTotal, sizeof(int), 1, fptr);

    /* Move to end of old text to append new text — but must shift subsequent bytes forward by strlen(new) */
    fseek(fptr, textLenPos + sizeof(int) + oldTextLen, SEEK_SET);
    int start = ftell(fptr); /* position after old text (start of tail) */
    fseek(fptr, 0, SEEK_END);
    int end = ftell(fptr);

    int newTextLen = strlen(new);

    /* Use temp file to shift tail forward by newTextLen bytes */
    FILE* tmp = fopen("temp.dat", "wb");
    if (!tmp)
    {
        fclose(fptr);
        return;
    }

    /* Copy head [0..start-1] */
    if (start > 0)
        copy_bytes(fptr, tmp, 0, 0, start);

    /* Copy tail [start..end-1] to tmp at offset start+newTextLen */
    int tailCount = end - start;
    if (tailCount > 0)
        copy_bytes(fptr, tmp, start, start + newTextLen, tailCount);

    fclose(tmp);
    fclose(fptr);

    remove(DiaryUserFileName);
    rename("temp.dat", DiaryUserFileName);

    /* Write appended text at position 'start' */
    fptr = fopen(DiaryUserFileName, "rb+");
    if (!fptr)
        return;
    fseek(fptr, start, SEEK_SET);
    for (i = 0; i < newTextLen; i++)
        fwrite(new + i, sizeof(char), 1, fptr);

    fclose(fptr);
}

/* Read & print chosen note (title and text) */
void ReadContent(char DiaryUserFileName[])
{
    fptr = fopen(DiaryUserFileName, "rb");
    if (!fptr)
        return;

    fseek(fptr, sizeof(User.username) + sizeof(User.password) + sizeof(User.amount_of_content), SEEK_SET);
    for (i = 1; i < position; i++)
    {
        fread(&len, sizeof(int), 1, fptr); /* title len */
        fseek(fptr, len, SEEK_CUR);
        fread(&len, sizeof(int), 1, fptr); /* text len */
        fseek(fptr, len, SEEK_CUR);
    }

    /* Read title */
    fread(&len, sizeof(int), 1, fptr);
    printf("\n\t\t\t");
    for (i = 0; i < len; i++)
    {
        fread(&ch, sizeof(char), 1, fptr);
        printf("%c", ch);
    }
    printf("\n\n");

    /* Read text */
    fread(&len, sizeof(int), 1, fptr);
    for (i = 0; i < len; i++)
    {
        fread(&ch, sizeof(char), 1, fptr);
        printf("%c", ch);
    }
    printf("\n\n");
    fclose(fptr);
}

/* Delete chosen content (remove its bytes and decrement amount_of_content) */
void DeleteContent(char DiaryUserFileName[])
{
    fptr = fopen(DiaryUserFileName, "rb+");
    if (!fptr)
        return;

    /* Update amount_of_content on disk */
    fseek(fptr, sizeof(User.username) + sizeof(User.password), SEEK_SET);
    fwrite(&User.amount_of_content, sizeof(int), 1, fptr);

    /* Seek to content area and find start of record to delete */
    fseek(fptr, sizeof(User.username) + sizeof(User.password) + sizeof(User.amount_of_content), SEEK_SET);
    for (i = 1; i < position; i++)
    {
        fread(&len, sizeof(int), 1, fptr); /* title len */
        fseek(fptr, len, SEEK_CUR);
        fread(&len, sizeof(int), 1, fptr); /* text len */
        fseek(fptr, len, SEEK_CUR);
    }

    /* 'start' is now at the beginning of the record to delete */
    int start = ftell(fptr);

    /* Calculate content record size (title+int + text+int) to remove */
    fread(&len, sizeof(int), 1, fptr);
    fseek(fptr, len, SEEK_CUR);
    int contentLen = sizeof(int) + len;
    fread(&len, sizeof(int), 1, fptr);
    fseek(fptr, len, SEEK_CUR);
    contentLen += sizeof(int) + len;

    /* Determine file end */
    fseek(fptr, 0, SEEK_END);
    int end = ftell(fptr);

    /* Copy head and tail excluding the content to delete into temp, then replace file */
    FILE* ptr = fopen("temp.dat", "wb");
    if (!ptr)
    {
        fclose(fptr);
        return;
    }

    /* Copy head [0..start-1] */
    if (start > 0)
        copy_bytes(fptr, ptr, 0, 0, start);

    /* Copy tail starting at start+contentLen to new pos 'start' */
    int tailSrc = start + contentLen;
    int tailCount = end - tailSrc;
    if (tailCount > 0)
        copy_bytes(fptr, ptr, tailSrc, start, tailCount);

    fclose(ptr);
    fclose(fptr);

    remove(DiaryUserFileName);
    rename("temp.dat", DiaryUserFileName);
}

/* -------------------------
   User Menu & Authentication
   ------------------------- */

void UserMenu(char DiaryUserFileName[])
{
    Encryption(DiaryUserFileName, DECRYPT, encryptionCode);
    Backup(DiaryUserFileName, MAKE_THE_BACKUP_FILE, encryptionCode);

    do
    {
        clear_screen();
        printf("\t\t\tHello %s :)\n\n", User.username);
        printf("1) Write a Note\n2) Append in Note\n3) Read a Note\n4) Remove a Note\n5) Change Username/Password\n6) Log Out\n\nChoose from 1 to 6: ");
        choice = read_key();
        putchar(choice);
        delay(100);
        printf("\n");

        if (choice == '1')
        {
            clear_screen();
            title = (char*)malloc((MAX_TITLE_LEN + 1) * sizeof(char));
            printf("Enter Title: (max. 50 char.) ");
            scanf(" %50[^\n]", title);
            int titleLen = strlen(title);

            text = (char*)malloc((MAX_TEXT_LEN + 1) * sizeof(char));
            printf("Enter Text: (max. 10000 char.)\n\n");
            scanf(" %10000[^\n]", text);
            int textLen = strlen(text);

            if (User.amount_of_content == 0)
                position = 1;
            else
            {
                Display(DiaryUserFileName);
                printf("\n%d slot(s) available\n\n", User.amount_of_content + 1);
                do
                {
                    printf("Where to insert from 1 to %d? ", User.amount_of_content + 1);
                    scanf(" %c", &ch);
                    position = ch - '0';
                } while (!(position >= 1 && position <= User.amount_of_content + 1));
            }

            User.amount_of_content++;
            InsertContent(DiaryUserFileName, titleLen, textLen);
            Backup(DiaryUserFileName, MAKE_THE_BACKUP_FILE, encryptionCode);

            printf("\nContent Inserted in Slot %d...\n", position);
            pause_console();

            free(title);
            free(text);
        }
        else if (choice == '2')
        {
            clear_screen();
            if (User.amount_of_content != 0)
            {
                Display(DiaryUserFileName);
                printf("\n");
                if (User.amount_of_content == 1)
                    position = 1;
                else
                {
                    do
                    {
                        printf("Where to append from 1 to %d? ", User.amount_of_content);
                        scanf(" %c", &ch);
                        position = ch - '0';
                    } while (!(position >= 1 && position <= User.amount_of_content));
                }

                new = (char*)malloc((MAX_TEXT_LEN + 1) * sizeof(char));
                printf("Enter new Text: (max. 10000 char.)\n\n");
                scanf(" %10000[^\n]s", new);
                AppendInContent(DiaryUserFileName);
                free(new);
                Backup(DiaryUserFileName, MAKE_THE_BACKUP_FILE, encryptionCode);

                printf("\nContent Appended in Slot %d...\n", position);
            }
            else
            {
                printf("Nothing to append in...\n");
            }
            pause_console();
        }
        else if (choice == '3')
        {
            clear_screen();
            if (User.amount_of_content != 0)
            {
                Display(DiaryUserFileName);
                printf("\n");
                if (User.amount_of_content == 1)
                    position = 1;
                else
                {
                    do
                    {
                        printf("Which one to read from 1 to %d? ", User.amount_of_content);
                        scanf(" %c", &ch);
                        position = ch - '0';
                    } while (!(position >= 1 && position <= User.amount_of_content));
                }
                ReadContent(DiaryUserFileName);
            }
            else
            {
                printf("Nothing to read...\n");
            }
            pause_console();
        }
        else if (choice == '4')
        {
            clear_screen();
            if (User.amount_of_content != 0)
            {
                Display(DiaryUserFileName);
                printf("\n");
                if (User.amount_of_content == 1)
                    position = 1;
                else
                {
                    do
                    {
                        printf("Which one to delete from 1 to %d? ", User.amount_of_content);
                        scanf(" %c", &ch);
                        position = ch - '0';
                    } while (!(position >= 1 && position <= User.amount_of_content));
                }
                char sure;
                do
                {
                    printf("Are you sure you want to delete content in slot %d? (y/n) ", position);
                    scanf(" %c", &sure);
                    if (sure == 'y')
                    {
                        User.amount_of_content--;
                        DeleteContent(DiaryUserFileName);
                        Backup(DiaryUserFileName, MAKE_THE_BACKUP_FILE, encryptionCode);

                        printf("\nContent in Slot %d Deleted...\n", position);
                    }
                    else if (sure == 'n')
                        printf("\nContent in Slot %d Not Deleted...\n", position);
                } while (!(sure == 'y' || sure == 'n'));
            }
            else
            {
                printf("Nothing to delete...\n");
            }
            pause_console();
        }
        else if (choice == '5')
        {
            while (1)
            {
                clear_screen();
                printf("\t\t\tWhat to change?\n\n");
                printf("1) Username\n2) Password\n\n");
                scanf(" %c", &ch);
                if (ch == '1')
                {
                    char user[MAX_USERNAME_LEN + 1];
                    printf("Enter new username: (max. 20 char.) ");
                    scanf(" %20[^\n]", user);
                    strcpy(User.username, user);

                    fptr = fopen(DiaryUserFileName, "rb+");
                    if (fptr)
                    {
                        fwrite(&User.username, sizeof(User.username), 1, fptr);
                        fclose(fptr);
                    }

                    printf("Username Changed!!!\n");
                    pause_console();
                    break;
                }
                else if (ch == '2')
                {
                    char pass[MAX_PASSWORD_LEN + 1];
                    printf("Enter new password: (max. 20 char.) ");
                    setPassword(pass);
                    strcpy(User.password, pass);

                    fptr = fopen(DiaryUserFileName, "rb+");
                    if (fptr)
                    {
                        fseek(fptr, sizeof(User.username), SEEK_SET);
                        fwrite(&User.password, sizeof(User.password), 1, fptr);
                        fclose(fptr);
                    }

                    printf("Password Changed!!!\n");
                    pause_console();
                    break;
                }
            }
        }
    } while (choice != '6');

    Encryption(DiaryUserFileName, ENCRYPT, encryptionCode);
    Backup(DiaryUserFileName, REMOVE_THE_BACKUP_FILE, encryptionCode);
    clear_encryptionCode(encryptionCode);

    printf("Bye Bye!!!\n");
}

/* Verify ID, load encryptionCode for that user, check username/password */
int ID_Verification(char DiaryUserFileName[])
{
    clear_screen();
    char idString[ID_LENGTH + 1];
    int flag;
    char user[MAX_USERNAME_LEN + 1], pass[MAX_PASSWORD_LEN + 1];

    do
    {
        flag = 1;
        printf("Enter ID: ");
        scanf(" %5s", idString);
        if (strlen(idString) != ID_LENGTH)
        {
            flag = 0;
        }
        else
        {
            for (i = 0; i < ID_LENGTH; i++)
            {
                if (idString[i] < '0' || idString[i] > '9')
                {
                    flag = 0;
                    break;
                }
            }
        }

        if (!flag)
            printf("It must contain 5 exact numbers...\\n");
    } while (flag == 0);

    strcpy(DiaryUserFileName, idString);
    strcat(DiaryUserFileName, ".dat");

    fptr = fopen(DiaryUserFileName, "rb");
    if (fptr == NULL)
    {
        printf("ID doesn't exist...\n");
        return 0;
    }
    else
        fclose(fptr);

    /* Convert filename -> integer id */
    int desiredID = 0;
    for (i = 0; i < 5; i++)
    {
        desiredID *= 10;
        desiredID += DiaryUserFileName[i] - '0';
    }

    /* Read settings.dat and fetch the corresponding encryptionCode */
    fptr = fopen("settings.dat", "rb");
    if (!fptr)
        return 0;
    fseek(fptr, sizeof(settings.no_of_user) + sizeof(settings.color), SEEK_SET);
    for (i = 0; i < settings.no_of_user; i++)
    {
        int checkID;
        fread(&checkID, sizeof(checkID), 1, fptr);
        if (desiredID == checkID)
        {
            fread(&encryptionCode, sizeof(encryptionCode), 1, fptr);
            break;
        }
        else
            fseek(fptr, sizeof(encryptionCode), SEEK_CUR);
    }
    fclose(fptr);

    printf("Enter Name: ");
    scanf(" %20[^\n]", user);
    printf("Enter Password: ");
    setPassword(pass);

    /* Decrypt the user file temporarily to verify credentials */
    Encryption(DiaryUserFileName, DECRYPT, encryptionCode);
    fptr = fopen(DiaryUserFileName, "rb");
    if (!fptr)
        return 0;
    fread(&User.username, sizeof(User.username), 1, fptr);
    fread(&User.password, sizeof(User.password), 1, fptr);
    fread(&User.amount_of_content, sizeof(User.amount_of_content), 1, fptr);
    fclose(fptr);
    Encryption(DiaryUserFileName, ENCRYPT, encryptionCode);

    if (strcmp(User.username, user) != 0)
    {
        printf("Invalid Username...\n");
        clear_encryptionCode(encryptionCode);
        return 0;
    }
    if (strcmp(User.password, pass) != 0)
    {
        printf("Invalid Password...\n");
        clear_encryptionCode(encryptionCode);
        return 0;
    }
    return 1;
}

/* -------------------------
   Main program flow
   ------------------------- */

int main()
{
    generate_encryption_code();
    pause_console();
    char DiaryUserFileName[ID_LENGTH + 5];

    fptr = fopen("Encryption Code.dat", "rb");
    if (!fptr) // or if (fptr == NULL)
    {
        // WARNING: Existing encrypted Diary data may no longer be accessible if the original encryption code was deleted.
        char message[] = { "Encryption Code.dat was not found, A new encryption code must be created before Diary can start.\nCompile \"encryption_code.c\" and run \"encryption_code\" application to generate \"Encryption Code.dat\" file." };

        /* If encryption code file is missing, exit. */
        printf("%s\n", message);
        pause_console();
        return;
    }
    fclose(fptr);

    initialize();

    do
    {
        clear_screen();
        printf("\t\t\t\t\t\tWELCOME TO DIARY!!!!!\n\n");
        printf("1) Log In\n2) Register a User\n3) Change Console Colour\n4) Delete a User\n5) Quit\n\nChoose from 1 to 5: ");
        choice = read_key();
        putchar(choice);
        delay(100);
        printf("\n");

        if (choice == '1')
        {
            if (ID_Verification(DiaryUserFileName) == 1)
                UserMenu(DiaryUserFileName);
            pause_console();
        }
        else if (choice == '2')
        {
            char re_enter[21];
            while (1)
            {
                clear_screen();
                printf("Enter new Name: (max. 20 char.) ");
                scanf(" %20[^\n]", User.username);
                printf("Enter new Password: (max. 20 char.) ");
                setPassword(User.password);
                printf("Re-Enter Password: ");
                setPassword(re_enter);
                if (strcmp(User.password, re_enter) != 0)
                {
                    printf("Passwords do not match...\n");
                    pause_console();
                }
                else
                    break;
            }

            int newID = ID_Generation(); /* generate ID and save encryptionCode */
            sprintf(DiaryUserFileName, "%d", newID);
            strcat(DiaryUserFileName, ".dat");

            User.amount_of_content = 0;
            fptr = fopen(DiaryUserFileName, "wb");
            if (fptr)
            {
                fwrite(&User.username, sizeof(User.username), 1, fptr);
                fwrite(&User.password, sizeof(User.password), 1, fptr);
                fwrite(&User.amount_of_content, sizeof(User.amount_of_content), 1, fptr);
                fclose(fptr);
            }

            Encryption(DiaryUserFileName, ENCRYPT, encryptionCode);
            clear_encryptionCode(encryptionCode);
        }
        else if (choice == '3')
        {
            clear_screen();
            printf("0 = Black      8 = Gray\n");
            printf("1 = Blue       9 = Light Blue\n");
            printf("2 = Green      A = Light Green\n");
            printf("3 = Aqua       B = Light Aqua\n");
            printf("4 = Red        C = Light Red\n");
            printf("5 = Purple     D = Light Purple\n");
            printf("6 = Yellow     E = Light Yellow\n");
            printf("7 = White      F = Bright White\n\n");
            printf("Choose Background Color: ");
            scanf(" %c", &settings.color[0]);
            printf("Choose Foreground Color: ");
            scanf(" %c", &settings.color[1]);
            changecolour();
            printf("\nDone!!!\n");
            pause_console();
        }
        if (choice == '4')
        {
            if (ID_Verification(DiaryUserFileName) == 1)
            {
                DeleteUser(DiaryUserFileName);
                Backup(DiaryUserFileName, REMOVE_THE_BACKUP_FILE, encryptionCode);
                clear_encryptionCode(encryptionCode);
                printf("User Deleted!!!\n");
            }
            pause_console();
        }
    } while (choice != '5');

    /* Before exit, re-encrypt settings and clean up backup */
    Encryption("settings.dat", ENCRYPT, encryptSettings);
    Backup("settings.dat", REMOVE_THE_BACKUP_FILE, encryptSettings);

    printf("See you next time!!!\n");
    pause_console();
    return 0;
}