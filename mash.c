#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAXSTR 255
#define ARGCNT 6

void parse(char ***data, char *cmd); 

int main(int argc, char *argv[])

{

char cmd1[MAXSTR];
char cmd2[MAXSTR];
char cmd3[MAXSTR];
char file[MAXSTR];

// This will get command 1;
printf("mash-1->");
fgets(cmd1, 100, stdin);

// This will get command 2;
printf("mash-2->");
fgets(cmd2, 100, stdin);

// This will get command 3;
printf("mash-3->");
fgets(cmd3, 100, stdin);

// This will get file
printf("File->");
fgets(file, 100, stdin);

char ** args1;
parse(&args1, cmd1);

char ** args2;
parse(&args2, cmd2);

char ** args3;
parse(&args3, cmd3);

//THIS IS FOR OUTPUTING ONLY
// for (int i=0;i<ARGCNT;i++)
//     printf("i=%d args1[i]=%s\n",i,*(args1 + i));
// printf("\n");

// for (int i=0;i<ARGCNT;i++)
//     printf("i=%d args2[i]=%s\n",i,*(args2 + i));
// printf("\n");

// for (int i=0;i<ARGCNT;i++)
//     printf("i=%d args3[i]=%s\n",i,*(args3 + i));
// printf("\n");



int status = execvp(args1[0], args1);
printf("CMD1:[SHELL 1] STATUS CODE=%d\n", status);

return 0;
}

void parse(char ***data, char *cmd) {

     *data = (char**) malloc(sizeof(char*) * ARGCNT);

    char * str = strtok(cmd, " ");
    // printf("Please no seg\n");
    int cnt = 0;
    while (str != NULL && cnt < ARGCNT) {
        // printf("in Func: %s\n", str);
        *(*data + cnt) = str;
        str = strtok(NULL, " ");
        cnt++;
    }
    //This way it will make sure the last value is null 
    //Even if someone entered full lenght.
    *(data + 5) = 0;
}

//IF WE GET IN TROUBLE CODE

// This will get command 1;
//printf("mash-1->");
//fscanf(stdin, "%s", cmd1);
//fgets(cmd1, 100, stdin);

// // This will get command 2;
// printf("mash-2->");
// fscanf(stdin, "%s", cmd2);

// // This will get command 3;
// printf("mash-3->");
// fscanf(stdin, "%s", cmd3);

// // This will get file
// printf("file->");
// fscanf(stdin, "%s", file);

// char ** args = malloc(sizeof(char *) * ARGCNT);

// char * str = strtok(cmd1, " ");

// int cnt = 0;
// while (str != NULL && cnt < ARGCNT) {
//     //printf("%s\n", str);
//     *(args + cnt) = str;
//     str = strtok(NULL, " ");
//     cnt++;
// }

// char ** args = malloc(sizeof(char *) * ARGCNT);
// *(args + 0) = cmd1;
// *(args + 1) = cmd2;
// *(args + 2) = cmd3;
// *(args + 3) = file;
// *(args + 5) = 0;