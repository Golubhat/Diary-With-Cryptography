#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<conio.h>
#include<time.h>
#include<unistd.h>
#include"encryption code.h"

FILE *fptr;
int i,len;
char ch,choice;
enum Decision
{
    ENCRYPT = 1,
    DECRYPT = -1,
    MAKE_THE_BACKUP_FILE,
    REMOVE_THE_BACKUP_FILE
};

void Encryption(char fileName[],enum Decision direction)
{
    fptr = fopen(fileName,"rb+");
    fseek(fptr,0,SEEK_END);
    int length = ftell(fptr);
    for(i=0;i<length;i++)
    {
        fseek(fptr,i,SEEK_SET);
        fread(&ch,sizeof(char),1,fptr);
        ch += direction*encrypt[i%256];
        fseek(fptr,i,SEEK_SET);
        fwrite(&ch,sizeof(char),1,fptr);
    }
    fclose(fptr);
}
void Backup(char fileName[],enum Decision choice)
{
    char fileInBackup[20] = {"Backup//"};
    strcat(fileInBackup,fileName);
    if(choice==MAKE_THE_BACKUP_FILE)
    {
        FILE *write = fopen(fileInBackup,"wb");
        FILE *read = fopen(fileName,"rb");
        fseek(read,0,SEEK_END);
        int length = ftell(read);
        for(i=0;i<length;i++)
        {
            fseek(read,i,SEEK_SET);
            fread(&ch,sizeof(char),1,read);
            fseek(write,i,SEEK_SET);
            fwrite(&ch,sizeof(char),1,write);
        }
        fclose(write);
        fclose(read);

        Encryption(fileInBackup,ENCRYPT);
    }
    if(choice==REMOVE_THE_BACKUP_FILE)
        remove(fileInBackup);
}
void delay(unsigned int mseconds)
{
    clock_t goal = mseconds + clock();
    while (goal > clock());
}

struct DiarySettings
{
    int no_of_user;
    char color[2];
}
settings;
void initialize()
{
    mkdir("Backup");
    fptr = fopen("settings.dat","rb");
    if(fptr==NULL)
    {
        settings.no_of_user = 0;
        strcpy(settings.color,"07");
        fptr = fopen("settings.dat","wb");
        fwrite(&settings.no_of_user,sizeof(settings.no_of_user),1,fptr);
        fwrite(&settings.color,sizeof(settings.color),1,fptr);        
    }
    else
    {
        fclose(fptr);
        Encryption("settings.dat",DECRYPT);
        
        fptr = fopen("settings.dat","rb");
        fread(&settings.no_of_user,sizeof(settings.no_of_user),1,fptr);
        fread(&settings.color,sizeof(settings.color),1,fptr);
    }
    fclose(fptr);
    Backup("settings.dat",MAKE_THE_BACKUP_FILE);
    
    char colorCommand[9]={"color "};
    strcat(colorCommand,settings.color);
    system(colorCommand);
}
int IDgenerate()
{
    int i,flag;
    int randomNumber,IDfromSettings;
    srand(time(NULL));
    do
    {
        flag = 1;
        randomNumber = rand()%90000+10000;
        fptr = fopen("settings.dat","rb");
        fread(&settings.no_of_user,sizeof(int),1,fptr);
        fseek(fptr,sizeof(settings.no_of_user)+sizeof(settings.color),SEEK_SET);
        for(i=0;i<settings.no_of_user;i++)
        {
            fread(&IDfromSettings,sizeof(int),1,fptr);
            if(randomNumber==IDfromSettings)
            { 
              flag=0; 
              break;
            }
        }
        fclose(fptr);
        if(flag==1)
        {
            settings.no_of_user++;
            fptr = fopen("settings.dat","rb+");
            fwrite(&settings.no_of_user,sizeof(settings.no_of_user),1,fptr);
            fwrite(&settings.color,sizeof(settings.color),1,fptr);
            for(i=1;i<settings.no_of_user;i++)
                fseek(fptr,sizeof(int),SEEK_CUR);
            fwrite(&randomNumber,sizeof(int),1,fptr);
            fclose(fptr);

            Backup("settings.dat",MAKE_THE_BACKUP_FILE);
            return randomNumber;
        }
    } 
    while(1); 
}
void DeleteUser(char DiaryUserFileName[])
{
    remove(DiaryUserFileName);
    settings.no_of_user--;
    fptr = fopen("settings.dat","rb+");
    fwrite(&settings.no_of_user,sizeof(settings.no_of_user),1,fptr);
    int id = 0;
    for(i=0;i<5;i++)
    {
        id *= 10;
        id += DiaryUserFileName[i]-48;
    }
    int IDfromSettings,start,end;
    fseek(fptr,sizeof(settings.no_of_user)+sizeof(settings.color),SEEK_SET);
    while(1)
    {
        start = ftell(fptr);
        fread(&IDfromSettings,sizeof(int),1,fptr);
        if(id==IDfromSettings)
            break;
    }
    fseek(fptr,0,SEEK_END);
    end = ftell(fptr);
    FILE *ptr = fopen("temp.dat","wb");
    for(i=0;i<start;i++)
    {  
        fseek(fptr, i, SEEK_SET);
        fread(&ch, sizeof(char), 1, fptr);
        fseek(ptr, i, SEEK_SET); 
        fwrite(&ch, sizeof(char), 1, ptr);
    }
    for(i=start;i<end-sizeof(int);i++)
    {  
        fseek(fptr, i+sizeof(int), SEEK_SET);
        fread(&ch, sizeof(char), 1, fptr); 
        fseek(ptr, i, SEEK_SET); 
        fwrite(&ch, sizeof(char), 1, ptr);
    }
    fclose(ptr);    
    fclose(fptr);
    remove("settings.dat");
    rename("temp.dat","settings.dat");

    Backup("settings.dat",MAKE_THE_BACKUP_FILE);
}
void changecolour()
{
    char colorCommand[9]={"color "};
    strcat(colorCommand,settings.color);
    system(colorCommand);
    fptr = fopen("settings.dat","rb+");
    fseek(fptr,sizeof(settings.no_of_user),SEEK_SET);
    fwrite(&settings.color,sizeof(settings.color),1,fptr);
    fclose(fptr);

    Backup("settings.dat",MAKE_THE_BACKUP_FILE);
}

struct DiaryUser
{
    int ID;
    char username[21];
    char password[21];
    int amount_of_content;
}
User;
char *title,*text,*new;
int position;
void Display(char DiaryUserFileName[])
{
    system("cls");
    FILE *userfile = fopen(DiaryUserFileName,"rb");
    fseek(userfile,sizeof(User.ID)+sizeof(User.username)+sizeof(User.password),SEEK_SET);
    fread(&User.amount_of_content, sizeof(User.amount_of_content), 1, userfile);
    for(i=0;i<User.amount_of_content;i++)
    {
        fread(&len, sizeof(int), 1, userfile);
        printf("\n%d) ",i+1);
        for(int j=0;j<len;j++)
        {
             fread(&ch,sizeof(char),1,userfile);
             printf("%c",ch);
        }
        fread(&len, sizeof(int), 1, userfile);
        fseek(userfile,len,SEEK_CUR);
    }
    fclose(userfile);
    printf("\n\n");
}
void InsertContent(char DiaryUserFileName[],int titleLen,int textLen)
{
    fptr = fopen(DiaryUserFileName,"rb+");
    fseek(fptr,sizeof(User.ID)+sizeof(User.username)+sizeof(User.password),SEEK_SET);
    fwrite(&User.amount_of_content,sizeof(int),1,fptr);
    fseek(fptr,sizeof(User.ID)+sizeof(User.username)+sizeof(User.password)+sizeof(User.amount_of_content),SEEK_SET);
    int contentLen = sizeof(int)+titleLen+sizeof(int)+textLen;
    for (i=1;i<position;i++)
    {
        fread(&len, sizeof(int), 1, fptr);
        fseek(fptr,len,SEEK_CUR);
        fread(&len, sizeof(int), 1, fptr);
        fseek(fptr,len,SEEK_CUR);
    }
    int start = ftell(fptr);
    fseek(fptr,0,SEEK_END);
    int end = ftell(fptr);
    for(i=end-1;i>=start;i--)
    {  
        fseek(fptr, i, SEEK_SET);
        fread(&ch, sizeof(char), 1, fptr); 
        fseek(fptr, i+contentLen, SEEK_SET);
        fwrite(&ch, sizeof(char), 1, fptr);
    }
    fseek(fptr,start,SEEK_SET);
    fwrite(&titleLen,sizeof(int),1,fptr);
    for(i=0;i<titleLen;i++)
       fwrite(title+i,sizeof(char),1,fptr);
    fwrite(&textLen,sizeof(int),1,fptr);
    for(i=0;i<textLen;i++)
       fwrite(text+i,sizeof(char),1,fptr);
    fclose(fptr);
}
void AppendInContent(char DiaryUserFileName[])
{
    fptr = fopen(DiaryUserFileName,"rb+");
    fseek(fptr,sizeof(User.ID)+sizeof(User.username)+sizeof(User.password)+sizeof(User.amount_of_content),SEEK_SET);
    for (i=1;i<position;i++)
    {
        fread(&len, sizeof(int), 1, fptr);
        fseek(fptr,len,SEEK_CUR);
        fread(&len, sizeof(int), 1, fptr);
        fseek(fptr,len,SEEK_CUR);
    }
    fread(&len, sizeof(int), 1, fptr);
    fseek(fptr,len,SEEK_CUR);
    int start = ftell(fptr);

    fread(&len, sizeof(int), 1, fptr);
    int newLen = strlen(new)+len;
    fseek(fptr,start,SEEK_SET);
    fwrite(&newLen, sizeof(int), 1, fptr);

    fseek(fptr,len,SEEK_CUR);
    start = ftell(fptr);
    fseek(fptr,0,SEEK_END);
    int end = ftell(fptr);

    newLen = strlen(new);
    for(i=end-1;i>=start;i--)
    {  
        fseek(fptr, i, SEEK_SET);
        fread(&ch, sizeof(char), 1, fptr); 
        fseek(fptr, i+newLen, SEEK_SET);
        fwrite(&ch, sizeof(char), 1, fptr);
    }
    fseek(fptr,start,SEEK_SET);
    for(i=0;i<newLen;i++)
       fwrite(new+i,sizeof(char),1,fptr);
    fclose(fptr);
}
void ReadContent(char DiaryUserFileName[])
{
    fptr = fopen(DiaryUserFileName,"rb");
    fseek(fptr,sizeof(User.ID)+sizeof(User.username)+sizeof(User.password)+sizeof(User.amount_of_content),SEEK_SET);
    for(i=1;i<position;i++)
    {
        fread(&len, sizeof(int), 1, fptr);
        fseek(fptr,len,SEEK_CUR);
        fread(&len, sizeof(int), 1, fptr);
        fseek(fptr,len,SEEK_CUR);
    }
    fread(&len, sizeof(int), 1, fptr);
    printf("\n\t\t\t");
    for(i=0;i<len;i++)
    {
        fread(&ch,sizeof(char),1,fptr);
        printf("%c",ch);
    }
    printf("\n\n");
    fread(&len, sizeof(int), 1, fptr);
    for(i=0;i<len;i++)
    {
        fread(&ch,sizeof(char),1,fptr);
        printf("%c",ch);
    }
    printf("\n\n");
    fclose(fptr);    
}
void DeleteContent(char DiaryUserFileName[])
{
    fptr = fopen(DiaryUserFileName,"rb+");
    fseek(fptr,sizeof(User.ID)+sizeof(User.username)+sizeof(User.password),SEEK_SET);
    fwrite(&User.amount_of_content,sizeof(int),1,fptr);
    fseek(fptr,sizeof(User.ID)+sizeof(User.username)+sizeof(User.password)+sizeof(User.amount_of_content),SEEK_SET);
    for (i=1;i<position;i++)
    {
        fread(&len, sizeof(int), 1, fptr);
        fseek(fptr,len,SEEK_CUR);
        fread(&len, sizeof(int), 1, fptr);
        fseek(fptr,len,SEEK_CUR);
    }
    int start = ftell(fptr);

    fread(&len, sizeof(int), 1, fptr);
    fseek(fptr,len,SEEK_CUR);
    int contentLen = sizeof(int)+len;
    fread(&len, sizeof(int), 1, fptr);
    fseek(fptr,len,SEEK_CUR);
    contentLen += sizeof(int)+len;

    fseek(fptr,0,SEEK_END);
    int end = ftell(fptr);

    FILE *ptr = fopen("temp.dat","wb");
    for(i=0;i<start;i++)
    {  
        fseek(fptr, i, SEEK_SET);
        fread(&ch, sizeof(char), 1, fptr);
        fseek(ptr, i, SEEK_SET);
        fwrite(&ch, sizeof(char), 1, ptr);
    }
    for(i=start;i<end-contentLen;i++)
    {  
        fseek(fptr, i+contentLen, SEEK_SET);
        fread(&ch, sizeof(char), 1, fptr); 
        fseek(ptr, i, SEEK_SET);
        fwrite(&ch, sizeof(char), 1, ptr);
    }
    fclose(ptr);    
    fclose(fptr);
    remove(DiaryUserFileName);
    rename("temp.dat",DiaryUserFileName);
}
void UserMenu(char DiaryUserFileName[])
{
    Encryption(DiaryUserFileName,DECRYPT);
    Backup(DiaryUserFileName,MAKE_THE_BACKUP_FILE);
    do
    {
        system("cls");
        printf("\t\t\tHello %s :)\n\n",User.username);
        printf("1) Write a Note\n2) Append in Note\n3) Read a Note\n4) Remove a Note\n5) Change Username/Password\n6) Log Out\n\nChoose from 1 to 6: ");
        choice = getch();
        putch(choice);
        delay(100);
        printf("\n");
        
        if(choice=='1')
        {
            system("cls");
            title = (char *)malloc(50*sizeof(char));
            printf("Enter Title: (max. 50 char.) ");
            scanf(" %50[^\n]s",title);
            int titleLen = strlen(title);
            text = (char *)malloc(10000*sizeof(char));
            printf("Enter Text: (max. 10000 char.)\n\n");
            scanf(" %10000[^\n]s",text);
            int textLen = strlen(text);

            if(User.amount_of_content==0)
                position = 1;
            else
            {
                Display(DiaryUserFileName);
                printf("\n%d slot(s) available\n\n",User.amount_of_content+1);
                do
                {
                    printf("Where to insert from 1 to %d? ",User.amount_of_content+1);
                    scanf(" %c",&ch);
                    position = ch-48;
                }
                while(!(position>=1 && position<=User.amount_of_content+1));
            }
            User.amount_of_content++;
            InsertContent(DiaryUserFileName,titleLen,textLen);
            Backup(DiaryUserFileName,MAKE_THE_BACKUP_FILE);

            printf("\nContent Inserted in Slot %d...\n",position);
            system("pause");

            free(title);
            free(text);
        }
        else if(choice=='2')
        {
            system("cls");
            if(User.amount_of_content!=0)
            {
                if(User.amount_of_content==1)
                    position = 1;
                else
                {
                    Display(DiaryUserFileName);
                    printf("\n");
                    do
                    {
                        printf("Where to append from 1 to %d? ",User.amount_of_content);
                        scanf(" %c",&ch);
                        position = ch-48;
                    }
                    while(!(position>=1 && position<=User.amount_of_content));
                }
                new = (char *)malloc(10000*sizeof(char));
                printf("Enter new Text: (max. 10000 char.)\n\n");
                scanf(" %10000[^\n]s",new);
                AppendInContent(DiaryUserFileName);
                free(new);
                Backup(DiaryUserFileName,MAKE_THE_BACKUP_FILE);

                printf("\nContent Appended in Slot %d...\n",position);
            }
            else
                printf("Nothing to append in...\n");
            system("pause");
        }
        else if(choice=='3')
        {
            system("cls");
            if(User.amount_of_content!=0)
            {
                if(User.amount_of_content==1)
                    position = 1;
                else
                {
                    Display(DiaryUserFileName);
                    printf("\n");
                    do
                    {
                        printf("Which one to read from 1 to %d? ",User.amount_of_content);
                        scanf(" %c",&ch);
                        position = ch-48;
                    }
                    while(!(position>=1 && position<=User.amount_of_content));                    
                }
                ReadContent(DiaryUserFileName);
            }
            else
                printf("Nothing to read...\n");
            system("pause");
        }
        else if(choice=='4')
        {
            system("cls");
            if(User.amount_of_content!=0)
            {
                if(User.amount_of_content==1)
                    position = 1;
                else
                {
                    Display(DiaryUserFileName);
                    printf("\n");
                    do
                    {
                        printf("What to delete from 1 to %d? ",User.amount_of_content);
                        scanf(" %c",&ch);
                        position = ch-48;
                    }
                    while(!(position>=1 && position<=User.amount_of_content));
                }
                User.amount_of_content--;
                DeleteContent(DiaryUserFileName);
                Backup(DiaryUserFileName,MAKE_THE_BACKUP_FILE);

                printf("\nContent in Slot %d Deleted...\n",position);
            }
            else
                printf("Nothing to delete...\n");
            system("pause");
        }
        else if(choice=='5')
        {
            while(1)
            {
               system("cls");
               printf("\t\t\tWhat to change?\n\n");
               printf("1) Username\n2) Password\n\n");
               scanf(" %c",&ch);
               if(ch=='1')
               {
                  new = (char *)malloc(20*sizeof(char));
                  printf("Enter new username: (max. 20 char.) ");
                  scanf(" %20s",new);
                  fptr = fopen(DiaryUserFileName,"rb+");
                  fseek(fptr,sizeof(User.ID),SEEK_SET);
                  strcpy(User.username,new);
                  fwrite(&User.username,sizeof(User.username),1,fptr);
                  fclose(fptr);
                  free(new);
                  Backup("settings.dat",MAKE_THE_BACKUP_FILE);

                  printf("Username Changed!!!\n");
                  system("pause");
                  break;
               }
               else if(ch=='2')
               {
                  new = (char *)malloc(20*sizeof(char));
                  printf("Enter new password: (max. 20 char.) ");
                  scanf(" %20s",new);
                  fptr = fopen(DiaryUserFileName,"rb+");
                  fseek(fptr,sizeof(User.ID)+sizeof(User.username),SEEK_SET);
                  strcpy(User.password,new);
                  fwrite(&User.password,sizeof(User.password),1,fptr);
                  fclose(fptr);
                  free(new);
                  Backup("settings.dat",MAKE_THE_BACKUP_FILE);

                  printf("Password Changed!!!\n");
                  system("pause");
                  break;
               }
            }
        }
    }
    while(choice!='6');

    Encryption(DiaryUserFileName,ENCRYPT);
    Backup(DiaryUserFileName,REMOVE_THE_BACKUP_FILE);

    printf("Bye Bye!!!\n");
}

void setPassword(char thePassword[])
{
    i = 0;
    while((ch = getch()) != 13)    // 13 is ASCII value of Enter key
    {
        if(ch == 8)                // 8 is ASCII value of Backspace key
        {
            if(i > 0)
            {
                putch('\b');
                putch(' ');
                putch('\b');
                i--;
            }
        }
        else
        {
            if(i < 20)
            {
                thePassword[i++] = ch;
                putch('*');
            }
        }
    }
    thePassword[i] = '\0';
    printf("\n");
}
int ID_Verification(char DiaryUserFileName[])
{
    system("cls");
    char *idString;
    int flag;
    char user[21],pass[21];
    do
    {
        flag = 1;
        printf("Enter ID: ");
        scanf(" %as",&idString);
        for(i=0;i<5;i++)
        {
           if( !(idString[i]-48 >=0 && idString[i]-48 <=9) || strlen(idString)>5 )
           {
               flag = 0;
               printf("It must contain 5 exact numbers...\n");
               break;
           }
        }
    }
    while(flag==0);
    
    strcpy(DiaryUserFileName,idString);
    strcat(DiaryUserFileName,".dat");  
    fptr = fopen(DiaryUserFileName,"rb");
    if(fptr==NULL)
    {
        printf("ID doesn't exist...\n");
        return 0;       
    }
    else
        fclose(fptr);

    printf("Enter Name: ");
    scanf(" %20[^\n]s",user);
    printf("Enter Password: ");
    setPassword(pass);

    Encryption(DiaryUserFileName,DECRYPT);
    fptr = fopen(DiaryUserFileName,"rb");
    fseek(fptr,sizeof(User.ID),SEEK_SET);
    fread(&User.username,sizeof(User.username),1,fptr);
    fread(&User.password,sizeof(User.password),1,fptr);
    fclose(fptr);
    Encryption(DiaryUserFileName,ENCRYPT);
    if(strcmp(User.username,user)!=0)
    {
        printf("Invalid Username...\n");
        return 0;
    }
    if(strcmp(User.password,pass)!=0)
    {
        printf("Invalid Password...\n");
        return 0;
    }
    else
        return 1;
}

int main()
{
    char DiaryUserFileName[10];
    initialize();
    do
    {
         system("cls");
         printf("\t\t\t\t\t\tWELCOME TO DIARY!!!!!\n\n");
         printf("1) Log In\n2) Register a User\n3) Change Console Colour\n4) Delete a User\n5) Quit\n\nChoose from 1 to 5: ");
         choice = getch();
         putch(choice);
         delay(100);
         printf("\n");

         if(choice=='1')
         {
            if(ID_Verification(DiaryUserFileName)==1)
                UserMenu(DiaryUserFileName);
            system("pause");
         }
         else if(choice=='2')
         {
             char re_enter[21];
             while(1)
             {
              system("cls");
              printf("Enter new Name: (max. 20 char.) ");
              scanf(" %20[^\n]s",User.username);
              printf("Enter new Password: (max. 20 char.) ");
              setPassword(User.password);
              printf("Re-Enter Password: ");
              setPassword(re_enter);
              if(strcmp(User.password,re_enter)!=0)
              {
                printf("Passwords do not match...\n");
                system("pause");
              }
              else
                break;
             }
             User.ID = IDgenerate();
             printf("Your ID is %d\n",User.ID);
             system("pause");

             User.amount_of_content = 0;
             sprintf(DiaryUserFileName,"%d",User.ID);
             strcat(DiaryUserFileName,".dat");
             fptr = fopen(DiaryUserFileName,"wb");
             fwrite(&User.ID,sizeof(User.ID),1,fptr);
             fwrite(&User.username,sizeof(User.username),1,fptr);
             fwrite(&User.password,sizeof(User.password),1,fptr);
             fwrite(&User.amount_of_content,sizeof(User.amount_of_content),1,fptr);
             fclose(fptr);

             Encryption(DiaryUserFileName,ENCRYPT);
         }
         else if(choice=='3')
         {
            char background,foreground;
            system("cls");
            printf("0 = Black      8 = Gray\n");
            printf("1 = Blue       9 = Light Blue\n");
            printf("2 = Green      A = Light Green\n");
            printf("3 = Aqua       B = Light Aqua\n");
            printf("4 = Red        C = Light Red\n");
            printf("5 = Purple     D = Light Purple\n");
            printf("6 = Yellow     E = Light Yellow\n");
            printf("7 = White      F = Bright White\n\n");
            printf("Choose Background Color: ");
            scanf(" %c",&settings.color[0]);
            printf("Choose Foreground Color: ");
            scanf(" %c",&settings.color[1]);
            changecolour();
            printf("\nDone!!!\n");
            system("pause");
         }
         if(choice=='4')
         {
            if(ID_Verification(DiaryUserFileName)==1)
            {
                DeleteUser(DiaryUserFileName);
                Backup(DiaryUserFileName,REMOVE_THE_BACKUP_FILE);
                printf("User Deleted!!!\n");
            }
            system("pause");
         }
    } 
    while(choice!='5');

    Encryption("settings.dat",ENCRYPT);
    Backup("settings.dat",REMOVE_THE_BACKUP_FILE);

    printf("See you next time!!!\n");
    system("pause");
}
