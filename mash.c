#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAXSTR 255
#define ARGCNT 5

int main(int argc, char *argv[])

{

char cmd[MAXSTR];
char arg1[MAXSTR];
char arg2[MAXSTR];
char file[MAXSTR];

// This will get command 1;
printf("mash-1->");
fscanf(stdin, "%s", cmd);

// This will get command 2;
printf("mash-2->");
fscanf(stdin, "%s", arg1);

// This will get command 3;
printf("mash-3->");
fscanf(stdin, "%s", arg2);

// This will get file
printf("file->");
fscanf(stdin, "%s", file);

char ** args = malloc(sizeof(char *) * ARGCNT);
*(args + 0) = cmd;
*(args + 1) = arg1;
*(args + 2) = arg2;
*(args + 3) = file;
*(args + 4) = 0;

for (int i=0;i<ARGCNT;i++)
    printf("i=%d args[i]=%s\n",i,*(args + i));

int status = execvp(args[0], args);
printf("STATUS CODE=%d\n",status);

return 0;
}
