#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

// todo: remove unused imports

#define MAXSTR 255
#define ARGCNT 6

void parse(char ***data, char *cmd); 
void run_commands(char ** cmd1, char ** cmd2, char ** cmd3);

/*
 * Driver for program. 
 */
int main(int argc, char *argv[]) {

    char cmd1[MAXSTR];
    char cmd2[MAXSTR];
    char cmd3[MAXSTR];
    char file[MAXSTR];

    // This will get command 1;
    printf("mash-1->");
    fgets(cmd1, MAXSTR, stdin);

    // This will get command 2;
    printf("mash-2->");
    fgets(cmd2, MAXSTR, stdin);

    // This will get command 3;
    printf("mash-3->");
    fgets(cmd3, MAXSTR, stdin);

    // This will get file
    printf("File->");
    fgets(file, MAXSTR, stdin);

    char ** args1;
    parse(&args1, cmd1);

    char ** args2;
    parse(&args2, cmd2);

    char ** args3;
    parse(&args3, cmd3);

    run_commands(args1, args2, args3);

/* * * * * * * * * * * * * * * * * * * * * * * * * * 
    THIS IS FOR OUTPUTING ONLY
    for (int i=0;i<ARGCNT;i++)
        printf("i=%d args1[i]=%s\n",i,*(args1 + i));
    printf("\n");

    for (int i=0;i<ARGCNT;i++)
        printf("i=%d args2[i]=%s\n",i,*(args2 + i));
    printf("\n");

    for (int i=0;i<ARGCNT;i++)
        printf("i=%d args3[i]=%s\n",i,*(args3 + i));
    printf("\n");
 * * * * * * * * * * * * * * * * * * * * * * * * * */

    //todo: remove as this in in run_commands function
    // int status = execvp(args1[0], args1);
    // printf("CMD1:[SHELL 1] STATUS CODE=%d\n", status);

    return 0;
}

/*
 * Will parse the terminal inputed comands into char arrays.
 */
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

/*
 * Run three commands entered in parallel. 
 */
void run_commands(char ** cmd1, char ** cmd2, char ** cmd3) {
       
    printf("Starting forks pid(%d)\n", getpid());
    int status;
    pid_t p1, p2, p3;

    p1 = fork(); // parent starts fork 1
    if (p1 == 0) { 
        // do child 1 stuff
        printf("--start p1 forks pid(%d)\n", getpid());
        
    } else if (p1 > 0) {
        // parent starts fork 2
        p2 = fork();
        if (p2 == 0) {
            // do child 2 stuff
            printf("--start p2 forks pid(%d)\n", getpid());
        
        } else if (p2 > 0) {
            // parent starts fork 3
            p3 = fork();
            if (p3 == 0) {
                // do child 3 stuff
                printf("--start p3 forks pid(%d)\n", getpid());

            } else if (p3 > 0) {
                // parent made three threads with fork
                // wait for children to finish
                waitpid(p1, &status, 0);
                waitpid(p2, &status, 0);
                waitpid(p3, &status, 0);
                printf("Children finished: p1(%d) p2(%d) p3(%d)\n", p1, p2, p3);
            }
        }
    }
    
    printf("Ending pid(%d)\n", getpid());

    // int status = execvp(args1[0], args1);
    // printf("CMD1:[SHELL 1] STATUS CODE=%d\n", status);
}




// todo: remove
// char ** args = malloc(sizeof(char *) * ARGCNT);
// *(args + 0) = cmd1;
// *(args + 1) = cmd2;
// *(args + 2) = cmd3;
// *(args + 3) = file;
// *(args + 5) = 0;
