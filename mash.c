#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

// todo: remove unused imports

#define MAXSTR 255
#define ARGCNT 6

void parse(char ***data, char *cmd, char *filename); 
void run_commands(char * file, char ** cmd1, char ** cmd2, char ** cmd3);
void execute_command(char** cmd, char cmd_number);

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
    parse(&args1, cmd1, file);

    char ** args2;
    parse(&args2, cmd2, file);

    char ** args3;
    parse(&args3, cmd3, file);

    run_commands(file, args1, args2, args3);

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
void parse(char ***data, char *cmd, char *filename) {

    *data = (char**) malloc(sizeof(char*) * ARGCNT);

    char * str = strtok(cmd, " ");
    int cnt = 0;
    while (str != NULL && cnt < ARGCNT) {
        // printf("in Func: %s\n", str);
        *(*data + cnt) = str;
        str = strtok(NULL, " ");
        cnt++;
    }
    //This way it will make sure the last value is null 
    //Even if someone entered full length.
    *(data + cnt) = &filename;
    *(data + (cnt + 1)) = 0;
}

/*
 * Run three commands entered in parallel. 
 */
void run_commands(char * file, char ** cmd1, char ** cmd2, char ** cmd3) {
       
    printf("Starting forks pid(%d)\n", getpid());
    int status;
    pid_t p1, p2, p3;
    clock_t parent_start, p1_start, p2_start, p3_start;
    clock_t parent_end, p1_end, p2_end, p3_end;

    p1 = fork(); // parent starts fork 1
    if (p1 == 0) { 
        // do child 1 stuff
        printf("--start p1 forks pid(%d)\n", getpid());
        p1_start = clock();
        execute_command(cmd1, '1');
        
    } else if (p1 > 0) {
        // parent starts fork 2
        p2 = fork();
        if (p2 == 0) {
            // do child 2 stuff
            printf("--start p2 forks pid(%d)\n", getpid());
            p2_start = clock();
            execute_command(cmd1, '2');
        
        } else if (p2 > 0) {
            // parent starts fork 3
            p3 = fork();
            if (p3 == 0) {
                // do child 3 stuff
                printf("--start p3 forks pid(%d)\n", getpid());
                p3_start = clock();
                execute_command(cmd1, '3');

            } else if (p3 > 0) {
                // parent made three threads with fork
                // wait for children to finish
                waitpid(p1, &status, 0);
                printf("First process finished...\n");
                p1_end = clock();

                waitpid(p2, &status, 0);
                printf("Second process finished...\n");
                p2_end = clock();

                waitpid(p3, &status, 0);
                printf("Third process finished...\n");
                p3_end = clock();

                printf("Children finished: p1(%d) p2(%d) p3(%d)\n", p1, p2, p3);
                //todo: read files and output results 
                // command  1 results

                printf("Result took:%dms\n", 
                    ((int) (p1_end - p1_start)) / CLOCKS_PER_SEC );

                // command 2 results

                printf("Result took:%dms\n", 
                    ((int) (p2_end - p2_start)) / CLOCKS_PER_SEC );

                // command 3 results

                printf("Result took:%dms\n", 
                    ((int) (p3_end - p3_start)) / CLOCKS_PER_SEC );
            }
        }
    }
    
    printf("Ending pid(%d)\n", getpid());

    // int status = execvp(args1[0], args1);
    // printf("CMD1:[SHELL 1] STATUS CODE=%d\n", status);
}

/*
 * Execute a single command and write out to a file.
 */
void execute_command(char** cmd, char cmd_number) {

    char file_name[6] = "file";
    file_name[4] = cmd_number;
    file_name[5] = 0;

    int new_file_handler;
    close(STDOUT_FILENO);

    new_file_handler = open(file_name, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);

    printf("in file %c", cmd_number);
    printf("\n");

    execvp(cmd[0], cmd); //todo: why is this not outputting to file?
    //stuff doesnt execute after here
}



// todo: remove
// char ** args = malloc(sizeof(char *) * ARGCNT);
// *(args + 0) = cmd1;
// *(args + 1) = cmd2;
// *(args + 2) = cmd3;
// *(args + 3) = file;
// *(args + 5) = 0;
